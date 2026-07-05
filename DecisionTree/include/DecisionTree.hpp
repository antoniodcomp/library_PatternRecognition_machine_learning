#pragma once
#include "DataTypes.hpp"
#include "TreeNode.hpp"
#include <memory>

namespace ml {
    class DecisionTree : public Classifier {
    public:

        DecisionTree();
        

    private:
        int max_deep;
        double gain_min;
        std::shared_ptr<TreeNode> root;

        double calculate_gini(double Labels&y);
        std::shared_ptr<TreeNode> build_tree();
        double find_best_split(const Matrix& X, const Labels& y, const Limiares& L);
        int predict_class(const std::shared_ptr<TreeNode> node, x)

    };
}