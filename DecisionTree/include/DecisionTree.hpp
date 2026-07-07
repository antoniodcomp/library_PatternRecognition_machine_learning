#pragma once
#include "DataTypes.hpp"
#include "TreeNode.hpp"
#include <memory>

namespace ml {
    class DecisionTree {
        public:
            DecisionTree(int max_depth, double min_gain);
            
        private:
            int max_deep;
            double gain_min;
            std::shared_ptr<TreeNode> root;

            double calculate_gini(const Labels& y);
            std::shared_ptr<TreeNode> build_tree();
            BestSplit find_best_split(const Matrix& X, const Labels& y, const Labels& L);
            int predict_class(const std::shared_ptr<TreeNode> node, const Data& x);

    };
}