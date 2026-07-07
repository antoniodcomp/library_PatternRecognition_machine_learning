#pragma once
#include <vector>

namespace ml{
    
    struct Data{
        int id;
    };

    using Matrix = std::vector<std::vector<Data>>;
    using Labels = std::vector<Data>;
}
