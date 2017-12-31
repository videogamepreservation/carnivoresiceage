#define INITGUID
#include "Hunt.h"
#include "stdio.h"

typedef struct _TMenuSet {
	int x0, y0;
	int Count;
	char Item[32][32];
} TMenuSet;


TMenuSet Options[3];
char HMLtxt[3][12];
char CKtxt[2][16];
char Restxt[8][24];
char Textxt[3][12];
char Ontxt[2][12];
char Systxt[2][12];
int CurPlayer = -1;
int MaxDino, AreaMax, LoadCount;

#define REGLISTX 320
#define REGLISTY 370

BOOL NEWPLAYER = FALSE;

int  MapVKKey(int k)
{
	if (k==VK_LBUTTON) return 124;
	if (k==VK_RBUTTON) return 125;
	return MapVirtualKey(k , 0);
}




POINT p;
int OptMode = 0;
int OptLine = 0;


void wait_mouse_release()
{
    while (GetAsyncKeyState(VK_RBUTTON) & 0x80000000);
	while (GetAsyncKeyState(VK_LBUTTON) & 0x80000000);
		
}


int GetTextW(HDC hdc, LPSTR s)
{
  SIZE sz;
  GetTextExtentPoint(hdc, s, strlen(s), &sz);
  return sz.cx;
}

void PrintText(LPSTR s, int x, int y, int rgb)
{
  HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcCMain,hbmpVideoBuf);   
  SetBkMode(hdcCMain, TRANSPARENT);     
   
  SetTextColor(hdcCMain, 0x00000000);  
  TextOut(hdcCMain, x+1, y+1, s, strlen(s));
  SetTextColor(hdcCMain, rgb);
  TextOut(hdcCMain, x, y, s, strlen(s));

  SelectObject(hdcCMain,hbmpOld);		  
}


void DoHalt(LPSTR Mess)
{  
  AudioStop();
  
  if (strlen(Mess)) {
	PrintLog("ABNORMAL_HALT: ");
	PrintLog(Mess);
	PrintLog("\n");
	ShutDown3DHardware();  
    Audio_Shutdown();
    EnableWindow(hwndMain, FALSE);
    MessageBox(NULL, Mess, "Carnivores Termination", IDOK | MB_SYSTEMMODAL | MB_ICONEXCLAMATION);
	CloseLog();
    TerminateProcess(GetCurrentProcess(), 0);
  } else QUITMODE=1;
  
}


void WaitRetrace()
{
    BOOL bv = FALSE;
    if (DirectActive)
      while (!bv)  lpDD->GetVerticalBlankStatus(&bv);
}



void Wait(int time)
{
  unsigned int t = timeGetTime() + time;
  while (t>timeGetTime()) ;
}


//#define loadww 210
//#define loadwh 106
char loadtxt[128];
TPicture LoadWall;

void UpdateLoadingWindow()
{

    HBITMAP hbmpOld = (HBITMAP)SelectObject(hdcCMain, hbmpVideoBuf);
    HFONT   hfntOld = (HFONT)SelectObject(hdcCMain, fnt_Small);


	for (int y=0; y<LoadWall.H/2; y++) 
		memcpy( (WORD*)lpVideoBuf + y*1024,
		        LoadWall.lpImage  + y*LoadWall.W,
				LoadWall.W*2);

	if (LoadCount)
	for (int y=0; y<LoadWall.H/2; y++) 
		memcpy( (WORD*)lpVideoBuf + y*1024,
		        LoadWall.lpImage  + (y+LoadWall.H/2)*LoadWall.W,
				(LoadWall.W*LoadCount/8)*2);

    //FillMemory(lpVideoBuf, 1024*loadwh*2, 1);
/*
    SetBkMode(hdcCMain, TRANSPARENT);     
	SetTextColor(hdcCMain, 0x000000);
    TextOut(hdcCMain, 19, LoadWall.H/2-22, loadtxt, strlen(loadtxt) );
	SetTextColor(hdcCMain, 0xB0B070);
    TextOut(hdcCMain, 18, LoadWall.H/2-23, loadtxt, strlen(loadtxt) );
*/
    BitBlt(hdcMain,0,0,LoadWall.W,LoadWall.H/2, hdcCMain,0,0, SRCCOPY);
    
    SelectObject(hdcCMain,hfntOld);
    SelectObject(hdcCMain,hbmpOld);    
}





void StartLoading()
{
  LoadPictureTGA(LoadWall,   "HUNTDAT\\MENU\\loading.tga");
  SetWindowPos(hwndMain, HWND_TOP, (GetSystemMetrics(SM_CXSCREEN) - LoadWall.W)/2, 
	                               (GetSystemMetrics(SM_CYSCREEN) - LoadWall.H/2)/2, LoadWall.W, LoadWall.H/2,
								   SWP_SHOWWINDOW);
}

void EndLoading()
{	  
    FillMemory(lpVideoBuf, 1024*768*2, 0);	
	_HeapFree(Heap, 0, (void*)LoadWall.lpImage);
}


void PrintLoad(char *t)
{
	strcpy(loadtxt, t);
	LoadCount++;
	UpdateLoadingWindow();

}


void SetVideoMode(int W, int H)
{
   WinW = W; 
   WinH = H;

   WinEX = WinW - 1;
   WinEY = WinH - 1;
   VideoCX = WinW / 2;
   VideoCY = WinH / 2;
    
   CameraW = (float)VideoCX*1.25f;
   CameraH = CameraW * (WinH * 1.3333f / WinW);
   
   SetWindowPos(hwndMain, HWND_TOP, 0, 0, GetSystemMetrics(SM_CXSCREEN), GetSystemMetrics(SM_CYSCREEN), SWP_SHOWWINDOW);
   SetCursorPos(VideoCX, VideoCY);
   
   LoDetailSky =(W>400);
   SetCursor(hcArrow);
   while (ShowCursor(FALSE)>=0) ;   
}



