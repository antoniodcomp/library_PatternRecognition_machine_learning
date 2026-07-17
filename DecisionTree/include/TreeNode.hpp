#pragma once
#include <memory>

namespace ml {

    struct TreeNode {
        int feature_idx = -1;
        double threshold = 0.0;

        int predicted_class = -1;
        bool is_leaf = false;

        std::shared_ptr<TreeNode> left = nullptr;
        std::shared_ptr<TreeNode> right = nullptr;

        TreeNode(int idx, double thresh) : feature_idx(idx), threshold(thresh), is_leaf(false) {}

        TreeNode(int pred_class) : predicted_class(pred_class), is_leaf(true) {}
    };

}
