#ifndef DEF_FUNC
#define DEF_FUNC


#include"include.h"

struct disk_t{
	char side;//0 = 未設置、1 = 黒,-1 = 白
	char select;//今現在選択されているかどうか
};

struct SideCount_t {
	int Bdisk;
	int Wdisk;
	int StillPut;
};

struct POSITION{
	int x;
	int y;
};

POSITION ChangeArrayToPosition(int);

char SetPlayFirst();//todo
//If human mode was selected,this func return NULL

void DispatchWnd(HDC, HDC, RECT);//裏画面を表画面に描写　complete
void DrawField(HDC);//盤面描写 complete
void DrawDisk(HDC, HBITMAP, HBITMAP, disk_t[64]);

int ControlDisk(HDC, disk_t[64], int, WPARAM);

int PutDisk(disk_t[64], int, char);//石配列に代入
int SurchSide(disk_t[64], int, char);

enum DIRECTION{
	UP,
	DOWN,
	RIGHT,
	LEFT,
	UPRIGHT,
	UPLEFT,
	DOWNRIGHT,
	DOWNLEFT,
};

int SurchChangeAbleDisk(disk_t*,int,char,DIRECTION);

SideCount_t CountScore(disk_t[64]);
void DrawScore(SideCount_t, HBITMAP, HBITMAP, HDC, char);
char CheckGame(SideCount_t,int); //配列を調べ、勝敗判定

void ResetField(disk_t[64]);


#define WndSizeX  1000
#define WndSizeY  700

#endif

/*
{//debug
TCHAR szbuf[128];

wsprintf(szbuf, TEXT(""));
MessageBox(hWnd, szbuf, TEXT("DEBUG!"), MB_OK);
}
*/