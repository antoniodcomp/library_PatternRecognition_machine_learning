#pragma once
#include "DataTypes.hpp"
#include "TreeNode.hpp"
#include <memory>
#include <vector>

namespace ml {

    struct BestSplit {
        int feature_idx = -1;
        double threshold = 0.0;
        double gain = -1.0;
    };

    class Classifier {
    public:
        virtual ~Classifier() = default;
        virtual void fit(const Matrix& X, const Labels& y) = 0;
        virtual Labels predict(const Matrix& X) const = 0;
    };

    class DecisionTree : public Classifier {
    public:
        DecisionTree(int max_depth = 10, double min_gain = 1e-7);
        void fit(const Matrix& X, const Labels& y) override;
        Labels predict(const Matrix& X) const override;

    private:
        int max_depth;
        double min_gain;
        std::shared_ptr<TreeNode> root;

        double calculate_gini(const Labels& y);
        int calculate_majority_vote(const Labels& y);
        BestSplit find_best_split(const Matrix& X, const Labels& y);
        void split_data(const Matrix& X, const Labels& y, int feature_idx, double threshold,
                        Matrix& X_left, Matrix& X_right, Labels& y_left, Labels& y_right);
        std::shared_ptr<TreeNode> build_tree(const Matrix& X, const Labels& y, int depth);

        int predict_class(const std::shared_ptr<TreeNode>& node, const std::vector<double>& x) const;
    };

}