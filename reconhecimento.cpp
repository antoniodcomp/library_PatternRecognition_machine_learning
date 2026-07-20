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

    // -- PROCESSAMENTO DO AUDIO GRAVADO AO VIVO -- //
    
    // APLICA WAVELET
    std::vector<double> wvlt_features_ao_vivo = apply_wavelet(features_ao_vivo, 4);
    size_t llen = ajust_wavelets_length(audios_da_base);
    wvlt_features_ao_vivo.resize(llen);

    // APLICA DTW
    std::vector<double> preprocessed_audio_ao_vivo = aligned_audios = audio_aligner(wvlt_features_ao_vivo, audios_da_base[45]);
    
    cout << "Analisando voz..." << endl;
    ml::Labels predicao = tree.predict({preprocessed_audio_ao_vivo});
    
    cout << "\nResultado Final:\n";
    if (predicao[0] == 1) {
        cout << "-> ACESSO PERMITIDO! Voz reconhecida.\n";
    } else {
        cout << "-> ACESSO NEGADO! Voz desconhecida.\n";
    }

    return 0;
}

