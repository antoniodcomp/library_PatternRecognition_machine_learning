#pragma once
#include <memory>


namespace ml{

    struct TreeNode{

        int caracat_idx= -1;
        double limiar = 0.0;

        int predict_class = -1
        bool is_leaf = false;

        std::shared_ptr<TreeNode> left = nullptr;
        std::shared_ptr<TreeNode> right = nullptr;

        TreeNode(int idx, double threshold) : caracat_idx(idx), threshold(threshold), is_leaf(false){}

        TreeNode(int predicted_class) : predict_class(predicted_class), is_leaf(true){}
    };

    struct BestSplit {
        int feature_idx = -1;
        double threshold = 0.0;
        double gain = -1.0;
    };
}

