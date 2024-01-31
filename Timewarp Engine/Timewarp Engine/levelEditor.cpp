#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <GLFW\glfw3.h>
#include <windows.h>
#include <fstream>

using namespace std;

wstring stringToWideString(const string& narrowString) {
    int wideStringLength = MultiByteToWideChar(CP_ACP, 0, narrowString.c_str(), -1, nullptr, 0);
    vector<wchar_t> buffer(wideStringLength);
    MultiByteToWideChar(CP_ACP, 0, narrowString.c_str(), -1, buffer.data(), wideStringLength);
    return wstring(buffer.data());
}

bool fileExists(const string& filename) {
    wstring wideFilename = stringToWideString(filename);
    DWORD attributes = GetFileAttributesW(wideFilename.c_str());
    return (attributes != INVALID_FILE_ATTRIBUTES && !(attributes & FILE_ATTRIBUTE_DIRECTORY));
}

bool createDirectory(const wstring& directoryName) {
    if (CreateDirectoryW(directoryName.c_str(), NULL) || GetLastError() == ERROR_ALREADY_EXISTS) {
        return true;
    }
    else {
        return false;
    }
}

int* loadLevelData(string dataPath) {
    int data[2] = {};
    ifstream dataFile(dataPath);
    dataFile >> data[0] >> data[1];
    dataFile.close();
    return data;
}

void writeFile(string filePath, string mode, string content) {
    if (mode == "write") {
        ofstream file;
        file.open(filePath);
        file << content << endl;
        file.close();
    }

    else {
        ofstream file;
        file.open(filePath, ios::app);
        file << content << endl;
        file.close();
    }
}

void levelMain(int levelID) {
    int width, height;
    string tilemapPath = "src/levels/" + to_string(levelID) + "/tilemap.txt";
    string dataPath = "src/levels/" + to_string(levelID) + "/data.txt";
    string levelDir = "src/levels/" + to_string(levelID);
    vector<vector<float>> tilemap;
    if (fileExists(tilemapPath)) { 
        cout << "LEVEL EXISTS @ " << tilemapPath << endl; 
        //load the existing directory
        int* data = loadLevelData(dataPath);
        width = data[0];
        height = data[1];
        tilemap = loadLevel(levelID);
    }
    else { 
        cout << "LEVEL DOES NOT EXIST @ " << tilemapPath << endl; 
        //create a new directory
        bool success = createDirectory(stringToWideString(levelDir));
        cout << "WIDTH & HEIGHT? E.g. 10 10" << endl << "> ";
        cin >> width >> height;
        
        string row = "";
        for (int i = 0; i < width; i++) { row += "0 "; }
        row.erase(row.end() - 1);
        for (int i = 0; i < width; i++) { writeFile(tilemapPath, "append", row); }
        writeFile(dataPath, "write", to_string(width) + " " + to_string(height));
        tilemap = loadLevel(levelID);
    }

    float playerX = -1.0f;
    float playerY = -3.5f;

    vector<bool> playerCrouchingVector(false);

    rendererInit();
    updateTilemap(tilemap);
    Shader tile_shader = makeSimpleTileShader();

    int right = 0;
    int left = 0;
    int up = 0;
    int down = 0;

    while (true) {
        tilemapRender(playerX, playerY, tilemap, tile_shader);
        if (getKey(GLFW_KEY_RIGHT)) { right++; }
        else { right = 0; }
    }
}