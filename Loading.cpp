//#ifdef _load

#include "Hunt.h"

#define winw  240
#define winh  120

HWND hwndLoad;
bool _EndLoading;
HANDLE hLoadThread;
DWORD hLOADID;
HDC hdcLoad, hdcCLoad;
HBITMAP hbmpLoad;
void *lpLoadVideoBuf;
char txt[128];
bool lock;

void CreateLoadDIB()
{
    hdcLoad=GetDC(hwndLoad);
    hdcCLoad = CreateCompatibleDC(hdcLoad);    
	
	SetBkMode(hdcCLoad, TRANSPARENT);
	SetTextColor(hdcCLoad, 0x909090);		  

	BITMAPINFOHEADER bmih;
	bmih.biSize = sizeof( BITMAPINFOHEADER ); 	
    bmih.biWidth  = winw; 
    bmih.biHeight = -winh; 	
    bmih.biPlanes = 1; 
    bmih.biBitCount = 16;
    bmih.biCompression = BI_RGB; 
    bmih.biSizeImage = 0;
    bmih.biXPelsPerMeter = 400; 
    bmih.biYPelsPerMeter = 400; 
    bmih.biClrUsed = 0; 
    bmih.biClrImportant = 0;    

	BITMAPINFO binfo;
	binfo.bmiHeader = bmih;
	hbmpLoad = 
     CreateDIBSection(hdcLoad, &binfo, DIB_RGB_COLORS, &lpLoadVideoBuf, NULL, 0);   
	DeleteDC(hdcLoad);

}


void UpdateLWindow()
{
	lock = TRUE;
	HBITMAP hbmpOld = SelectObject(hdcCLoad, hbmpLoad);
    HFONT   hfntOld = SelectObject(hdcCLoad, fnt_Small);		  

	FillMemory(lpLoadVideoBuf, winw*winh*2, 1);
		      
    TextOut(hdcCLoad, 10, winh-16, txt, strlen(txt) );

    BitBlt(hdcLoad,0,0,winw,winh, hdcCLoad,0,0, SRCCOPY);
    MessageBeep(0xFFFFFFFF);

    SelectObject(hdcCLoad,hfntOld);
    SelectObject(hdcCLoad,hbmpOld);
	lock = FALSE;
}



LONG APIENTRY LoadWndProc( HWND hWnd, UINT message, UINT wParam, LONG lParam)
{

	
    switch (message) {
        case WM_CREATE: return 0;                
		//case WM_ERASEBKGND:
        //case WM_NCPAINT   : break;
        case WM_PAINT: {		  
          PAINTSTRUCT ps;
          HDC  hdc =  BeginPaint(hWnd, &ps );          
          EndPaint(hWnd, &ps);          
		  UpdateLWindow();
          break;
        } 
        default:
            return (DefWindowProc(hWnd, message, wParam, lParam));
    }
	
    return 0;
}



DWORD WINAPI ProcessLoading (LPVOID ptr)
{	
    WNDCLASS wc;
    wc.style = CS_OWNDC;
    wc.lpfnWndProc = (WNDPROC)LoadWndProc; 
    wc.cbClsExtra = 0;                  
    wc.cbWndExtra = 0;                  
    wc.hInstance = hInst;
    wc.hIcon = wc.hIcon = LoadIcon(hInst,"ACTION");
    wc.hCursor = NULL;
	wc.hbrBackground = GetStockObject( BLACK_BRUSH );
    wc.lpszMenuName = NULL;
	wc.lpszClassName = "HuntLoadingWindow";
    if (!RegisterClass(&wc)) return FALSE;
    
    hwndLoad = CreateWindow(
        "HuntLoadingWindow","Loading...",
  	    WS_VISIBLE | WS_POPUP,
		400-winw/2, 300-winh/2, winw, winh, NULL,  NULL, hInst, NULL );

	CreateLoadDIB();

	ShowWindow(hwndLoad, SW_SHOW);	

	MSG msg;
	while (!_EndLoading) {
   	  if( PeekMessage( &msg, hwndLoad, NULL, NULL, PM_REMOVE ) ) {        
		   TranslateMessage( &msg );
		   DispatchMessage( &msg );
	  } else {
	   Sleep(100);			   
	   UpdateLWindow();		
	  }
	}  

   DestroyWindow(hwndLoad);

   ExitThread(0);
   return 0;
}


void PrintLoad(char *t)
{	
	if (t) {		
		strcpy(txt, t);			    		
	}
}

void StartLoading()
{
	hLoadThread = CreateThread(NULL, 0, ProcessLoading, NULL, 0, &hLOADID);
	SetThreadPriority(hLoadThread, THREAD_PRIORITY_HIGHEST);
}


void EndLoading()
{		
	_EndLoading = TRUE;
		
	//SetFocus(hwndMain);				
	//SetActiveWindow(hwndMain);
	//ShowWindow(hwndMain, SW_SHOW);
	//BringWindowToTop(hwndMain);
    
}

//#endif