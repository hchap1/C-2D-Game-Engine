#include <vector>
using namespace std;

class gameState {
public:
	gameState(vector<float> pXP, vector<float> pYP)
		: playerXPositions(pXP), playerYPositions(pYP) {
	}

	void addPosition() {

	}

private:
	vector<float> playerXPositions;
	vector<float> playerYPositions;
};