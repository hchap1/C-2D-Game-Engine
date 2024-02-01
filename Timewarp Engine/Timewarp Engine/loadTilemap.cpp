#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TIMEWARP ENGINE\loadTilemap.h>
#include <algorithm>

std::vector<std::vector<float>> loadLevel(int levelID) {
    const std::string filename = "src/levels/" + std::to_string(levelID) + "/tilemap.txt";
    std::ifstream file(filename);
    if (!file.is_open()) {
        std::cerr << "Error opening file: " << filename << std::endl;
    }

    std::vector<std::vector<float>> tilemap;

    std::string line;
    while (std::getline(file, line)) {
        std::istringstream iss(line);
        float number;
        std::vector<float> row;
        while (iss >> number) {
            row.push_back(static_cast<float>(number / 10));
        }

        // Append row
        tilemap.push_back(row);
    }

    file.close();

    std::reverse(tilemap.begin(), tilemap.end());
        

    return tilemap;
}

int* loadLevelData(int levelID) {
    int data[4] = {};
    std::ifstream dataFile("src/levels/" + std::to_string(levelID) + "/data.txt");
    dataFile >> data[0] >> data[1] >> data[2] >> data[3];
    dataFile.close();
    return data;
}
