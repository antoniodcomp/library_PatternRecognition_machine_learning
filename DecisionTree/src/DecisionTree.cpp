#include "DecisionTree.hpp"
#include <unordered_map>

namespace ml {

    DecisionTree::DecisionTree(int max_depth, double min_gain) 
        : max_depth(max_depth), min_gain(min_gain), root(nullptr) {}


    double DecisionTree::calculate_gini(const Labels& y) {
        int total_labels = y.size();
        if(y.empty()) return 0.0;
        
        std::unordered_map<int, int> count_label;
        for(auto l : y){
            count_label[l]++;
        }

        double gini = 1.0;
        for(auto& p : count_label){
            double probability = static_cast<double>(p.second) / total_labels;
            gini -= probability * probability;
        }

        return gini;
    }


    BestSplit DecisionTree::find_best_split(const Matrix& X, const Labels& y) {
        BestSplit best;
        double impurity_pai = calculate_gini(y);

        int n = X.size();
        if(n == 0) return best;
        int dimen = X[0].size();

        for(int j = 0; j < dimen; j++){
            for(int i = 0; i < n; i++){
                double limiar = X[i][j];
                Labels y_esq, y_dir;

                for(int k = 0; k < n; k++){
                    if(X[k][j] <= limiar){
                        y_esq.push_back(y[k]);
                    }else{
                        y_dir.push_back(y[k]);
                    }
                }

                if(y_esq.empty() || y_dir.empty()) continue;

                double peso_esq = (double)y_esq.size() / n;
                double peso_dir = (double)y_dir.size() / n;

                double gain = impurity_pai - (peso_esq * calculate_gini(y_esq) + peso_dir * calculate_gini(y_dir));

                if(gain > best.gain){
                    best.gain = gain;
                    best.feature_idx = j;
                    best.threshold = limiar;
                }
            }
        }

        return best;
    }

    void DecisionTree::split_data(const Matrix& X, const Labels& y, int feature_idx, double threshold,
                                  Matrix& X_left, Matrix& X_right, Labels& y_left, Labels& y_right) {
        int n = X.size();
        if(n == 0) return;

        for (int i = 0; i < n; i++) {
            if (X[i][feature_idx] <= threshold) {
                X_left.push_back(X[i]);
                y_left.push_back(y[i]);
            } else {
                X_right.push_back(X[i]);
                y_right.push_back(y[i]);
            }
        }
    }

    int DecisionTree::calculate_majority_vote(const Labels& y) {
        std::unordered_map<int, int> count;
        int maior_classe = -1;
        int classe_escolhida = -1;

        for(auto l : y){
            count[l]++;
            if(count[l] > maior_classe){
                maior_classe = count[l];
                classe_escolhida = l;
            }
        }

        return classe_escolhida;
    }

    std::shared_ptr<TreeNode> DecisionTree::build_tree(const Matrix& X, const Labels& y, int depth) {
        if(depth >= max_depth || y.empty()){
            int l = calculate_majority_vote(y);
            return std::make_shared<TreeNode>(l);
        }

        BestSplit best = find_best_split(X, y);

        if(best.gain <= min_gain){
            int l = calculate_majority_vote(y);
            return std::make_shared<TreeNode>(l);
        }

        Matrix X_dir, X_esq;
        Labels y_dir, y_esq;

        split_data(X, y, best.feature_idx, best.threshold, X_esq, X_dir, y_esq, y_dir);

        auto node = std::make_shared<TreeNode>(best.feature_idx, best.threshold);
        node->left = build_tree(X_esq, y_esq, depth + 1);
        node->right = build_tree(X_dir, y_dir, depth + 1);

        return node;
    }

    int DecisionTree::predict_class(const std::shared_ptr<TreeNode>& node, const std::vector<double>& x) const {
        if(node->is_leaf) return node->predicted_class;

        if(x[node->feature_idx] <= node->threshold){
            return predict_class(node->left, x);
        }
        return predict_class(node->right, x);
    }

    void DecisionTree::fit(const Matrix& X, const Labels& y) {
        this->root = build_tree(X, y, 0);
    }

    Labels DecisionTree::predict(const Matrix& X) const {
        Labels preds;
        for(const auto& x : X) {
            preds.push_back(predict_class(this->root, x));
        }
        return preds;
    }
}
