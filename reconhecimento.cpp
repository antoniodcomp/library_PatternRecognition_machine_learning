#include <iostream>
#include <vector>
#include <thread>
#include <chrono>
#include <SFML/Audio.hpp>
#include "DecisionTree.hpp"
#include "Preprocessing/dtw_utils.h"
#include "Preprocessing/wavelet_utils.h"

using namespace std;

int main(){
    cout << "\n=========================================\n";
    cout << "      MODO DE RECONHECIMENTO AO VIVO     \n";
    cout << "=========================================\n";

    ml::DecisionTree tree;

    if(!tree.load("modelo_arvore.dat")){
        cerr << "Erro: Nao foi possivel carregar 'modelo_arvore.dat'. Execute o Treinamento primeiro!" << endl;
        return 1;
    }

    if(!sf::SoundBufferRecorder::isAvailable()){
        cerr << "Erro: Nenhum microfone encontrado no sistema!" << endl;
        return 1;
    }

    sf::SoundBufferRecorder gravador;
    cout << "\nPressione [ENTER] e fale: 'chocolate, zebra, banana, taxi'" << endl;
    cin.get();

    unsigned int taxaAmostragem = 22050;
    gravador.start(taxaAmostragem);

    cout << "\n[ GRAVANDO... Fale agora! ]" << endl;
    std::this_thread::sleep_for(std::chrono::milliseconds(3600));
    gravador.stop();
    cout << "[ GRAVACAO CONCLUIDA ]\n" << endl;

    const sf::SoundBuffer& buffer = gravador.getBuffer();

    if (buffer.saveToFile("audio_ao_vivo.wav")) {
        cout << "[ DEBUG ] Audio capturado salvo com sucesso em 'audio_ao_vivo.wav'." << endl;
    } else {
        cerr << "[ AVISO ] Nao foi possivel salvar o arquivo 'audio_ao_vivo.wav'." << endl;
    }

    
    const sf::Int16* amostras = buffer.getSamples();
    size_t contagem_gravada = buffer.getSampleCount();

    vector<double> features_ao_vivo;
    const size_t NUM_AMOSTRAS = 78246;

    for(size_t i = 0; i < NUM_AMOSTRAS; ++i){
        if(i < contagem_gravada){
            features_ao_vivo.push_back(amostras[i] / 32768.0);
        } else {
            features_ao_vivo.push_back(0.0);
        }
    }

    apply_wavelet(features_ao_vivo, 5);
    
    cout << "Analisando voz..." << endl;
    ml::Labels predicao = tree.predict({features_ao_vivo});
    
    cout << "\nResultado Final:\n";
    if (predicao[0] == 1) {
        cout << "-> ACESSO PERMITIDO! Voz reconhecida.\n";
    } else {
        cout << "-> ACESSO NEGADO! Voz desconhecida.\n";
    }

    return 0;
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

