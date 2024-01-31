#include <TIMEWARP ENGINE\gameLoop.h>
#include <TIMEWARP ENGINE\levelEditor.h>
#include <string>
#include <iostream>
#include <locale>
using namespace std;

int main() {
	string userInput;
	int levelID;

	cout << "[L]evel editor, [P]lay level?" << endl << "> ";
	cin >> userInput;

	cout << "Enter level ID." << endl << "> ";
	cin >> levelID;
	
	char choiceChar = (char)tolower(userInput[0]);
	string choice(1, choiceChar);
	if (choice == "p") {
		gameMain(levelID);
	}
	else if (choice == "l") {
		levelMain(levelID);
	}
}