#ifndef WAVELET_UTILS_H
#define WAVELET_UTILS_H

#include <vector>
#include <string>

// Declarações das funções
std::vector<double> haar_wavelet_function(std::vector<double> signal);
std::vector<double> apply_wavelts(const std::vector<double>& signal, const unsigned int level);
size_t ajust_wavelets_length(std::vector<std::vector<double>>& caract_X);
std::vector<double> load_txt(const std::string& filepath);

#endif // WAVELET_UTILS_H