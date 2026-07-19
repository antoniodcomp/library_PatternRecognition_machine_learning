#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <string>
#include "DecisionTree.hpp" // Ajuste o caminho se necessário
#include <algorithm>
#include <random>

using namespace std;

int main() {
    ml::Matrix X;
    ml::Labels y;
    ml::Matrix X_train, X_test;
    ml::Labels y_train, y_test;

    vector<int> position_not_my_voice;
    vector<int> position_my_voice;

    cout << "Abrindo arquivos de dados..." << std::endl;
    ifstream infile("carac_pca.txt"); 
    ifstream infile2("labels.txt");
    
    if (!infile.is_open()) {
        cerr << "Erro ao abrir carac_pca.txt" << std::endl;
        return 1;
    }

    std::string line;
    int line_idx = 0;

    std::cout << "Lendo as caracteristicas do PCA..." << std::endl;
    while (getline(infile, line)) {
        if (line.empty()) continue;

        std::stringstream ss(line);
        std::vector<double> features;
        double val;
        
        // Lê as 43 colunas diretamente para o vetor
        while (ss >> val) {
            features.push_back(val);
        }

        X.push_back(features);
    }

    // 2. Lendo os rótulos (Blindado contra '\r')
    std::cout << "Lendo os rotulos (Labels)..." << std::endl;
    while (getline(infile2, line)) {
        if (line.empty()) continue;

        std::stringstream ss_label(line);
        std::string ll;
        ss_label >> ll; // Extrai apenas a palavra, ignorando espaços e quebras invisíveis

        if (ll == "VO38592") {
            y.push_back(1);
            position_my_voice.push_back(line_idx);
        } else {
            y.push_back(0);
            position_not_my_voice.push_back(line_idx);
        }

        line_idx++;
    }

    cout << "Foram lidas " << X.size() << " amostras." << endl;
    if (X.empty()) {
        cerr << "O arquivo esta vazio ou as features nao foram lidas corretamente." << endl;
        return 1;
    }

    if (X.size() != y.size()) {
        std::cerr << "ERRO CRITICO: O numero de audios (" << X.size() 
                  << ") nao bate com o numero de rotulos (" << y.size() << ")!\n";
        return 1;
    }
    
    cout << "Cada amostra foi resumida para " << X[0].size() << " caracteristicas." << endl;

    // 3. Embaralhamento usando Semente Fixa (Para testes controlados)
    random_device rd;
    mt19937 gerador(rd()); // Mude para rd() apenas quando terminar todos os ajustes
    //mt19937 gerador(42)

    shuffle(position_not_my_voice.begin(), position_not_my_voice.end(), gerador);
    shuffle(position_my_voice.begin(), position_my_voice.end(), gerador);

    // 4. Divisão Treino e Teste (70%)
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

    cout << "\nDivisao concluida:" << endl;
    cout << "- Treino: " << X_train.size() << " amostras" << endl;
    cout << "- Teste: " << X_test.size() << " amostras" << endl;

    bool has_your_voice = false;
    for (int label : y) {
        if (label == 1) has_your_voice = true;
    }

    if (!has_your_voice) {
        cout << "\n[AVISO] Voce ainda nao adicionou a sua voz ao dataset!\n";
    }

    // 5. Treinamento da Árvore de Decisão
    // Como agora temos apenas 5 características ricas, podemos usar uma árvore de profundidade 5
    ml::DecisionTree tree(5, 1e-5);
    
    cout << "\nTreinando a arvore de decisao..." << endl;
    tree.fit(X_train, y_train);

    // 6. Avaliação
    cout << "Predicao concluida. Verificando acertos nos dados de teste." << endl;
    ml::Labels preds = tree.predict(X_test);

    ml::Metrics relatorio = tree.evaluate(y_test, preds);
    relatorio.printReport();

    // 7. Salvando o Modelo
    //tree.save("modelo_arvore.dat");
    //cout << "\nModelo salvo com sucesso em 'modelo_arvore.dat'" << endl;

    return 0;
}