#include <iostream>
#include <vector>
#include <cmath>
#include <fstream>
#include <filesystem>
#include <iomanip>
#include <algorithm>
#include <string>

using namespace std;
namespace fs = std::filesystem;

std::vector<double> haar_wavelet_function(std::vector<double> signal) {

    if(signal.size() % 2 != 0) {
        signal.push_back(signal.back());
    }

    size_t n = signal.size() / 2;
    std::vector<double> low_pass(n);

    for(size_t i = 0; i < n; i++) {
        double odd = signal[2*i];
        double even = signal[2*i + 1];

        low_pass[i] = (odd+even) / std::sqrt(2.0);
    }

    return low_pass;
}

std::vector<double> apply_wavelts(const std::vector<double>& signal, const unsigned int level) {
    std::vector<double> curr_aprox = signal;
    
    for(unsigned int i = 0; i < level; i++) {
        curr_aprox = haar_wavelet_function(curr_aprox);
        if(curr_aprox.size() < 2) break;
    }

    return curr_aprox;
}

size_t ajust_wavelets_length(std::vector<std::vector<double>>& caract_X) {
    size_t llen = 1e9;

    for(auto& s: caract_X) {
        if(s.size() < llen) llen = s.size();
    }

    for (auto& s : caract_X) {
        if (s.size() > llen) {
            s.resize(llen);
        }
    }

    return llen;
}

std::vector<double> load_txt(const std::string& filepath) {
    std::vector<double> data;
    std::ifstream file(filepath);
    double val;
    while (file >> val) {
        data.push_back(val);
    }
    return data;
}

