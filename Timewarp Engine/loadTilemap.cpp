#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TIMEWARP ENGINE\loadTilemap.h>

std::vector<std::vector<int>> loadLevel(int levelID) {
    const std::string filename = "levels/" + std::to_string(levelID) + "/tilemap.txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    std::vector<std::vector<int>> tilemap;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        int number;
        std::vector<int> row;
        while (iss >> number) {
            row.push_back(number);
        }

        // Append row
        tilemap.push_back(row);
    }

    file.close();

    return tilemap;
}
