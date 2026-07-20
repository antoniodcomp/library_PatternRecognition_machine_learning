#ifndef DTW_UTILS_H
#define DTW_UTILS_H

#include <vector>
#include <string>
#include <filesystem>

namespace fs = std::filesystem;

struct DTWResult {
    std::vector<std::vector<double>> dtw;
    double distance;
};

// Declarações das funções
DTWResult fast_DTW_distance(const std::vector<double>& audio1, const std::vector<double>& audio2, int window = 10);
int select_reference_audio(const std::vector<std::vector<double>>& matrix_voices);
void get_labels(const fs::path& path);
std::vector<double> audio_aligner(const std::vector<double>& audio1, const std::vector<double>& audio2, int window = 10);
std::vector<std::vector<double>> load_matrix(const std::string& filepath);

#endif // DTW_UTILS_H