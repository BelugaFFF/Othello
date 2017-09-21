#include"include.h"

/*
char SetPlayFirst() {//todo

}
*/

POSITION ChangeArrayToPosition(int position) {
	int x, y;
	POSITION p;
	if (position > 0 && position < 63) {
		y = (position  / 8) + 1;
		x = (position  % 8) + 1;
	}
	else if (position == 0) { y = 1; x = 1; }
	else { x = 8; y = 8; }

	p.x = x;
	p.y = y;

	return p;
}

void  DispatchWnd(HDC hdc, HDC hdc_mem, RECT rc) {
	BitBlt(hdc, 0, 0, rc.right - rc.left, rc.bottom - rc.top, hdc_mem, 0, 0, SRCCOPY);
}
//裏画面を表画面に転送 complete

void DrawField(HDC hdc_mem) {
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;
	int i;

	hPen = CreatePen(PS_SOLID, 2, RGB(0, 0, 0));//黒
	hOldPen = (HPEN)SelectObject(hdc_mem, hPen);
	hBrush = CreateSolidBrush(RGB(0, 255, 0));//緑
	hOldBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);

	Rectangle(hdc_mem, 100, 100, 612, 612);
	for (i = 1; i <= 7; i++) {
		MoveToEx(hdc_mem, (512 / 8)*i + 100, 100, NULL);
		LineTo(hdc_mem, (512 / 8)*i + 100, 612);
		MoveToEx(hdc_mem, 100, (512 / 8)*i + 100, NULL);
		LineTo(hdc_mem, 612, (512 / 8)*i + 100);
	}//500*500pxの盤面を描写

	 //todo 操作方法の説明文、黒番か白番かを表示

	DeleteObject(hPen);

	SelectObject(hdc_mem, hOldPen);
	SelectObject(hdc_mem, hOldBrush);
}
//盤面描写 complete

void DrawDisk(HDC hdc_mem, HBITMAP Bdisk, HBITMAP Wdisk, disk_t field[64]) {
	HDC hdc_bmp;
	BITMAP bmp_info;
	hdc_bmp = CreateCompatibleDC(hdc_mem);
	GetObject(Bdisk, (int)sizeof(BITMAP), &bmp_info);

	for (int y = 0; y <= 7; y++) {
		for (int x = 0; x <= 7; x++) {
			if (field[y * 8 + x].side == 1) {
				SelectObject(hdc_bmp, Bdisk);

				TransparentBlt(
					hdc_mem,
					100 + (x * 64),
					100 + (y * 64),
					bmp_info.bmWidth,
					bmp_info.bmHeight,
					hdc_bmp,
					0,
					0,
					bmp_info.bmWidth,
					bmp_info.bmHeight,
					RGB(255, 255, 254)
				);

			}
			if (field[y * 8 + x].side == -1) {
				SelectObject(hdc_bmp, Wdisk);

				TransparentBlt(
					hdc_mem,
					100 + (x * 64),
					100 + (y * 64),
					bmp_info.bmWidth,
					bmp_info.bmHeight,
					hdc_bmp,
					0,
					0,
					bmp_info.bmWidth,
					bmp_info.bmHeight,
					RGB(255, 255, 254)
				);

			}
			if (field[y * 8 + x].select == 1) {
				HPEN hPen, hOldPen;
				HBRUSH hBrush, hOldBrush;

				hPen = CreatePen(PS_DOT, 2, RGB(255, 0, 0));// (4/7)太さを3異常にすると、盤面の縁に赤い線が残るバグを確認
				hOldPen = (HPEN)SelectObject(hdc_mem, hPen);
				hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
				hOldBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);

				Rectangle(hdc_mem,
					100 + (x * 64),
					100 + (y * 64),
					100 + (x * 64) + bmp_info.bmWidth,
					100 + (y * 64) + bmp_info.bmHeight);
				DeleteObject(hPen);
				DeleteObject(hBrush);
				SelectObject(hdc_mem, hOldPen);
				SelectObject(hdc_mem, hOldBrush);
			}
		}
	}


	DeleteDC(hdc_bmp);
}
//石描写　complete

int ControlDisk(HDC hdc_mem,
	disk_t field[64],
	int OldSelect,/*前のselectフラグが立っている配列の番号*/
	WPARAM direction) {

	switch (direction) {
	case VK_UP:
		if (OldSelect >= 8) {
			field[OldSelect].select = 0;
			OldSelect -= 8;
			field[OldSelect].select = 1;
		}
		else {
			MessageBeep(MB_OK);
		}
		break;
	case VK_DOWN:
		if (OldSelect <= 55) {
			field[OldSelect].select = 0;
			OldSelect += 8;
			field[OldSelect].select = 1;
		}
		else {
			MessageBeep(MB_OK);
		}
		break;
	case VK_RIGHT:
		if (OldSelect < 63) {
			field[OldSelect].select = 0;
			OldSelect += 1;
			field[OldSelect].select = 1;
		}
		else {
			MessageBeep(MB_OK);
		}
		break;
	case VK_LEFT:
		if (OldSelect > 0) {
			field[OldSelect].select = 0;
			OldSelect -= 1;
			field[OldSelect].select = 1;
		}
		else {
			MessageBeep(MB_OK);
		}
		break;
	case VK_SHIFT:
		break;
	default:
		MessageBeep(MB_OK);
		break;
	}

	return OldSelect;
}
//カーソル動かす　complete 
int PutDisk(disk_t field[64], int OldSelect, char Player) {
	if (field[OldSelect].side != 0) {
		return NULL;
	}
	else {
		if (Player == 1) {
			field[OldSelect].side = 1;
		}
		if (Player == -1) {
			field[OldSelect].side = -1;
		}
	}

	return 1;
}

int SurchChangeAbleDisk(disk_t *Array, int CurrentPosition, 
	char DiskColor,DIRECTION Direction) {
	int x, y;
	int updateX, updateY;

	POSITION p;
	p = ChangeArrayToPosition(CurrentPosition);

	switch (Direction) {
	case UP:
		updateX = 0;
		updateY = -1;
		break;
	case DOWN:
		updateX = 0;
		updateY = 1;
		break;
	case RIGHT:
		updateX = 1;
		updateY = 0;
		break;
	case LEFT:
		updateX = -1;
		updateY = 0;
		break;
	case UPRIGHT:
		updateX = 1;
		updateY = -1;
		break;
	case UPLEFT:
		updateX = -1;
		updateY = -1;
		break;
	case DOWNRIGHT:
		updateX = 1;
		updateY = 1;
		break;
	case DOWNLEFT:
		updateX = -1;
		updateY = 1;
		break;
	default:
		return -1;
	}

	x = p.x; y = p.y;
	x += updateX;
	y += updateY;

	if (Array[(x + (8 * (y - 1)))].side == (DiskColor * -1)) return NULL;
	if (Array[(x + (8 * (y - 1)))].side == 0) return NULL;
	//直近のマスが空か、自分の石があるかを判定

	for(x = p.x, y = p.y;
		p.x < 9 && p.x > 0 &&
		p.y < 9 && p.y > 0;
		x += updateX, y += updateY){
		if (Array[(x + (8 * (y - 1)))].side == DiskColor) return 1;
		if (Array[(x + (8 * (y - 1)))].side == 0) return NULL;
	}

	return NULL;
}


int SurchSide(disk_t Array[64], int CurNum, char Player) {
	int CurX = CurNum / 8, CurY = CurNum % 8;//xは行、yは列を表す。PC画面のxｙとは逆
	int b[7] = { 0, 0, 0, 0, 0, 0, 0, };//ひっくり返す石の場所を記録
	int i;
	int result = NULL;

	if (CurX > 1 && Array[8 * CurX + CurY - 8].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX - 1, y = CurY; x >= 0; x--) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++) 
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//上検索 成功
	}

	if (CurX < 6 && Array[8 * CurX + CurY + 8].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX + 1, y = CurY; x <= 7; x++) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++) 
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//下検索 成功
	}

	if (CurY < 6 && Array[8 * CurX + CurY + 1].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX, y = CurY + 1; y <= 7; y++) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++)
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//右検索 成功
	}

	if (CurY > 1 && Array[8 * CurX + CurY - 1].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX, y = CurY - 1; y >= 0; y--) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++)
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//左検索 成功
	}

	if (CurX > 1 && CurY > 1 && Array[8 * CurX + CurY - 9].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX - 1, y = CurY - 1; x >= 0 || y >= 0; x--, y--) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++) 
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//左上検索 成功
	}

	if (CurX < 6 && CurY > 1 && Array[8 * CurX + CurY + 7].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX + 1, y = CurY - 1; x <= 7 || y >= 0; x++, y--) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++) 
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//左下検索 成功
	}

	if (CurX > 1 && CurY < 6 && Array[8 * CurX + CurY - 7].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX - 1, y = CurY + 1; x >= 0 || y <= 7; x--, y++) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++) 
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//右上検索 成功
	}

	if (CurX < 6 && CurY < 6 && Array[8 * CurX + CurY + 9].side == (Player * -1)) {
		//検索方向が存在し、相手の石がある事を確認
		i = 0;

		for (int x = CurX + 1, y = CurY + 1; x <= 7 || y <= 7; x++, y++) {
			b[i] = 8 * x + y;

			if (Array[8 * x + y].side == 0) break;
			if (Array[8 * x + y].side == Player) {
				for (int j = 0; j <= (i - 1); j++) 
					Array[b[j]].side = Array[b[j]].side * -1;
				result = 1;
				break;
			}

			i++;
		}//右下検索 成功
	}

	return result;
}
//反転　complete

SideCount_t CountScore(disk_t field[64]) {
	SideCount_t diskside{ 0, 0, 0 };

	for (int i = 0; i <= 63; i++) {
		if (field[i].side == 1) {
			diskside.Bdisk++;
		}
		else if (field[i].side == -1) {
			diskside.Wdisk++;
		}
		else if (field[i].side == 0) {
			diskside.StillPut++;
		}
		else {
			diskside.StillPut = -1;
			return diskside;
		}
	}

	return diskside;
}

void DrawScore(SideCount_t sidedisk, HBITMAP Bdisk, HBITMAP Wdisk, HDC hdc_mem, char Player) {
	HDC hdc_bmp;
	BITMAP bmp_info;
	HPEN hPen, hOldPen;
	HBRUSH hBrush, hOldBrush;

	TCHAR str_Title[128], str_Bdisk[128], str_Wdisk[128];

	hdc_bmp = CreateCompatibleDC(hdc_mem);
	GetObject(Bdisk, (int)sizeof(BITMAP), &bmp_info);

	SelectObject(hdc_bmp, Bdisk);

	TransparentBlt(
		hdc_mem,
		700,
		100 + 50,
		bmp_info.bmWidth,
		bmp_info.bmHeight,
		hdc_bmp,
		0,
		0,
		bmp_info.bmWidth,
		bmp_info.bmHeight,
		RGB(255, 255, 0)/*This mean,don't use*/
	);

	SelectObject(hdc_bmp, Wdisk);

	TransparentBlt(
		hdc_mem,
		700,
		100 + 50 + 70,
		bmp_info.bmWidth,
		bmp_info.bmHeight,
		hdc_bmp,
		0,
		0,
		bmp_info.bmWidth,
		bmp_info.bmHeight,
		RGB(255, 255, 0)/*This mean,don't use*/
	);

	hPen = CreatePen(PS_DOT, 2, RGB(255, 0, 0));
	hOldPen = (HPEN)SelectObject(hdc_mem, hPen);
	hBrush = (HBRUSH)GetStockObject(NULL_BRUSH);
	hOldBrush = (HBRUSH)SelectObject(hdc_mem, hBrush);

	if (Player == 1) {
		Rectangle(hdc_mem, 700 + 1, 100 + 50 + 1, 700 + 64, 100 + 50 + 64);
	}
	else if (Player == -1) {
		Rectangle(hdc_mem, 700 + 1, 100 + 50 + 70 + 1, 700 + 64, 100 + 50 + 70 + 64);
	}

	wsprintf(str_Title, TEXT("SCORE"));
	wsprintf(str_Bdisk, TEXT("%d"), sidedisk.Bdisk);
	wsprintf(str_Wdisk, TEXT("%d"), sidedisk.Wdisk);

	SetTextColor(hdc_mem, RGB(0, 0, 0));
	TextOut(hdc_mem, 700, 100, str_Title, lstrlen(str_Title));
	TextOut(hdc_mem, 700 + 70, 200, str_Bdisk, lstrlen(str_Bdisk));
	TextOut(hdc_mem, 700 + 70, 220 + 60, str_Wdisk, lstrlen(str_Wdisk));

	DeleteDC(hdc_bmp);
	DeleteObject(hPen);
	DeleteObject(hBrush);
	SelectObject(hdc_mem, hOldPen);
	SelectObject(hdc_mem, hOldBrush);
}

char CheckGame(SideCount_t SideDisk,int ContinueAble) {
	if (SideDisk.StillPut == 0 || ContinueAble == 0) {
		if (SideDisk.Bdisk > SideDisk.Wdisk)
			MessageBox(NULL, TEXT("BLACK WIN!!"), TEXT("GAME!!"), MB_OK);
		else if (SideDisk.Bdisk < SideDisk.Wdisk)
			MessageBox(NULL, TEXT("WHITE WIN!!"), TEXT("GAME!!"), MB_OK);
		else if (SideDisk.Bdisk == SideDisk.Wdisk)
			MessageBox(NULL, TEXT("DRAW,,,"), TEXT("GAME!!"), MB_OK);

		return 1;
	}

	return 0;
}

void ResetField(disk_t field[64]) {
	for (int i = 0; i <= 63; i++) {
		field[i].side = 0;
		field[i].select = 0;
	}

	field[8 * 3 + 4 - 1].side = 1;
	field[8 * 3 + 5 - 1].side = -1;
	field[8 * 4 + 4 - 1].side = -1;
	field[8 * 4 + 5 - 1].side = 1;//初期配置
}