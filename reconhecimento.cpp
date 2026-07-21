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


    // cout << "[DEBUG] Tentando salvar arquivo de áudio..." << endl;
    // if (buffer.saveToFile("audio_ao_vivo.wav")) {
    //     cout << "[ DEBUG ] Audio capturado salvo com sucesso em 'audio_ao_vivo.wav'." << endl;
    // } else {
    //     cerr << "[ AVISO ] Nao foi possivel salvar o arquivo 'audio_ao_vivo.wav'." << endl;
    // }

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

    
    features_ao_vivo = apply_wavelts(features_ao_vivo, 5);

    //std::cout << "[DEBUG] Tamanho das features geradas: " << features_ao_vivo.size() << std::endl;

    //cout << "[DEBUG] Analisando voz na Arvore de Decisao..." << endl;
    ml::Labels predicao = tree.predict({features_ao_vivo});
    
    cout << "\nResultado Final:\n";
    if (predicao[0] == 1) {
        cout << "-> ACESSO PERMITIDO! Voz reconhecida.\n";
    } else {
        cout << "-> ACESSO NEGADO! Voz desconhecida.\n";
    }

    cout << "\nPressione [ENTER] para sair..." << endl;
    cin.ignore(10000, '\n');
    cin.get();

    return 0;
}

