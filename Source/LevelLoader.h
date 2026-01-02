#pragma once
#include <string>
#include <vector>

struct LevelData {
    int rows = 0;
    int cols = 0;
    // std::vector<int> grid; // 0 = empty, 1 = filled, tùy game
    std::vector<std::vector<int>> tiles;
    std::vector<std::vector<std::string>> objects;
};

class LevelLoader {
public:
    static bool loadFromFile(const std::string& filename, LevelData& outData);
};
