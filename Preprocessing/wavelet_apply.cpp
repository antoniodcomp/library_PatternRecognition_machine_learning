#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <string>

using namespace std;
namespace fs = std::filesystem;

std::vector<double> haar_wavelet_function(std::vector<double> signal) {

    if(signal.size() % 2 != 0) {
        signal.push_back(signal.back());
    }

    size_t n = signal.size() / 2;
    std::vector<double> low_pass(n);

    for(size_t i = 0; i < n; i++) {
        double odd = signal[2*i];
        double even = signal[2*i + 1];

        low_pass[i] = (odd+even) / std::sqrt(2.0);
    }

    return low_pass;
}

std::vector<double> apply_wavelts(const std::vector<double>& signal, const unsigned int level) {
    std::vector<double> curr_aprox = signal;
    
    for(unsigned int i = 0; i < level; i++) {
        curr_aprox = haar_wavelet_function(curr_aprox);
        if(curr_aprox.size() < 2) break;
    }

    return curr_aprox;
}

void ajust_wavelets_length(std::vector<std::vector<double>>& caract_X) {
    size_t llen = 1e9;

    for(auto& s: caract_X) {
        if(s.size() < llen) llen = s.size();
    }

    for (auto& s : caract_X) {
        if (s.size() > llen) {
            s.resize(llen);
        }
    }

}

std::vector<double> load_txt(const std::string& filepath) {
    std::vector<double> data;
    std::ifstream file(filepath);
    double val;
    while (file >> val) {
        data.push_back(val);
    }
    return data;
}

int main() {
    std::vector<std::vector<double>> audios;
    std::string path_str = "amostras_voz-20260716T105843Z-1-001/amostras_voz";
    
    std::vector<fs::path> paths;
    for (const auto& entry : fs::directory_iterator(path_str)) {
        if (entry.path().extension() == ".txt") {
            paths.push_back(entry.path());
        }
    }
    std::sort(paths.begin(), paths.end());

    for (const auto& p : paths) {
        audios.push_back(load_txt(p.string()));
    }

    std::cout << "Total de audios: " << audios.size() << " | Amostras no primeiro: " << audios[0].size() << std::endl;

    std::vector<std::vector<double>> caract_X;
    for (const auto& audio : audios) {
        caract_X.push_back(apply_wavelts(audio, 4));
    }

    ajust_wavelets_length(caract_X);

    std::cout << "Processamento concluído!" << std::endl;
    std::cout << "Espaço de Características: " << caract_X.size() << "x" << caract_X[0].size() << std::endl;

    std::ofstream outfile("espaco_caracteristicascpp.txt");
    outfile << std::fixed << std::setprecision(6);
    
    for (const auto& row : caract_X) {
        for (size_t i = 0; i < row.size(); ++i) {
            outfile << row[i] << (i == row.size() - 1 ? "" : " ");
        }
        outfile << "\n";
    }

    std::cout << "Arquivo salvo com sucesso em: espaco_caracteristicas.txt" << std::endl;

    return 0;
}