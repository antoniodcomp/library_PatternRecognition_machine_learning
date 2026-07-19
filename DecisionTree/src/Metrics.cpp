#include "Metrics.hpp"

namespace ml{
    double Metrics::accuracy() const {
        int total = TP + TN + FP + FN;
        if(total == 0) return 0.0;
        return static_cast<double>(TP + TN) / total;
    }

    double Metrics::precision() const{
        int total = TP + FP;
        if(total == 0) return 0.0;
        return static_cast<double>(TP) / total;
    }

    double Metrics::recall() const{
        int total = TP + FN;
        if(total == 0) return 0.0;
        return static_cast<double> (TP) / total;
    }

    double Metrics::f1_score() const{
        double prec = precision();
        double rec = recall();
        if(prec + rec == 0.0) return 0.0;
        return 2.0 * (prec * rec) / (prec + rec);
    }

    void Metrics::printReport() const {
                std::cout << "\n=========================================\n";
                std::cout << "         RELATORIO DE DESEMPENHO         \n";
                std::cout << "=========================================\n";
                std::cout << "Matriz de Confusao:\n";
                std::cout << "                 Predito: SIM(1) | Predito: NAO(0)\n";
                std::cout << "Real: SIM(1)  |      TP: " << TP << "       |      FN: " << FN << "\n";
                std::cout << "Real: NAO(0)  |      FP: " << FP << "       |      TN: " << TN << "\n";
                std::cout << "-----------------------------------------\n";
                std::cout << "Acuracia : " << accuracy() * 100.0 << " %\n";
                std::cout << "Precisao : " << precision() * 100.0 << " %  (De todas que a IA disse ser sua voz, quantas realmente eram?)\n";
                std::cout << "Recall   : " << recall() * 100.0 << " %  (De todas as suas vozes no teste, quantas a IA encontrou?)\n";
                std::cout << "F1-Score : " << f1_score() * 100.0 << " %\n";
                std::cout << "=========================================\n";
            }
}