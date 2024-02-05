/*
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
    int data[4] = {};
    ifstream dataFile(dataPath);
    dataFile >> data[0] >> data[1] >> data[2] >> data[3];
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

bool isIllegalBlock(int blockID) {
    if (blockID == 5) { return true; }
    if (blockID == 7) { return true; }
    if (blockID == 9) { return true; }
    if (blockID == 11) { return true; }
    if (blockID == 13) { return true; }
    if (blockID == 15) { return true; }
    return false;
}

void levelMain(int levelID) {
    int width, height;
    int pX = 0;
    int pY = 0;
    string tilemapPath = "src/levels/" + to_string(levelID) + "/tilemap.txt";
    string dataPath = "src/levels/" + to_string(levelID) + "/data.txt";
    string levelDir = "src/levels/" + to_string(levelID);
    vector<vector<float>> tilemap;
    if (fileExists(tilemapPath)) { 
        cout << "LOADING LEVEL @ " << tilemapPath << endl; 
        //load the existing directory
        int* data = loadLevelData(dataPath);
        width = data[0];
        height = data[1];
        pX = data[2];
        pY = data[3];

        tilemap = loadLevel(levelID);
    }
    else { 
        cout << "CREATING LEVEL @ " << tilemapPath << endl; 
        //create a new directory
        bool success = createDirectory(stringToWideString(levelDir));
        cout << "WIDTH & HEIGHT? E.g. 10 10" << endl << "> ";
        cin >> width >> height;
        
        string midRow = "";
        for (int i = 0; i < width; i++) { 
            if (i > 0 && i < width - 1) { midRow += "0 "; }
            else { midRow += "2 "; }
        }
        string topRow = "";
        for (int i = 0; i < width; i++) {
            topRow += "1 ";
        }
        string bottomRow = "";
        for (int i = 0; i < width; i++) {
            if (i > 0 && i < width - 1) { bottomRow += "1 "; }
            else { bottomRow += "2 "; }
        }
        midRow.erase(midRow.end() - 1);
        topRow.erase(topRow.end() - 1);
        bottomRow.erase(bottomRow.end() - 1);

        writeFile(tilemapPath, "append", topRow);
        for (int i = 0; i < height - 2; i++) { writeFile(tilemapPath, "append", midRow); }
        writeFile(tilemapPath, "append", bottomRow);
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

    int speed = 30;

    glfwSwapInterval(1);

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

        std::cout << "PLAYER Y: " << playerY << " bH * height: " << height * bH * -1 << std::endl;

        if ((up == 1 || up > speed) && playerY > bH * height * -1 + bH) { playerY -= bH; }
        if ((down == 1 || down > speed) && playerY < - bH) { playerY += bH; }
        if ((right == 1 || right > speed) && playerX > bW * width *-1 + bW) { playerX -= bW; }
        if ((left == 1 || left > speed) && playerX < -bW) { playerX += bW; }

        //Calculate the closest X to the player [index].
        int blocksOnHalfScreenX = static_cast<int>(1 / bW);
        int indexOfFirstBlockX = static_cast<int>(playerX * (blocksOnHalfScreenX * -1));

        //Calculate the closest Y to the player [index].
        int blocksOnHalfScreenY = static_cast<int>(1 / bH);
        int indexOfFirstBlockY = static_cast<int>(playerY * (blocksOnHalfScreenY * -1));

        if (increase == 1) { 
            targetBlock += 0.1f; 
            if (isIllegalBlock(static_cast<int>(targetBlock * 10))) {
                cout << "SKIPPING BLOCK: " << targetBlock << endl;
                targetBlock += 0.1f;
            }
            cout << "BLOCKID: " << targetBlock << endl;
        }
        if (decrease == 1) { 
            targetBlock -= 0.1f; 
            if (isIllegalBlock(static_cast<int>(targetBlock * 10))) {
                cout << "SKIPPING BLOCK: " << targetBlock << endl;
                targetBlock -= 0.1f;
            }
            cout << "BLOCKID: " << targetBlock << endl;
        }

        if (getKey(GLFW_KEY_SPACE)) {
            tilemap[indexOfFirstBlockY][indexOfFirstBlockX] = targetBlock;
            updateTilemap(tilemap);
        }
        if (getKey(GLFW_KEY_LEFT_SHIFT)) {
            pX = indexOfFirstBlockX;
            pY = indexOfFirstBlockY;
            cout << "SET SPAWN: " << pX << ", " << pY + 1 << endl;
        }
    }
    string confirm;
    cout << "[SAVE] or [DELETE]" << endl << "> ";
    cin >> confirm;
    bool first = true;
    if (confirm == "save") {
        cout << "WRITING WITH WIDTH: " << width << " HEIGHT: " << height << endl;
        for (int i = height; i > 0; i--) {
            vector<float> rowVector = tilemap[i - 1];
            string tempRow = "";
            for (int j = 0; j < width; j++) {
                float element = rowVector[j];
                tempRow += to_string(static_cast<int>(element * 10.0f));
                tempRow += " ";
            }
            tempRow.erase(tempRow.end() - 1);
            if (!first) { writeFile(tilemapPath, "append", tempRow); }
            else { writeFile(tilemapPath, "write", tempRow); first = false; }
        }
        writeFile(dataPath, "write", to_string(width) + " " + to_string(height) + " " + to_string(pX) + " " + to_string(pY + 1));
        cout << "FILE SAVED." << endl;
    }
}
*/