#include"include.h"


Othello_AI::Othello_AI() {
	
}
Othello_AI::~Othello_AI() {

}

int Othello_AI::AIAnswer(disk_t *field,enum DIFFICULTY difficluty,
	char CpuColor,HWND hWnd){
	DiskColor = CpuColor;
	ReferCurrentField(field);

	int TurnAble[64];
	int count = 0;

	srand((unsigned)time(NULL));
	TurnAble[0] = 70;

	if (difficluty == EASY) {
		for(int i = 0;i < 64;i++){
			ResetVirtualField();
			 
			if (SurchSide(VirtualField, i, DiskColor) != NULL) {
				{//debug
					TCHAR szbuf[128];

					wsprintf(szbuf, TEXT("hello?AI"));
					MessageBox(hWnd, szbuf, TEXT("DEBUG!"), MB_OK);
				}//‚±‚±‚Ü‚Å—ˆ‚Ä‚È‚¢

				TurnAble[count] = i;
				count++;
			}
		}

		if (count > 1)answer = TurnAble[rand() % count];
		else answer = TurnAble[0];
	}
	else if (difficluty == NORMAL) {
		answer = 71;//error
	}

	return answer;
}

void Othello_AI::ReferCurrentField(disk_t *field) {
	for (int i = 0; i < 64; i++) {
		CurrentFieldState[i] = field[i];
	}
}

void Othello_AI::ResetVirtualField() {
	for (int i = 0; i < 64; i++) {
		VirtualField[i] = CurrentFieldState[i];
	}
}