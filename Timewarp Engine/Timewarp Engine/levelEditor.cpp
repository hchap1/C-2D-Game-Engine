#include <TIMEWARP ENGINE\loadTilemap.h>
#include <iostream>
#include <vector>
#include <TIMEWARP ENGINE\renderer.h>
#include <SHADER CLASS\shader.h>
#include <GLFW\glfw3.h>
#include <windows.h>
#include <fstream>

using namespace std;
float bW;
float bH;
float wW;
float wH;

void setBlockAndWindowSize(float blockX, float blockY, float windowX, float windowY) {
    bW = blockX;
    bH = blockY;
    wW = windowX;
    wH = windowY;
}

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

    vector<bool> playerCrouchingVector(false);

    rendererInit(false);
    updateTilemap(tilemap);
    Shader tile_shader = makeSimpleTileShader();
    Shader outline_shader = makeOutlineShader();

    int right = 0;
    int left = 0;
    int up = 0;
    int down = 0;

    int increase = 0;
    int decrease = 0;

    float playerX = bW * -5.5f;
    float playerY = bH * -5.5f;
    float targetBlock = 0.0f;

    while (!getKey(GLFW_KEY_ENTER)) {
        tilemapRender(playerX, playerY, tilemap, tile_shader, outline_shader);
        if (getKey(GLFW_KEY_D)) { right++; }
        else { right = 0; }
        if (getKey(GLFW_KEY_A)) { left++; }
        else { left = 0; }
        if (getKey(GLFW_KEY_W)) { up++; }
        else { up = 0; }
        if (getKey(GLFW_KEY_S)) { down++; }
        else { down = 0; }
        if (getKey(GLFW_KEY_EQUAL)) { increase++; }
        else { increase = 0; }
        if (getKey(GLFW_KEY_MINUS)) { decrease++; }
        else { decrease = 0; }

        if (up == 1) { playerY -= bH; }
        if (down == 1) { playerY += bH; }
        if (right == 1) { playerX -= bW; }
        if (left == 1) { playerX += bW; }

        //Calculate the closest X to the player [index].
        int blocksOnHalfScreenX = static_cast<int>(1 / bW);
        int indexOfFirstBlockX = static_cast<int>(playerX * (blocksOnHalfScreenX * -1));

        //Calculate the closest Y to the player [index].
        int blocksOnHalfScreenY = static_cast<int>(1 / bH);
        int indexOfFirstBlockY = static_cast<int>(playerY * (blocksOnHalfScreenY * -1));

        if (increase == 1) { targetBlock += 0.1f; }
        if (decrease == 1) { targetBlock -= 0.1f; }

        if (getKey(GLFW_KEY_SPACE)) { 
            tilemap[indexOfFirstBlockY][indexOfFirstBlockX] = targetBlock;
            updateTilemap(tilemap);
        }
    }
    string confirm;
    cout << "[SAVE] or [DELETE]" << endl << "> ";
    cin >> confirm;
    if (confirm == "save") {
        cout << "WRITING WITH WIDTH: " << width << " HEIGHT: " << height << endl;
        writeFile(tilemapPath, "write", "");
        for (vector<float> rowVector : tilemap) {
            string tempRow = "";
            for (float element : rowVector) {
                tempRow += to_string((int)(element * 10.0f));
                tempRow += " ";
            }
            tempRow.erase(tempRow.end() - 1);
            writeFile(tilemapPath, "append", tempRow);
        }
        cout << "FILE SAVED." << endl;
    }
}