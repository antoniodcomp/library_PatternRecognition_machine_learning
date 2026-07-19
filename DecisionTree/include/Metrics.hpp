#pragma once
#include <iostream>
#include <vector>
#include "DataTypes.hpp"

namespace ml {
    class Metrics{
    public:
        int TP = 0; // True Positive  (Era sua voz, e o modelo acertou)
        int TN = 0; // True Negative  (Era outra voz, e o modelo acertou)
        int FP = 0; // False Positive (Era outra voz, mas o modelo achou que era a sua)
        int FN = 0; // False Negative (Era sua voz, mas o modelo não reconheceu)

        Metrics(const Labels& y_true, const Labels& y_pred){
            for(size_t i = 0; i < y_true.size(); ++i){
                if(y_true[i] == 1 && y_pred[i] == 1) TP++;
                else if(y_true[i] == 0 && y_pred[i] == 0) TN++;
                else if(y_true[i] == 0 && y_pred[i] == 1) FP++;
                else if(y_true[i] == 1 && y_pred[i] == 0) FN++;
            }
        }

        double accuracy() const;
        double precision() const;
        double recall() const;
        double f1_score() const;
        void printReport() const;

    };

}