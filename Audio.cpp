#include <windows.h>
#include <math.h>

#define _AUDIO_
#include "audio.h"

HANDLE hAudioThread;
DWORD  AudioTId;
WAVEFORMATEX wf;
HWND hwndApp;
BOOL AudioNeedRestore;
char logtt[128];
void PrintLog(LPSTR l);

int  InitDirectSound( HWND );
BOOL CALLBACK EnumerateSoundDevice(GUID FAR *, LPSTR, LPSTR, LPVOID );

void Audio_MixSound(int DestAddr, int SrcAddr, int MixLen, int LVolume, int RVolume);
void Audio_MixChannels();
void Audio_MixAmbient();
void Audio_MixAmbient3d();


DWORD WINAPI ProcessAudioThread (LPVOID ptr)
{
   for (;;) {
    if (iSoundActive)
       ProcessAudio();
    Sleep(70);
   }
   return 0;
}



void Init_SetCooperative()
{
	
}


int InitDirectSound( HWND hwnd)
{
   PrintLog("\n");
   PrintLog("==Init Direct Sound==\n");
 
   HRESULT hres;
   iTotalSoundDevices = 0;   

   lpSoundBuffer = (char*) _SoundBufferData;
   if( !lpSoundBuffer )
      return 0;
   PrintLog("Back Sound Buffer created.\n");

   for( int i = 0; i < MAX_SOUND_DEVICE; i++ )       
      sdd[i].DSC.dwSize = sizeof( DSCAPS );
      

   hres = DirectSoundEnumerate( (LPDSENUMCALLBACK)EnumerateSoundDevice, NULL);
   if( hres != DS_OK ) {
      wsprintf(logtt, "DirectSoundEnumerate Error: %Xh\n", hres);
	  PrintLog(logtt);
      return 0;
   }
   PrintLog("DirectSoundEnumerate: Ok\n");


   iTotal16SD = 0;
   for( i = 0; i < iTotalSoundDevices; i++ ) {
      LPDIRECTSOUND lpds;
      if( DirectSoundCreate( &sdd[i].Guid, &lpds, NULL ) != DS_OK ) continue;

      if( lpds->GetCaps( &sdd[i].DSC ) != DS_OK ) continue;

      if( sdd[i].DSC.dwFlags & (DSCAPS_PRIMARY16BIT | DSCAPS_PRIMARYSTEREO | DSCAPS_SECONDARY16BIT | DSCAPS_SECONDARYSTEREO ) ) {
         sdd[i].status = 1;
         iTotal16SD++;
		 wsprintf(logtt,"Acceptable device: %d\n",i);
		 PrintLog(logtt);
      }
   }

   if (!iTotal16SD) return 0;
   iCurrentDriver = 0;
   while( !sdd[iCurrentDriver].status )
	   iCurrentDriver++;
   
   wsprintf(logtt,"Device selected  : %d\n",iCurrentDriver);
   PrintLog(logtt);


   hres = DirectSoundCreate( &sdd[iCurrentDriver].Guid, &lpDS, NULL );   
   if( (hres != DS_OK) || (!lpDS) ) {
	  wsprintf(logtt, "DirectSoundCreate Error: %Xh\n", hres);
	  PrintLog(logtt);
      return 0;
   }
   PrintLog("DirectSoundCreate: Ok\n");


   PrimaryMode = TRUE;
   PrintLog("Attempting to set WRITEPRIMARY CooperativeLevel:\n");
   hres = lpDS->SetCooperativeLevel( hwnd, DSSCL_WRITEPRIMARY );
   if (hres != DS_OK) {	  
	  wsprintf(logtt, "SetCooperativeLevel Error: %Xh\n", hres);
	  PrintLog(logtt);
      PrimaryMode = FALSE;
   } else
    PrintLog("Set Cooperative  : Ok\n");


   if (!PrimaryMode) {
	   PrintLog("Attempting to set EXCLUSIVE CooperativeLevel:\n");
	   hres = lpDS->SetCooperativeLevel( hwnd, DSSCL_EXCLUSIVE);
       if (hres != DS_OK) {	     
	     wsprintf(logtt, "==>>SetCooperativeLevel Error: %Xh\n", hres);
	     PrintLog(logtt);
		 return 0;
	   }   
       PrintLog("Set Cooperative  : Ok\n");
   }


/*======= creating primary buffer ==============*/
   CopyMemory( &WaveFormat, &wf, sizeof( WAVEFORMATEX ) );
   
   DSBUFFERDESC dsbd;
   dsbd.dwSize = sizeof( DSBUFFERDESC );
   dsbd.dwFlags = DSBCAPS_PRIMARYBUFFER;
   dsbd.dwBufferBytes = 0;
   dsbd.lpwfxFormat = NULL; 
   dsbd.dwReserved = 0;
   
   hres = lpDS->CreateSoundBuffer( &dsbd, &lpdsPrimary, NULL );
   if( hres != DS_OK ) {
	  wsprintf(logtt, "==>>CreatePrimarySoundBuffer Error: %Xh\n", hres);
	  PrintLog(logtt);
      return 0;
   }   
   PrintLog("CreateSoundBuffer: Ok (Primary)\n");
   lpdsWork = lpdsPrimary;

   hres = lpdsPrimary->SetFormat( &wf );
   if( hres != DS_OK ) {
	  wsprintf(logtt, "SetFormat Error: %Xh\n", hres);
	  PrintLog(logtt);
      return 0;
   }   
   PrintLog("SetFormat        : Ok\n");
   




   if (PrimaryMode) goto SKIPSECONDARY;

// ========= creating secondary ================//
   dsbd.dwSize = sizeof( DSBUFFERDESC );
   dsbd.dwFlags = 0;
   dsbd.dwBufferBytes = 2*8192;
   dsbd.lpwfxFormat = &wf; 
   dsbd.dwReserved = 0;
   
   hres = lpDS->CreateSoundBuffer( &dsbd, &lpdsSecondary, NULL );
   if( hres != DS_OK ) {
	  wsprintf(logtt, "CreateSecondarySoundBuffer Error: %Xh\n", hres);
	  PrintLog(logtt);
      return 0;
   }   
   PrintLog("CreateSoundBuffer: Ok (Secondary)\n");
   lpdsWork = lpdsSecondary;


SKIPSECONDARY:
   
   
   DSBCAPS dsbc;
   dsbc.dwSize = sizeof( DSBCAPS );
   lpdsWork->GetCaps( &dsbc );
   iBufferLength = dsbc.dwBufferBytes;
   iBufferLength /= 8192;
         
   hres = lpdsWork->Play( 0, 0, DSBPLAY_LOOPING );
   if( hres != DS_OK ) {
      wsprintf(logtt, "Play Error: %Xh\n", hres);
	  PrintLog(logtt);
      return 0;   
   }
   PrintLog("Play             : Ok\n");
   



   iSoundActive = 1;
   FillMemory( channel, sizeof( CHANNEL )*MAX_CHANNEL, 0 );
   ambient.iLength = 0;

   hAudioThread = CreateThread(NULL, 0, ProcessAudioThread, NULL, 0, &AudioTId);
   SetThreadPriority(hAudioThread, THREAD_PRIORITY_HIGHEST);

   PrintLog("Direct Sound activated.\n");
   return 1;
}


void InitAudioSystem(HWND hw)
{   
   hwndApp = hw;
   wf.wFormatTag      = WAVE_FORMAT_PCM;
   wf.nChannels       = 2;
   wf.nSamplesPerSec  = 11025*2;
   wf.nAvgBytesPerSec = 44100*2;
   wf.nBlockAlign     = 4;
   wf.wBitsPerSample  = 16;
   wf.cbSize          = 0;
   if( !InitDirectSound( hwndApp ) )
      PrintLog("Sound System failed\n");
}


void AudioStop()
{
  FillMemory(&ambient, sizeof(ambient), 0);
  FillMemory(&mambient, sizeof(mambient), 0);
  FillMemory(&ambient2, sizeof(ambient2), 0);
  FillMemory(channel, sizeof( CHANNEL )*MAX_CHANNEL, 0 );
  AudioNeedRestore = TRUE;
}


void AudioSetCameraPos(float cx, float cy, float cz, float ca, float cb)
{
  xCamera = (int) cx;
  yCamera = (int) cy;
  zCamera = (int) cz;
  alphaCamera = ca;
  cosa = (float)cos(ca);
  sina = (float)sin(ca);
}


void SetAmbient(int length, short int* lpdata, int av)
{
   if (!iSoundActive) return;
   
   if (ambient.lpData == lpdata) return;
      
   ambient2 = ambient;

   ambient.iLength = length;
   ambient.lpData = lpdata;
   ambient.iPosition = 0;
   ambient.volume = 0;
   ambient.avolume = av;
}


void SetAmbient3d(int length, short int* lpdata, float cx, float cy, float cz)
{
   if (!iSoundActive) return;
   
   if (mambient.iPosition >= length) mambient.iPosition = 0;
      
   mambient.iLength = length;
   mambient.lpData = lpdata;
   mambient.x = cx;
   mambient.y = cy;
   mambient.z = cz;   
}



void AddVoice3dv(int length, short int* lpdata, float cx, float cy, float cz, int vol)
{
   if (!iSoundActive) return;
   if (lpdata == 0) return;

   for( int i = 0; i < MAX_CHANNEL; i++ ) 
     if( !channel[i].status ) {                  
         channel[i].iLength = length;
         channel[i].lpData  = lpdata;
         channel[i].iPosition = 0;

         channel[i].x = (int)cx;
         channel[i].y = (int)cy;
         channel[i].z = (int)cz;

         channel[i].status = 1;
		 channel[i].volume = vol;

         return;
      }
   return;
}


void AddVoice3d(int length, short int* lpdata, float cx, float cy, float cz)
{
   AddVoice3dv(length, lpdata, cx, cy, cz, 256);
}


void AddVoicev(int length, short int* lpdata, int v)
{      
   AddVoice3dv(length, lpdata, 0, 0, 0, v);   
}



void AddVoice(int length, short int* lpdata)
{
   AddVoice3dv(length, lpdata, 0,0,0, 256);
}




BOOL CALLBACK EnumerateSoundDevice( GUID* lpGuid, LPSTR lpstrDescription, LPSTR lpstrModule, LPVOID lpContext)
{
   if( lpGuid == NULL )
       if( !iTotalSoundDevices )
           return TRUE;
       else
           return FALSE;   
   wsprintf(logtt,"Device%d: ",iTotalSoundDevices);
   PrintLog(logtt);
   PrintLog(lpstrDescription);
   PrintLog("/");
   PrintLog(lpstrModule);
   PrintLog("\n");
   

   CopyMemory( &sdd[iTotalSoundDevices].Guid, lpGuid, sizeof( GUID ) );

   iTotalSoundDevices++;

   return TRUE;
}


void Audio_Shutdown()
{
   if (!iSoundActive) return;
   
   lpdsWork->Stop();	   
   TerminateThread(hAudioThread ,0);
}

void Audio_Restore()
{
   if (!iSoundActive) return;
   
   lpdsWork->Stop();	   
   lpdsWork->Restore();
   HRESULT hres = lpdsWork->Play( 0, 0, DSBPLAY_LOOPING );   
   if (hres != DS_OK) AudioNeedRestore = TRUE;
                 else AudioNeedRestore = FALSE;
   
   if (!AudioNeedRestore)
	   PrintLog("Audio restored.\n");
}


int ProcessAudio()
{
   LPVOID lpStart1;
   LPVOID lpStart2;
   DWORD  len1, len2;
   HRESULT hres;
   static int PrevBlock = 1;            

   if (AudioNeedRestore) Audio_Restore();
   if (AudioNeedRestore) return 1;

   hres = lpdsWork->GetCurrentPosition( &len1, &dwWritePos );
   hres = lpdsWork->GetCurrentPosition( &len2, &dwWritePos );
   if( hres != DS_OK ) { AudioNeedRestore = TRUE; return 0; }

   if ( (len1>len2) || (len1<len2+16) ) 
       hres = lpdsWork->GetCurrentPosition( &len2, &dwWritePos );   
   
   if (len1+len2==0) {
	   Sleep(5);
	   lpdsWork->GetCurrentPosition( &len2, &dwWritePos );
       if (!len2) { AudioNeedRestore = TRUE; return 0; }
   }

   dwPlayPos = len2;

   int CurBlock = dwPlayPos / (4096*2);
   if (CurBlock==PrevBlock) return 1;

   if( (int)dwPlayPos < CurBlock*4096*2 + 2) return 1; // it's no time to put info

   FillMemory( lpSoundBuffer, MAX_BUFFER_LENGTH*2, 0 );
   Audio_MixChannels();
   Audio_MixAmbient();
   Audio_MixAmbient3d();

   PrevBlock = CurBlock; 
   int NextBlock = (CurBlock + 1) % iBufferLength;
   hres = lpdsWork->Lock(NextBlock*4096*2, 4096*2, &lpStart1, &len1, &lpStart2, &len2, 0 );
   if( hres != DS_OK ) {	     	     
         return 0;      
		 }
   
   CopyMemory( lpStart1, lpSoundBuffer, 4096*2);
         
   hres = lpdsWork->Unlock( lpStart1, len1, lpStart2, len2 );
   if( hres != DS_OK ) 
         return 0;         
   
   return 1;
}


void CalcLRVolumes(int v0, int x, int y, int z, int &lv, int &rv)
{
   if (x==0) {
    lv = v0*180;
    rv = v0*180;
    return;  }

   v0*=200;
   x-=xCamera;
   y-=yCamera;
   z-=zCamera;
   float xx = (float)x * cosa + (float)z * sina;
   float yy = (float)y;
   float zz = (float)fabs((float)z * cosa - (float)x * sina);

   float xa = (float)fabs(xx);
   float l = 0.8f;
   float r = 0.8f;
   float d = (float)sqrt( xx*xx + yy*yy + zz*zz) - MIN_RADIUS;
   float k;
   if (d<=0) k=1.f;
    //else k = (MAX_RADIUS - d) / MAX_RADIUS;
     else k = 1224.f / (1224 + d);
   if (d>6000) {
	   d-=6000;
	   k = k * (4000 - d) / (4000);
	 }
   if (k<0) k=0.f;

   float fi = (float)atan2(xa, zz);
   r = 0.7f + 0.3f * fi / (3.141593f/2.f);
   l = 0.7f - 0.6f * fi / (3.141593f/2.f);

   if (xx>0) { lv=(int)(v0*l*k); rv=(int)(v0*r*k); }
        else { lv=(int)(v0*r*k); rv=(int)(v0*l*k); }
}


void Audio_MixChannels()
{
    int iMixLen;
    int srcofs;
    int LV, RV;

    for( int i = 0; i < MAX_CHANNEL; i++ )        
      if( channel[ i ].status ) {
         if( channel[ i ].iPosition + 2048*2 >= channel[ i ].iLength )
             iMixLen = (channel[ i ].iLength - channel[ i ].iPosition)>>1;
          else
             iMixLen = 1024*2;

         srcofs = (int) channel[i].lpData + channel[i].iPosition;
         CalcLRVolumes(channel[i].volume, channel[i].x, channel[i].y, channel[i].z, LV, RV);
         if (LV || RV)
           Audio_MixSound((int)lpSoundBuffer, srcofs, iMixLen, LV, RV);

         if (channel[ i ].iPosition + 2048*2 >= channel[ i ].iLength ) 
           channel[ i ].status = 0; else channel[ i ].iPosition += 2048*2;         
      }
}


void Audio_DoMixAmbient(AMBIENT &ambient)
{
    if (ambient.lpData==0) return;
    int iMixLen,srcofs;    
	int v = (32000 * ambient.volume * ambient.avolume) / 256 / 256;

    if( ambient.iPosition + 2048*2 >= ambient.iLength )
         iMixLen = (ambient.iLength - ambient.iPosition)>>1;
       else iMixLen = 1024*2;

    srcofs = (int) ambient.lpData + ambient.iPosition;
    Audio_MixSound((int)lpSoundBuffer, srcofs, iMixLen, v, v);

    if (ambient.iPosition + 2048*2 >= ambient.iLength ) 
       ambient.iPosition=0; else ambient.iPosition += 2048*2;
    
    if (iMixLen<1024*2) {
      Audio_MixSound((int)lpSoundBuffer + iMixLen*4, 
                     (int)ambient.lpData, 1024*2-iMixLen, v, v);
      ambient.iPosition+=(1024*2-iMixLen)*2;
    }
}


void Audio_MixAmbient()
{
	Audio_DoMixAmbient(ambient);
	if (ambient.volume<256) ambient.volume = min(ambient.volume+16, 256);

	if (ambient2.volume) Audio_DoMixAmbient(ambient2);
	if (ambient2.volume>0) ambient2.volume = max(ambient2.volume-16, 0);    
}









void Audio_MixAmbient3d()
{
    if (mambient.lpData==0) return;
    int iMixLen,srcofs;    
	int LV, RV;

	CalcLRVolumes(256, (int)mambient.x, (int)mambient.y, (int)mambient.z, LV, RV);
	if (!(LV || RV)) return;


    if( mambient.iPosition + 2048*2 >= mambient.iLength )
         iMixLen = (mambient.iLength - mambient.iPosition)>>1;
       else iMixLen = 1024*2;

    srcofs = (int) mambient.lpData + mambient.iPosition;
    Audio_MixSound((int)lpSoundBuffer, srcofs, iMixLen, LV, RV);

    if (mambient.iPosition + 2048*2 >= mambient.iLength ) 
        mambient.iPosition=0; else mambient.iPosition += 2048*2;
    
    if (iMixLen<1024*2) {
      Audio_MixSound((int)lpSoundBuffer + iMixLen*4, 
                     (int)mambient.lpData, 1024*2-iMixLen, LV, RV);
      mambient.iPosition+=(1024*2-iMixLen)*2;
    }
}






void Audio_MixSound(int DestAddr, int SrcAddr, int MixLen, int LVolume, int RVolume) 
{
_asm {
       mov      edi, DestAddr                    
       mov      ecx, MixLen         
       mov      esi, SrcAddr
     }
      
SOUND_CYCLE :

_asm {                
       movsx    eax, word ptr [esi]
       imul     LVolume
       sar      eax, 16
       mov      bx, word ptr [edi]
               
       add      ax, bx
       jo       LEFT_CHECK_OVERFLOW
       mov      word ptr [edi], ax
       jmp      CYCLE_RIGHT
 }
LEFT_CHECK_OVERFLOW :
__asm {
       cmp      bx, 0
       js       LEFT_MAX_NEGATIVE
       mov      ax, 32767
       mov      word ptr [edi], ax
       jmp      CYCLE_RIGHT
}
LEFT_MAX_NEGATIVE :
__asm  mov      word ptr [edi], -32767
                           
      


CYCLE_RIGHT :
__asm {
       movsx    eax, word ptr [esi]
       imul     dword ptr RVolume
       sar      eax, 16
       mov      bx, word ptr [edi+2]
               
       add      ax, bx
       jo       RIGHT_CHECK_OVERFLOW
       mov      word ptr [edi+2], ax
       jmp      CYCLE_CONTINUE
} 
RIGHT_CHECK_OVERFLOW :
__asm {
       cmp      bx, 0
       js       RIGHT_MAX_NEGATIVE
       mov      word ptr [edi+2], 32767
       jmp      CYCLE_CONTINUE
}
RIGHT_MAX_NEGATIVE :
__asm  mov      word ptr [edi+2], -32767
      
CYCLE_CONTINUE :  
__asm {
       add      edi, 4
       add      esi, 2
       dec      ecx
       jnz      SOUND_CYCLE
}
}