#pragma once
#include <vector>

namespace ml {

    using FeatureType = double;
    
    // Rótulos de classificação (ex: 0, 1, 2)
    using LabelType = int;
    using Labels = std::vector<LabelType>;

    using Matrix = std::vector<std::vector<FeatureType>>;

}