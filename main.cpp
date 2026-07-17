#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "DecisionTree.hpp"
#include <algorithm>
#include <random>

using namespace std;

int main() {

    cout << "\n=========================================\n";
    cout << "      MODO DE RECONHECIMENTO AO VIVO     \n";
    cout << "=========================================\n";

    if(!sf::SoundBufferRecorder::isAvailable()){
        cerr << "Erro: Nenhum microfone encontrado no sistema!" << std;
        return 1;
    }

    sf::SoundBufferRecorder gravador;

    cout << "Pressione [ENTER] e fale: 'chocolate, zebra, banana, taxi'" << endl;
    cin.get();


    unsigned int taxaAmostragem = 22050;
    gravador.start(taxaAmostragem);

    std::this_thread::sleep_for(std::chrono::milliseconds(3600));
    gravador.stop();
    std::cout << "[ GRAVACAO CONCLUIDA ]\n" << std::endl;







    ml::Matrix X;
    ml::Labels y;
    ml::Matrix X_train, X_test;
    ml::Labels y_train, y_test;

    vector<int>position_not_my_voice;
    vector<int>position_my_voice;

    cout << "Lendo os dados de espaco_caracteristicas.txt..." << std::endl;
    ifstream infile("espaco_caracteristicas.txt");
    if (!infile.is_open()) {
        cerr << "Erro ao abrir espaco_caracteristicas.txt" << std::endl;
        return 1;
    }

    std::string line;
    int line_idx = 0;
    
    
    const int NUM_VOZES_ORIGINAIS = 61; 

    while (getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<double> features;
        double val;
        while (ss >> val) {
            features.push_back(val);
        }

        X.push_back(features);
        
        
        if (line_idx < NUM_VOZES_ORIGINAIS) {
            y.push_back(0);
            position_not_my_voice.push_back(line_idx);
        } else {
            y.push_back(1);
            position_my_voice.push_back(line_idx);
        }
        
        line_idx++;
    }

    cout << "Foram lidas " << X.size() << " amostras." << endl;
    if(X.empty()) {
        cerr << "O arquivo esta vazio ou as features não foram lidas corretamente." << endl;
        return 1;
    }
    cout << "Cada amostra possui " << X[0].size() << " caracteristicas." << endl;

    random_device rd;

    mt19937 gerador(rd());

    shuffle(position_not_my_voice.begin(), position_not_my_voice.end(), gerador);
    shuffle(position_my_voice.begin(), position_my_voice.end(), gerador);

    double train_ratio = 0.7;
    size_t train_size_not_my_voice = position_not_my_voice.size() * train_ratio;
    size_t train_size_my_voice = position_my_voice.size() * train_ratio;

    for (size_t i = 0; i < position_not_my_voice.size(); ++i) {
        int idx = position_not_my_voice[i];
        if (i < train_size_not_my_voice) {
            X_train.push_back(X[idx]);
            y_train.push_back(0);
        } else {
            X_test.push_back(X[idx]);
            y_test.push_back(0);
        }
    }

    for (size_t i = 0; i < position_my_voice.size(); ++i) {
        int idx = position_my_voice[i];
        if (i < train_size_my_voice) {
            X_train.push_back(X[idx]);
            y_train.push_back(1);
        } else {
            X_test.push_back(X[idx]);
            y_test.push_back(1);
        }
    }

    cout << "\nDivisão concluída:" << endl;
    cout << "- Treino: " << X_train.size() << " amostras" << endl;
    cout << "- Teste: " << X_test.size() << " amostras" << endl;

    bool has_your_voice = false;
    for(int label : y) if(label == 1) has_your_voice = true;

    if (!has_your_voice) {
        cout << "\n[AVISO] Você ainda não adicionou a sua voz ao final do arquivo 'espaco_caracteristicas.txt'!\n";
        cout << "Por favor, extraia as características da sua voz, cole na última linha e rode novamente.\n";
    }

    ml::DecisionTree tree(10, 1e-5);
    
    cout << "\nTreinando a arvore de decisao..." << endl;
    tree.fit(X_train, y_train);

    cout << "Predicao concluida. Verificando acertos nos dados de teste." << endl;
    ml::Labels preds = tree.predict(X_test);

    int acertos = 0;
    for (size_t i = 0; i < preds.size(); ++i) {
        if (y_test[i] == preds[i]) acertos++;
        
        if (y_test[i] == 1) {
            std::cout << "Sua voz no Teste -> Predita como: " 
                      << (preds[i] == 1 ? "Sua voz (1)" : "Outra (0)") << "\n";
        }
    }
    
    std::cout << "Acuracia no Teste: " << (double)acertos / y_test.size() * 100.0 << "%" << std::endl;

    return 0;
}
