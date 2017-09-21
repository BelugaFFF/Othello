#ifndef HEADER_AI
#define HEADER_AI

#include"include.h"

enum DIFFICULTY {
	EASY,
	NORMAL,
};

const int EvalField_Opening[64] =
{ 45, -11, 4, -1, -1, 4, -11, 45,
 -11, -16, -1, -3, -3, -1, -16, -11,
 4, -1, 2, -1, -1, 2, -1, 4,
-1, -3, -1,  0,  0, -1, -3, -1,
-1, -3, -1,  0,  0, -1, -3, -1,
4, -1,  2, -1, -1,  2, -1,  4,
-11,-16, -1, -3, -3, -1,-16,-11,
45,-11,  4, -1, -1,  4,-11, 45 };

class Othello_AI {
private:
	int answer;
	char DiskColor;

	disk_t CurrentFieldState[64];
	disk_t VirtualField[64];

private:
	void ReferCurrentField(disk_t*);
	void ResetVirtualField();
public:
	Othello_AI();
	~Othello_AI();
	int AIAnswer(disk_t*, enum DIFFICULTY,char,HWND);
};

#endif
