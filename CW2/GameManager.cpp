#include <iostream>
#include <cstdlib>

enum gameState {
	Searching,
	Solved,
};

class GameManager {

public:
	gameState state = Searching;

	bool lightToggles[4] = { false,false,false,true };

	void puzzleCheck() {
		//std::cout << "Checking puzzle" << std::endl;
		bool puzzleTrue = true;
		for (int i = 0;i < 4;i++) {
			if (lightToggles[i] != puzzleAnswer[i]) {
				puzzleTrue = false;
				break;
			}
		}

		if (puzzleTrue) {
			//std::cout << "puzzle match" << std::endl;
			state = Solved;
		}
	}

private:
	bool puzzleAnswer[4] = { true,true,false,true };

};

