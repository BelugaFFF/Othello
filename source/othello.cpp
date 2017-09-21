
#include"include.h"


LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);//ウィンドウプロシージャ　
ATOM InitApp(HINSTANCE);//ウィンドウクラスの登録　ATOMは整数値
BOOL InitInstance(HINSTANCE, int);//ウィンドウ生成

TCHAR szClassName[] = TEXT("source.c");
HINSTANCE hInst;

int WINAPI WinMain(
	HINSTANCE hCurInst,//カレントインスタンスハンドル
	HINSTANCE hPrevInstance,
	PSTR lpCmdLine,
	int nCmdShow) {

	MSG msg;//メッセージ構造体
	BOOL bRed;
	hInst = hCurInst;

	if (!InitApp(hCurInst))
		return FALSE;
	if (!InitInstance(hCurInst, nCmdShow))
		return FALSE;
	while ((bRed = GetMessage(&msg, NULL, 0, 0)) != 0){
		if (bRed == -1){
			break;
		}
		else{
			TranslateMessage(&msg);//仮想キーメッセージを文字メッセージに変換
			DispatchMessage(&msg);//ウィンドウプロシージャに送信

		}
	}

	return (int)msg.wParam;
}

ATOM InitApp(HINSTANCE hInst){
	WNDCLASSEX wc;
	wc.cbSize = sizeof(WNDCLASSEX);//構造体のサイズ
	wc.style = CS_HREDRAW | CS_VREDRAW;//クラスのスタイル
	wc.lpfnWndProc = WndProc;//プロシージャ名
	wc.cbClsExtra = 0;//補助メモリ
	wc.cbWndExtra = 0;//補助メモリ
	wc.hInstance = hInst;//インスタンス
	wc.hIcon = (HICON)LoadImage(//アイコン
		NULL, MAKEINTRESOURCE(IDI_APPLICATION),
		IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hCursor = (HCURSOR)LoadImage(//カーソル
		NULL, MAKEINTRESOURCE(IDC_ARROW),
		IMAGE_CURSOR, 0, 0, LR_DEFAULTSIZE | LR_SHARED);
	wc.hbrBackground =//背景ブラシ
		(HBRUSH)GetStockObject(WHITE_BRUSH);
	wc.lpszMenuName = NULL;//メニュー名
	wc.lpszClassName = szClassName;//クラス名
	wc.hIconSm = (HICON)LoadImage(//小さいアイコン
		NULL, MAKEINTRESOURCE(IDI_APPLICATION),
		IMAGE_ICON, 0, 0, LR_DEFAULTSIZE | LR_SHARED);

	return (RegisterClassEx(&wc));



}
//ウィンドウの生成
BOOL InitInstance(HINSTANCE hInst, int nCmdShow)
{
	HWND hWnd;

	hWnd = CreateWindow(szClassName,
		TEXT("ネコでもわかるwindowsプログラミング"),//タイトル

		WS_OVERLAPPEDWINDOW ^ WS_MAXIMIZEBOX ^ WS_THICKFRAME | WS_VISIBLE,
		CW_USEDEFAULT,//ウィンドウ表示位置
		CW_USEDEFAULT,
		WndSizeX,//ウィンドウ幅（ｘ）
		WndSizeY,//ウィンドウ高さ（ｙ）
		NULL,
		NULL,
		hInst,
		NULL
		);
	if (!hWnd)
		return FALSE;
	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	return TRUE;

}

LRESULT CALLBACK WndProc(HWND hWnd, UINT msg, WPARAM wp, LPARAM lp)
{
	static HDC hdc,hdc_mem;
	static HBITMAP Wdisk,Bdisk,hBmp,hBmpPrev;
	static RECT rc;
	static disk_t field[64];
	PAINTSTRUCT ps;
	BITMAP bmp_info;

	static int OldSelect = 0;
	static char DiskColor = 1;// 1 = Brack, -1 = White
	SideCount_t  SideCount;

	static Othello_AI *OthelloAI;
	static char CpuColor;//if this vallue has 0, a game mode is human mode.
	int PlayerPutFinish = 0;

	static int CheckContinueAble = 0;
	static char gameflg = 0;

	switch (msg){
	case WM_CREATE://初期化
		hdc = GetDC(hWnd);
		hdc_mem = CreateCompatibleDC(hdc);
		GetClientRect(hWnd, &rc);
		hBmp = CreateCompatibleBitmap(hdc,rc.right - rc.left,rc.bottom - rc.top);
		hBmpPrev = (HBITMAP)SelectObject(hdc_mem, hBmp);//裏画面設定

		Bdisk = LoadBitmap(hInst, TEXT("BLACKDISK"));
		Wdisk = LoadBitmap(hInst, TEXT("WHITEDISK"));//画像ロード
		GetObject(Bdisk, (int)sizeof(BITMAP), &bmp_info);

		ResetField(field);

		field[OldSelect].select = 1; //セレクト位置

		OthelloAI = new Othello_AI;

		ReleaseDC(hWnd, hdc);
		DeleteObject(hBmp);

		//CpuColor = SetPlayFirst();//todo
		CpuColor = 0;

		break;

	case WM_PAINT://fieldを参照して最終的な描写をする

		hdc = BeginPaint(hWnd, &ps);

		DrawField(hdc_mem);
		DrawDisk(hdc_mem, Bdisk, Wdisk, field);
		SideCount = CountScore(field);
		if (SideCount.StillPut == -1)
			MessageBox(NULL,
			 TEXT("Something trouble has been happened!!"),
			 TEXT("The illegal number is substitute to {StillPut}"),
			 MB_OK);
		DrawScore(SideCount, Bdisk, Wdisk, hdc_mem, DiskColor);

		DispatchWnd(hdc, hdc_mem, rc);

		if(CheckContinueAble >= 2) gameflg = CheckGame(SideCount,0);
		else gameflg = CheckGame(SideCount, 1);

		if (gameflg == 1) {
			if (MessageBox(NULL, TEXT("NEXT GAME?"), TEXT("CONTINUE"), MB_YESNO) == IDYES) {
				CheckContinueAble = 0;
				gameflg = 0;

				ResetField(field);
				field[OldSelect].select = 0;
				OldSelect = 0;
				field[OldSelect].select = 1;
				InvalidateRect(hWnd, &rc, FALSE);
			}
			else {
				PostMessage(hWnd, WM_CLOSE, 0, 0);
			}
		}

		EndPaint(hWnd,&ps);

		break;

	case WM_KEYDOWN:
		if (gameflg != 0) break;

		OldSelect = ControlDisk(hdc_mem, field, OldSelect, wp);

		if (wp == VK_ESCAPE && CpuColor != DiskColor){
			DiskColor *= -1;
			CheckContinueAble++;
			InvalidateRect(hWnd, &rc, FALSE);
			break;
		}

		if (wp == VK_SHIFT && CpuColor != DiskColor){
			if (PutDisk(field, OldSelect, DiskColor) == NULL){
				MessageBeep(MB_OK);
				break;
			}

			if (SurchSide(field, OldSelect, DiskColor) == NULL){
				field[OldSelect].side = 0;
				//やり直しの場合、field.sideの値をもとに戻す
				MessageBeep(MB_OK);
			}else{
				DiskColor = DiskColor * -1;
				CheckContinueAble = 0;
				PlayerPutFinish = 1;
			}
		}

		//debug
		if (wp == VK_SPACE) {
			int p = SurchChangeAbleDisk(field, OldSelect, DiskColor, RIGHT);
			{//debug
				TCHAR szbuf[128];

				wsprintf(szbuf, TEXT("p = %d"),p);
				MessageBox(hWnd, szbuf, TEXT("DEBUG!"), MB_OK);
			}

		}
		
		InvalidateRect(hWnd, &rc, FALSE);

		if (CpuColor != 0 && CpuColor == DiskColor && PlayerPutFinish == 1) {
			int answer;
			answer = OthelloAI->AIAnswer(field,EASY,CpuColor,hWnd);
			if (answer == 70) {
				DiskColor *= -1;
				CheckContinueAble++;
			}
			else {
				SurchSide(field, answer, DiskColor);
				DiskColor = DiskColor * -1;
				CheckContinueAble = 0;
			}
			InvalidateRect(hWnd, &rc, FALSE);
		}

		break;
//入力に応じた処理
	case WM_DESTROY:
		SelectObject(hdc_mem, hBmpPrev);
		DeleteDC(hdc_mem);

		DeleteObject(Wdisk);
		DeleteObject(Bdisk);
		DeleteObject(hBmpPrev);

		delete OthelloAI;

		PostQuitMessage(0);
		break;
	default:
		return (DefWindowProc(hWnd, msg, wp, lp));
	}
	return 0;
}
