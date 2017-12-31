#include <windows.h>
#include "hunt.h"

#define req_versionH 0x0001
#define req_versionL 0x0002

HINSTANCE hAudioDLL = NULL;
void DoHalt(LPSTR);

typedef void (WINAPI * LPFUNC1)(void);
typedef void (WINAPI * LPFUNC2)(HWND, HANDLE);
typedef void (WINAPI * LPFUNC3)(float, float, float, float, float);
typedef void (WINAPI * LPFUNC4)(int, short int*, int);
typedef void (WINAPI * LPFUNC5)(int, short int*, float, float, float);
typedef void (WINAPI * LPFUNC6)(int, short int*, float, float, float, int);

typedef int  (WINAPI * LPFUNC7)(void);
typedef void (WINAPI * LPFUNC8)(int, float);

typedef void (WINAPI * LPFUNC9)(int, AudioQuad *);
LPFUNC9 audio_uploadgeometry;

LPFUNC1 audio_restore;
LPFUNC1 audiostop;
LPFUNC1 audio_shutdown;

LPFUNC2 initaudiosystem;
LPFUNC3 audiosetcamerapos;
LPFUNC4 setambient;
LPFUNC5 setambient3d;
LPFUNC6 addvoice3dv;
LPFUNC7 audio_getversion;
LPFUNC8 audio_setenvironment;


void Audio_Shutdown()
{
	if (audio_shutdown) audio_shutdown();
	if (hAudioDLL)  	FreeLibrary(hAudioDLL);
	hAudioDLL = NULL;
	audio_shutdown = NULL;
}


void InitAudioSystem(HWND hw, HANDLE hlog, int  driver)
{
	Audio_Shutdown();

	switch (driver) {
	case 0:
	  hAudioDLL = LoadLibrary("a_soft.dll");
	  if (!hAudioDLL) DoHalt("Can't load A_SOFT.DLL");     
	  break;
	case 1:
	 hAudioDLL = LoadLibrary("a_ds3d.dll");
	 if (!hAudioDLL) DoHalt("Can't load A_DS3D.DLL");
	 break;
	case 2:
	 hAudioDLL = LoadLibrary("a_a3d.dll");
	 if (!hAudioDLL) DoHalt("Can't load A_A3D.DLL");
	 break;
	case 3:
	 hAudioDLL = LoadLibrary("a_eax.dll");
	 if (!hAudioDLL) DoHalt("Can't load A_EAX.DLL");
	 break;     
	}
	

	initaudiosystem   = (LPFUNC2) GetProcAddress(hAudioDLL, "InitAudioSystem");
	if (!initaudiosystem) DoHalt("Can't find procedure address.");
	
	audio_restore     = (LPFUNC1) GetProcAddress(hAudioDLL, "Audio_Restore");
	if (!audio_restore) DoHalt("Can't find procedure address.");

	audiostop         = (LPFUNC1) GetProcAddress(hAudioDLL, "AudioStop");
	if (!audiostop)   DoHalt("Can't find procedure address.");

	audio_shutdown    = (LPFUNC1) GetProcAddress(hAudioDLL, "Audio_Shutdown");
	if (!audio_shutdown) DoHalt("Can't find procedure address.");

	audiosetcamerapos = (LPFUNC3) GetProcAddress(hAudioDLL, "AudioSetCameraPos");
	if (!audiosetcamerapos) DoHalt("Can't find procedure address.");

	setambient        = (LPFUNC4) GetProcAddress(hAudioDLL, "SetAmbient");
	if (!setambient) DoHalt("Can't find procedure address.");

	setambient3d      = (LPFUNC5) GetProcAddress(hAudioDLL, "SetAmbient3d");
	if (!setambient3d) DoHalt("Can't find procedure address.");

	addvoice3dv       = (LPFUNC6) GetProcAddress(hAudioDLL, "AddVoice3dv");
	if (!addvoice3dv) DoHalt("Can't find procedure address.");	

	audio_getversion  = (LPFUNC7) GetProcAddress(hAudioDLL, "Audio_GetVersion");
	if (!audio_getversion) DoHalt("Can't find procedure address.");

	audio_setenvironment = (LPFUNC8) GetProcAddress(hAudioDLL, "Audio_SetEnvironment");
	if (!audio_setenvironment) DoHalt("Can't find procedure address.");

	audio_uploadgeometry = (LPFUNC9) GetProcAddress(hAudioDLL, "Audio_UploadGeometry");
	if (!audio_uploadgeometry) DoHalt("Can't find procedure Audio_UploadGeometry address.");

	int v1 = audio_getversion()>>16;
	int v2 = audio_getversion() & 0xFFFF;
	if ( (v1!=req_versionH) || (v2<req_versionL) )
		DoHalt("Incorrect audio driver version.");

	initaudiosystem(hw, hlog);	    	
}

void Audio_UploadGeometry()
{
	UploadGeometry();
	audio_uploadgeometry(AudioFCount, data);
}

void AudioStop()
{
    audiostop();
}

void Audio_Restore()
{
	if (audio_restore)
  	  audio_restore();
}



void AudioSetCameraPos(float cx, float cy, float cz, float ca, float cb)
{	
	audiosetcamerapos(cx, cy, cz, ca, cb);		
}


void Audio_SetEnvironment(int e, float f)
{
   audio_setenvironment(e, f);
}

void SetAmbient(int length, short int* lpdata, int av)
{
	setambient(length, lpdata, av);
}


void SetAmbient3d(int length, short int* lpdata, float cx, float cy, float cz)
{
	setambient3d(length, lpdata, cx, cy, cz);
}


void AddVoice3dv(int length, short int* lpdata, float cx, float cy, float cz, int vol)
{
	addvoice3dv(length, lpdata, cx, cy, cz, vol);
}




void AddVoice3d(int length, short int* lpdata, float cx, float cy, float cz)
{
   AddVoice3dv(length, lpdata, cx, cy, cz, 256);   
}


void AddVoicev(int length, short int* lpdata, int v)
{      
   AddVoice3dv(length, lpdata, 0,0,0, v);   
}


void AddVoice(int length, short int* lpdata)
{
   AddVoice3dv(length, lpdata, 0,0,0, 256);
}