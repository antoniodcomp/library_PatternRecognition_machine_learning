#include "../include/DecisionTree.hpp"
#include <unordered_map>

namespace ml{

    DecisionTree::DecisionTree(int max_depth, double min_gain) 
        : max_deep(max_depth), gain_min(min_gain), root(nullptr) {}


    double DecisionTree::calculate_gini(const Labels&y){
    int total_labels = y.size();
        if(y.empty()) return 0.0;
        
        std::unordered_map<int, int> count_label;
        for(auto& l: y){
            count_label[l.id]++;
        }

        double gini = 1.0;

        for(auto& p: count_label){
            double probability = static_cast<double>(p.second) / total_labels;
            gini -= probability * probability;
        }

        return gini;
    }


    BestSplit DecisionTree::find_best_split(const Matrix& X, const Labels& y, const Labels& L){
        BestSplit best;
        double impurity_pai = calculate_gini(y);

        int n = X.size();
        if(n == 0) return best;
        int dimen = X[0].size();


        for(int j = 0; j < dimen; j++){
            for(int i = 0; i < n; i++){
                double limiar = X[i][j].id;
                Labels y_esq, y_dir;

                for(int k = 0; k < n; k++){
                    if(X[k][j].id <= limiar){
                        y_esq.push_back(y[k]);
                    }else{
                        y_dir.push_back(y[k]);
                    }
                }

                if(y_esq.empty() || y_dir.empty()) continue;


                double peso_esq = (double)y_esq.size()/n;
                double peso_dir = (double)y_dir.size()/n;

                double gain = impurity_pai - (peso_esq*calculate_gini(y_esq) + peso_dir*calculate_gini(y_dir));

                if(gain > best.gain){
                    best.gain = gain;
                    best.caract_idx = j;
                    best.thr = limiar;
                }
            }
        }

        return best;
    }

    void DecisionTree::split_data(const Matrix& X, const Matrix& X_dir, const Matrix& X_esq, const Labels& y_dir, const Labels& y_esq ,double limiar){
        int n = X.size();
        if(n == 0) return best;
        int dimen = X[0].size();


        for(int i=0; i < dimen; i++){
            for (int i = 0; i < n; i++){
                for (int k = 0; k < n; k++) {
                    if (X[k][j] <= threshold) {
                        X_esq.push_back(X[k][j]);
                        y_esq.push_back(y[k]);
                    } else {
                        X_dir.push_back(X[k][j]);
                        y_dir.push_back(y[k]);
                    }
                }
            }
        }

    }

    int DecisionTree::calculate_majority_vote(const Labels& y){
        std::unordered_map<int, int> count;
        int maior_classe = -1;
        int clase_escolhida = -1;

        for(auto& l: y){
            count[l]++;
            if(count[l] > maior_classe){
                maior_classe = count[l];
                classe_escolhida = l;
            }
        }

        return l;
    }

    std::shared_ptr<TreeNode> DecisionTree::build_tree(const Matrix& X, const Labels&y, int depth){
        std::shared_ptr<TreeNode> node;
        if(depth >= max_deep || y.empty()){
            int l = calculate_majority_vote(y);
            return std::make_shared<TreeNode>(l);
        }

        BestSplit g = find_best_split(X, y);

        if(g.gain <= beta){
            int l = calculate_majority_vote(y);
            return std::make_shared<TreeNode>(l);
        }

        Matrix X_dir, X_esq;
        Labels y_dir, y_esq;

        split_dataset(X_dir, X_esq, y_dir, y_esq, best.threshold);

        node->left = build_tree(X_esq, current_depth + 1);
        node->right = build_tree(X_dir, current_depth + 1);

        auto node = std:make_shared<TreeNode>(best.caract_idx, best.threshold);

        return node;

    }

    int DecisionTree::predict_class(std::shared_ptr<TreeNode> node, const std::vector<double>& x){
        if(node->is_leaf) return node->predict_class;

        if(x[node->caract_idx] <= node->threshold){
            predict_class(node->left, x)
        }
    }

    void DecisionTree::fit(const Matrix& X, const Labels&y){
        this->root = build_tree(X, y, 0)
    }
}
