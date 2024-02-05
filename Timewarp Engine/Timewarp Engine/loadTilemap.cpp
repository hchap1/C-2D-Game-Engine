#include <iostream>
#include <fstream>
#include <sstream>
#include <vector>
#include <TIMEWARP ENGINE\loadTilemap.h>
#include <algorithm>

std::vector<std::vector<int>> loadTilemap(int levelID) {
    const std::string filename = "src/levels/" + std::to_string(levelID) + "/tilemap.txt";
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
