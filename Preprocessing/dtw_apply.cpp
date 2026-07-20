#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <string>

namespace fs = std::filesystem;

struct DTWResult {
    std::vector<std::vector<double>> dtw;
    double distance;
};

// Calculo da Distância Fast-DTW
DTWResult fast_DTW_distance(const std::vector<double>& audio1, const std::vector<double>& audio2, int window = 10) {
    int n = static_cast<int>(audio1.size());
    int m = static_cast<int>(audio2.size());

    window = std::max(window, std::abs(n - m));

    // inicializa a matriz DTW com 1e9 (Infinito)
    std::vector<std::vector<double>> dtw(n, std::vector<double>(m, 1e9));

    // caso base
    dtw[0][0] = std::abs(audio1[0] - audio2[0]);

    // inicializa a primeira coluna
    int limit_col = std::min(n, window + 1);
    for (int i = 1; i < limit_col; ++i) {
        dtw[i][0] = dtw[i - 1][0] + std::abs(audio1[i] - audio2[0]);
    }

    // inicializa a primeira linha
    int limit_row = std::min(m, window + 1);
    for (int j = 1; j < limit_row; ++j) {
        dtw[0][j] = dtw[0][j - 1] + std::abs(audio1[0] - audio2[j]);
    }

    // preenchimento da matriz dentro do raio da janela
    for (int i = 1; i < n; ++i) {
        int j_min = std::max(1, i - window);
        int j_max = std::min(m, i + window + 1);

        for (int j = j_min; j < j_max; ++j) {
            double cost = std::abs(audio1[i] - audio2[j]);
            double min_prev = std::min({dtw[i - 1][j], dtw[i][j - 1], dtw[i - 1][j - 1]});
            dtw[i][j] = cost + min_prev;
        }
    }

    return {dtw, dtw[n - 1][m - 1]};
}

// seleção do Áudio de Referência (Menor distância total para todos os outros)
int select_reference_audio(const std::vector<std::vector<double>>& matrix_voices) {
    int l = static_cast<int>(matrix_voices.size());
    int reference_audio = -1;
    double res_min = 1e9;

    for (int i = 0; i < l; ++i) {
        double res = 0.0;
        for (int j = 0; j < l; ++j) {
            DTWResult result = fast_DTW_distance(matrix_voices[i], matrix_voices[j]);
            res += result.distance;
            std::cout << res << "\n";
        }

        std::cout << i << " audio analyzed\n";
        if (res < res_min) {
            res_min = res;
            reference_audio = i;
        }
    }

    std::cout << "reference: " << reference_audio << "\n";
    return reference_audio;
}

// extração e Salvamento dos Rótulos (Labels)
void get_labels(const fs::path& path) {
    std::vector<fs::path> files;
    for (const auto& entry : fs::directory_iterator(path)) {
        if (entry.path().extension() == ".txt") {
            files.push_back(entry.path());
        }
    }
    std::sort(files.begin(), files.end());

    std::ofstream outfile("audio_labels.txt");
    for (const auto& file : files) {
        std::string filename = file.filename().string();
        std::string label = filename.substr(0, std::min<size_t>(7, filename.length()));
        outfile << label << "\n";
    }
}

// alinhador Temporal e Reamostragem/Interpolação
std::vector<double> audio_aligner(const std::vector<double>& audio1, const std::vector<double>& audio2, int window = 10) {
    int n = static_cast<int>(audio1.size());
    int m = static_cast<int>(audio2.size());

    DTWResult result = fast_DTW_distance(audio1, audio2, window);
    const auto& dtw = result.dtw;

    // backtracking
    int i = n - 1;
    int j = m - 1;

    std::vector<int> caminho_i;
    std::vector<int> caminho_j;

    caminho_i.push_back(i);
    caminho_j.push_back(j);

    while (i > 0 || j > 0) {
        if (i == 0) {
            j--;
        } else if (j == 0) {
            i--;
        } else {
            double diag = dtw[i - 1][j - 1];
            double cima = dtw[i - 1][j];
            double esq  = dtw[i][j - 1];

            if (diag <= cima && diag <= esq) {
                i--;
                j--;
            } else if (cima < esq) {
                i--;
            } else {
                j--;
            }
        }
        caminho_i.push_back(i);
        caminho_j.push_back(j);
    }

    // inverte o caminho (já que começamos do final)
    std::reverse(caminho_i.begin(), caminho_i.end());
    std::reverse(caminho_j.begin(), caminho_j.end());

    std::vector<double> audio2_alinhado(n, 0.0);

    // mapeia e calcula a média das correspondências para cada índice da referência
    for (int idx_ref = 0; idx_ref < n; ++idx_ref) {
        double sum = 0.0;
        int count = 0;

        for (size_t k = 0; k < caminho_i.size(); ++k) {
            if (caminho_i[k] == idx_ref) {
                int idx_audio2 = caminho_j[k];
                sum += audio2[idx_audio2];
                count++;
            }
        }

        if (count > 0) {
            audio2_alinhado[idx_ref] = sum / count;
        }
    }

    return audio2_alinhado;
}

// auxiliar para carregar matriz do arquivo espaco_caracteristicas.txt
std::vector<std::vector<double>> load_matrix(const std::string& filepath) {
    std::vector<std::vector<double>> matrix;
    std::ifstream file(filepath);
    std::string line;

    while (std::getline(file, line)) {
        if (line.empty()) continue;
        std::stringstream ss(line);
        std::vector<double> row;
        double val;
        while (ss >> val) {
            row.push_back(val);
        }
        if (!row.empty()) {
            matrix.push_back(row);
        }
    }
    return matrix;
}

int main() {
    std::ios_base::sync_with_stdio(false);
    std::cin.tie(NULL);

    fs::path path_audios("amostras_voz-20260716T105843Z-1-001/amostras_voz");
    get_labels(path_audios);

    std::vector<std::vector<double>> matrix_voices = load_matrix("espaco_caracteristicas.txt");
    if (matrix_voices.empty()) {
        std::cerr << "Erro ao carregar espaco_caracteristicas.txt" << std::endl;
        return 1;
    }

    int l = static_cast<int>(matrix_voices.size());
    int reference_audio = select_reference_audio(matrix_voices);

    std::vector<std::vector<double>> aligned_audios(l);

    for (int i = 0; i < l; ++i) {
        aligned_audios[i] = audio_aligner(matrix_voices[reference_audio], matrix_voices[i]);
    }

    // Salva os áudios alinhados em arquivo
    std::ofstream outfile("aligned_audioscp.txt");
    outfile << std::fixed << std::setprecision(6);

    for (const auto& row : aligned_audios) {
        for (size_t j = 0; j < row.size(); ++j) {
            outfile << row[j] << (j == row.size() - 1 ? "" : " ");
        }
        outfile << "\n";
    }

    std::cout << "Áudios alinhados salvos com sucesso em: aligned_audios.txt" << std::endl;

    return 0;
}