#include <dsound.h>

#ifdef _AUDIO_
   #define _SOUND_H_
#else
   #define _SOUND_H_ extern
#endif

#define MAX_SOUND_DEVICE      16
#define MAX_VOICE             1024
#define MAX_CHANNEL           16
#define MAX_SB_LENGTH         1024             // number of shorts of the buffer
#define MAX_BUFFER_LENGTH     MAX_SB_LENGTH*4  // real lenght of buffer in bytes

#define LEFT_LOW        32
#define LEFT_MIDDLE     88
#define LEFT_HIGH       144

#define RIGHT_LOW       144
#define RIGHT_MIDDLE    200
#define RIGHT_HIGH      256

#define MAX_RADIUS         6000
#define MIN_RADIUS         512


typedef struct tagSOUNDDEVICEDESC {
   GUID Guid;
   char* lpstrDescription;
   char* lpstrModule;
   DSCAPS DSC;
   int status;
} SOUNDDEVICEDESC;

typedef struct tagCHANNEL {   
   int status;     // 0 means that channel is free
   int iLength;    // filled when appropriate voice is assigned to the channel 
   int iPosition;  // position of next portion to read
   short int* lpData;
   int x, y, z;    // coords of the voice
   int volume;   // for aligning
} CHANNEL;

typedef struct tagAMBIENT {
   int iLength;
   short int* lpData;      // voice this channel is referenced to   
   int iPosition;  // position of next portion to read   
   int volume, avolume;
} AMBIENT;

typedef struct tagMAMBIENT {
   int iLength;
   short int* lpData;      // voice this channel is referenced to   
   int iPosition;  // position of next portion to read         
   float x,y,z;    
} MAMBIENT;





void  AddVoice(int, short int*);
void  AddVoicev(int, short int*);
void  AddVoice3d(int, short int*, float, float, float);
void  AddVoice3dv(int, short int*, float, float, float, int);

void SetAmbient(int, short int*, int);
void SetAmbient3d(int, short int*, float, float, float);
void SetAmbient3d(int, short int*, float, float, float, int);
void InitAudioSystem(HWND);

int  ProcessAudio( void );
void AudioSetCameraPosition( float, float, float,  float, float);
void AudioStop();



_SOUND_H_ SOUNDDEVICEDESC sdd[MAX_SOUND_DEVICE];
_SOUND_H_ int iTotalSoundDevices;
_SOUND_H_ int iTotal16SD;
_SOUND_H_ int iCurrentDriver;
_SOUND_H_ int iBufferLength;
_SOUND_H_ int iSoundActive; // prevent playing voices on not ready device
_SOUND_H_ int iNeedNextWritePosition; // is set to 1 when first voice is added
_SOUND_H_ DWORD dwPlayPos;
_SOUND_H_ DWORD dwWritePos;

_SOUND_H_ char _SoundBufferData[1024*16];
_SOUND_H_ char* lpSoundBuffer;

_SOUND_H_ LPDIRECTSOUND lpDS;
_SOUND_H_ WAVEFORMATEX WaveFormat;
_SOUND_H_ LPDIRECTSOUNDBUFFER lpdsPrimary, lpdsSecondary, lpdsWork;
_SOUND_H_ BOOL PrimaryMode;

_SOUND_H_ CHANNEL channel[ MAX_CHANNEL ];
_SOUND_H_ AMBIENT ambient, ambient2;
_SOUND_H_ MAMBIENT mambient;

_SOUND_H_ int xCamera, yCamera, zCamera;
_SOUND_H_ float alphaCamera, cosa, sina;
