#pragma once
#include "DataTypes.hpp"

namespace ml{

    using std::vector<std::vector<Data>> Matrix;
    using std::vector<Data> Labels;


    struct Data{
        int id;
    }

}

struct node{
    Data key;
    int heigth, size;
    shared_pointer leff;
    shared_pointer rigth;

    node(Data key){
        key = key;
        left = rigth = nullptr;
        heigth = size = 1
    }
}

struct 