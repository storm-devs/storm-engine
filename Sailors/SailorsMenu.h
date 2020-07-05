
#include "Sailors.h"


class Menu{

public:
	string line[10];
	int selected;
	int blocked;
	int count;
	CVECTOR cameraAng;
	CVECTOR cameraPos;
	DWORD dltTime;

	bool keyPressed;
	int key, shiftKey;

	Sailors * sailrs;
	string _fileName_save;
	string _fileName_load;

	Menu(){
		count= 3;
		selected= 0;
		blocked= -1;
		keyPressed= false;
	};

	void Draw(VDX8RENDER * rs, SailorsPoints &sailorsPoints);
	void OnKeyPress(SailorsPoints &sailorsPoints);
	void ChangeControl(int key, SailorsPoints &sailorsPoints);

	void ChangeControl1(int key, SailorsPoints &sailorsPoints);
	void ChangeControl2(int key, SailorsPoints &sailorsPoints);
	void ChangeControl3(int key, SailorsPoints &sailorsPoints);

	void Update(SailorsPoints &sailorsPoints);
};



