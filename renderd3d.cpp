#ifdef _d3d
#include "Hunt.h"

#include "stdio.h"


#define d3debvcG   400
#define d3debvc   1024

#undef  TCMAX 
#undef  TCMIN 
#define TCMAX (((128<<16)-62024) / 128.f / 65535.f)
#define TCMIN (((000<<16)+62024) / 128.f / 65535.f)

#define _ZSCALE - 16.f
#define _AZSCALE (1.f /  16.f);

Vector2di ORList[2048];
int ORLCount = 0;

LPDIRECTDRAWSURFACE     lpddPrimary               = NULL;
LPDIRECTDRAWSURFACE     lpddBack                  = NULL;
LPDIRECTDRAWSURFACE     lpddZBuffer               = NULL;
LPDIRECTDRAWSURFACE     lpddTexture               = NULL;
DDSURFACEDESC           ddsd;

LPDIRECT3D              lpd3d                     = NULL;
LPDIRECT3DDEVICE        lpd3dDevice               = NULL;
LPDIRECT3DVIEWPORT      lpd3dViewport             = NULL;
LPDIRECT3DEXECUTEBUFFER lpd3dExecuteBuffer        = NULL;
LPDIRECT3DEXECUTEBUFFER lpd3dExecuteBufferG       = NULL;
LPD3DTLVERTEX           lpVertex, lpVertexG;
WORD                    *lpwTriCount;

HRESULT                 hRes;
D3DEXECUTEBUFFERDESC    d3dExeBufDesc;
D3DEXECUTEBUFFERDESC    d3dExeBufDescG;
LPD3DINSTRUCTION        lpInstruction, lpInstructionG;
LPD3DPROCESSVERTICES    lpProcessVertices;
LPD3DTRIANGLE           lpTriangle;
LPD3DLINE               lpLine;
LPD3DSTATE              lpState;

BOOL                    fDeviceFound              = FALSE;
DWORD                   dwDeviceBitDepth          = 0UL;
GUID                    guidDevice;
char                    szDeviceName[256];
char                    szDeviceDesc[256];
D3DDEVICEDESC           d3dHWDeviceDesc;
D3DTEXTUREHANDLE        hTexture, hGTexture;
HDC                     ddBackDC;

BOOL                    D3DACTIVE;
BOOL                    VMFORMAT565;
BOOL                    STARTCONV555;
BOOL                    HANDLECHANGED;

#define NUM_INSTRUCTIONS       5UL
#define NUM_STATES            20UL
#define NUM_PROCESSVERTICES    1UL

void RenderFSRect(DWORD Color);
void d3dDownLoadTexture(int i, int w, int h, LPVOID tptr);
BOOL d3dAllocTexture(int i, int w, int h);
void d3dSetTexture(LPVOID tptr, int w, int h);
void RenderShadowClip (TModel*, float, float, float, float, float, float, int, float, float);
void ResetTextureMap();


int WaterAlphaL = 255;
int d3dTexturesMem;
int d3dLastTexture;
int d3dMemUsageCount;
int d3dMemLoaded;
int GVCnt;

BOOL LINEARFILTER = TRUE;

int zs;
float SunLight;
float TraceK,SkyTraceK,FogYGrad,FogYBase;;
int SunScrX, SunScrY;
int SkySumR, SkySumG, SkySumB;
int LowHardMemory;
int lsw;
int vFogT[1024];
BOOL SmallFont;


typedef struct _d3dmemmap {
  int cpuaddr, size, lastused;  
  LPDIRECTDRAWSURFACE     lpddTexture;
  D3DTEXTUREHANDLE        hTexture;    
} Td3dmemmap;    


#define d3dmemmapsize 128
Td3dmemmap d3dMemMap[d3dmemmapsize+2];


WORD conv_555(WORD c)
{
	return (c & 31) + ( (c & 0xFFE0) >> 1 );
}

void conv_pic555(TPicture &pic)
{
	if (!HARD3D) return;
	for (int y=0; y<pic.H; y++)
		for (int x=0; x<pic.W; x++)
			*(pic.lpImage + x + y*pic.W) = conv_555(*(pic.lpImage + x + y*pic.W));
}


void CalcFogLevel_Gradient(Vector3d v)
{
  FogYBase =  CalcFogLevel(v);	  
  if (FogYBase>0) {
   v.y+=800;
   FogYGrad = (CalcFogLevel(v) - FogYBase) / 800.f;      
  } else FogYGrad=0;
}


void Hardware_ZBuffer(BOOL bl)
{
	if (!bl) {		
		DDBLTFX ddbltfx;
		ddbltfx.dwSize = sizeof( DDBLTFX );
        ddbltfx.dwFillDepth = 0x0000;
        lpddZBuffer->Blt( NULL, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &ddbltfx );
	}
}

void d3dClearBuffers()
{
  DDBLTFX ddbltfx;

  ddbltfx.dwSize = sizeof( DDBLTFX );
  
  if (VMFORMAT565) ddbltfx.dwFillColor = (SkyR>>3)*32*32*2 + (SkyG>>2)*32 + (SkyB>>3);
              else ddbltfx.dwFillColor = (SkyR>>3)*32*32   + (SkyG>>3)*32 + (SkyB>>3);  

  lpddBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );  
   
  ddbltfx.dwSize = sizeof( DDBLTFX );
  ddbltfx.dwFillDepth = 0x0000;
  lpddZBuffer->Blt( NULL, NULL, NULL, DDBLT_DEPTHFILL | DDBLT_WAIT, &ddbltfx );
}


void d3dStartBuffer()
{
   ZeroMemory(&d3dExeBufDesc, sizeof(d3dExeBufDesc));
   d3dExeBufDesc.dwSize = sizeof(d3dExeBufDesc);
   hRes = lpd3dExecuteBuffer->Lock( &d3dExeBufDesc );
   if (FAILED(hRes)) DoHalt("Error locking execute buffer");
   lpVertex = (LPD3DTLVERTEX)d3dExeBufDesc.lpData;   
}



void d3dStartBufferG()
{
   ZeroMemory(&d3dExeBufDescG, sizeof(d3dExeBufDescG));
   d3dExeBufDescG.dwSize = sizeof(d3dExeBufDescG);
   hRes = lpd3dExecuteBufferG->Lock( &d3dExeBufDescG );
   if (FAILED(hRes)) DoHalt("Error locking execute buffer");   

   GVCnt     = 0;
   hGTexture = -1;

   lpVertexG = (LPD3DTLVERTEX)d3dExeBufDescG.lpData;   
   lpInstructionG = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDescG.lpData + d3debvcG*3);   

   lpInstructionG->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstructionG->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstructionG->wCount  = 1U;
   lpInstructionG++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstructionG;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = d3debvcG*3;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;
   
   lpInstructionG = (LPD3DINSTRUCTION)lpProcessVertices;

   if (!LINEARFILTER) {
	 lpInstructionG->bOpcode = D3DOP_STATERENDER;
     lpInstructionG->bSize = sizeof(D3DSTATE);
     lpInstructionG->wCount = 2;
     lpInstructionG++;
	 lpState = (LPD3DSTATE)lpInstructionG;
     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
     lpState->dwArg[0] = D3DFILTER_LINEAR;
     lpState++;

     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
     lpState->dwArg[0] = D3DFILTER_LINEAR;
     lpState++;
     lpInstructionG = (LPD3DINSTRUCTION)lpState;     
   }
   
   if (FOGENABLE) {
	 lpInstructionG->bOpcode = D3DOP_STATERENDER;
     lpInstructionG->bSize = sizeof(D3DSTATE);
     lpInstructionG->wCount = 1;
     lpInstructionG++;
     lpState = (LPD3DSTATE)lpInstructionG;
     lpState->drstRenderStateType = D3DRENDERSTATE_FOGCOLOR;
	 lpState->dwArg[0] = CurFogColor;
     
     lpState++;	 
     lpInstructionG = (LPD3DINSTRUCTION)lpState;     
   }
         
}



void d3dStartBufferGBMP()
{
   ZeroMemory(&d3dExeBufDescG, sizeof(d3dExeBufDescG));
   d3dExeBufDescG.dwSize = sizeof(d3dExeBufDescG);
   hRes = lpd3dExecuteBufferG->Lock( &d3dExeBufDescG );
   if (FAILED(hRes)) DoHalt("Error locking execute buffer");   

   GVCnt     = 0;
   hGTexture = -1;

   lpVertexG = (LPD3DTLVERTEX)d3dExeBufDescG.lpData;   
   lpInstructionG = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDescG.lpData + d3debvcG*3);   

   lpInstructionG->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstructionG->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstructionG->wCount  = 1U;
   lpInstructionG++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstructionG;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = d3debvcG*3;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;
   
   lpInstructionG = (LPD3DINSTRUCTION)lpProcessVertices;

    
	 lpInstructionG->bOpcode = D3DOP_STATERENDER;
     lpInstructionG->bSize = sizeof(D3DSTATE);
     if (FOGENABLE) lpInstructionG->wCount = 6;
	           else lpInstructionG->wCount = 5;
     lpInstructionG++;
     lpState = (LPD3DSTATE)lpInstructionG;

	 lpState->drstRenderStateType = D3DRENDERSTATE_COLORKEYENABLE;
     lpState->dwArg[0] = TRUE;
     lpState++;

	 lpState->drstRenderStateType = D3DRENDERSTATE_ALPHATESTENABLE;
     lpState->dwArg[0] = TRUE;
     lpState++;  

     lpState->drstRenderStateType = D3DRENDERSTATE_ALPHAFUNC;
     lpState->dwArg[0] = D3DCMP_GREATER;
     lpState++;  

     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
     lpState->dwArg[0] = D3DFILTER_NEAREST;
     lpState++;

     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
     lpState->dwArg[0] = D3DFILTER_NEAREST;
     lpState++;

	 if (FOGENABLE) {
      lpState->drstRenderStateType = D3DRENDERSTATE_FOGCOLOR;
	  lpState->dwArg[0] = CurFogColor;
      /*if (UNDERWATER) lpState->dwArg[0] = 0x00004560;	
	             else lpState->dwArg[0] = 0x00606065;*/
      lpState++;	 
	 }

     lpInstructionG = (LPD3DINSTRUCTION)lpState;     
     LINEARFILTER = FALSE;
}


void d3dEndBufferG(BOOL ColorKey)
{   	 
   if (!lpVertexG) return;

   if (ColorKey) {
     lpInstructionG->bOpcode = D3DOP_STATERENDER;
     lpInstructionG->bSize = sizeof(D3DSTATE);   
     lpInstructionG->wCount = 5;
     lpInstructionG++;
     
	 lpState = (LPD3DSTATE)lpInstructionG;

	 lpState->drstRenderStateType = D3DRENDERSTATE_COLORKEYENABLE;
     lpState->dwArg[0] = FALSE;
     lpState++;

	 lpState->drstRenderStateType = D3DRENDERSTATE_ALPHATESTENABLE;
     lpState->dwArg[0] = FALSE;
     lpState++;  

     lpState->drstRenderStateType = D3DRENDERSTATE_ALPHAFUNC;
     lpState->dwArg[0] = D3DCMP_GREATEREQUAL;
     lpState++;  

     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
     lpState->dwArg[0] = D3DFILTER_LINEAR;
     lpState++;

     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
     lpState->dwArg[0] = D3DFILTER_LINEAR;
     lpState++;
	 lpInstructionG = (LPD3DINSTRUCTION)lpState;
   }
  
   lpInstructionG->bOpcode = D3DOP_EXIT;
   lpInstructionG->bSize   = 0UL;
   lpInstructionG->wCount  = 0U;

   lpInstructionG = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDescG.lpData + d3debvcG*3);   

   lpInstructionG->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstructionG->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstructionG->wCount  = 1U;
   lpInstructionG++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstructionG;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = GVCnt;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;


   lpd3dExecuteBufferG->Unlock( );

   dFacesCount+=GVCnt/3;
   lpInstructionG = NULL;
   lpVertexG      = NULL;
   GVCnt          = 0;
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBufferG, lpd3dViewport, D3DEXECUTE_UNCLIPPED);
   LINEARFILTER = TRUE;
   
}



void d3dFlushBuffer(int fproc1, int fproc2)
{	
    
   BOOL ColorKey = (fproc2>0);

   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   
   lpInstruction->wCount = 3;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
   lpState->dwArg[0] = hTexture;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
   lpState->dwArg[0] = D3DFILTER_LINEAR;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
   lpState->dwArg[0] = D3DFILTER_LINEAR;
   lpState++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstruction->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstruction->wCount  = 1U;
   lpInstruction++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstruction;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = (fproc1+fproc2)*3;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpProcessVertices;
   lpInstruction->bOpcode = D3DOP_TRIANGLE;
   lpInstruction->bSize   = sizeof(D3DTRIANGLE);
   lpInstruction->wCount  = fproc1;
   lpInstruction++;
   lpTriangle             = (LPD3DTRIANGLE)lpInstruction;   
   
   int ii = 0;
   for (int i=0; i<fproc1; i++) {  	   
	lpTriangle->wV1    = ii++;
    lpTriangle->wV2    = ii++;	
    lpTriangle->wV3    = ii++;	
	lpTriangle->wFlags = 0;
	lpTriangle++;	
   }

   lpInstruction = (LPD3DINSTRUCTION)lpTriangle;

   if (ColorKey) {    
    lpInstruction->bOpcode = D3DOP_STATERENDER;
    lpInstruction->bSize = sizeof(D3DSTATE);
    lpInstruction->wCount = 5;
    lpInstruction++;
    lpState = (LPD3DSTATE)lpInstruction;

	lpState->drstRenderStateType = D3DRENDERSTATE_COLORKEYENABLE;
    lpState->dwArg[0] = TRUE;
    lpState++;

	lpState->drstRenderStateType = D3DRENDERSTATE_ALPHATESTENABLE;
    lpState->dwArg[0] = TRUE;
    lpState++;  

    lpState->drstRenderStateType = D3DRENDERSTATE_ALPHAFUNC;
    lpState->dwArg[0] = D3DCMP_GREATER;
    lpState++;  

    lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
    lpState->dwArg[0] = D3DFILTER_NEAREST;
    lpState++;

    lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
    lpState->dwArg[0] = D3DFILTER_NEAREST;
    lpState++;

	lpInstruction = (LPD3DINSTRUCTION)lpState;

	lpInstruction->bOpcode = D3DOP_TRIANGLE;
    lpInstruction->bSize   = sizeof(D3DTRIANGLE);
    lpInstruction->wCount  = fproc2;
    lpInstruction++;
    lpTriangle             = (LPD3DTRIANGLE)lpInstruction;

    for (i=0; i<fproc2; i++) {  	   
	 lpTriangle->wV1    = ii++;
     lpTriangle->wV2    = ii++;	
     lpTriangle->wV3    = ii++;	
	 lpTriangle->wFlags = 0;
	 lpTriangle++;	
    }

	lpInstruction = (LPD3DINSTRUCTION)lpTriangle;

	lpInstruction->bOpcode = D3DOP_STATERENDER;
    lpInstruction->bSize = sizeof(D3DSTATE);
    lpInstruction->wCount = 5;
    lpInstruction++;
    lpState = (LPD3DSTATE)lpInstruction;

	lpState->drstRenderStateType = D3DRENDERSTATE_COLORKEYENABLE;
    lpState->dwArg[0] = FALSE;
    lpState++;

	lpState->drstRenderStateType = D3DRENDERSTATE_ALPHATESTENABLE;
    lpState->dwArg[0] = FALSE;
    lpState++;  

    lpState->drstRenderStateType = D3DRENDERSTATE_ALPHAFUNC;
    lpState->dwArg[0] = D3DCMP_GREATEREQUAL;
    lpState++;  

    lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
    lpState->dwArg[0] = D3DFILTER_LINEAR;
    lpState++;

    lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
    lpState->dwArg[0] = D3DFILTER_LINEAR;
    lpState++;
	lpInstruction = (LPD3DINSTRUCTION)lpState;

   }

   
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );
   LINEARFILTER = TRUE;
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);
   //if (FAILED(hRes)) DoHalt("Error execute buffer");   
   dFacesCount+=fproc1+fproc2;
}


DWORD BitDepthToFlags( DWORD dwBitDepth )
{
   switch( dwBitDepth ) {
      case  1UL: return DDBD_1;
      case  2UL: return DDBD_2;
      case  4UL: return DDBD_4;
      case  8UL: return DDBD_8;
      case 16UL: return DDBD_16;
      case 24UL: return DDBD_24;
      case 32UL: return DDBD_32;
      default  : return 0UL;
   }
}

DWORD FlagsToBitDepth( DWORD dwFlags )
{
   if (dwFlags & DDBD_1)       return 1UL;
   else if (dwFlags & DDBD_2)  return 2UL;
   else if (dwFlags & DDBD_4)  return 4UL;
   else if (dwFlags & DDBD_8)  return 8UL;
   else if (dwFlags & DDBD_16) return 16UL;
   else if (dwFlags & DDBD_24) return 24UL;
   else if (dwFlags & DDBD_32) return 32UL;
   else                        return 0UL;
}




HRESULT FillExecuteBuffer_State( LPDIRECT3DEXECUTEBUFFER lpd3dExecuteBuffer)
{
   HRESULT              hRes;
   D3DEXECUTEBUFFERDESC d3dExeBufDesc;
   LPD3DINSTRUCTION     lpInstruction;      
   LPD3DSTATE           lpState;
  

   ZeroMemory(&d3dExeBufDesc, sizeof(d3dExeBufDesc));
   d3dExeBufDesc.dwSize = sizeof(d3dExeBufDesc);
   hRes = lpd3dExecuteBuffer->Lock( &d3dExeBufDesc );
   if (FAILED(hRes)) {
         PrintLog( "Error locking execute buffer");
      return hRes;
   }   
   
   
   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 24;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_ZENABLE;
   lpState->dwArg[0] = TRUE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_ZWRITEENABLE;
   lpState->dwArg[0] = TRUE;
   lpState++;
   
   lpState->drstRenderStateType = D3DRENDERSTATE_ZFUNC;
   lpState->dwArg[0] = D3DCMP_GREATEREQUAL;
   lpState++;
   
   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREPERSPECTIVE;
   lpState->dwArg[0] = TRUE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAPBLEND;
   if (OPT_ALPHA_COLORKEY)  lpState->dwArg[0] = D3DTBLEND_MODULATEALPHA;
                      else  lpState->dwArg[0] = D3DTBLEND_MODULATE;

   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
   lpState->dwArg[0] = D3DFILTER_LINEAR;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
   lpState->dwArg[0] = D3DFILTER_LINEAR;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_SHADEMODE;
   lpState->dwArg[0] = D3DSHADE_GOURAUD;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_DITHERENABLE;
   lpState->dwArg[0] = TRUE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_CULLMODE;
   lpState->dwArg[0] = D3DCULL_NONE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_BLENDENABLE;
   lpState->dwArg[0] = TRUE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_COLORKEYENABLE;
   lpState->dwArg[0] = FALSE;
   lpState++;
   
   lpState->drstRenderStateType = D3DRENDERSTATE_ALPHABLENDENABLE;
   lpState->dwArg[0] = TRUE;
   lpState++;


   lpState->drstRenderStateType = D3DRENDERSTATE_ALPHATESTENABLE;
   lpState->dwArg[0] = FALSE;
   lpState++;  

   lpState->drstRenderStateType = D3DRENDERSTATE_ALPHAREF;
   lpState->dwArg[0] = 0;
   lpState++;  

   lpState->drstRenderStateType = D3DRENDERSTATE_ALPHAFUNC;
   lpState->dwArg[0] = D3DCMP_GREATEREQUAL;
   lpState++;  



   lpState->drstRenderStateType = D3DRENDERSTATE_SPECULARENABLE;
   lpState->dwArg[0] = FALSE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_FOGENABLE;
   lpState->dwArg[0] = FOGENABLE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_FOGCOLOR;
   lpState->dwArg[0] = 0x00606070;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_SRCBLEND;
   lpState->dwArg[0] = D3DBLEND_SRCALPHA;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_DESTBLEND;
   lpState->dwArg[0] = D3DBLEND_INVSRCALPHA;
   lpState++;


   lpState->drstRenderStateType = D3DRENDERSTATE_STIPPLEDALPHA;
   lpState->dwArg[0] = FALSE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_WRAPU;
   lpState->dwArg[0] = FALSE;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_WRAPV;
   lpState->dwArg[0] = FALSE;
   lpState++;


   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );

   //PrintLog( "Execute buffer filled successfully\n" );

   return DD_OK;

}



void SetRenderStates(BOOL ZWRITE, int DST_BLEND)
{
   HRESULT              hRes;
   D3DEXECUTEBUFFERDESC d3dExeBufDesc;
   LPD3DINSTRUCTION     lpInstruction;      
   LPD3DSTATE           lpState;
  

   ZeroMemory(&d3dExeBufDesc, sizeof(d3dExeBufDesc));
   d3dExeBufDesc.dwSize = sizeof(d3dExeBufDesc);
   hRes = lpd3dExecuteBuffer->Lock( &d3dExeBufDesc );
   if (FAILED(hRes)) {
         PrintLog( "Error locking execute buffer");
      return;
   }   
   
   
   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 2;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;
   

   lpState->drstRenderStateType = D3DRENDERSTATE_ZWRITEENABLE;
   lpState->dwArg[0] = ZWRITE;
   lpState++;
                  
   lpState->drstRenderStateType = D3DRENDERSTATE_DESTBLEND;
   lpState->dwArg[0] = DST_BLEND;
   lpState++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );
   lpd3dDevice->Execute( lpd3dExecuteBuffer, 
	                     lpd3dViewport, 
						 D3DEXECUTE_UNCLIPPED );
}



HRESULT WINAPI EnumDeviceCallback(
   LPGUID lpGUID, 
   LPSTR lpszDeviceDesc, 
   LPSTR lpszDeviceName, 
   LPD3DDEVICEDESC lpd3dHWDeviceDesc, 
   LPD3DDEVICEDESC lpd3dSWDeviceDesc,
   LPVOID lpUserArg )
{
   LPD3DDEVICEDESC lpd3dDeviceDesc;

   wsprintf(logt,"ENUMERATE: DDesc: %s DName: %s\n", lpszDeviceDesc, lpszDeviceName);
   //PrintLog(logt);
   if( !lpd3dHWDeviceDesc->dcmColorModel )
      return D3DENUMRET_OK; // we don't need SW rasterizer

   lpd3dDeviceDesc = lpd3dHWDeviceDesc;

   if( (lpd3dDeviceDesc->dwDeviceRenderBitDepth & dwDeviceBitDepth) == 0UL )
      return D3DENUMRET_OK;

   if( !(lpd3dDeviceDesc->dpcTriCaps.dwShadeCaps & D3DPSHADECAPS_COLORGOURAUDRGB) )
      return D3DENUMRET_OK;

   fDeviceFound = TRUE;
   CopyMemory( &guidDevice, lpGUID, sizeof(GUID) );
   strcpy( szDeviceDesc, lpszDeviceDesc );
   strcpy( szDeviceName, lpszDeviceName );
   CopyMemory( &d3dHWDeviceDesc, lpd3dHWDeviceDesc, sizeof(D3DDEVICEDESC) );   

   return D3DENUMRET_CANCEL;
}






HRESULT CreateDirect3D( HWND hwnd )
{   
   HRESULT hRes;
   PrintLog("\n");
   PrintLog("=== Init Direct3D ===\n" );

   hRes = lpDD->SetCooperativeLevel( hwnd, DDSCL_EXCLUSIVE|DDSCL_FULLSCREEN );
   if (FAILED(hRes)) DoHalt("Error setting cooperative level\n");      

   hRes = lpDD->SetDisplayMode( WinW, WinH, 16 );
   if (FAILED(hRes)) {
	   PrintLog("DDraw: can't set selected video mode\n");
	   WinW = 640;
	   WinH = 480;
	   OptRes = 3;
	   hRes = lpDD->SetDisplayMode( WinW, WinH, 16 );
   }
   if (FAILED(hRes)) DoHalt("Error setting display mode\n");
   wsprintf(logt, "Set Display mode %dx%d, 16bpp\n", WinW, WinH);
   PrintLog(logt);
   
   hRes = lpDD->QueryInterface( IID_IDirect3D, (LPVOID*) &lpd3d);
   if (FAILED(hRes)) DoHalt("Error quering Direct3D interface\n");
   PrintLog("QueryInterface: Ok. (IID_IDirect3D)\n");
            
   DDSURFACEDESC       ddsd;
   ZeroMemory(&ddsd, sizeof(ddsd));
   ddsd.dwSize         = sizeof(ddsd);
   ddsd.dwFlags        = DDSD_CAPS;
   ddsd.ddsCaps.dwCaps = DDSCAPS_PRIMARYSURFACE;
   hRes = lpDD->CreateSurface( &ddsd, &lpddPrimary, NULL );
   if (FAILED(hRes)) DoHalt( "Error creating primary surface\n");
   
   ZeroMemory(&ddsd, sizeof(ddsd) );
   ddsd.dwSize = sizeof(ddsd);
   hRes = lpddPrimary->GetSurfaceDesc( &ddsd);
   if (FAILED(hRes)) DoHalt("Error getting surface description\n");
   PrintLog("CreateSurface: Ok. (Primary)\n");

   dwDeviceBitDepth = BitDepthToFlags(ddsd.ddpfPixelFormat.dwRGBBitCount);

   fDeviceFound = FALSE;
   hRes = lpd3d->EnumDevices( EnumDeviceCallback, &fDeviceFound);
   if (FAILED(hRes) ) DoHalt("EnumDevices failed.\n");
   if (!fDeviceFound ) DoHalt("No devices found.\n");
   PrintLog("EnumDevices: Ok.\n");
   
   return DD_OK;
}




HRESULT CreateDevice(DWORD dwWidth, DWORD dwHeight)
{
   DDSURFACEDESC   ddsd;
   HRESULT         hRes;
   DWORD           dwZBufferBitDepth;   
   
   ZeroMemory(&ddsd, sizeof(ddsd));
   ddsd.dwSize         = sizeof(ddsd);
   ddsd.dwFlags        = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT;
   ddsd.dwWidth        = dwWidth;
   ddsd.dwHeight       = dwHeight;
   ddsd.ddsCaps.dwCaps = DDSCAPS_3DDEVICE | DDSCAPS_OFFSCREENPLAIN | DDSCAPS_VIDEOMEMORY;
   hRes = lpDD->CreateSurface( &ddsd, &lpddBack, NULL);
   if (FAILED(hRes)) DoHalt("Error creating back buffer surface\n");
   PrintLog("CreateSurface: Ok. (BackBuffer)\n");   

         
   if( d3dHWDeviceDesc.dwDeviceZBufferBitDepth != 0UL ) {
      dwZBufferBitDepth = FlagsToBitDepth( d3dHWDeviceDesc.dwDeviceZBufferBitDepth );

      ZeroMemory(&ddsd, sizeof(ddsd));
      ddsd.dwSize            = sizeof(ddsd);
      ddsd.dwFlags           = DDSD_CAPS|DDSD_WIDTH|DDSD_HEIGHT|DDSD_ZBUFFERBITDEPTH;
      ddsd.ddsCaps.dwCaps    = DDSCAPS_ZBUFFER | DDSCAPS_VIDEOMEMORY;
      ddsd.dwWidth           = dwWidth;
      ddsd.dwHeight          = dwHeight;
      ddsd.dwZBufferBitDepth = 16;//dwZBufferBitDepth;
      hRes = lpDD->CreateSurface( &ddsd, &lpddZBuffer, NULL);
/*
	  if (FAILED(hRes)) {
	   ddsd.dwZBufferBitDepth = 16;//dwZBufferBitDepth;
       hRes = lpDD->CreateSurface( &ddsd, &lpddZBuffer, NULL);
	   PrintLog("Z-Buffer: 16 bit\n");
	  } else 
	   PrintLog("Z-Buffer: 32 bit\n");
*/
      if (FAILED(hRes)) DoHalt("Error creating z-buffer\n");
         
      hRes = lpddBack->AddAttachedSurface( lpddZBuffer );
      if (FAILED(hRes)) DoHalt("Error attaching z-buffer\n");         
	  PrintLog("CreateSurface: Ok. (Z-buffer)\n");
   }

      
   hRes = lpddBack->QueryInterface( guidDevice, (LPVOID*) &lpd3dDevice);
   if (FAILED(hRes)) DoHalt("Error quering device interface\n");           
   PrintLog("QueryInterface: Ok. (lpd3dDevice)\n");

   return DD_OK;
}




HRESULT CreateScene(void)
{

    HRESULT              hRes;
    DWORD                dwVertexSize;
    DWORD                dwInstructionSize;
    DWORD                dwExecuteBufferSize;
    D3DEXECUTEBUFFERDESC d3dExecuteBufferDesc;
    D3DEXECUTEDATA       d3dExecuteData;        

    hRes = lpd3d->CreateViewport( &lpd3dViewport, NULL );
    if (FAILED(hRes)) DoHalt("Error creating viewport\n");
    PrintLog("CreateViewport: Ok.\n");

    hRes = lpd3dDevice->AddViewport( lpd3dViewport );
    if (FAILED(hRes)) DoHalt("Error adding viewport\n");
	//PrintLog("AddViewport: Ok.\n");
       

    D3DVIEWPORT d3dViewport;   
    ZeroMemory(&d3dViewport, sizeof(d3dViewport));
    d3dViewport.dwSize   = sizeof(d3dViewport);
    d3dViewport.dwX      = 0UL;
    d3dViewport.dwY      = 0UL;
    d3dViewport.dwWidth  = (DWORD)WinW;
    d3dViewport.dwHeight = (DWORD)WinH;
    d3dViewport.dvScaleX = D3DVAL((float)d3dViewport.dwWidth / 2.0);
    d3dViewport.dvScaleY = D3DVAL((float)d3dViewport.dwHeight / 2.0);
    d3dViewport.dvMaxX   = D3DVAL(1.0);
    d3dViewport.dvMaxY   = D3DVAL(1.0);
    
	lpd3dViewport->SetViewport( &d3dViewport);

    
    //=========== CREATING EXECUTE BUFFER ======================//
    dwVertexSize        = ((d3debvc*3)            * sizeof(D3DVERTEX));
    dwInstructionSize   = (NUM_INSTRUCTIONS    * sizeof(D3DINSTRUCTION))     +
                          (NUM_STATES          * sizeof(D3DSTATE))           +
                          (NUM_PROCESSVERTICES * sizeof(D3DPROCESSVERTICES)) +
                          ((d3debvc)              * sizeof(D3DTRIANGLE));

    dwExecuteBufferSize = dwVertexSize + dwInstructionSize;
    ZeroMemory(&d3dExecuteBufferDesc, sizeof(d3dExecuteBufferDesc));
    d3dExecuteBufferDesc.dwSize       = sizeof(d3dExecuteBufferDesc);
    d3dExecuteBufferDesc.dwFlags      = D3DDEB_BUFSIZE;
    d3dExecuteBufferDesc.dwBufferSize = dwExecuteBufferSize;
    hRes = lpd3dDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpd3dExecuteBuffer, NULL);	
	if (FAILED(hRes)) DoHalt( "Error creating execute buffer\n");       
    PrintLog("CreateExecuteBuffer: Ok.\n");

	ZeroMemory(&d3dExecuteData, sizeof(d3dExecuteData));
    d3dExecuteData.dwSize              = sizeof(d3dExecuteData);
    d3dExecuteData.dwVertexCount       = d3debvc;
    d3dExecuteData.dwInstructionOffset = dwVertexSize;
    d3dExecuteData.dwInstructionLength = dwInstructionSize;	
    hRes = lpd3dExecuteBuffer->SetExecuteData( &d3dExecuteData );
    if (FAILED(hRes)) DoHalt("Error setting execute data\n");


		//=========== CREATING EXECUTE BUFFER ======================//
    dwVertexSize        = ((d3debvcG*3)            * sizeof(D3DVERTEX));
    dwInstructionSize   = (300                * sizeof(D3DINSTRUCTION))     +
                          (300                * sizeof(D3DSTATE))           +
                          (10                 * sizeof(D3DPROCESSVERTICES)) +
                          ((d3debvcG)              * sizeof(D3DTRIANGLE));

    dwExecuteBufferSize = dwVertexSize + dwInstructionSize;
    ZeroMemory(&d3dExecuteBufferDesc, sizeof(d3dExecuteBufferDesc));
    d3dExecuteBufferDesc.dwSize       = sizeof(d3dExecuteBufferDesc);
    d3dExecuteBufferDesc.dwFlags      = D3DDEB_BUFSIZE;
    d3dExecuteBufferDesc.dwBufferSize = dwExecuteBufferSize;
    hRes = lpd3dDevice->CreateExecuteBuffer( &d3dExecuteBufferDesc, &lpd3dExecuteBufferG, NULL);	
	if (FAILED(hRes)) DoHalt( "Error creating execute buffer\n");       
    PrintLog("CreateExecuteBuffer: Ok.\n");

	ZeroMemory(&d3dExecuteData, sizeof(d3dExecuteData));
    d3dExecuteData.dwSize              = sizeof(d3dExecuteData);
    d3dExecuteData.dwVertexCount       = d3debvcG;
    d3dExecuteData.dwInstructionOffset = dwVertexSize;
    d3dExecuteData.dwInstructionLength = dwInstructionSize;	
    hRes = lpd3dExecuteBufferG->SetExecuteData( &d3dExecuteData );
    if (FAILED(hRes)) DoHalt("Error setting execute data\n");
       



	FillExecuteBuffer_State(lpd3dExecuteBuffer);
    hRes = lpd3dDevice->Execute( lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);
	
    return DD_OK;		
}




void Init3DHardware()
{
   HARD3D = TRUE;
   PrintLog("\n");
   PrintLog("==Init Direct Draw==\n");
   HRESULT hres;

   hres = DirectDrawCreate( NULL, &lpDD, NULL );
   if( hres != DD_OK ) {      
	  wsprintf(logt, "DirectDrawCreate Error: %Xh\n", hres);      
	  DoHalt(logt);	   }
   PrintLog("DirectDrawCreate: Ok\n");
   
   DirectActive = TRUE;
}



void d3dDetectCaps()
{

	for (int t=0; t<d3dmemmapsize; t++) {
		if (!d3dAllocTexture(t, 256, 256)) break;
	}

	d3dTexturesMem = t*256*256*2;


    d3dDownLoadTexture(0, 256, 256, SkyPic);	
	DWORD T;
	T = timeGetTime();	
	for (t=0; t<10; t++) d3dDownLoadTexture(0, 256, 256, SkyPic);
	T = timeGetTime() - T;	
	
	wsprintf(logt, "DETECTED: Texture memory : %dK.\n", d3dTexturesMem>>10);
	PrintLog(logt);
	ResetTextureMap();

	wsprintf(logt, "DETECTED: Texture transfer speed: %dK/sec.\n", 128*10000 / T);
	PrintLog(logt);


	DDSURFACEDESC ddsd;	 
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
    ddsd.dwSize = sizeof(DDSURFACEDESC);
	if( lpddBack->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) return;    
	lpddBack->Unlock(ddsd.lpSurface);
	if (ddsd.ddpfPixelFormat.dwGBitMask == 0x3E0) VMFORMAT565=FALSE; else VMFORMAT565=TRUE;
	if (VMFORMAT565) 
		PrintLog("DETECTED: PixelFormat RGB565\n");			
	else {
		PrintLog("DETECTED: PixelFormat RGB555\n");
        if (!STARTCONV555) {
			STARTCONV555 = TRUE;
		    conv_pic555(PausePic);
		    conv_pic555(ExitPic);
		    conv_pic555(TrophyExit);		    
			conv_pic555(MapPic);		
		    conv_pic555(TrophyPic); 
			for (int c=0; c<20; c++) 
				if (DinoInfo[c].CallIcon.W)
					conv_pic555(DinoInfo[c].CallIcon);

			for (int w=0; w<TotalW; w++) 
			 if (Weapon.BulletPic[w].W)
		       conv_pic555(Weapon.BulletPic[w]);
		}		
	}
		
    
}


void d3dTestDrawTri(DWORD color, float tx)
{
	 d3dStartBuffer();
	 
	 lpVertex->sx       = 0;
     lpVertex->sy       = 0;
     lpVertex->sz       = _ZSCALE / 10;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = color;
	 lpVertex->specular = 0xFF000000;
     lpVertex->tu       = tx;
     lpVertex->tv       = tx;
     lpVertex++;

	 lpVertex->sx       = 100;
     lpVertex->sy       = 0;
     lpVertex->sz       = _ZSCALE / 10;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = color;
	 lpVertex->specular = 0xFF000000;
     lpVertex->tu       = tx;
     lpVertex->tv       = tx;
     lpVertex++;

	 lpVertex->sx       = 0;
     lpVertex->sy       = 100;
     lpVertex->sz       = _ZSCALE / 10;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = color;
	 lpVertex->specular = 0xFF000000;
     lpVertex->tu       = tx;
     lpVertex->tv       = tx;
     lpVertex++;
       
     d3dFlushBuffer(0, 1);
	 
}


int  d3dTestAlpha()
{    
	int RF = 0;
	
    lpd3dDevice->BeginScene( );

//	RenderSkyPlane();
	d3dSetTexture(TFX_SPECULAR.lpImage, TFX_SPECULAR.W, TFX_SPECULAR.W);
	
	DDBLTFX ddbltfx;
	ddbltfx.dwSize = sizeof( DDBLTFX );  
    ddbltfx.dwFillColor = 0xFFFF;              
    lpddBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );  

	FillExecuteBuffer_State(lpd3dExecuteBuffer);
    hRes = lpd3dDevice->Execute( lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);

//========= test opacity ==========//
   
    //d3dSetTexture(TFX_SPECULAR.lpImage, TFX_SPECULAR.W, TFX_SPECULAR.W);            
    SetRenderStates(FALSE, D3DBLEND_INVSRCALPHA);

   d3dTestDrawTri(0xFFFFFFFF, 0.0);   
    
    
   CopyBackToDIB();
   if (*((WORD*)lpVideoBuf)==0) PrintLog("[OPACITY: Failed] ");
   else { PrintLog("[OPACITY: Ok.] "); RF+=4; }
   lpd3dDevice->EndScene( );
   lpd3dDevice->BeginScene( );
/*
   ShowVideo();
   lpd3dDevice->BeginScene( );
   Sleep(1000);
*/

//=========== test transparent mode =============//
   lpd3dDevice->BeginScene( );
   ddbltfx.dwSize = sizeof( DDBLTFX );  
   ddbltfx.dwFillColor = 0x0000;              
   lpddBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );  

   d3dTestDrawTri(0x00FFFFFF, 0.5);
	 
   CopyBackToDIB();
   if (*((WORD*)lpVideoBuf)!=0) PrintLog("[TRANSPARENT: Failed] ");
   else { PrintLog("[TRANSPARENT: Ok.] "); RF+=2; }
   lpd3dDevice->EndScene( );
   lpd3dDevice->BeginScene( );
/*
   ShowVideo();
   lpd3dDevice->BeginScene( );
   Sleep(1000);
*/
//=========== test env map ==================//
   SetRenderStates(FALSE, D3DBLEND_ONE);

   lpd3dDevice->BeginScene( );
   ddbltfx.dwSize = sizeof( DDBLTFX );  
   ddbltfx.dwFillColor = 0x0000;              
   lpddBack->Blt( NULL, NULL, NULL, DDBLT_COLORFILL | DDBLT_WAIT, &ddbltfx );  

   d3dTestDrawTri(0x00FFFFFF, 0.5);

   CopyBackToDIB();
   if (*((WORD*)lpVideoBuf)!=0) PrintLog("[ENV.BLEND : Failed]\n");
   else { PrintLog("[ENV.BLEND : Ok.]\n"); RF+=1; }
   lpd3dDevice->EndScene( );
   lpd3dDevice->BeginScene( );
/*
   ShowVideo();
   lpd3dDevice->BeginScene( );
   Sleep(1000);
*/
   SetRenderStates(TRUE, D3DBLEND_INVSRCALPHA);	
   lpd3dDevice->EndScene( );  
   return RF;
}



void Activate3DHardware()
{   	
	SetVideoMode(WinW,WinH);

    HRESULT hRes = CreateDirect3D(hwndMain);
    if (FAILED(hRes)) DoHalt("CreateDirect3D Failed.\n");

	hRes = CreateDevice((DWORD)WinW, (DWORD)WinH);
    if (FAILED(hRes))  DoHalt("Create Device Failed.\n");

	d3dClearBuffers();

    hRes = CreateScene();
    if (FAILED(hRes))  DoHalt("CreateScene Failed.\n");   

	d3dDetectCaps();	

	OPT_ALPHA_COLORKEY=FALSE;
	PrintLog("TEST COLOR KEY: ");
	int r1 = d3dTestAlpha();
	
	OPT_ALPHA_COLORKEY=TRUE;
    PrintLog("TEST ALPHA KEY: ");
	int r2 = d3dTestAlpha();
	OPT_ALPHA_COLORKEY = (r2>=r1);
	if (OPT_ALPHA_COLORKEY) PrintLog("ALPHA KEY mode selected.\n");
	                   else PrintLog("COLOR KEY mode selected.\n");
	
	hRes = lpd3dDevice->BeginScene( );

	if (OptText==0) LOWRESTX = TRUE;
    if (OptText==1) LOWRESTX = FALSE;
	if (OptText==2) LOWRESTX = FALSE;
    d3dMemLoaded = 0;
	                   
    D3DACTIVE = TRUE;    
	
    d3dLastTexture = d3dmemmapsize+1;
	PrintLog("=== Direct3D started === \n");
	PrintLog("\n");

}


void ResetTextureMap()
{
  d3dEndBufferG(FALSE);

  d3dMemUsageCount = 0;
  //d3dMemLoaded = 0;
  d3dLastTexture = d3dmemmapsize+1;
  for (int m=0; m<d3dmemmapsize+2; m++) {
      d3dMemMap[m].lastused    = 0;
      d3dMemMap[m].cpuaddr     = 0;
	  if (d3dMemMap[m].lpddTexture) {
		  d3dMemMap[m].lpddTexture->Release();
		  d3dMemMap[m].lpddTexture = NULL;
	  }      
  }
}



void ShutDown3DHardware()
{
  D3DACTIVE = FALSE;
  
  if (lpd3dDevice)
    hRes = lpd3dDevice->EndScene();
  
  ResetTextureMap();

  lpInstructionG = NULL;
  lpVertexG      = NULL;

  if (NULL != lpd3dExecuteBuffer) {
      lpd3dExecuteBuffer->Release( );
	  lpd3dExecuteBufferG->Release( );	  
      lpd3dExecuteBuffer = NULL;
   }

  if (NULL != lpd3dViewport) {
      lpd3dViewport->Release( );
      lpd3dViewport = NULL;
   }

  if (NULL != lpd3dDevice) {
      lpd3dDevice->Release( );
      lpd3dDevice = NULL;
   }

  if (NULL != lpddZBuffer) {
      lpddZBuffer->Release( );
      lpddZBuffer = NULL;
   }
 
  if (NULL != lpddBack) {
      lpddBack->Release();
      lpddBack = NULL;
   }

  if (NULL != lpddPrimary) {
        lpddPrimary->Release( );
        lpddPrimary = NULL;
   }

  if (NULL != lpd3d) {
      lpd3d->Release( );
      lpd3d = NULL;
   }

  lpDD->SetCooperativeLevel( hwndMain, DDSCL_NORMAL);
}





void InsertFxMM(int m)
{
   for (int mm=d3dmemmapsize-1; mm>m; mm--)
    d3dMemMap[m] = d3dMemMap[m-1];
}



BOOL d3dAllocTexture(int i, int w, int h)
{
   DDSURFACEDESC ddsd;
   DDPIXELFORMAT ddpf;
   ZeroMemory( &ddpf, sizeof(DDPIXELFORMAT) );
   ddpf.dwSize  = sizeof(DDPIXELFORMAT);
   ddpf.dwFlags = DDPF_RGB; 

   if (OPT_ALPHA_COLORKEY) {
   ddpf.dwFlags |= DDPF_ALPHAPIXELS;
   ddpf.dwRGBAlphaBitMask  = 0x8000;   
   }

   ddpf.dwRGBBitCount = 16;   
   ddpf.dwRBitMask = 0x7c00;
   ddpf.dwGBitMask = 0x3e0; 
   ddpf.dwBBitMask = 0x1f;      

   ZeroMemory(&ddsd, sizeof(ddsd));
   ddsd.dwSize          = sizeof(ddsd);
   ddsd.dwFlags         = DDSD_CAPS | DDSD_WIDTH | DDSD_HEIGHT | DDSD_PIXELFORMAT;
   ddsd.dwWidth         = w;
   ddsd.dwHeight        = h;
   CopyMemory( &ddsd.ddpfPixelFormat, &ddpf, sizeof(DDPIXELFORMAT) );

   ddsd.ddsCaps.dwCaps = DDSCAPS_TEXTURE | DDSCAPS_VIDEOMEMORY;

   hRes = lpDD->CreateSurface( &ddsd, &d3dMemMap[i].lpddTexture, NULL);
   if (FAILED(hRes)) {
	  d3dMemMap[i].lpddTexture = NULL;      
      return FALSE;   }
   
   DDCOLORKEY ddck;
   ddck.dwColorSpaceLowValue = ddck.dwColorSpaceHighValue = 0x0000;
   d3dMemMap[i].lpddTexture->SetColorKey(DDCKEY_SRCBLT, &ddck);
   
   return TRUE;
}



void d3dDownLoadTexture(int i, int w, int h, LPVOID tptr)   
{   
   DDSURFACEDESC ddsd;

   ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
   ddsd.dwSize = sizeof(DDSURFACEDESC);
   



   if( d3dMemMap[i].lpddTexture->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) return;
   CopyMemory( ddsd.lpSurface, tptr, w*h*2 );
   d3dMemMap[i].lpddTexture->Unlock( ddsd.lpSurface );   

   IDirect3DTexture* Texture;
   if( d3dMemMap[i].lpddTexture->QueryInterface( IID_IDirect3DTexture, (LPVOID*)&Texture ) != S_OK ) return;
   if( Texture->GetHandle( lpd3dDevice, &d3dMemMap[i].hTexture ) != D3D_OK ) return;
   Texture->Release( );

   d3dMemMap[i].cpuaddr = (int) tptr;
   d3dMemMap[i].size    = w*h*2;
   d3dMemLoaded+=w*h*2;
   //---------------------------------------------------------------------------
}


int DownLoadTexture(LPVOID tptr, int w, int h)
{   
   int textureSize = w*w*2;
   int fxm = 0;
   int m;

//========== if no memory used ==========//
   if (!d3dMemMap[0].cpuaddr) {
     d3dAllocTexture(0, w, h);
	 d3dDownLoadTexture(0, w, h, tptr);
     return 0;
   }


//====== search for last used block and try to alloc next ============//
   for (m = 0; m < d3dmemmapsize; m++) 
	 if (!d3dMemMap[m].cpuaddr) 
		 if (d3dAllocTexture(m, w, h)) {
			 d3dDownLoadTexture(m, w, h, tptr);
             return m;
		 } else break;

   
		    
//====== search for unused texture and replace it with new ============//
   int unusedtime = 2;
   int rt = -1;
   for (m = 0; m < d3dmemmapsize; m++) {
	 if (!d3dMemMap[m].cpuaddr) break;
	 if (d3dMemMap[m].size != w*h*2) continue;

	 int ut = d3dMemUsageCount - d3dMemMap[m].lastused;
     
	 if (ut >= unusedtime) {
            unusedtime = ut;
            rt = m;   }
	}

   if (rt!=-1) {
	   d3dDownLoadTexture(rt, w, h, tptr);
	   return rt;
   }
   
   ResetTextureMap();

   d3dAllocTexture(0, w, h);
   d3dDownLoadTexture(0, w, h, tptr);
   return 0;
}




void d3dSetTexture(LPVOID tptr, int w, int h)
{    
	
  if (d3dMemMap[d3dLastTexture].cpuaddr == (int)tptr) return;

  int fxm = -1;
  for (int m=0; m<d3dmemmapsize; m++) {
     if (d3dMemMap[m].cpuaddr == (int)tptr) { fxm = m; break; }
     if (!d3dMemMap[m].cpuaddr) break;
  }

  if (fxm==-1) fxm = DownLoadTexture(tptr, w, h);

  d3dMemMap[fxm].lastused = d3dMemUsageCount;  
  hTexture = d3dMemMap[fxm].hTexture;  
  d3dLastTexture = fxm;

  
}






float GetTraceK(int x, int y)
{	
	
  if (x<8 || y<8 || x>WinW-8 || y>WinH-8) return 0.f;
  
  float k = 0;

  DDSURFACEDESC ddsd;	 
  ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  if( lpddZBuffer->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) {	  
	  return 0;
  }

  WORD CC = 200;
  int bw = (ddsd.lPitch>>1);
  if ( *((WORD*)ddsd.lpSurface + (y+0)*bw  + x+0) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y+10)*bw + x+0) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y-10)*bw + x+0) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y+0)*bw  + x+10) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y+0)*bw  + x-10) < CC ) k+=1.f;

  if ( *((WORD*)ddsd.lpSurface + (y+8)*bw + x+8) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y+8)*bw + x-8) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y-8)*bw + x+8) < CC ) k+=1.f;
  if ( *((WORD*)ddsd.lpSurface + (y-8)*bw + x-8) < CC ) k+=1.f;  
    
  lpddZBuffer->Unlock(ddsd.lpSurface);
  k/=9.f;
  
  DeltaFunc(TraceK, k, TimeDt / 1024.f);
  return TraceK;
}


void AddSkySum(WORD C)
{  
  int R,G,B;

  if (VMFORMAT565) {
	  R = C>>11;  G = (C>>5) & 63; B = C & 31;
  } else {
	  R = C>>10; G = (C>>5) & 31; B = C & 31; C=C*2;
  }
  
  SkySumR += R*8;
  SkySumG += G*4;
  SkySumB += B*8;
}


float GetSkyK(int x, int y)
{	
  if (x<10 || y<10 || x>WinW-10 || y>WinH-10) return 0.5;
  SkySumR = 0;
  SkySumG = 0;
  SkySumB = 0;  
  float k = 0;

  DDSURFACEDESC ddsd;	 
  ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  if( lpddBack->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) {	  
	  return 0;
  }
  
  int bw = (ddsd.lPitch>>1);
  AddSkySum(*((WORD*)ddsd.lpSurface + (y+0)*bw + x+0));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y+6)*bw + x+0));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y-6)*bw + x+0));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y+0)*bw + x+6));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y+0)*bw + x-6));

  AddSkySum(*((WORD*)ddsd.lpSurface + (y+4)*bw + x+4));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y+4)*bw + x-4));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y-4)*bw + x+4));
  AddSkySum(*((WORD*)ddsd.lpSurface + (y-4)*bw + x-4));
    
  lpddBack->Unlock(ddsd.lpSurface);
  
  SkySumR-=SkyTR*9;
  SkySumG-=SkyTG*9;
  SkySumB-=SkyTB*9;

  k = (float)sqrt(SkySumR*SkySumR + SkySumG*SkySumG + SkySumB*SkySumB) / 9;  

  if (k>80) k = 80;
  if (k<  0) k = 0;
  k = 1.0f - k/80.f;
  if (k<0.2) k=0.2f;
  if (OptDayNight==2) k=0.3 + k/2.75;
  DeltaFunc(SkyTraceK, k, (0.07f + (float)fabs(k-SkyTraceK)) * (TimeDt / 512.f) );
  return SkyTraceK;
}







void TryHiResTx()
{
  int UsedMem = 0;
  for (int m=0; m<d3dmemmapsize; m++) {
   if (!d3dMemMap[m].cpuaddr) break;   
   if (d3dMemMap[m].lastused+2>=d3dMemUsageCount)
      UsedMem+= d3dMemMap[m].size;
  }
/*
  wsprintf(logt, "TOTALL: %d USED: %d", d3dTexturesMem, UsedMem);
  AddMessage(logt);
*/
  if (UsedMem*4 < (int)d3dTexturesMem)
    LOWRESTX = FALSE;  
}


void ShowVideo()
{	
	/*
  char t[128];
  wsprintf(t, "T-mem loaded: %dK", d3dMemLoaded >> 10);
  if (d3dMemLoaded) AddMessage(t);
  */

   if (d3dMemLoaded > 200*1024) LowHardMemory++;
                           else LowHardMemory=0;
   if (LowHardMemory>2) {
	   LOWRESTX = TRUE;
	   LowHardMemory = 0;  }

   if (OptText==0) LOWRESTX = TRUE;
   if (OptText==1) LOWRESTX = FALSE;
   if (OptText==2)
      if (LOWRESTX && (Takt & 63)==0) TryHiResTx();



  if (UNDERWATER)
	  RenderFSRect(CurFogColor+0x70000000);  
	  
  if (OptDayNight!=2)
  if (!UNDERWATER && (SunLight>1.0f) ) {   
   RenderFSRect(0xFFFFC0 + ((int)SunLight<<24));
  }
  
  RenderHealthBar();

   
  d3dMemUsageCount++;  
  d3dMemLoaded = 0;


  hRes = lpd3dDevice->EndScene();

  hRes = lpddPrimary->Blt( NULL, lpddBack, NULL, DDBLT_WAIT, NULL );  
  
  d3dClearBuffers();

  hRes = lpd3dDevice->BeginScene( );
  
}



void CopyBackToDIB()
{
  DDSURFACEDESC ddsd;	 
  ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  if( lpddBack->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) return;
    
  WORD *lpVMem = (WORD*) ddsd.lpSurface;
  ddsd.lPitch/=2;

  for (int y=0; y<=256; y++) 
		CopyMemory( (WORD*)lpVideoBuf + y*1024,
		            lpVMem + y*ddsd.lPitch, 
					256*2);
    
  lpddBack->Unlock(ddsd.lpSurface);
}

void CopyHARDToDIB()
{
  DDSURFACEDESC ddsd;	 
  ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  if( lpddPrimary->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) return;
    
  WORD *lpVMem = (WORD*) ddsd.lpSurface;
  ddsd.lPitch/=2;

  for (int y=0; y<=WinH; y++) 
		CopyMemory( (WORD*)lpVideoBuf + y*1024,
		            lpVMem + y*ddsd.lPitch, 
					WinW*2);
    
  lpddPrimary->Unlock(ddsd.lpSurface);
}





void FXPutBitMap(int x0, int y0, int w, int h, int smw, LPVOID lpData)
{  
    DDSURFACEDESC ddsd;	 
	ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
    ddsd.dwSize = sizeof(DDSURFACEDESC);
    if( lpddBack->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) return;
    
	WORD *lpVMem = (WORD*) ddsd.lpSurface;
	ddsd.lPitch/=2;
	lpVMem+=x0+y0 * ddsd.lPitch;

	for (int y=0; y<h; y++) 
		CopyMemory( lpVMem + y*ddsd.lPitch, ((WORD*)lpData)+y*smw, w*2);
    
	lpddBack->Unlock(ddsd.lpSurface);
}



void DrawPicture(int x, int y, TPicture &pic)
{
  FXPutBitMap(x, y, pic.W, pic.H, pic.W, pic.lpImage);
}



void ddTextOut(int x, int y, LPSTR t, int color)
{
  lpddBack->GetDC( &ddBackDC );
  SetBkMode( ddBackDC, TRANSPARENT );

  HFONT oldfont;
  if (SmallFont) oldfont = (HFONT)SelectObject(ddBackDC, fnt_Small);
    
  SetTextColor(ddBackDC, 0x00101010);
  TextOut(ddBackDC, x+2, y+1, t, strlen(t));

  SetTextColor(ddBackDC, color);
  TextOut(ddBackDC, x+1, y, t, strlen(t));

  if (SmallFont) SelectObject(ddBackDC, oldfont);
  
  lpddBack->ReleaseDC( ddBackDC );
}


void DrawTrophyText(int x0, int y0)
{
	int x;
	SmallFont = TRUE;
    HFONT oldfont = (HFONT)SelectObject(hdcMain, fnt_Small);  
	int tc = TrophyBody;
	
	int   dtype = TrophyRoom.Body[tc].ctype;
	int   time  = TrophyRoom.Body[tc].time;
	int   date  = TrophyRoom.Body[tc].date;
	int   wep   = TrophyRoom.Body[tc].weapon;
	int   score = TrophyRoom.Body[tc].score;
	float scale = TrophyRoom.Body[tc].scale;
	float range = TrophyRoom.Body[tc].range;
	char t[32];

	x0+=14; y0+=18;
    x = x0;
	ddTextOut(x, y0   , "Name: ", 0x00BFBFBF);  x+=GetTextW(hdcMain,"Name: ");
    ddTextOut(x, y0   , DinoInfo[dtype].Name, 0x0000BFBF);    

	x = x0;
	ddTextOut(x, y0+16, "Weight: ", 0x00BFBFBF);  x+=GetTextW(hdcMain,"Weight: ");
	if (OptSys)
     sprintf(t,"%3.2ft ", DinoInfo[dtype].Mass * scale * scale / 0.907);
	else
     sprintf(t,"%3.2fT ", DinoInfo[dtype].Mass * scale * scale);     

    ddTextOut(x, y0+16, t, 0x0000BFBF);    x+=GetTextW(hdcMain,t);
    ddTextOut(x, y0+16, "Length: ", 0x00BFBFBF); x+=GetTextW(hdcMain,"Length: ");
     
	if (OptSys)
	 sprintf(t,"%3.2fft", DinoInfo[dtype].Length * scale / 0.3);
	else
	 sprintf(t,"%3.2fm", DinoInfo[dtype].Length * scale);

	ddTextOut(x, y0+16, t, 0x0000BFBF); 
	
	x = x0;
	ddTextOut(x, y0+32, "Weapon: ", 0x00BFBFBF);  x+=GetTextW(hdcMain,"Weapon: ");
	 wsprintf(t,"%s    ", WeapInfo[wep].Name);
    ddTextOut(x, y0+32, t, 0x0000BFBF);   x+=GetTextW(hdcMain,t);
    ddTextOut(x, y0+32, "Score: ", 0x00BFBFBF);   x+=GetTextW(hdcMain,"Score: ");
	 wsprintf(t,"%d", score);
	ddTextOut(x, y0+32, t, 0x0000BFBF); 


	x = x0;
	ddTextOut(x, y0+48, "Range of kill: ", 0x00BFBFBF);  x+=GetTextW(hdcMain,"Range of kill: ");
	if (OptSys) sprintf(t,"%3.1fft", range / 0.3);
	else        sprintf(t,"%3.1fm", range);
    ddTextOut(x, y0+48, t, 0x0000BFBF);  


	x = x0;
	ddTextOut(x, y0+64, "Date: ", 0x00BFBFBF);  x+=GetTextW(hdcMain,"Date: ");
	if (OptSys)
	 wsprintf(t,"%d.%d.%d   ", ((date>>10) & 255), (date & 255), date>>20);
	else
     wsprintf(t,"%d.%d.%d   ", (date & 255), ((date>>10) & 255), date>>20);

    ddTextOut(x, y0+64, t, 0x0000BFBF);   x+=GetTextW(hdcMain,t);
    ddTextOut(x, y0+64, "Time: ", 0x00BFBFBF);   x+=GetTextW(hdcMain,"Time: ");
	 wsprintf(t,"%d:%02d", ((time>>10) & 255), (time & 255));
	ddTextOut(x, y0+64, t, 0x0000BFBF); 

	SmallFont = FALSE;

	SelectObject(hdcMain, oldfont);
	
}




void Render_LifeInfo(int li)
{
    int x,y;
	SmallFont = TRUE;
    //HFONT oldfont = SelectObject(hdcMain, fnt_Small);
		
	int    ctype = Characters[li].CType;
	float  scale = Characters[li].scale;	
	char t[32];
	
    x = VideoCX + WinW / 64;
	y = VideoCY + (int)(WinH / 6.8);
		
    ddTextOut(x, y, DinoInfo[ctype].Name, 0x0000b000);    
		
	if (OptSys) sprintf(t,"Weight: %3.2ft ", DinoInfo[ctype].Mass * scale * scale / 0.907);
	else        sprintf(t,"Weight: %3.2fT ", DinoInfo[ctype].Mass * scale * scale);     
    
	ddTextOut(x, y+16, t, 0x0000b000);

	int R  = (int)(VectorLength( SubVectors(Characters[li].pos, PlayerPos) )*3 / 64.f);
	if (OptSys) sprintf(t,"Distance: %dft ", R);
	else        sprintf(t,"Distance: %dm  ", R/3);     

	ddTextOut(x, y+32, t, 0x0000b000);
    
	SmallFont = FALSE;
	//SelectObject(hdcMain, oldfont);	
}


void RenderFXMMap()
{
  
}


void ShowControlElements()
{        
  char buf[128];
  
  //ddTextOut(100, 100, "!", 0x0020A0A0);

  lpddBack->GetDC( &ddBackDC );
  lpddBack->ReleaseDC( ddBackDC );

  
  if (TIMER) {
   wsprintf(buf,"msc: %d", TimeDt);
   ddTextOut(WinEX-81, 11, buf, 0x0020A0A0);

   wsprintf(buf,"polys: %d", dFacesCount);   
   ddTextOut(WinEX-90, 24, buf, 0x0020A0A0);

   wsprintf(buf,"%d", Env);   
   ddTextOut(10, 24, buf, 0x0020A0A0);
  }

  if (MessageList.timeleft) {
    if (RealTime>MessageList.timeleft) MessageList.timeleft = 0;
    ddTextOut(10, 10, MessageList.mtext, 0x0020A0A0);
  } 

  if (ExitTime) {	  
	  int y = WinH / 3;
	  wsprintf(buf,"Preparing for evacuation...");
      ddTextOut(VideoCX - GetTextW(hdcCMain, buf)/2, y, buf, 0x0060C0D0);
	  wsprintf(buf,"%d seconds left.", 1 + ExitTime / 1000);
	  ddTextOut(VideoCX - GetTextW(hdcCMain, buf)/2, y + 18, buf, 0x0060C0D0);
  }  
}









void ClipVector(CLIPPLANE& C, int vn)
{
  int ClipRes = 0;
  float s,s1,s2;
  int vleft  = (vn-1); if (vleft <0) vleft=vused-1;
  int vright = (vn+1); if (vright>=vused) vright=0;
  
  MulVectorsScal(cp[vn].ev.v, C.nv, s); /*s=SGN(s-0.01f);*/
  if (s>=0) return;

  MulVectorsScal(cp[vleft ].ev.v, C.nv, s1); /* s1=SGN(s1+0.01f); */ //s1+=0.0001f;
  MulVectorsScal(cp[vright].ev.v, C.nv, s2); /* s2=SGN(s2+0.01f); */ //s2+=0.0001f;
  
  if (s1>0) {
   ClipRes+=1;
   /*
   CalcHitPoint(C,cp[vn].ev.v,
                  cp[vleft].ev.v, hleft.ev.v);  

   float ll = VectorLength(SubVectors(cp[vleft].ev.v, cp[vn].ev.v));
   float lc = VectorLength(SubVectors(hleft.ev.v, cp[vn].ev.v));
   lc = lc / ll;
   */
   float lc = -s / (s1-s);
   hleft.ev.v.x = cp[vn].ev.v.x + ((cp[vleft].ev.v.x - cp[vn].ev.v.x) * lc);
   hleft.ev.v.y = cp[vn].ev.v.y + ((cp[vleft].ev.v.y - cp[vn].ev.v.y) * lc);
   hleft.ev.v.z = cp[vn].ev.v.z + ((cp[vleft].ev.v.z - cp[vn].ev.v.z) * lc);   

   hleft.tx = cp[vn].tx + ((cp[vleft].tx - cp[vn].tx) * lc);
   hleft.ty = cp[vn].ty + ((cp[vleft].ty - cp[vn].ty) * lc);
   hleft.ev.Light = cp[vn].ev.Light + (int)((cp[vleft].ev.Light - cp[vn].ev.Light) * lc);   
   hleft.ev.ALPHA = cp[vn].ev.ALPHA + (int)((cp[vleft].ev.ALPHA - cp[vn].ev.ALPHA) * lc);   
   hleft.ev.Fog   = cp[vn].ev.Fog   +      ((cp[vleft].ev.Fog   - cp[vn].ev.Fog  ) * lc);
  }

  if (s2>0) {
   ClipRes+=2;
   /*
   CalcHitPoint(C,cp[vn].ev.v,
                  cp[vright].ev.v, hright.ev.v);  

   float ll = VectorLength(SubVectors(cp[vright].ev.v, cp[vn].ev.v));
   float lc = VectorLength(SubVectors(hright.ev.v, cp[vn].ev.v));
   lc = lc / ll;*/
   float lc = -s / (s2-s);
   hright.ev.v.x = cp[vn].ev.v.x + ((cp[vright].ev.v.x - cp[vn].ev.v.x) * lc);
   hright.ev.v.y = cp[vn].ev.v.y + ((cp[vright].ev.v.y - cp[vn].ev.v.y) * lc);
   hright.ev.v.z = cp[vn].ev.v.z + ((cp[vright].ev.v.z - cp[vn].ev.v.z) * lc);   

   hright.tx = cp[vn].tx + ((cp[vright].tx - cp[vn].tx) * lc);
   hright.ty = cp[vn].ty + ((cp[vright].ty - cp[vn].ty) * lc);
   hright.ev.Light = cp[vn].ev.Light + (int)((cp[vright].ev.Light - cp[vn].ev.Light) * lc);   
   hright.ev.ALPHA = cp[vn].ev.ALPHA + (int)((cp[vright].ev.ALPHA - cp[vn].ev.ALPHA) * lc);   
   hright.ev.Fog   = cp[vn].ev.Fog   +      ((cp[vright].ev.Fog   - cp[vn].ev.Fog  ) * lc);
  }

  if (ClipRes == 0) {
      u--; vused--; 
      cp[vn] = cp[vn+1];
      cp[vn+1] = cp[vn+2];
      cp[vn+2] = cp[vn+3];
      cp[vn+3] = cp[vn+4];
      cp[vn+4] = cp[vn+5];
      cp[vn+5] = cp[vn+6];
      //memcpy(&cp[vn], &cp[vn+1], (15-vn)*sizeof(ClipPoint)); 
  }
  if (ClipRes == 1) {cp[vn] = hleft; }
  if (ClipRes == 2) {cp[vn] = hright;}
  if (ClipRes == 3) {
    u++; vused++;
    //memcpy(&cp[vn+1], &cp[vn], (15-vn)*sizeof(ClipPoint)); 
    cp[vn+6] = cp[vn+5];
    cp[vn+5] = cp[vn+4];
    cp[vn+4] = cp[vn+3];
    cp[vn+3] = cp[vn+2];
    cp[vn+2] = cp[vn+1];
    cp[vn+1] = cp[vn];
           
    cp[vn] = hleft;
    cp[vn+1] = hright;
    } 
}



void DrawTPlaneClip(BOOL SECONT)
{
   if (!WATERREVERSE) {
    MulVectorsVect(SubVectors(ev[1].v, ev[0].v), SubVectors(ev[2].v, ev[0].v), nv);   
    if (nv.x*ev[0].v.x  +  nv.y*ev[0].v.y  +  nv.z*ev[0].v.z<0) return;       
   }

   cp[0].ev = ev[0]; cp[1].ev = ev[1]; cp[2].ev = ev[2];

   if (ReverseOn) 
    if (SECONT) {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
	   cp[2].tx = TCMAX;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;
       break;        
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
     }
    } else {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMAX;       
       break; 
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
     }
    }
   else
    if (SECONT) {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMAX;       
       break;
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
     } 
    } else {
     switch (TDirection) {
      case 0:
       cp[0].tx = TCMIN;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMAX;
       break;
      case 1:
       cp[0].tx = TCMIN;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMIN;
       cp[2].tx = TCMAX;   cp[2].ty = TCMIN;       
       break;
      case 2:
       cp[0].tx = TCMAX;   cp[0].ty = TCMAX;
       cp[1].tx = TCMIN;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMIN;
       break;
      case 3:
       cp[0].tx = TCMAX;   cp[0].ty = TCMIN;
       cp[1].tx = TCMAX;   cp[1].ty = TCMAX;
       cp[2].tx = TCMIN;   cp[2].ty = TCMAX;
       break;
     }
    }
   
   vused = 3;


   for (u=0; u<vused; u++) cp[u].ev.v.z+=16.0f;
   for (u=0; u<vused; u++) ClipVector(ClipZ,u);
   for (u=0; u<vused; u++) cp[u].ev.v.z-=16.0f;
   if (vused<3) return;

   for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) return; 
   for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) return; 
   for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) return; 
   for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) return; 
            
   for (u=0; u<vused; u++) {     
     cp[u].ev.scrx = VideoCXf - cp[u].ev.v.x / cp[u].ev.v.z * CameraW;          
	 cp[u].ev.scry = VideoCYf + cp[u].ev.v.y / cp[u].ev.v.z * CameraH;  
   }
    
 
   if (!lpVertexG) 
       d3dStartBufferG();
	 
   if (GVCnt>380) {
		 if (lpVertexG) d3dEndBufferG(FALSE);
		 d3dStartBufferG();
   }

   if (hGTexture!=hTexture) {
     hGTexture=hTexture;
	 lpInstructionG->bOpcode = D3DOP_STATERENDER;
     lpInstructionG->bSize = sizeof(D3DSTATE);
     lpInstructionG->wCount = 1;
     lpInstructionG++;
     lpState = (LPD3DSTATE)lpInstructionG;
     lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
     lpState->dwArg[0] = hTexture;
     lpState++;	 
     lpInstructionG = (LPD3DINSTRUCTION)lpState;     

	 lpwTriCount = &(lpInstructionG->wCount);
	 lpInstructionG->bOpcode = D3DOP_TRIANGLE;
     lpInstructionG->bSize   = sizeof(D3DTRIANGLE);
     lpInstructionG->wCount  = 0; 	 	 
     lpInstructionG++;
     
   }

           	   	 
   lpTriangle             = (LPD3DTRIANGLE)lpInstructionG;

   for (u=0; u<vused-2; u++) {    

     lpVertexG->sx       = (float)cp[0].ev.scrx;
     lpVertexG->sy       = (float)cp[0].ev.scry;
     lpVertexG->sz       = _ZSCALE / cp[0].ev.v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(cp[0].ev.Light) * 0x00010101 | ((int)cp[0].ev.ALPHA<<24);
	 lpVertexG->specular = (255-(int)cp[0].ev.Fog)<<24;//0x7F000000;
     lpVertexG->tu       = (float)(cp[0].tx);// / (128.f*65536.f);
     lpVertexG->tv       = (float)(cp[0].ty);// / (128.f*65536.f);
     lpVertexG++;	

	 lpVertexG->sx       = (float)cp[u+1].ev.scrx;
     lpVertexG->sy       = (float)cp[u+1].ev.scry;
     lpVertexG->sz       = _ZSCALE / cp[u+1].ev.v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(cp[u+1].ev.Light) * 0x00010101 | ((int)cp[u+1].ev.ALPHA<<24);
	 lpVertexG->specular = (255-(int)cp[u+1].ev.Fog)<<24;//0x7F000000;
     lpVertexG->tu       = (float)(cp[u+1].tx);// / (128.f*65536.f);
     lpVertexG->tv       = (float)(cp[u+1].ty);// / (128.f*65536.f);
     lpVertexG++;	

	 lpVertexG->sx       = (float)cp[u+2].ev.scrx;
     lpVertexG->sy       = (float)cp[u+2].ev.scry;
     lpVertexG->sz       = _ZSCALE / cp[u+2].ev.v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(cp[u+2].ev.Light) * 0x00010101 | ((int)cp[u+2].ev.ALPHA<<24);
	 lpVertexG->specular = (255-(int)cp[u+2].ev.Fog)<<24;//0x7F000000;
     lpVertexG->tu       = (float)(cp[u+2].tx);// / (128.f*65536.f);
     lpVertexG->tv       = (float)(cp[u+2].ty);// / (128.f*65536.f);
     lpVertexG++;	    	 
          
     lpTriangle->wV1    = GVCnt;
     lpTriangle->wV2    = GVCnt+1;	
     lpTriangle->wV3    = GVCnt+2;	
	 lpTriangle->wFlags = 0;
	 lpTriangle++;	 
     *lpwTriCount = (*lpwTriCount) + 1;

	 GVCnt+=3;             	 
	}            	
     
     lpInstructionG = (LPD3DINSTRUCTION)lpTriangle;
}








void DrawTPlane(BOOL SECONT)
{
   int n;   
   
   //if (!WATERREVERSE) {  
    MulVectorsVect(SubVectors(ev[1].v, ev[0].v), SubVectors(ev[2].v, ev[0].v), nv);   
    if (nv.x*ev[0].v.x  +  nv.y*ev[0].v.y  +  nv.z*ev[0].v.z<0) return;       
   //}

   Mask1=0x007F;   
   for (n=0; n<3; n++) {     
	 if (ev[n].DFlags & 128) return;     
     Mask1=Mask1 & ev[n].DFlags;  }
   if (Mask1>0) return;

   if (ReverseOn) 
    if (SECONT) {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
	   scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;        
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
     }
    } else {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;       
       break; 
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
     }
    }
   else
   if (SECONT) {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;       
       break;
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
     } 
    } else {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;       
       break;
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
     }
    }
	

	int alpha1 = 255;
	int alpha2 = 255;
	int alpha3 = 255;

  //if (!WATERREVERSE)
	
   if (zs > (ctViewR-8)<<8) {    
     int zz;
     zz = (int)VectorLength(ev[0].v) - 256 * (ctViewR-4);
     if (zz > 0) alpha1 = max(0, 255 - zz / 3); else alpha1 = 255;
	 
     zz = (int)VectorLength(ev[1].v) - 256 * (ctViewR-4);
     if (zz > 0) alpha2 = max(0, 255 - zz / 3); else alpha2 = 255;

     zz = (int)VectorLength(ev[2].v) - 256 * (ctViewR-4);
     if (zz > 0) alpha3 = max(0, 255 - zz / 3); else alpha3 = 255;
   }
          
     if (!lpVertexG) 
       d3dStartBufferG();
	 
	 if (GVCnt>380) {
		 if (lpVertexG) d3dEndBufferG(FALSE);
		 d3dStartBufferG();
	 }

     lpVertexG->sx       = (float)ev[0].scrx;
     lpVertexG->sy       = (float)ev[0].scry;
     lpVertexG->sz       = _ZSCALE / ev[0].v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(ev[0].Light) * 0x00010101 | alpha1<<24;     
	 lpVertexG->specular = (255-(int)ev[0].Fog)<<24;//0x7F000000;
     lpVertexG->tu       = (float)(scrp[0].tx);// / (128.f*65536.f);
     lpVertexG->tv       = (float)(scrp[0].ty);// / (128.f*65536.f);
     lpVertexG++;	

	 lpVertexG->sx       = (float)ev[1].scrx;
     lpVertexG->sy       = (float)ev[1].scry;
     lpVertexG->sz       = _ZSCALE / ev[1].v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(ev[1].Light) * 0x00010101 | alpha2<<24;     
	 lpVertexG->specular = (255-(int)ev[1].Fog)<<24;//0x7F000000;
     lpVertexG->tu       = (float)(scrp[1].tx);// / (128.f*65536.f);
     lpVertexG->tv       = (float)(scrp[1].ty);// / (128.f*65536.f);
     lpVertexG++;	

	 lpVertexG->sx       = (float)ev[2].scrx;
     lpVertexG->sy       = (float)ev[2].scry;
     lpVertexG->sz       = _ZSCALE / ev[2].v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(ev[2].Light) * 0x00010101 | alpha3<<24;     
	 lpVertexG->specular = (255-(int)ev[2].Fog)<<24;//0x7F000000;
     lpVertexG->tu       = (float)(scrp[2].tx);// / (128.f*65536.f);
     lpVertexG->tv       = (float)(scrp[2].ty);// / (128.f*65536.f);
     lpVertexG++;		 	      

     if (hGTexture!=hTexture) {
      hGTexture=hTexture;
	  lpInstructionG->bOpcode = D3DOP_STATERENDER;
      lpInstructionG->bSize = sizeof(D3DSTATE);
      lpInstructionG->wCount = 1;
      lpInstructionG++;
      lpState = (LPD3DSTATE)lpInstructionG;
      lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
      lpState->dwArg[0] = hTexture;
      lpState++;	 
      lpInstructionG = (LPD3DINSTRUCTION)lpState;     

	  lpwTriCount = (&lpInstructionG->wCount);
	  lpInstructionG->bOpcode = D3DOP_TRIANGLE;
      lpInstructionG->bSize   = sizeof(D3DTRIANGLE);
      lpInstructionG->wCount  = 0; 	  
      lpInstructionG++;     
	 }
     
     lpTriangle             = (LPD3DTRIANGLE)lpInstructionG;      	   	 
     lpTriangle->wV1    = GVCnt;
     lpTriangle->wV2    = GVCnt+1;	
     lpTriangle->wV3    = GVCnt+2;	
	 lpTriangle->wFlags = 0;
	 lpTriangle++;	 
	 *lpwTriCount = (*lpwTriCount) + 1;
     lpInstructionG = (LPD3DINSTRUCTION)lpTriangle;

	 GVCnt+=3; 	   
}




void DrawTPlaneW(BOOL SECONT)
{
   int n;   
 
   Mask1=0x007F;   
   for (n=0; n<3; n++) {     
	 if (ev[n].DFlags & 128) return;     
     Mask1=Mask1 & ev[n].DFlags;  }
   if (Mask1>0) return;

   /*
   for (n=0; n<3; n++) {     	 
     scrp[n].x = (float)(VideoCX) - (ev[n].v.x / ev[n].v.z * CameraW);
	 scrp[n].y = (float)(VideoCY) + (ev[n].v.y / ev[n].v.z * CameraH);
	 scrp[n].Light = ev[n].Light;	 
   } */

   if (ReverseOn) 
    if (SECONT) {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
	   scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;        
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
     }
    } else {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;       
       break; 
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
     }
    }
   else
   if (SECONT) {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;       
       break;
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
     } 
    } else {
     switch (TDirection) {
      case 0:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMAX;
       break;
      case 1:
       scrp[0].tx = TCMIN;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMIN;
       scrp[2].tx = TCMAX;   scrp[2].ty = TCMIN;       
       break;
      case 2:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMAX;
       scrp[1].tx = TCMIN;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMIN;
       break;
      case 3:
       scrp[0].tx = TCMAX;   scrp[0].ty = TCMIN;
       scrp[1].tx = TCMAX;   scrp[1].ty = TCMAX;
       scrp[2].tx = TCMIN;   scrp[2].ty = TCMAX;
       break;
     }
    }
	

   if (!UNDERWATER)
   if (zs > (ctViewR-8)<<8) {    
     float zz;
     zz = VectorLength(ev[0].v) - 256 * (ctViewR-4);
     if (zz > 0) ev[0].ALPHA = max(0.f,255.f - zz / 3.f);
	 
     zz = VectorLength(ev[1].v) - 256 * (ctViewR-4);
     if (zz > 0) ev[1].ALPHA = max(0.f, 255.f - zz / 3.f);

     zz = VectorLength(ev[2].v) - 256 * (ctViewR-4);
     if (zz > 0) ev[2].ALPHA = max(0,255.f - zz / 3.f);
   }

        
     if (!lpVertexG) 
       d3dStartBufferG();
	 
	 if (GVCnt>380) {
		 if (lpVertexG) d3dEndBufferG(FALSE);
		 d3dStartBufferG();
	 }

     lpVertexG->sx       = (float)ev[0].scrx;
     lpVertexG->sy       = (float)ev[0].scry;
     lpVertexG->sz       = _ZSCALE / ev[0].v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(ev[0].Light) * 0x00010101 | ev[0].ALPHA<<24;     
	 lpVertexG->specular = (255-(int)ev[0].Fog)<<24;
     lpVertexG->tu       = (float)(scrp[0].tx);
     lpVertexG->tv       = (float)(scrp[0].ty);
     lpVertexG++;	

	 lpVertexG->sx       = (float)ev[1].scrx;
     lpVertexG->sy       = (float)ev[1].scry;
     lpVertexG->sz       = _ZSCALE / ev[1].v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(ev[1].Light) * 0x00010101 | ev[1].ALPHA<<24;     
	 lpVertexG->specular = (255-(int)ev[1].Fog)<<24;
     lpVertexG->tu       = (float)(scrp[1].tx);
     lpVertexG->tv       = (float)(scrp[1].ty);
     lpVertexG++;	

	 lpVertexG->sx       = (float)ev[2].scrx;
     lpVertexG->sy       = (float)ev[2].scry;
     lpVertexG->sz       = _ZSCALE / ev[2].v.z;
     lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
     lpVertexG->color    = (int)(ev[2].Light) * 0x00010101 | ev[2].ALPHA<<24;     
	 lpVertexG->specular = (255-(int)ev[2].Fog)<<24;
     lpVertexG->tu       = (float)(scrp[2].tx);
     lpVertexG->tv       = (float)(scrp[2].ty);
     lpVertexG++;		 	      

     if (hGTexture!=hTexture) {
      hGTexture=hTexture;
	  lpInstructionG->bOpcode = D3DOP_STATERENDER;
      lpInstructionG->bSize = sizeof(D3DSTATE);
      lpInstructionG->wCount = 1;
      lpInstructionG++;
      lpState = (LPD3DSTATE)lpInstructionG;
      lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
      lpState->dwArg[0] = hTexture;
      lpState++;	 
      lpInstructionG = (LPD3DINSTRUCTION)lpState;     

	  lpwTriCount = (&lpInstructionG->wCount);
	  lpInstructionG->bOpcode = D3DOP_TRIANGLE;
      lpInstructionG->bSize   = sizeof(D3DTRIANGLE);
      lpInstructionG->wCount  = 0; 	  
      lpInstructionG++;     
	 }
     
     lpTriangle             = (LPD3DTRIANGLE)lpInstructionG;      	   	 
     lpTriangle->wV1    = GVCnt;
     lpTriangle->wV2    = GVCnt+1;	
     lpTriangle->wV3    = GVCnt+2;	
	 lpTriangle->wFlags = 0;
	 lpTriangle++;	 
	 *lpwTriCount = (*lpwTriCount) + 1;
     lpInstructionG = (LPD3DINSTRUCTION)lpTriangle;

	 GVCnt+=3; 	   
}



void _RenderObject(int x, int y)
{	 	  	  
	  int ob = OMap[y][x];

	  if (!MObjects[ob].model) {
		  //return;
		  wsprintf(logt,"Incorrect model at [%d][%d]!", x, y);
		  DoHalt(logt);
	  }

	  //int mlight = -(RandomMap[y & 31][x & 31] >> 5) + (LMap[y][x]>>1) + 96;
	  int FI = (FMap[y][x] >> 2) & 3;	  
	  float fi = CameraAlpha + (float)(FI * 2.f*pi / 4.f);


	  int mlight;
	  	  if (MObjects[ob].info.flags & ofDEFLIGHT) 
		  mlight = MObjects[ob].info.DefLight; else
	  if (MObjects[ob].info.flags & ofGRNDLIGHT) 
	  {		  		  
		  mlight = 128;		  
		  CalcModelGroundLight(MObjects[ob].model, x*256+128, y*256+128, FI);
		  FI = 0;
	  }
	  else 
		  mlight = -(RandomMap[y & 31][x & 31] >> 5) + (LMap[y][x]>>1) + 96;


      if (mlight >192) mlight =192;
	  if (mlight < 64) mlight = 64;
      
	  v[0].x = x*256+128 - CameraX;
      v[0].z = y*256+128 - CameraZ;                   
      v[0].y = (float)(HMapO[y][x]) * ctHScale - CameraY;

	  float zs = VectorLength(v[0]);


      //if (v[0].y + MObjects[ob].info.YHi < (int)(HMap[y][x]+HMap[y+1][x+1]) / 2 * ctHScale - CameraY) return;
        
	  CalcFogLevel_Gradient(v[0]);
	  	  
	        
	  v[0] = RotateVector(v[0]);
	  GlassL = 0;
      
      if (zs > 256 * (ctViewR-8))
	   GlassL=min(255,(int)(zs - 256 * (ctViewR-8)) / 4);

	  if (GlassL==255) return;      
      
	  if (MObjects[ob].info.flags & ofANIMATED) 
	   if (MObjects[ob].info.LastAniTime!=RealTime) {
        MObjects[ob].info.LastAniTime=RealTime;	   
		CreateMorphedObject(MObjects[ob].model,
		                    MObjects[ob].vtl,
							RealTime % MObjects[ob].vtl.AniTime);
	   }	  

	  

	  if (MObjects[ob].info.flags & ofNOBMP) zs = 0;
/*
	  if (!NeedWater && zs>ctViewRM*256 && zs<(ctViewRM+1)*256) {
       GlassL = 255-(int)(zs - ctViewRM*256);
	   RenderBMPModel(&MObjects[ob].bmpmodel, v[0].x, v[0].y, v[0].z, mlight-32);	   	   
	   GlassL=255-GlassL;
	   RenderModel(MObjects[ob].model, v[0].x, v[0].y, v[0].z, mlight, FI, fi, CameraBeta);	   	   
	  } else	   */
	  if (zs>ctViewRM*256)
		  RenderBMPModel(&MObjects[ob].bmpmodel, v[0].x, v[0].y, v[0].z, mlight-16);
	  else
      if (v[0].z<-256*8)
       RenderModel(MObjects[ob].model, v[0].x, v[0].y, v[0].z, mlight, FI, fi, CameraBeta);	   
      else
       RenderModelClip(MObjects[ob].model, v[0].x, v[0].y, v[0].z, mlight, FI, fi, CameraBeta);       
	   
}

void RenderObject(int x, int y)
{
	if (OMap[y][x]==255) return;
	if (!MODELS) return;		 	
	if (ORLCount>2000) return;
	ORList[ORLCount].x = x;
	ORList[ORLCount].y = y;
	ORLCount++;	
}



void RenderModelsList()
{
  d3dEndBufferG(FALSE);
  for (int o=0; o<ORLCount; o++)
        _RenderObject(ORList[o].x, ORList[o].y);
  ORLCount=0;
  d3dEndBufferG(TRUE);
}

void ProcessMap(int x, int y, int r)
{ 
   //WATERREVERSE = FALSE;
   if (x>=ctMapSize-1 || y>=ctMapSize-1 ||
	   x<0 || y<0) return;   

   float BackR = BackViewR;

   if (OMap[y][x]!=255) BackR+=MObjects[OMap[y][x]].info.BoundR;

   ev[0] = VMap[y-CCY+128][x-CCX+128];
   if (ev[0].v.z>BackR) return;        
   
   int t1 = TMap1[y][x];   
   ReverseOn = (FMap[y][x] & fmReverse);   
   TDirection = (FMap[y][x] & 3);
             
   x = x - CCX + 128;
   y = y - CCY + 128;

   ev[1] = VMap[y][x+1];            

   if (ReverseOn) ev[2] = VMap[y+1][x];          
             else ev[2] = VMap[y+1][x+1];          

   //Mask1=0x007F;   
   //for (n=0; n<3; n++) Mask1&=ev[n].DFlags;  
   //if (Mask1) return;
           
   float xx = (ev[0].v.x + VMap[y+1][x+1].v.x) / 2;
   float yy = (ev[0].v.y + VMap[y+1][x+1].v.y) / 2;
   float zz = (ev[0].v.z + VMap[y+1][x+1].v.z) / 2;   

   if ( fabs(xx*FOVK) > -zz + BackR) return;
   
    
   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);          
   
   /*
   if (MIPMAP && (zs > 256 * 10 && t1 || LOWRESTX)) d3dSetTexture(Textures[t1]->DataB, 64, 64);
                                               else d3dSetTexture(Textures[t1]->DataA, 128, 128);
   */   
   if (!LOWRESTX) {
     if (zs > 256 * 20) d3dSetTexture(Textures[t1]->DataC, 32, 32); else
     if (zs > 256 * 10) d3dSetTexture(Textures[t1]->DataB, 64, 64);
                   else d3dSetTexture(Textures[t1]->DataA, 128, 128);
   } else {
    if (zs > 256 * 10) d3dSetTexture(Textures[t1]->DataC, 32, 32);
                  else d3dSetTexture(Textures[t1]->DataB, 64, 64);
   }

   if (r>8) DrawTPlane(FALSE);
       else DrawTPlaneClip(FALSE);    

   if (ReverseOn) { ev[0] = ev[2]; ev[2] = VMap[y+1][x+1]; } 
             else { ev[1] = ev[2]; ev[2] = VMap[y+1][x];   }

   if (r>8) DrawTPlane(TRUE);
       else DrawTPlaneClip(TRUE);
     
   x = x + CCX - 128;
   y = y + CCY - 128;

   if (OMap[y][x]==255) return;   
   
   if (zz<BackR)
      RenderObject(x, y);

}



void ProcessMap2(int x, int y, int r)
{ 
   //WATERREVERSE = FALSE;
   if (x>=ctMapSize-1 || y>=ctMapSize-1 ||
	   x<0 || y<0) return;   

   ev[0] = VMap[y-CCY+128][x-CCX+128];            
   if (ev[0].v.z>BackViewR) return;        
   
   int t1 = TMap2[y][x];         
   TDirection = ((FMap[y][x]>>8) & 3);
   ReverseOn = FALSE;   
             
   x = x - CCX + 128;
   y = y - CCY + 128;

   ev[1] = VMap[y][x+2];
   if (ReverseOn) ev[2] = VMap[y+2][x];          
             else ev[2] = VMap[y+2][x+2];          
           
   float xx = (ev[0].v.x + VMap[y+2][x+2].v.x) / 2;
   float yy = (ev[0].v.y + VMap[y+2][x+2].v.y) / 2;
   float zz = (ev[0].v.z + VMap[y+2][x+2].v.z) / 2;   

   if ( fabs(xx*FOVK) > -zz + BackViewR) return;
       
   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);          
   if (zs>ctViewR*256) return;
      
   d3dSetTexture(Textures[t1]->DataB, 64, 64);   

   DrawTPlane(FALSE);
       
   if (ReverseOn) { ev[0] = ev[2]; ev[2] = VMap[y+2][x+2]; } 
             else { ev[1] = ev[2]; ev[2] = VMap[y+2][x];   }

   DrawTPlane(TRUE);
   
     
   x = x + CCX - 128;
   y = y + CCY - 128;

   RenderObject(x  , y);
   RenderObject(x+1, y);
   RenderObject(x  , y+1);
   RenderObject(x+1, y+1);
}




void ProcessMapW(int x, int y, int r)
{    

   if (!( (FMap[y  ][x  ] & fmWaterA) &&
	      (FMap[y  ][x+1] & fmWaterA) &&	   
		  (FMap[y+1][x  ] & fmWaterA) &&
		  (FMap[y+1][x+1] & fmWaterA) )) return;
	   

   WATERREVERSE = TRUE;
   int t1 = WaterList[ WMap[y][x] ].tindex;

   ev[0] = VMap2[y-CCY+128][x-CCX+128];   
   if (ev[0].v.z>BackViewR) return;
   
   ReverseOn = FALSE;
   TDirection = 0;
                
   x = x - CCX + 128;
   y = y - CCY + 128;
   ev[1] = VMap2[y][x+1];               
   ev[2] = VMap2[y+1][x+1];          
           
   float xx = (ev[0].v.x + VMap2[y+1][x+1].v.x) / 2;
   float yy = (ev[0].v.y + VMap2[y+1][x+1].v.y) / 2;
   float zz = (ev[0].v.z + VMap2[y+1][x+1].v.z) / 2;   
   
   if ( fabs(xx*FOVK) > -zz + BackViewR) return;
       
   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);  
   if (zs > ctViewR*256) return;
   /*      
   if (MIPMAP && (zs > 256 * 10 && t1 || LOWRESTX)) d3dSetTexture(Textures[t1]->DataB, 64, 64);
                                               else d3dSetTexture(Textures[t1]->DataA, 128, 128);   
    */
   if (!LOWRESTX) {
     if (zs > 256 * 20) d3dSetTexture(Textures[t1]->DataC, 32, 32); else
     if (zs > 256 * 10) d3dSetTexture(Textures[t1]->DataB, 64, 64);
                   else d3dSetTexture(Textures[t1]->DataA, 128, 128);
   } else {
    if (zs > 256 * 10) d3dSetTexture(Textures[t1]->DataC, 32, 32);
                  else d3dSetTexture(Textures[t1]->DataB, 64, 64);
   }

   if (r>8) DrawTPlaneW(FALSE);
     else DrawTPlaneClip(FALSE);    
   
   ev[1] = ev[2]; ev[2] = VMap2[y+1][x];  
  
   if (r>8) DrawTPlaneW(TRUE);
       else DrawTPlaneClip(TRUE);      
   WATERREVERSE = FALSE;

}


void ProcessMapW2(int x, int y, int r)
{    
   if (!( (FMap[y  ][x  ] & fmWaterA) &&
	      (FMap[y  ][x+2] & fmWaterA) &&	   
		  (FMap[y+2][x  ] & fmWaterA) &&
		  (FMap[y+2][x+2] & fmWaterA) )) return;
   
   int t1 = WaterList[ WMap[y][x] ].tindex;

   ev[0] = VMap2[y-CCY+128][x-CCX+128];   
   if (ev[0].v.z>BackViewR) return;
   
   //WATERREVERSE = TRUE;
   ReverseOn = FALSE;
   TDirection = 0;
                
   x = x - CCX + 128;
   y = y - CCY + 128;
   ev[1] = VMap2[y][x+2];           
   ev[2] = VMap2[y+2][x+2];
           
   float xx = (ev[0].v.x + VMap2[y+2][x+2].v.x) / 2;
   float yy = (ev[0].v.y + VMap2[y+2][x+2].v.y) / 2;
   float zz = (ev[0].v.z + VMap2[y+2][x+2].v.z) / 2;   
   
   if ( fabs(xx*FOVK) > -zz + BackViewR) return;
       
   zs = (int)sqrt( xx*xx + zz*zz + yy*yy);  
   if (zs > ctViewR*256) return;
         
   
   d3dSetTexture(Textures[t1]->DataB, 64, 64);   
   
   DrawTPlaneW(FALSE);
   ev[1] = ev[2]; ev[2] = VMap2[y+2][x];   
   
   DrawTPlaneW(TRUE);   
}


void RenderGround()
{
   for (r=ctViewR; r>=ctViewR1; r-=2) {
     
     for (int x=r; x>0; x-=2) {
      ProcessMap2(CCX-x, CCY+r, r);
      ProcessMap2(CCX+x, CCY+r, r);
	  ProcessMap2(CCX-x, CCY-r, r); 		
      ProcessMap2(CCX+x, CCY-r, r); 	
     }    
    
     ProcessMap2(CCX, CCY-r, r); 	
     ProcessMap2(CCX, CCY+r, r); 	

	 for (int y=r-2; y>0; y-=2) {
      ProcessMap2(CCX+r, CCY-y, r);
      ProcessMap2(CCX+r, CCY+y, r);
      ProcessMap2(CCX-r, CCY+y, r); 
      ProcessMap2(CCX-r, CCY-y, r);
     }
     ProcessMap2(CCX-r, CCY, r);
     ProcessMap2(CCX+r, CCY, r);
     
   } 

   
   r = ctViewR1-1;
   for (int x=r; x>-r; x--) {
	   ProcessMap(CCX+r, CCY+x, r);
	   ProcessMap(CCX+x, CCY+r, r);
   }

   for (r=ctViewR1-2; r>0; r--) {
     
     for (int x=r; x>0; x--) {
      ProcessMap(CCX-x, CCY+r, r);
      ProcessMap(CCX+x, CCY+r, r);
	  ProcessMap(CCX-x, CCY-r, r); 		
      ProcessMap(CCX+x, CCY-r, r); 	
     }    	 
    
     ProcessMap(CCX, CCY-r, r); 	
     ProcessMap(CCX, CCY+r, r); 	

	 for (int y=r-1; y>0; y--) {
      ProcessMap(CCX+r, CCY-y, r);
      ProcessMap(CCX+r, CCY+y, r);
      ProcessMap(CCX-r, CCY+y, r); 
      ProcessMap(CCX-r, CCY-y, r);
     }
     ProcessMap(CCX-r, CCY, r);
     ProcessMap(CCX+r, CCY, r);
   
   } 

   ProcessMap(CCX, CCY, 0);   
}



void RenderWCircles()
{
	      
   TWCircle *wptr;
   Vector3d rpos;
   for (int c=0; c<WCCount; c++) {
      
      wptr = &WCircles[c];
      rpos.x = wptr->pos.x - CameraX;
      rpos.y = wptr->pos.y - CameraY;
      rpos.z = wptr->pos.z - CameraZ;

      float r = (float)max( fabs(rpos.x), fabs(rpos.z) );
      int ri = -1 + (int)(r / 256.f + 0.4f);
      if (ri < 0) ri = 0;
      if (ri > ctViewR) continue;

      rpos = RotateVector(rpos);

      if ( rpos.z > BackViewR) continue;
      if ( fabs(rpos.x) > -rpos.z + BackViewR ) continue;      
	  if ( fabs(rpos.y) > -rpos.z + BackViewR ) continue;      
      


	  GlassL = 255 - (2000-wptr->FTime) / 38;

	  CreateMorphedModel(WCircleModel.mptr, &WCircleModel.Animation[0], (int)(wptr->FTime), wptr->scale);

      if ( fabs(rpos.z) + fabs(rpos.x) < 1000)
       RenderModelClip(WCircleModel.mptr,
                       rpos.x, rpos.y, rpos.z, 250, 0, 0, CameraBeta);
      else 
       RenderModel(WCircleModel.mptr,
                   rpos.x, rpos.y, rpos.z, 250, 0, 0, CameraBeta);

   }
   GlassL = 0;
}

void RenderWater()
{  
  SetRenderStates(FALSE, D3DBLEND_INVSRCALPHA);

  
  for (int r=ctViewR; r>=ctViewR1; r-=2) {
     
     for (int x=r; x>0; x-=2) {
      ProcessMapW2(CCX-x, CCY+r, r);
      ProcessMapW2(CCX+x, CCY+r, r);
	  ProcessMapW2(CCX-x, CCY-r, r); 		
      ProcessMapW2(CCX+x, CCY-r, r); 	
     }    
    
     ProcessMapW2(CCX, CCY-r, r); 	
     ProcessMapW2(CCX, CCY+r, r); 	

	 for (int y=r-2; y>0; y-=2) {
      ProcessMapW2(CCX+r, CCY-y, r);
      ProcessMapW2(CCX+r, CCY+y, r);
      ProcessMapW2(CCX-r, CCY+y, r); 
      ProcessMapW2(CCX-r, CCY-y, r);
     }
     ProcessMapW2(CCX-r, CCY, r);
     ProcessMapW2(CCX+r, CCY, r);
   } 

   for (int y=-ctViewR1+2; y<ctViewR1; y++)
     for (int x=-ctViewR1+2; x<ctViewR1; x++)
       ProcessMapW(CCX+x, CCY+y, max(abs(x), abs(y)));

	

   d3dEndBufferG(FALSE);

   FogYBase = 0;
   SetRenderStates(FALSE, D3DBLEND_ONE);
   RenderWCircles();
   SetRenderStates(TRUE, D3DBLEND_INVSRCALPHA);
}



void RenderPoint(float cx, float cy, float z, float _R, DWORD RGBA, DWORD RGBA2)
{  
/*
  RGBA = (RGBA  & 0xFF00FF00) + ((RGBA  & 0x000000FF)<<16) + ((RGBA  & 0x00FF0000)>>16);
  RGBA2= (RGBA2 & 0xFF00FF00) + ((RGBA2 & 0x000000FF)<<16) + ((RGBA2 & 0x00FF0000)>>16);
  */
  float  R = (float)((int)(      _R*16.f)) / 16.f;    
  float  R2= (float)((int)(0.6f* _R*16.f)) / 16.f;    
  float sz = _ZSCALE / z;  

  if (R>1) R=1;
  DWORD A = (int)(((RGBA>>25) + (RGBA2>>25)) * R);

  RGBA = (RGBA & 0x00FFFFFF) | (A<<24);
  
  lpVertex->sx       = cx;
  lpVertex->sy       = cy-R2;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;

  lpVertex->sx       = cx-R2;
  lpVertex->sy       = cy+R2;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;

  lpVertex->sx       = cx+R2;
  lpVertex->sy       = cy+R2;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 1
}

void RenderCircle(float cx, float cy, float z, float _R, DWORD RGBA, DWORD RGBA2)
{  
/*
  RGBA = (RGBA  & 0xFF00FF00) + ((RGBA  & 0x000000FF)<<16) + ((RGBA  & 0x00FF0000)>>16);
  RGBA2= (RGBA2 & 0xFF00FF00) + ((RGBA2 & 0x000000FF)<<16) + ((RGBA2 & 0x00FF0000)>>16);
  */
  float  R = (float)((int)(      _R*16.f)) / 16.f;  
  float R2 = (float)((int)(0.65f*_R*16.f)) / 16.f;  
  float sz = _ZSCALE / z;  
  
  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;

  lpVertex->sx       = cx;
  lpVertex->sy       = cy-R;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;

  lpVertex->sx       = cx+R2;
  lpVertex->sy       = cy-R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 1
  
  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;
  
  lpVertex->sx       = cx+R2;
  lpVertex->sy       = cy-R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;

  lpVertex->sx       = cx+R;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 2


  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;
    
  lpVertex->sx       = cx+R;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
  
  lpVertex->sx       = cx+R2;
  lpVertex->sy       = cy+R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 3


  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;

  lpVertex->sx       = cx+R2;
  lpVertex->sy       = cy+R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
    
  lpVertex->sx       = cx;
  lpVertex->sy       = cy+R;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 4


  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;

  lpVertex->sx       = cx;
  lpVertex->sy       = cy+R;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;

  lpVertex->sx       = cx-R2;
  lpVertex->sy       = cy+R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;      
//================ 5
  

  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;

  lpVertex->sx       = cx-R2;
  lpVertex->sy       = cy+R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;

  lpVertex->sx       = cx-R;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;      
//================ 6

  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;  

  lpVertex->sx       = cx-R;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;      

  lpVertex->sx       = cx-R2;
  lpVertex->sy       = cy-R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 7

  lpVertex->sx       = cx;
  lpVertex->sy       = cy;  
  lpVertex->color    = RGBA;
  lpVertex->specular = 0xFF000000; lpVertex->sz  = sz; lpVertex->rhw = 1.f; lpVertex++;  

  lpVertex->sx       = cx-R2;
  lpVertex->sy       = cy-R2;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;      

  lpVertex->sx       = cx;
  lpVertex->sy       = cy-R;  
  lpVertex->color    = RGBA2;
  lpVertex->specular = 0xFF000000; lpVertex->sz = sz; lpVertex->rhw = 1.f;  lpVertex++;
//================ 8
}


void BuildTreeNoSort()
{
    Vector2di v[3];
	Current = -1;
    int LastFace = -1;
    TFace* fptr;
    int sg;
    
	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
  		v[0] = gScrp[fptr->v1]; 
        v[1] = gScrp[fptr->v2]; 
        v[2] = gScrp[fptr->v3];

        if (v[0].x == 0xFFFFFF) continue;
        if (v[1].x == 0xFFFFFF) continue;
        if (v[2].x == 0xFFFFFF) continue;         

        if (fptr->Flags & (sfDarkBack+sfNeedVC)) {
           sg = (v[1].x-v[0].x)*(v[2].y-v[1].y) - (v[1].y-v[0].y)*(v[2].x-v[1].x);
           if (sg<0) continue;
        }        
					
		fptr->Next=-1;
        if (Current==-1) { Current=f; LastFace = f; } else 
        { mptr->gFace[LastFace].Next=f; LastFace=f; }
		
	}
}

void BuildTreeNoSortf()
{
    Vector2df v[3];
	Current = -1;
    int LastFace = -1;
    TFace* fptr;
    int sg;
    
	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
  		v[0] = gScrpf[fptr->v1]; 
        v[1] = gScrpf[fptr->v2]; 
        v[2] = gScrpf[fptr->v3];

        if (v[0].x <-102300) continue;
        if (v[1].x <-102300) continue;
        if (v[2].x <-102300) continue;         

        if (fptr->Flags & (sfDarkBack+sfNeedVC)) {
           sg = (v[1].x-v[0].x)*(v[2].y-v[1].y) - (v[1].y-v[0].y)*(v[2].x-v[1].x);
           if (sg<0) continue;
        }        
					
		fptr->Next=-1;
        if (Current==-1) { Current=f; LastFace = f; } else 
        { mptr->gFace[LastFace].Next=f; LastFace=f; }
		
	}
}

int  BuildTreeClipNoSort()
{
	Current = -1;
    int fc = 0;
    int LastFace = -1;
    TFace* fptr;

	for (int f=0; f<mptr->FCount; f++)
	{        
        fptr = &mptr->gFace[f];
        
        if (fptr->Flags & (sfDarkBack + sfNeedVC) ) {
         MulVectorsVect(SubVectors(rVertex[fptr->v2], rVertex[fptr->v1]), SubVectors(rVertex[fptr->v3], rVertex[fptr->v1]), nv);
         if (nv.x*rVertex[fptr->v1].x  +  nv.y*rVertex[fptr->v1].y  +  nv.z*rVertex[fptr->v1].z<0) continue;
        }

        fc++;
        fptr->Next=-1;
        if (Current==-1) { Current=f; LastFace = f; } else 
        { mptr->gFace[LastFace].Next=f; LastFace=f; }
                		
	}
    return fc;
}



void RenderBMPModel(TBMPModel* mptr, float x0, float y0, float z0, int light)
{   
   if (fabs(y0) > -(z0-256*6)) return;   
      
   int minx = 10241024;
   int maxx =-10241024;
   int miny = 10241024;
   int maxy =-10241024;
     
   BOOL FOGACTIVE = (FOGON && (FogYBase>0));

   for (int s=0; s<4; s++) {                  

	if (FOGACTIVE) {
	 vFogT[s] = 255-(int) (FogYBase + mptr->gVertex[s].y * FogYGrad);
	 if (vFogT[s]<0  ) vFogT[s] = 0;
	 if (vFogT[s]>255) vFogT[s]=255;
	 vFogT[s]<<=24;
	} else vFogT[s]=255<<24;
	
    rVertex[s].x = mptr->gVertex[s].x + x0;    	
    rVertex[s].y = mptr->gVertex[s].y * cb + y0;
    rVertex[s].z = mptr->gVertex[s].y * sb + z0;

    if (rVertex[s].z<-256) {
     gScrp[s].x = VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     gScrp[s].y = VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 
	} else return;

     if (gScrp[s].x > maxx) maxx = gScrp[s].x;
     if (gScrp[s].x < minx) minx = gScrp[s].x;
     if (gScrp[s].y > maxy) maxy = gScrp[s].y; 
     if (gScrp[s].y < miny) miny = gScrp[s].y; 
   }   

   if (minx == 10241024) return;
   if (minx>WinW || maxx<0 || miny>WinH || maxy<0) return;

   int argb = light * 0x00010101 + ((255-GlassL)<<24);
         
   float d = (float) sqrt(x0*x0 + y0*y0 + z0*z0);   
   d3dSetTexture(mptr->lpTexture, 128, 128);

 
   //d3dStartBuffer();
   
   if (!lpVertexG) 
       d3dStartBufferGBMP();
	 
   if (GVCnt>380) {
		 if (lpVertexG) d3dEndBufferG(TRUE);
		 d3dStartBufferGBMP();
   } 


   lpVertexG->sx       = (float)gScrp[0].x;
   lpVertexG->sy       = (float)gScrp[0].y;
   lpVertexG->sz       = _ZSCALE / rVertex[0].z;
   lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
   lpVertexG->color    = argb;
   lpVertexG->specular = vFogT[0];
   lpVertexG->tu       = (float)(0.0f);
   lpVertexG->tv       = (float)(0.0f);
   lpVertexG++;

   lpVertexG->sx       = (float)gScrp[1].x;
   lpVertexG->sy       = (float)gScrp[1].y;
   lpVertexG->sz       = _ZSCALE / rVertex[1].z;
   lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
   lpVertexG->color    = argb;
   lpVertexG->specular = vFogT[1];
   lpVertexG->tu       = (float)(0.995f);
   lpVertexG->tv       = (float)(0.0f);
   lpVertexG++;

   lpVertexG->sx       = (float)gScrp[2].x;
   lpVertexG->sy       = (float)gScrp[2].y;
   lpVertexG->sz       = _ZSCALE / rVertex[2].z;
   lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
   lpVertexG->color    = argb;
   lpVertexG->specular = vFogT[2];
   lpVertexG->tu       = (float)(0.995f);
   lpVertexG->tv       = (float)(0.995f);
   lpVertexG++;

//=========//

   lpVertexG->sx       = (float)gScrp[0].x;
   lpVertexG->sy       = (float)gScrp[0].y;
   lpVertexG->sz       = _ZSCALE / rVertex[0].z;
   lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
   lpVertexG->color    = argb;
   lpVertexG->specular = vFogT[0];
   lpVertexG->tu       = (float)(0.0f);
   lpVertexG->tv       = (float)(0.0f);
   lpVertexG++;

   lpVertexG->sx       = (float)gScrp[2].x;
   lpVertexG->sy       = (float)gScrp[2].y;
   lpVertexG->sz       = _ZSCALE / rVertex[2].z;
   lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
   lpVertexG->color    = argb;
   lpVertexG->specular = vFogT[2];
   lpVertexG->tu       = (float)(0.995f);
   lpVertexG->tv       = (float)(0.995f);
   lpVertexG++;

   lpVertexG->sx       = (float)gScrp[3].x;
   lpVertexG->sy       = (float)gScrp[3].y;
   lpVertexG->sz       = _ZSCALE / rVertex[3].z;
   lpVertexG->rhw      = lpVertexG->sz * _AZSCALE;
   lpVertexG->color    = argb;
   lpVertexG->specular = vFogT[3];
   lpVertexG->tu       = (float)(0.0f);
   lpVertexG->tv       = (float)(0.995f);
   lpVertexG++;

   //d3dFlushBuffer(0, 2);
   
     if (hGTexture!=hTexture) {
      hGTexture=hTexture;
	  lpInstructionG->bOpcode = D3DOP_STATERENDER;
      lpInstructionG->bSize = sizeof(D3DSTATE);
      lpInstructionG->wCount = 1;
      lpInstructionG++;
      lpState = (LPD3DSTATE)lpInstructionG;
      lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
      lpState->dwArg[0] = hTexture;
      lpState++;	 
      lpInstructionG = (LPD3DINSTRUCTION)lpState;     

	  lpwTriCount = (&lpInstructionG->wCount);
	  lpInstructionG->bOpcode = D3DOP_TRIANGLE;
      lpInstructionG->bSize   = sizeof(D3DTRIANGLE);
      lpInstructionG->wCount  = 0; 	  
      lpInstructionG++;     
	 }
     
     lpTriangle             = (LPD3DTRIANGLE)lpInstructionG;      	   	 
     lpTriangle->wV1    = GVCnt;
     lpTriangle->wV2    = GVCnt+1;	
     lpTriangle->wV3    = GVCnt+2;	
	 lpTriangle->wFlags = 0;
	 lpTriangle++;	 
	 
     lpTriangle->wV1    = GVCnt+3;
     lpTriangle->wV2    = GVCnt+4;	
     lpTriangle->wV3    = GVCnt+5;	
	 lpTriangle->wFlags = 0;
	 lpTriangle++;	 

	 *lpwTriCount = (*lpwTriCount) + 2;
     lpInstructionG = (LPD3DINSTRUCTION)lpTriangle;

	 GVCnt+=6;
}


void RenderModel(TModel* _mptr, float x0, float y0, float z0, int light, int VT, float al, float bt)
{   
   int f;   
   
   if (fabs(y0) > -(z0-256*6)) return;

   mptr = _mptr;
   
   float ca = (float)f_cos(al);
   float sa = (float)f_sin(al);   
   
   float cb = (float)f_cos(bt);
   float sb = (float)f_sin(bt);   

   float minx = 10241024;
   float maxx =-10241024;
   float miny = 10241024;
   float maxy =-10241024;

   BOOL FOGACTIVE = (FOGON && (FogYBase>0));

   int alphamask = (255-GlassL)<<24;
   int ml = light;
  
   TPoint3d p;
   for (int s=0; s<mptr->VCount; s++) {              
    p = mptr->gVertex[s];

	if (FOGACTIVE) {
	 vFogT[s] = 255-(int)(FogYBase + p.y * FogYGrad);
	 if (vFogT[s]<5  ) vFogT[s] = 5;
	 if (vFogT[s]>255) vFogT[s]=255;
	 vFogT[s]<<=24;
	} else vFogT[s] = 255<<24;


		
    rVertex[s].x = (p.x * ca + p.z * sa) + x0;

    float vz = p.z * ca - p.x * sa;

    rVertex[s].y = (p.y * cb - vz * sb)  + y0;
    rVertex[s].z = (vz  * cb + p.y * sb) + z0;

    if (rVertex[s].z<-32) {
     gScrpf[s].x = VideoCXf - (rVertex[s].x / rVertex[s].z * CameraW);
     gScrpf[s].y = VideoCYf + (rVertex[s].y / rVertex[s].z * CameraH); 
	} else gScrpf[s].x=-102400;

     if (gScrpf[s].x > maxx) maxx = gScrpf[s].x;
     if (gScrpf[s].x < minx) minx = gScrpf[s].x;
     if (gScrpf[s].y > maxy) maxy = gScrpf[s].y; 
     if (gScrpf[s].y < miny) miny = gScrpf[s].y; 
   }   

   if (minx == 10241024) return;
   if (minx>WinW || maxx<0 || miny>WinH || maxy<0) return;
   
   
   BuildTreeNoSortf(); 
   
   float d = (float) sqrt(x0*x0 + y0*y0 + z0*z0);
   if (LOWRESTX) d = 14*256;

   if (MIPMAP && (d > 12*256)) d3dSetTexture(mptr->lpTexture2, 128, 128);
                          else d3dSetTexture(mptr->lpTexture, 256, 256);      

   int PrevOpacity = 0;
   int NewOpacity = 0;
   int PrevTransparent = 0;
   int NewTransparent = 0;   
   
   d3dStartBuffer();

   int fproc1 = 0;
   int fproc2 = 0;
   f = Current;
   BOOL CKEY = FALSE;
   while( f!=-1 ) {       
     TFace *fptr = & mptr->gFace[f];
	 f = mptr->gFace[f].Next;

	 if (minx<0)
	  if (gScrpf[fptr->v1].x <0    && gScrpf[fptr->v2].x<0    && gScrpf[fptr->v3].x<0) continue;
	 if (maxx>WinW)
	  if (gScrpf[fptr->v1].x >WinW && gScrpf[fptr->v2].x>WinW && gScrpf[fptr->v3].x>WinW) continue;

	 if (fptr->Flags & (sfOpacity | sfTransparent)) fproc2++; else fproc1++;

	 int _ml = ml + mptr->VLight[VT][fptr->v1];
	 lpVertex->sx       = (float)gScrpf[fptr->v1].x;
     lpVertex->sy       = (float)gScrpf[fptr->v1].y;
     lpVertex->sz       = _ZSCALE / rVertex[fptr->v1].z;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = _ml * 0x00010101 | alphamask;     
	 lpVertex->specular = vFogT[fptr->v1];
     lpVertex->tu       = (float)(fptr->tax);
     lpVertex->tv       = (float)(fptr->tay);
     lpVertex++;

     _ml = ml + mptr->VLight[VT][fptr->v2];
	 lpVertex->sx       = (float)gScrpf[fptr->v2].x;
     lpVertex->sy       = (float)gScrpf[fptr->v2].y;
     lpVertex->sz       = _ZSCALE / rVertex[fptr->v2].z;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = _ml * 0x00010101 | alphamask;;     
	 lpVertex->specular = vFogT[fptr->v2];
     lpVertex->tu       = (float)(fptr->tbx);
     lpVertex->tv       = (float)(fptr->tby);
     lpVertex++;

	 _ml = ml + mptr->VLight[VT][fptr->v3];
	 lpVertex->sx       = (float)gScrpf[fptr->v3].x;
     lpVertex->sy       = (float)gScrpf[fptr->v3].y;
     lpVertex->sz       = _ZSCALE / rVertex[fptr->v3].z;
     lpVertex->rhw      = 1.f;	 
     lpVertex->color    = _ml * 0x00010101 | alphamask;;     
	 lpVertex->specular = vFogT[fptr->v3];
     lpVertex->tu       = (float)(fptr->tcx);
     lpVertex->tv       = (float)(fptr->tcy);
     lpVertex++;	 
     	 
          
     //f = mptr->gFace[f].Next;
   }   

   d3dFlushBuffer(fproc1, fproc2);
}







void RenderShadowClip(TModel* _mptr, 
                      float xm0, float ym0, float zm0, 
                      float x0, float y0, float z0, float cal, float al, float bt)
{   
   int f,CMASK,j; 
   mptr = _mptr;
   
   float cla = (float)f_cos(cal);
   float sla = (float)f_sin(cal);   

   float ca = (float)f_cos(al);
   float sa = (float)f_sin(al);   
   
   float cb = (float)f_cos(bt);
   float sb = (float)f_sin(bt);   
     
      
   BOOL BL = FALSE;
   for (int s=0; s<mptr->VCount; s++) {              
    float mrx = mptr->gVertex[s].x * cla + mptr->gVertex[s].z * sla;
    float mrz = mptr->gVertex[s].z * cla - mptr->gVertex[s].x * sla;

    float shx = mrx + mptr->gVertex[s].y * SunShadowK;
    float shz = mrz + mptr->gVertex[s].y * SunShadowK;
    float shy = GetLandH(shx + xm0, shz + zm0) - ym0;

    rVertex[s].x = (shx * ca + shz * sa)   + x0;
    float vz = shz * ca - shx * sa;
    rVertex[s].y = (shy * cb - vz * sb) + y0;
    rVertex[s].z = (vz * cb + shy * sb) + z0;     
    if (rVertex[s].z<0) BL=TRUE;

    if (rVertex[s].z>-256) { gScrp[s].x = 0xFFFFFF; gScrp[s].y = 0xFF; }
    else {   
     int f = 0;
     int sx =  VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     int sy =  VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 
     
     if (sx>=WinEX) f+=1;
     if (sx<=0    ) f+=2;

     if (sy>=WinEY) f+=4;
     if (sy<=0    ) f+=8;     

     gScrp[s].y = f;       
    } 

   }   
   
   if (!BL) return;

   
   float d = (float) sqrt(x0*x0 + y0*y0 + z0*z0);
   if (LOWRESTX) d = 14*256;
   if (MIPMAP && (d > 12*256)) d3dSetTexture(mptr->lpTexture2, 128, 128);
                          else d3dSetTexture(mptr->lpTexture, 256, 256);   
     
   BuildTreeClipNoSort();

   d3dStartBuffer();
   int fproc1 = 0;

   f = Current;
   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    
     
    CMASK = 0;
    CMASK|=gScrp[fptr->v1].y;
    CMASK|=gScrp[fptr->v2].y;
    CMASK|=gScrp[fptr->v3].y;         

    cp[0].ev.v = rVertex[fptr->v1];  cp[0].tx = fptr->tax;  cp[0].ty = fptr->tay; 
    cp[1].ev.v = rVertex[fptr->v2];  cp[1].tx = fptr->tbx;  cp[1].ty = fptr->tby; 
    cp[2].ev.v = rVertex[fptr->v3];  cp[2].tx = fptr->tcx;  cp[2].ty = fptr->tcy; 

    if (CMASK == 0xFF) {
     for (u=0; u<vused; u++) cp[u].ev.v.z+=16.0f;
     for (u=0; u<vused; u++) ClipVector(ClipZ,u);
     for (u=0; u<vused; u++) cp[u].ev.v.z-=16.0f;
     if (vused<3) goto LNEXT;
    }
  
    if (CMASK & 1) for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) goto LNEXT;
    if (CMASK & 2) for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) goto LNEXT;    
    if (CMASK & 4) for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) goto LNEXT;    
    if (CMASK & 8) for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) goto LNEXT;                 
       	
    for (j=0; j<vused-2; j++) {        
	   u = 0;
	   lpVertex->sx       = (float)(VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW));
       lpVertex->sy       = (float)(VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH));
       lpVertex->sz       = (_ZSCALE-0.5f) / cp[u].ev.v.z;
       lpVertex->rhw      = 1.f;
       lpVertex->color    = GlassL;
	   lpVertex->specular = 0xFF000000;
       lpVertex->tu       = 0.f;
       lpVertex->tv       = 0.f;
       lpVertex++;	   

	   u = j+1;
	   lpVertex->sx       = (float)(VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW));
       lpVertex->sy       = (float)(VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH));
       lpVertex->sz       = (_ZSCALE-0.5f) / cp[u].ev.v.z;
       lpVertex->rhw      = 1.f;
       lpVertex->color    = GlassL;
	   lpVertex->specular = 0xFF000000;
       lpVertex->tu       = 0.f;
       lpVertex->tv       = 0.f;
       lpVertex++;	   

	   u = j+2;
	   lpVertex->sx       = (float)(VideoCX - (int)(cp[u].ev.v.x / cp[u].ev.v.z * CameraW));
       lpVertex->sy       = (float)(VideoCY + (int)(cp[u].ev.v.y / cp[u].ev.v.z * CameraH));
       lpVertex->sz       = (_ZSCALE-0.5f) / cp[u].ev.v.z;
       lpVertex->rhw      = 1.f;
       lpVertex->color    = GlassL;
	   lpVertex->specular = 0xFF000000;
       lpVertex->tu       = 0.f;
       lpVertex->tv       = 0.f;
       lpVertex++;	   	   
	   fproc1++;
     }            

	

LNEXT:
     f = mptr->gFace[f].Next;          
   }


   d3dFlushBuffer(fproc1, 0);  
 
}





void RenderModelClip(TModel* _mptr, float x0, float y0, float z0, int light, int VT, float al, float bt)
{   
   int f,CMASK;   

   //if (fabs(y0) > -(z0-256*6)) return;

   mptr = _mptr;
   
   float ca = (float)f_cos(al);
   float sa = (float)f_sin(al);   
   
   float cb = (float)f_cos(bt);
   float sb = (float)f_sin(bt);   

   
   int flight = (int)light;
   DWORD almask;
   DWORD alphamask = (255-GlassL)<<24;
   
   
   BOOL BL = FALSE;   
   BOOL FOGACTIVE = (FOGON && (FogYBase>0));

   for (int s=0; s<mptr->VCount; s++) {                  	

	if (FOGACTIVE) {
	 vFogT[s] = 255-(int)(FogYBase + mptr->gVertex[s].y * FogYGrad);
	 if (vFogT[s]<5  ) vFogT[s] = 5;
	 if (vFogT[s]>255) vFogT[s]=255;	 
	} else vFogT[s] = 255;

		   
	rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa) /* * mdlScale */ + x0;
    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;
    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) /* * mdlScale */ + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) /* * mdlScale */ + z0;     
    if (rVertex[s].z<0) BL=TRUE;

    if (rVertex[s].z>-256) { gScrp[s].x = 0xFFFFFF; gScrp[s].y = 0xFF; }
    else {   
     int f = 0;
     int sx =  VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     int sy =  VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 
     
     if (sx>=WinEX) f+=1;
     if (sx<=0    ) f+=2;

     if (sy>=WinEY) f+=4;
     if (sy<=0    ) f+=8;     

     gScrp[s].y = f;       
    } 

   }   
   
   if (!BL) return;
	     
   if (LOWRESTX) d3dSetTexture(mptr->lpTexture2, 128, 128);
            else d3dSetTexture(mptr->lpTexture, 256, 256);
          
   BuildTreeClipNoSort();
      
   d3dStartBuffer();

   f = Current;
   int fproc1 = 0;
   int fproc2 = 0;
   BOOL CKEY = FALSE;

   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    
	     
    CMASK = 0;
	
    CMASK|=gScrp[fptr->v1].y;
    CMASK|=gScrp[fptr->v2].y;
    CMASK|=gScrp[fptr->v3].y;         

	
    cp[0].ev.v = rVertex[fptr->v1]; cp[0].tx = fptr->tax;  cp[0].ty = fptr->tay; cp[0].ev.Fog = vFogT[fptr->v1]; cp[0].ev.Light = mptr->VLight[VT][fptr->v1];
    cp[1].ev.v = rVertex[fptr->v2]; cp[1].tx = fptr->tbx;  cp[1].ty = fptr->tby; cp[1].ev.Fog = vFogT[fptr->v2]; cp[1].ev.Light = mptr->VLight[VT][fptr->v2];
    cp[2].ev.v = rVertex[fptr->v3]; cp[2].tx = fptr->tcx;  cp[2].ty = fptr->tcy; cp[2].ev.Fog = vFogT[fptr->v3]; cp[2].ev.Light = mptr->VLight[VT][fptr->v3]; 
   
	{
     for (u=0; u<vused; u++) cp[u].ev.v.z+= 8.0f;
     for (u=0; u<vused; u++) ClipVector(ClipZ,u);
     for (u=0; u<vused; u++) cp[u].ev.v.z-= 8.0f;
     if (vused<3) goto LNEXT;
    }
  
    if (CMASK & 1) for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) goto LNEXT;
    if (CMASK & 2) for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) goto LNEXT;    
    if (CMASK & 4) for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) goto LNEXT;    
    if (CMASK & 8) for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) goto LNEXT;
	almask = 0xFF000000;
	if (fptr->Flags & sfTransparent) almask = 0x70000000;
	if (almask > alphamask) 
		almask = alphamask;
                     
    for (u=0; u<vused-2; u++) {        	     
		 int _flight = flight + cp[0].ev.Light;	
	   	 lpVertex->sx       = VideoCXf - cp[0].ev.v.x / cp[0].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[0].ev.v.y / cp[0].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[0].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = _flight * 0x00010101 | almask;
		 lpVertex->specular = ((int)cp[0].ev.Fog)<<24;
         lpVertex->tu       = (float)(cp[0].tx);
         lpVertex->tv       = (float)(cp[0].ty);
         lpVertex++;

		 _flight = flight + cp[u+1].ev.Light;	
	   	 lpVertex->sx       = VideoCXf - cp[u+1].ev.v.x / cp[u+1].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[u+1].ev.v.y / cp[u+1].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[u+1].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = _flight * 0x00010101 | almask;
		 lpVertex->specular = ((int)cp[u+1].ev.Fog)<<24;
         lpVertex->tu       = (float)(cp[u+1].tx);
         lpVertex->tv       = (float)(cp[u+1].ty);
         lpVertex++;

		 _flight = flight + cp[u+2].ev.Light;	
	   	 lpVertex->sx       = VideoCXf - cp[u+2].ev.v.x / cp[u+2].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[u+2].ev.v.y / cp[u+2].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[u+2].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = _flight * 0x00010101 | almask;
		 lpVertex->specular = ((int)cp[u+2].ev.Fog)<<24;
         lpVertex->tu       = (float)(cp[u+2].tx);
         lpVertex->tv       = (float)(cp[u+2].ty);
         lpVertex++;

	     if (fptr->Flags & (sfOpacity | sfTransparent)) fproc2++; else fproc1++;
     }            
LNEXT:
     f = mptr->gFace[f].Next;
   }

  d3dFlushBuffer(fproc1, fproc2);

}



void RenderModelClipEnvMap(TModel* _mptr, float x0, float y0, float z0, float al, float bt)
{
   int f,CMASK;   
   mptr = _mptr;
   
   float ca = (float)f_cos(al);
   float sa = (float)f_sin(al);      
   float cb = (float)f_cos(bt);
   float sb = (float)f_sin(bt);   
   
   DWORD PHCOLOR = 0xFFFFFFFF;   
   
   BOOL BL = FALSE;   
   

   for (int s=0; s<mptr->VCount; s++) {                  	
		   
	rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa) /* * mdlScale */ + x0;
    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;
    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) /* * mdlScale */ + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) /* * mdlScale */ + z0;     
    if (rVertex[s].z<0) BL=TRUE;

    if (rVertex[s].z>-256) { gScrp[s].x = 0xFFFFFF; gScrp[s].y = 0xFF; }
    else {   
     int f = 0;
     int sx =  VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     int sy =  VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 
     
     if (sx>=WinEX) f+=1;
     if (sx<=0    ) f+=2;

     if (sy>=WinEY) f+=4;
     if (sy<=0    ) f+=8;     

     gScrp[s].y = f;       
    } 

   }   
   

   d3dSetTexture(TFX_ENVMAP.lpImage, TFX_ENVMAP.W, TFX_ENVMAP.W);            
   SetRenderStates(FALSE, D3DBLEND_ONE);
          
   BuildTreeClipNoSort();
      
   d3dStartBuffer();

   f = Current;
   int fproc1 = 0;
  
   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    
	if (!(fptr->Flags & sfEnvMap)) goto LNEXT;

	
    CMASK = 0;
	
    CMASK|=gScrp[fptr->v1].y;
    CMASK|=gScrp[fptr->v2].y;
    CMASK|=gScrp[fptr->v3].y;         

	
    cp[0].ev.v = rVertex[fptr->v1]; cp[0].tx = PhongMapping[fptr->v1].x/256.f;  cp[0].ty = PhongMapping[fptr->v1].y/256.f;
    cp[1].ev.v = rVertex[fptr->v2]; cp[1].tx = PhongMapping[fptr->v2].x/256.f;  cp[1].ty = PhongMapping[fptr->v2].y/256.f;
    cp[2].ev.v = rVertex[fptr->v3]; cp[2].tx = PhongMapping[fptr->v3].x/256.f;  cp[2].ty = PhongMapping[fptr->v3].y/256.f;

	
   
	{
     for (u=0; u<vused; u++) cp[u].ev.v.z+= 8.0f;
     for (u=0; u<vused; u++) ClipVector(ClipZ,u);
     for (u=0; u<vused; u++) cp[u].ev.v.z-= 8.0f;
     if (vused<3) goto LNEXT;
    }
  
    if (CMASK & 1) for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) goto LNEXT;
    if (CMASK & 2) for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) goto LNEXT;    
    if (CMASK & 4) for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) goto LNEXT;    
    if (CMASK & 8) for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) goto LNEXT;	
                     
    for (u=0; u<vused-2; u++) {        	     		 
	   	 lpVertex->sx       = VideoCXf - cp[0].ev.v.x / cp[0].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[0].ev.v.y / cp[0].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[0].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = PHCOLOR;
		 lpVertex->specular = 0xFF000000;
         lpVertex->tu       = (float)(cp[0].tx);
         lpVertex->tv       = (float)(cp[0].ty);
         lpVertex++;
		 
	   	 lpVertex->sx       = VideoCXf - cp[u+1].ev.v.x / cp[u+1].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[u+1].ev.v.y / cp[u+1].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[u+1].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = PHCOLOR;
		 lpVertex->specular = 0xFF000000;
         lpVertex->tu       = (float)(cp[u+1].tx);
         lpVertex->tv       = (float)(cp[u+1].ty);
         lpVertex++;
		 
	   	 lpVertex->sx       = VideoCXf - cp[u+2].ev.v.x / cp[u+2].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[u+2].ev.v.y / cp[u+2].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[u+2].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = PHCOLOR;
		 lpVertex->specular = 0xFF000000;
         lpVertex->tu       = (float)(cp[u+2].tx);
         lpVertex->tv       = (float)(cp[u+2].ty);
         lpVertex++;

	     fproc1++;
     }            
LNEXT:
     f = mptr->gFace[f].Next;
   }

  d3dFlushBuffer(fproc1, 0);
  SetRenderStates(TRUE, D3DBLEND_INVSRCALPHA);
}



void RenderModelClipPhongMap(TModel* _mptr, float x0, float y0, float z0, float al, float bt)
{   
   int f,CMASK;   
   mptr = _mptr;
   
   float ca = (float)f_cos(al);
   float sa = (float)f_sin(al);      
   float cb = (float)f_cos(bt);
   float sb = (float)f_sin(bt);   

   int   rv = SkyR +64; if (rv>255) rv = 255;
   int   gv = SkyG +64; if (gv>255) gv = 255;
   int   bv = SkyB +64; if (bv>255) bv = 255;
   DWORD PHCOLOR = 0xFF000000 + (rv<<16) + (gv<<8) + bv;      
   
   BOOL BL = FALSE;   
   
   for (int s=0; s<mptr->VCount; s++) {                  	
		   
	rVertex[s].x = (mptr->gVertex[s].x * ca + mptr->gVertex[s].z * sa) /* * mdlScale */ + x0;
    float vz = mptr->gVertex[s].z * ca - mptr->gVertex[s].x * sa;
    rVertex[s].y = (mptr->gVertex[s].y * cb - vz * sb) /* * mdlScale */ + y0;
    rVertex[s].z = (vz * cb + mptr->gVertex[s].y * sb) /* * mdlScale */ + z0;     
    if (rVertex[s].z<0) BL=TRUE;

    if (rVertex[s].z>-256) { gScrp[s].x = 0xFFFFFF; gScrp[s].y = 0xFF; }
    else {   
     int f = 0;
     int sx =  VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     int sy =  VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); 
     
     if (sx>=WinEX) f+=1;
     if (sx<=0    ) f+=2;

     if (sy>=WinEY) f+=4;
     if (sy<=0    ) f+=8;     

     gScrp[s].y = f;       
    } 

   }   
   
   d3dSetTexture(TFX_SPECULAR.lpImage, TFX_SPECULAR.W, TFX_SPECULAR.W);            
   SetRenderStates(FALSE, D3DBLEND_ONE);
          
   BuildTreeClipNoSort();
      
   d3dStartBuffer();

   f = Current;
   int fproc1 = 0;
   

   while( f!=-1 ) {  
    
    vused = 3;
    TFace *fptr = &mptr->gFace[f];    
	if (!(fptr->Flags & sfPhong)) goto LNEXT;
	     
    CMASK = 0;
	
    CMASK|=gScrp[fptr->v1].y;
    CMASK|=gScrp[fptr->v2].y;
    CMASK|=gScrp[fptr->v3].y;         
		
    cp[0].ev.v = rVertex[fptr->v1]; cp[0].tx = PhongMapping[fptr->v1].x/256.f;  cp[0].ty = PhongMapping[fptr->v1].y/256.f;
    cp[1].ev.v = rVertex[fptr->v2]; cp[1].tx = PhongMapping[fptr->v2].x/256.f;  cp[1].ty = PhongMapping[fptr->v2].y/256.f;
    cp[2].ev.v = rVertex[fptr->v3]; cp[2].tx = PhongMapping[fptr->v3].x/256.f;  cp[2].ty = PhongMapping[fptr->v3].y/256.f;
   
	{
     for (u=0; u<vused; u++) cp[u].ev.v.z+= 8.0f;
     for (u=0; u<vused; u++) ClipVector(ClipZ,u);
     for (u=0; u<vused; u++) cp[u].ev.v.z-= 8.0f;
     if (vused<3) goto LNEXT;
    }
  
    if (CMASK & 1) for (u=0; u<vused; u++) ClipVector(ClipA,u); if (vused<3) goto LNEXT;
    if (CMASK & 2) for (u=0; u<vused; u++) ClipVector(ClipC,u); if (vused<3) goto LNEXT;    
    if (CMASK & 4) for (u=0; u<vused; u++) ClipVector(ClipB,u); if (vused<3) goto LNEXT;    
    if (CMASK & 8) for (u=0; u<vused; u++) ClipVector(ClipD,u); if (vused<3) goto LNEXT;	
                     
    for (u=0; u<vused-2; u++) {        	     		 
	   	 lpVertex->sx       = VideoCXf - cp[0].ev.v.x / cp[0].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[0].ev.v.y / cp[0].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[0].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = PHCOLOR;
		 lpVertex->specular = 0xFF000000;
         lpVertex->tu       = (float)(cp[0].tx);
         lpVertex->tv       = (float)(cp[0].ty);
         lpVertex++;
		 
	   	 lpVertex->sx       = VideoCXf - cp[u+1].ev.v.x / cp[u+1].ev.v.z * CameraW;
         lpVertex->sy       = VideoCYf + cp[u+1].ev.v.y / cp[u+1].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[u+1].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = PHCOLOR;
		 lpVertex->specular = 0xFF000000;
         lpVertex->tu       = (float)(cp[u+1].tx);
         lpVertex->tv       = (float)(cp[u+1].ty);
         lpVertex++;
		 
	   	 lpVertex->sx       = VideoCX - cp[u+2].ev.v.x / cp[u+2].ev.v.z * CameraW;
         lpVertex->sy       = VideoCY + cp[u+2].ev.v.y / cp[u+2].ev.v.z * CameraH;
         lpVertex->sz       = _ZSCALE / cp[u+2].ev.v.z;
         lpVertex->rhw      = lpVertex->sz * _AZSCALE;
         lpVertex->color    = PHCOLOR;
		 lpVertex->specular = 0xFF000000;
         lpVertex->tu       = (float)(cp[u+2].tx);
         lpVertex->tv       = (float)(cp[u+2].ty);
         lpVertex++;

	     fproc1++;
     }            
LNEXT:
     f = mptr->gFace[f].Next;
   }

  d3dFlushBuffer(fproc1, 0);
  SetRenderStates(TRUE, D3DBLEND_INVSRCALPHA);
}

void RenderModelSun(TModel* _mptr, float x0, float y0, float z0, int Alpha)
{   
   int f;   

   mptr = _mptr;

   int minx = 10241024;
   int maxx =-10241024;
   int miny = 10241024;
   int maxy =-10241024;

               
   for (int s=0; s<mptr->VCount; s++) {              
    rVertex[s].x = mptr->gVertex[s].x + x0;
	rVertex[s].y = mptr->gVertex[s].y + y0;
	rVertex[s].z = mptr->gVertex[s].z + z0;    

    if (rVertex[s].z>-64) gScrp[s].x = 0xFFFFFF; else {
     gScrp[s].x = VideoCX + (int)(rVertex[s].x / (-rVertex[s].z) * CameraW);
     gScrp[s].y = VideoCY - (int)(rVertex[s].y / (-rVertex[s].z) * CameraH); }

     if (gScrp[s].x > maxx) maxx = gScrp[s].x;
     if (gScrp[s].x < minx) minx = gScrp[s].x;
     if (gScrp[s].y > maxy) maxy = gScrp[s].y;
     if (gScrp[s].y < miny) miny = gScrp[s].y; 
   }   

   if (minx == 10241024) return;
   if (minx>WinW || maxx<0 || miny>WinH || maxy<0) return;
    
   BuildTreeNoSort(); 
   
   d3dSetTexture(mptr->lpTexture2, 128, 128);  
    
   d3dStartBuffer();

   DWORD alpha = Alpha;
   alpha = (alpha<<24) | 0x00FFFFFF;
   int fproc1 = 0;
   f = Current;
   while( f!=-1 ) {       

     TFace *fptr = & mptr->gFace[f];

	 fproc1++;

	 lpVertex->sx       = (float)gScrp[fptr->v1].x;
     lpVertex->sy       = (float)gScrp[fptr->v1].y;
     lpVertex->sz       = 0.0002;//_ZSCALE / rVertex[fptr->v1].z;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = alpha;
	 lpVertex->specular = 0xFF000000;
     lpVertex->tu       = (float)(fptr->tax);
     lpVertex->tv       = (float)(fptr->tay);
     lpVertex++;

	 lpVertex->sx       = (float)gScrp[fptr->v2].x;
     lpVertex->sy       = (float)gScrp[fptr->v2].y;
     lpVertex->sz       = 0.0002;//_ZSCALE / rVertex[fptr->v2].z;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = alpha;
	 lpVertex->specular = 0xFF000000;
     lpVertex->tu       = (float)(fptr->tbx);
     lpVertex->tv       = (float)(fptr->tby);
     lpVertex++;

	 lpVertex->sx       = (float)gScrp[fptr->v3].x;
     lpVertex->sy       = (float)gScrp[fptr->v3].y;
     lpVertex->sz       = 0.0002;//_ZSCALE / rVertex[fptr->v3].z;
     lpVertex->rhw      = 1.f;
     lpVertex->color    = alpha;
	 lpVertex->specular = 0xFF000000;
     lpVertex->tu       = (float)(fptr->tcx);
     lpVertex->tv       = (float)(fptr->tcy);
     lpVertex++;	 
     	           
     f = mptr->gFace[f].Next;
   }   

   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 2;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
   lpState->dwArg[0] = hTexture;
   lpState++;   
   
   lpState->drstRenderStateType = D3DRENDERSTATE_DESTBLEND;
   lpState->dwArg[0] = D3DBLEND_ONE;
   lpState++;

   
   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstruction->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstruction->wCount  = 1U;
   lpInstruction++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstruction;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = fproc1*3;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpProcessVertices;
   lpInstruction->bOpcode = D3DOP_TRIANGLE;
   lpInstruction->bSize   = sizeof(D3DTRIANGLE);
   lpInstruction->wCount  = fproc1;
   lpInstruction++;
   lpTriangle             = (LPD3DTRIANGLE)lpInstruction;   
   
   int ii = 0;
   for (int i=0; i<fproc1; i++) {  	   
	lpTriangle->wV1    = ii++;
    lpTriangle->wV2    = ii++;	
    lpTriangle->wV3    = ii++;	
	lpTriangle->wFlags = 0;
	lpTriangle++;	
   }

    lpInstruction = (LPD3DINSTRUCTION)lpTriangle;
  
    lpInstruction->bOpcode = D3DOP_STATERENDER;
    lpInstruction->bSize = sizeof(D3DSTATE);
    lpInstruction->wCount = 1;
    lpInstruction++;
    lpState = (LPD3DSTATE)lpInstruction;

    lpState->drstRenderStateType = D3DRENDERSTATE_DESTBLEND;
    lpState->dwArg[0] = D3DBLEND_INVSRCALPHA;
    lpState++;

	lpInstruction = (LPD3DINSTRUCTION)lpState;	

	lpInstruction->bOpcode = D3DOP_STATERENDER;
    lpInstruction->bSize = sizeof(D3DSTATE);
    lpInstruction->wCount = 3;
    lpInstruction++;
    lpState = (LPD3DSTATE)lpInstruction;

	lpState->drstRenderStateType = D3DRENDERSTATE_COLORKEYENABLE;
    lpState->dwArg[0] = FALSE;
    lpState++;

    lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
    lpState->dwArg[0] = D3DFILTER_LINEAR;
    lpState++;

    lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
    lpState->dwArg[0] = D3DFILTER_LINEAR;
    lpState++;
	lpInstruction = (LPD3DINSTRUCTION)lpState;
  

   
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);
   //if (FAILED(hRes)) DoHalt("Error execute buffer");   
   dFacesCount+=fproc1;

}






void RenderNearModel(TModel* _mptr, float x0, float y0, float z0, int light, float al, float bt)
{   
   BOOL b = LOWRESTX;
   Vector3d v;
   v.x = 0; v.y =-128; v.z = 0;   

   CalcFogLevel_Gradient(v);   
   FogYGrad = 0;
      
   LOWRESTX = FALSE;
   RenderModelClip(_mptr, x0, y0, z0, light, 0, al, bt);
   LOWRESTX = b;
}



void RenderModelClipWater(TModel* _mptr, float x0, float y0, float z0, int light, int VT, float al, float bt)
{   
}



void RenderCharacter(int index)
{
}

void RenderExplosion(int index)
{
}

void RenderShip()
{
}


void RenderElements()
{
	d3dLastTexture = d3dmemmapsize+1;
  	hTexture = NULL;
	int fproc1 = 0;
    
	for (int eg = 0; eg<ElCount; eg++) {				
		for (int e = 0; e<Elements[eg].ECount; e++) {
			Vector3d rpos;
			TElement *el = &Elements[eg].EList[e];
			rpos.x = el->pos.x - CameraX;
			rpos.y = el->pos.y - CameraY;
			rpos.z = el->pos.z - CameraZ;
			float r = el->R;

			rpos = RotateVector(rpos);	  
            if (rpos.z > -64) continue;
            if ( fabs(rpos.x) > -rpos.z) continue;            
            if ( fabs(rpos.y) > -rpos.z) continue;            

			if (!fproc1) d3dStartBuffer();

			float sx = VideoCXf - (CameraW * rpos.x / rpos.z);
			float sy = VideoCYf + (CameraH * rpos.y / rpos.z);
			RenderCircle(sx, sy, rpos.z, -r*CameraW*0.64 / rpos.z, Elements[eg].RGBA, Elements[eg].RGBA2);			
			fproc1+=8;
			if (fproc1>256) { 
			 d3dFlushBuffer(fproc1, 0);
			 fproc1 = 0;
			}
		} 
		
	}
	if (fproc1) d3dFlushBuffer(fproc1, 0);

	
    fproc1 = 0;	
	for (int b=0; b<BloodTrail.Count; b++) {
		Vector3d rpos = BloodTrail.Trail[b].pos;
		DWORD A1 = (0xE0 * BloodTrail.Trail[b].LTime / 20000); if (A1>0xE0) A1=0xE0;
		DWORD A2 = (0x20 * BloodTrail.Trail[b].LTime / 20000); if (A2>0x20) A2=0x20;
        rpos.x = rpos.x - CameraX;
		rpos.y = rpos.y - CameraY;
	    rpos.z = rpos.z - CameraZ;

		rpos = RotateVector(rpos);	  
        if (rpos.z > -64) continue;
        if ( fabs(rpos.x) > -rpos.z) continue;            
        if ( fabs(rpos.y) > -rpos.z) continue;            

		if (!fproc1) d3dStartBuffer();

        float sx = VideoCXf - (CameraW * rpos.x / rpos.z);
	    float sy = VideoCYf + (CameraH * rpos.y / rpos.z);
		RenderCircle(sx, sy, rpos.z, -12*CameraW*0.64 / rpos.z, (A1<<24)+conv_xGx(0x700000), (A2<<24)+conv_xGx(0x300000));
		fproc1+=8;
		
		if (fproc1>256) {
	      d3dFlushBuffer(fproc1, 0);
		  fproc1 = 0;
		}
	}
	
	if (fproc1) d3dFlushBuffer(fproc1, 0);

    if (SNOW)
    for (int s=0; s<SnCount; s++) {
		Vector3d rpos = Snow[s].pos;
		

        rpos.x = rpos.x - CameraX;
		rpos.y = rpos.y - CameraY;
	    rpos.z = rpos.z - CameraZ;


		rpos = RotateVector(rpos);	  
        if (rpos.z > -64) continue;
        if ( fabs(rpos.x) > -rpos.z) continue;            
        if ( fabs(rpos.y) > -rpos.z) continue;            		

        float sx = VideoCXf - CameraW * rpos.x / rpos.z;
	    float sy = VideoCYf + CameraH * rpos.y / rpos.z;
		
		DWORD A1 = 0xFF;
		DWORD A2 = 0x30;
		if (Snow[s].ftime) {
			A1 = A1 * (2000-Snow[s].ftime) / 2000;
			A2 = A2 * (2000-Snow[s].ftime) / 2000;
		}

        if (!fproc1) d3dStartBuffer();
        if (rpos.z>-1624) {
		 RenderCircle(sx, sy, rpos.z, -8*CameraW*0.64 / rpos.z, (A1<<24)+conv_xGx(0xF0F0F0), (A2<<24)+conv_xGx(0xB0B0B0));
         fproc1+=8; 
        } else {
		 RenderPoint(sx, sy, rpos.z, -8*CameraW*0.64 / rpos.z, (A1<<24)+conv_xGx(0xF0F0F0), (A2<<24)+conv_xGx(0xB0B0B0));
         fproc1++;
        }

		if (fproc1>d3debvc) {
	      d3dFlushBuffer(fproc1, 0);
		  fproc1 = 0;
		}
	}

    if (fproc1) d3dFlushBuffer(fproc1, 0);
		
    GlassL = 0;
}


void RenderCharacterPost(TCharacter *cptr)
{      
   //mdlScale = 1.0f;
	
   CreateChMorphedModel(cptr);   
   
   float zs = (float)sqrt( cptr->rpos.x*cptr->rpos.x  +  cptr->rpos.y*cptr->rpos.y  +  cptr->rpos.z*cptr->rpos.z);  
   if (zs > ctViewR*256) return;      

   GlassL = 0;      
   if (zs > 256 * (ctViewR-4)) 
	   GlassL = min(255, (zs/4 - 64*(ctViewR-4)));
	

   waterclip = FALSE;     
      

   if ( cptr->rpos.z >-256*10) 
    RenderModelClip(cptr->pinfo->mptr, 
                cptr->rpos.x, cptr->rpos.y, cptr->rpos.z, 210, 0,
                -cptr->alpha + pi / 2 + CameraAlpha, 
                CameraBeta );   
   else
    RenderModel(cptr->pinfo->mptr, 
                cptr->rpos.x, cptr->rpos.y, cptr->rpos.z, 210, 0,
                -cptr->alpha + pi / 2 + CameraAlpha, 
                CameraBeta );

      
   if (!SHADOWS3D) return;
   if (zs > 256 * (ctViewR-8)) return;   
   
   int Al = 0x60;
   
   if (cptr->Health==0) {    
    int at = cptr->pinfo->Animation[cptr->Phase].AniTime;
	if (Tranq) return;	
    if (cptr->FTime==at-1) return;
    Al = Al * (at-cptr->FTime) / at;  }
   	if (cptr->AI==0) Al = 0x50;

    GlassL = (Al<<24) + 0x00222222;
   
   RenderShadowClip(cptr->pinfo->mptr, 
                cptr->pos.x, cptr->pos.y, cptr->pos.z,
                cptr->rpos.x, cptr->rpos.y, cptr->rpos.z, 
                pi/2-cptr->alpha,
                CameraAlpha, 
                CameraBeta );   

}



void RenderShipPost()
{
   if (Ship.State==-1) return;
   GlassL = 0;      
   zs = (int)VectorLength(Ship.rpos);
   if (zs > 256 * (ctViewR)) return;
   
   if (zs > 256 * (ctViewR-4)) 
	GlassL = min(255,(int)(zs - 256 * (ctViewR-4)) / 4);
   
      
   /*grConstantColorValue( (255-GlassL) << 24);*/

   CreateMorphedModel(ShipModel.mptr, &ShipModel.Animation[0], Ship.FTime, 1.0);
        
   if ( fabs(Ship.rpos.z) < 4000) 
    RenderModelClip(ShipModel.mptr,
                    Ship.rpos.x, Ship.rpos.y, Ship.rpos.z, 210, 0, -Ship.alpha -pi/2 + CameraAlpha, CameraBeta);
   else   
    RenderModel(ShipModel.mptr,
                Ship.rpos.x, Ship.rpos.y, Ship.rpos.z, 210, 0, -Ship.alpha -pi/2+ CameraAlpha, CameraBeta);
   
   /*grConstantColorValue( 0xFF000000);*/
}






void Render3DHardwarePosts()
{
	   

   

   TCharacter *cptr;
   for (int c=0; c<ChCount; c++) {
      cptr = &Characters[c];
      cptr->rpos.x = cptr->pos.x - CameraX;
      cptr->rpos.y = cptr->pos.y - CameraY;
      cptr->rpos.z = cptr->pos.z - CameraZ;

	        
      float r = (float)max( fabs(cptr->rpos.x), fabs(cptr->rpos.z) );
      int ri = -1 + (int)(r / 256.f + 0.5f);
      if (ri < 0) ri = 0;
      if (ri > ctViewR) continue;

	  if (FOGON) 
	   CalcFogLevel_Gradient(cptr->rpos);	  	  	          	  
	  
      cptr->rpos = RotateVector(cptr->rpos);

	  float br = BackViewR + DinoInfo[cptr->CType].Radius;
      if (cptr->rpos.z > br) continue;
      if ( fabs(cptr->rpos.x) > -cptr->rpos.z + br ) continue;            
      if ( fabs(cptr->rpos.y) > -cptr->rpos.z + br ) continue;            

      RenderCharacterPost(cptr);
   }   



   Ship.rpos.x = Ship.pos.x - CameraX;
   Ship.rpos.y = Ship.pos.y - CameraY;
   Ship.rpos.z = Ship.pos.z - CameraZ;
   float r = (float)max( fabs(Ship.rpos.x), fabs(Ship.rpos.z) );

   int ri = -1 + (int)(r / 256.f + 0.2f);
   if (ri < 0) ri = 0;
   if (ri < ctViewR) {	  
	  if (FOGON) 
	   CalcFogLevel_Gradient(Ship.rpos);	  	  	   

      Ship.rpos = RotateVector(Ship.rpos);
      if (Ship.rpos.z > BackViewR) goto NOSHIP;
      if ( fabs(Ship.rpos.x) > -Ship.rpos.z + BackViewR ) goto NOSHIP;

      RenderShipPost();
   }
NOSHIP: ;

   SunLight *= GetTraceK(SunScrX, SunScrY);   
}





void ClearVideoBuf()
{  
}



int  CircleCX, CircleCY;
WORD CColor;

void PutPixel(int x, int y)
{ *((WORD*)ddsd.lpSurface + y*lsw + x) = CColor; }

void Put8pix(int X,int Y)
{
  PutPixel(CircleCX + X, CircleCY + Y);
  PutPixel(CircleCX + X, CircleCY - Y);
  PutPixel(CircleCX - X, CircleCY + Y);
  PutPixel(CircleCX - X, CircleCY - Y);
  PutPixel(CircleCX + Y, CircleCY + X);
  PutPixel(CircleCX + Y, CircleCY - X);
  PutPixel(CircleCX - Y, CircleCY + X);
  PutPixel(CircleCX - Y, CircleCY - X);
}

void DrawCircle(int cx, int cy, int R)
{
   int d = 3 - (2 * R);
   int x = 0;
   int y = R;
   CircleCX=cx; 
   CircleCY=cy;
   do {
     Put8pix(x,y); x++;
     if (d < 0) d = d + (x<<2) + 6;  else 
	 { d = d + (x - y) * 4 + 10; y--; }
   } while (x<y);
   Put8pix(x,y);
}

void DrawBox( WORD *lfbPtr, int lsw, int xx, int yy, WORD c)
{
  yy = yy * lsw + xx;
  *(lfbPtr + yy) = c;
  *(lfbPtr + yy + 1) = c;
  yy+=lsw;
  *(lfbPtr + yy) = c;
  *(lfbPtr + yy + 1) = c;
}


void DrawHMap()
{  
  int c;

  DrawPicture(VideoCX-MapPic.W/2, VideoCY - MapPic.H/2-6, MapPic);
  
  ZeroMemory( &ddsd, sizeof(DDSURFACEDESC) );
  ddsd.dwSize = sizeof(DDSURFACEDESC);
  if( lpddBack->Lock( NULL, &ddsd, DDLOCK_WAIT, NULL ) != DD_OK ) return;    

  lsw = ddsd.lPitch / 2;
  int RShift, GShift;

  if (VMFORMAT565) {
	  RShift=11; GShift=6; 
  } else {
	  RShift=10; GShift=5; 
  }  

  int xx = VideoCX - 128 + (CCX>>2);
  int yy = VideoCY - 128 + (CCY>>2);

  if (yy<0 || yy>=WinH) goto endmap;
  if (xx<0 || xx>=WinW) goto endmap;
  DrawBox((WORD*)ddsd.lpSurface, lsw, xx+1, yy+1, 8<<RShift);
  DrawBox((WORD*)ddsd.lpSurface, lsw, xx, yy, 30<<RShift);
  /*
  *((WORD*)ddsd.lpSurface + yy*lsw + xx) = 30<<RShift;
  *((WORD*)ddsd.lpSurface + yy*lsw + xx + 1) = 30<<RShift;
  yy++;
  *((WORD*)ddsd.lpSurface + yy*lsw + xx) = 30<<RShift;
  *((WORD*)ddsd.lpSurface + yy*lsw + xx + 1) = 30<<RShift;
  */

  CColor =  4<<GShift; DrawCircle(xx+1, yy+1, (ctViewR/4));
  CColor = 18<<GShift; DrawCircle(xx, yy, (ctViewR/4));

  if (RadarMode)
  for (c=0; c<ChCount; c++) {   
   if (Characters[c].AI<10) continue;
   if (! (TargetDino & (1<<Characters[c].AI)) ) continue;
   if (!Characters[c].Health) continue;
   xx = VideoCX - 128 + (int)Characters[c].pos.x / 1024;
   yy = VideoCY - 128 + (int)Characters[c].pos.z / 1024;
   if (yy<=0 || yy>=WinH) goto endmap;
   if (xx<=0 || xx>=WinW) goto endmap;
   DrawBox((WORD*)ddsd.lpSurface, lsw, xx, yy, 31);   
  }
  
endmap:
  lpddBack->Unlock(ddsd.lpSurface);
}




void RenderSun(float x, float y, float z)
{
	SunScrX = VideoCX + (int)(x / (-z) * CameraW);
    SunScrY = VideoCY - (int)(y / (-z) * CameraH); 
	GetSkyK(SunScrX, SunScrY);	
	
	float d = (float)sqrt(x*x + y*y);
	if (d<2048) {
		SunLight = (220.f- d*220.f/2048.f);
		if (SunLight>140) SunLight = 140;
		SunLight*=SkyTraceK;
	}
	
     
	if (d>812.f) d = 812.f;
	d = (2048.f + d) / 3048.f;
	d+=(1.f-SkyTraceK)/2.f;
	if (OptDayNight==2)  d=1.5; 
    RenderModelSun(SunModel,  x*d, y*d, z*d, (int)(200.f* SkyTraceK));	
}



void RotateVVector(Vector3d& v)
{
   float x = v.x * ca - v.z * sa;
   float y = v.y;
   float z = v.z * ca + v.x * sa;

   float xx = x;
   float xy = y * cb + z * sb;
   float xz = z * cb - y * sb;
   
   v.x = xx; v.y = xy; v.z = xz;
}







void RenderSkyPlane()
{	
	
   Vector3d v,vbase;
   Vector3d tx,ty,nv;
   float p,q, qx, qy, qz, px, py, pz, rx, ry, rz, ddx, ddy;
   float lastdt = 0.f;

   d3dSetTexture(SkyPic, 256, 256);
   
   nv.x = 512; nv.y = 4024; nv.z=0;   

   cb = (float)f_cos(CameraBeta);
   sb = (float)f_sin(CameraBeta);
   SKYDTime = RealTime & ((1<<17) - 1);   

   float sh = - CameraY;

   if (MapMinY==10241024) 
	   MapMinY=0;
   sh = (float)((int)MapMinY)*ctHScale - CameraY;

   if (sh<-2024) sh=-2024;

   v.x = 0;
   v.z = ctViewR*256.f;
   v.y = sh;
   
   vbase.x = v.x;
   vbase.y = v.y * cb + v.z * sb;
   vbase.z = v.z * cb - v.y * sb;   
   if (vbase.z < 128) vbase.z = 128;
   int scry = VideoCY - (int)(vbase.y / vbase.z * CameraH);
   
   if (scry<0) return; 
   if (scry>WinEY+1) scry = WinEY+1;
   
   cb = (float)f_cos(CameraBeta-0.15);
   sb = (float)f_sin(CameraBeta-0.15);

   v.x = 0;
   v.z = 2*256.f*256.f;   
   v.y = 512;
   vbase.x = v.x;
   vbase.y = v.y * cb + v.z * sb;
   vbase.z = v.z * cb - v.y * sb;   
   if (vbase.z < 128) vbase.z = 128;
   int _scry = VideoCY - (int)(vbase.y / vbase.z * CameraH);
   if (scry > _scry) scry = _scry;

   
   tx.x=0.002f;  tx.y=0;     tx.z=0;
   ty.x=0.0f;    ty.y=0;     ty.z=0.002f;
   nv.x=0;       nv.y=-1.f;  nv.z=0;
      
   RotateVVector(tx);
   RotateVVector(ty);
   RotateVVector(nv);
      
   sh = 4*512*16;
   vbase.x = -CameraX;
   vbase.y = sh;
   vbase.z = +CameraZ;
   RotateVVector(vbase);

//============= calc render params =================//
   p = nv.x * vbase.x + nv.y * vbase.y + nv.z * vbase.z;
   ddx = vbase.x * tx.x  +  vbase.y * tx.y  +  vbase.z * tx.z;
   ddy = vbase.x * ty.x  +  vbase.y * ty.y  +  vbase.z * ty.z;   

   qx = CameraH * nv.x;   qy = CameraW * nv.y;   qz = CameraW*CameraH  * nv.z;
   px = p*CameraH*tx.x;   py = p*CameraW*tx.y;   pz = p*CameraW*CameraH* tx.z;
   rx = p*CameraH*ty.x;   ry = p*CameraW*ty.y;   rz = p*CameraW*CameraH* ty.z;

   px=px - ddx*qx;  py=py - ddx*qy;   pz=pz - ddx*qz;
   rx=rx - ddy*qx;  ry=ry - ddy*qy;   rz=rz - ddy*qz;

   float za = CameraW * CameraH * p / (qy * VideoCY + qz);
   float zb = CameraW * CameraH * p / (qy * (VideoCY-scry/2.f) + qz);
   float zc = CameraW * CameraH * p / (qy * (VideoCY-scry) + qz);

   float _za = fabs(za) - 100200.f; if (_za<0) _za=0.f;
   float _zb = fabs(zb) - 100200.f; if (_zb<0) _zb=0.f;
   float _zc = fabs(zc) - 100200.f; if (_zc<0) _zc=0.f;
   
   int alpha = (int)(255*40240 / (40240+_za));
   int alphb = (int)(255*40240 / (40240+_zb));
   int alphc = (int)(255*40240 / (40240+_zc));
   
   int sx1 = - VideoCX;
   int sx2 = + VideoCX;      

   float qx1 = qx * sx1 + qz;
   float qx2 = qx * sx2 + qz;
   float qyy;


//   d3dStartBuffer();
   ZeroMemory(&d3dExeBufDesc, sizeof(d3dExeBufDesc));
   d3dExeBufDesc.dwSize = sizeof(d3dExeBufDesc);
   hRes = lpd3dExecuteBufferG->Lock( &d3dExeBufDesc );
   if (FAILED(hRes)) DoHalt("Error locking execute buffer");
   lpVertex = (LPD3DTLVERTEX)d3dExeBufDesc.lpData;   

   float dtt = (float)(SKYDTime) / 512.f;

    float sky=0; 
	float sy = VideoCY - sky;
	qyy = qy * sy;
	q = qx1 + qyy;
	float fxa = (px * sx1 + py * sy + pz) / q;
	float fya = (rx * sx1 + ry * sy + rz) / q;
	q = qx2 + qyy;	
	float fxb = (px * sx2 + py * sy + pz) / q;
	float fyb = (rx * sx2 + ry * sy + rz) / q;	
            
	lpVertex->sx       = 0.f;
    lpVertex->sy       = (float)sky;
    lpVertex->sz       = 0.0001f;//-8.f / za;
    lpVertex->rhw      = 128.f / za;
	/*if (FOGENABLE) {
     lpVertex->color    = 0xFFFFFFFF;
	 lpVertex->specular = alpha<<24;               } else */{
	 lpVertex->color    = 0x00FFFFFF | alpha<<24;
	 lpVertex->specular = 0xFF000000;              }
    lpVertex->tu       = (fxa + dtt) / 256.f;
    lpVertex->tv       = (fya - dtt) / 256.f;
    lpVertex++;

	lpVertex->sx       = (float)WinW;
    lpVertex->sy       = (float)sky;
    lpVertex->sz       = 0.0001f;//-8.f / za;
    lpVertex->rhw      = 128.f / za;
    /*if (FOGENABLE) {
     lpVertex->color    = 0xFFFFFFFF;
	 lpVertex->specular = alpha<<24;               } else */{
	 lpVertex->color    = 0x00FFFFFF | alpha<<24;
	 lpVertex->specular = 0xFF000000;              }
    lpVertex->tu       = (fxb + dtt) / 256.f;
    lpVertex->tv       = (fyb - dtt) / 256.f;
    lpVertex++;           


	sky=scry/2.f; 
	sy = VideoCY - sky;
	qyy = qy * sy;
	q = qx1 + qyy;
	fxa = (px * sx1 + py * sy + pz) / q;
	fya = (rx * sx1 + ry * sy + rz) / q;	
	q = qx2 + qyy;	
	fxb = (px * sx2 + py * sy + pz) / q;
	fyb = (rx * sx2 + ry * sy + rz) / q;    	
        
	lpVertex->sx       = 0.f;
    lpVertex->sy       = (float)sky;
    lpVertex->sz       = 0.0001f;//-8.f / zb;
    lpVertex->rhw      = 128.f / zb;
    /*if (FOGENABLE) {
     lpVertex->color    = 0xFFFFFFFF;
	 lpVertex->specular = alphb<<24;               } else */{
	 lpVertex->color    = 0x00FFFFFF | alphb<<24;
	 lpVertex->specular = 0xFF000000;              }
    lpVertex->tu       = (fxa + dtt) / 256.f;
    lpVertex->tv       = (fya - dtt) / 256.f;
    lpVertex++;

	lpVertex->sx       = (float)WinW;
    lpVertex->sy       = (float)sky;
    lpVertex->sz       = 0.0001f;//-8.f / zb;
    lpVertex->rhw      = 128.f / zb;
    /*if (FOGENABLE) {
     lpVertex->color    = 0xFFFFFFFF;
	 lpVertex->specular = alphb<<24;               } else */{
	 lpVertex->color    = 0x00FFFFFF | alphb<<24;
	 lpVertex->specular = 0xFF000000;              }
    lpVertex->tu       = (fxb + dtt) / 256.f;
    lpVertex->tv       = (fyb - dtt) / 256.f;
    lpVertex++;           




	sky=(float)scry; 
	sy = VideoCY - sky;
	qyy = qy * sy;
	q = qx1 + qyy;
	fxa = (px * sx1 + py * sy + pz) / q;
	fya = (rx * sx1 + ry * sy + rz) / q;	
	q = qx2 + qyy;	
	fxb = (px * sx2 + py * sy + pz) / q;
	fyb = (rx * sx2 + ry * sy + rz) / q;    	
        
	lpVertex->sx       = 0.f;
    lpVertex->sy       = (float)sky;
    lpVertex->sz       = 0.0001f;//-8.f / zb;
    lpVertex->rhw      = 128.f / zc;
    /*if (FOGENABLE) {
     lpVertex->color    = 0xFFFFFFFF;
	 lpVertex->specular = alphc<<24;               } else */{
	 lpVertex->color    = 0x00FFFFFF | alphc<<24;
	 lpVertex->specular = 0xFF000000;              }
    lpVertex->tu       = (fxa + dtt) / 256.f;
    lpVertex->tv       = (fya - dtt) / 256.f;
    lpVertex++;

	lpVertex->sx       = (float)WinW;
    lpVertex->sy       = (float)sky;
    lpVertex->sz       = 0.0001f;//-8.f / zb;
    lpVertex->rhw      = 128.f / zc;
    /*if (FOGENABLE) {
     lpVertex->color    = 0xFFFFFFFF;
	 lpVertex->specular = alphc<<24;               } else */{
	 lpVertex->color    = 0x00FFFFFF | alphc<<24;
	 lpVertex->specular = 0xFF000000;              }
    lpVertex->tu       = (fxb + dtt) / 256.f;
    lpVertex->tv       = (fyb - dtt) / 256.f;
    lpVertex++;           
	
   

   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvcG*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 4;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
   lpState->dwArg[0] = hTexture;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMAG;
   lpState->dwArg[0] = D3DFILTER_LINEAR;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREMIN;
   lpState->dwArg[0] = D3DFILTER_LINEAR;
   lpState++;

   lpState->drstRenderStateType = D3DRENDERSTATE_FOGCOLOR;
   if (UNDERWATER) lpState->dwArg[0] = CurFogColor;
              else lpState->dwArg[0] = (SkyR<<16) + (SkyG<<8) + SkyB;
   lpState++;

   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstruction->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstruction->wCount  = 1U;
   lpInstruction++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstruction;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = 6;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;

   
   lpInstruction = (LPD3DINSTRUCTION)lpProcessVertices;
   lpInstruction->bOpcode = D3DOP_TRIANGLE;
   lpInstruction->bSize   = sizeof(D3DTRIANGLE);
   lpInstruction->wCount  = 4;
   lpInstruction++;
   lpTriangle             = (LPD3DTRIANGLE)lpInstruction;   
      
   lpTriangle->wV1    = 0;
   lpTriangle->wV2    = 1;	
   lpTriangle->wV3    = 2;
   lpTriangle->wFlags = 0;
   lpTriangle++;
   
   lpTriangle->wV1    = 1;
   lpTriangle->wV2    = 2;	
   lpTriangle->wV3    = 3;
   lpTriangle->wFlags = 0;
   lpTriangle++;


   lpTriangle->wV1    = 2;
   lpTriangle->wV2    = 3;	
   lpTriangle->wV3    = 4;
   lpTriangle->wFlags = 0;
   lpTriangle++;
   
   lpTriangle->wV1    = 3;
   lpTriangle->wV2    = 4;	
   lpTriangle->wV3    = 5;
   lpTriangle->wFlags = 0;
   lpTriangle++;

   lpInstruction = (LPD3DINSTRUCTION)lpTriangle;
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBufferG->Unlock( );
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBufferG, lpd3dViewport, D3DEXECUTE_UNCLIPPED);

   LINEARFILTER = TRUE;
            
   nv = RotateVector(Sun3dPos);
   SunLight = 0;
   if (nv.z < -2024) RenderSun(nv.x, nv.y, nv.z);	
}



void RenderFSRect(DWORD Color)
{
    d3dStartBuffer();

	lpVertex->sx       = 0.f;
    lpVertex->sy       = 0.f;
    lpVertex->sz       = 0.999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = Color;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;

	lpVertex->sx       = (float)WinW;
    lpVertex->sy       = 0.f;
    lpVertex->sz       = 0.999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = Color;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;

	lpVertex->sx       = 0.f;
    lpVertex->sy       = (float)WinH;
    lpVertex->sz       = 0.999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = Color;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;

	lpVertex->sx       = (float)WinW;
    lpVertex->sy       = (float)WinH;
    lpVertex->sz       = (float)0.999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = Color;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;


   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 1;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
   lpState->dwArg[0] = NULL;
   lpState++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstruction->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstruction->wCount  = 1U;
   lpInstruction++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstruction;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = 4;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;

   
   lpInstruction = (LPD3DINSTRUCTION)lpProcessVertices;
   lpInstruction->bOpcode = D3DOP_TRIANGLE;
   lpInstruction->bSize   = sizeof(D3DTRIANGLE);
   lpInstruction->wCount  = 2;
   lpInstruction++;
   lpTriangle             = (LPD3DTRIANGLE)lpInstruction;   
      
   lpTriangle->wV1    = 0;
   lpTriangle->wV2    = 1;	
   lpTriangle->wV3    = 2;
   lpTriangle->wFlags = 0;
   lpTriangle++;
   
   lpTriangle->wV1    = 1;
   lpTriangle->wV2    = 2;	
   lpTriangle->wV3    = 3;
   lpTriangle->wFlags = 0;
   lpTriangle++;

   lpInstruction = (LPD3DINSTRUCTION)lpTriangle;
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);	
}




void RenderHealthBar()
{

  if (MyHealth >= 100000) return;
  if (MyHealth == 000000) return;

  
  int L = WinW / 4;
  int x0 = WinW - (WinW / 20) - L;
  int y0 = WinH / 40;
  int G = min( (MyHealth * 240 / 100000), 160);
  int R = min( ( (100000 - MyHealth) * 240 / 100000), 160);
  
    
  int L0 = (L * MyHealth) / 100000;
  int H = WinH / 200;

  d3dStartBuffer();

  for (int y=0; y<4; y++) {	  
	lpVertex->sx       = (float)x0-1;
    lpVertex->sy       = (float)y0+y;
    lpVertex->sz       = 0.9999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0xF0000010;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;

	lpVertex->sx       = (float)x0+L0+1;
    lpVertex->sy       = (float)y0+y;
    lpVertex->sz       = 0.9999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0xF0000010;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;
  }

  for (y=1; y<3; y++) {	  
	lpVertex->sx       = (float)x0;
    lpVertex->sy       = (float)y0+y;
    lpVertex->sz       = 0.99999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0xF0000000 + (G<<8) + (R<<16);
	lpVertex->specular = 0xFF000000;// + (G<<8) + (R<<16);
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;

	lpVertex->sx       = (float)x0+L0;
    lpVertex->sy       = (float)y0+y;
    lpVertex->sz       = 0.99999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0xF0000000 + (G<<8) + (R<<16);
	lpVertex->specular = 0xFF000000;// + (G<<8) + (R<<16);
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;
  }



   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 1;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
   lpState->dwArg[0] = NULL;
   lpState++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstruction->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstruction->wCount  = 1U;
   lpInstruction++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstruction;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = 12;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpProcessVertices;
   lpInstruction->bOpcode = D3DOP_LINE;
   lpInstruction->bSize   = sizeof(D3DLINE);
   lpInstruction->wCount  = 6;
   lpInstruction++;
   lpLine                 = (LPD3DLINE)lpInstruction;   

   for (y=0; y<6; y++) {
    lpLine->wV1    = y*2;
    lpLine->wV2    = y*2+1;   
    lpLine++;
   }
      
   lpInstruction = (LPD3DINSTRUCTION)lpLine;
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);	

}



void Render_Cross(int sx, int sy) 
{

	float w = (float) WinW / 12.f;
	d3dStartBuffer();

	lpVertex->sx       = (float)sx-w;
    lpVertex->sy       = (float)sy;
    lpVertex->sz       = 0.99999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0x80000010;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;

	lpVertex->sx       = (float)sx+w;
    lpVertex->sy       = (float)sy;
    lpVertex->sz       = 0.99999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0x80000010;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;


    lpVertex->sx       = (float)sx;
    lpVertex->sy       = (float)sy-w;
    lpVertex->sz       = 0.99999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0x80000010;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;


	lpVertex->sx       = (float)sx;
    lpVertex->sy       = (float)sy+w;
    lpVertex->sz       = 0.99999f;
    lpVertex->rhw      = 1.f;
    lpVertex->color    = 0x80000010;
	lpVertex->specular = 0xFF000000;
    lpVertex->tu       = 0;
    lpVertex->tv       = 0;
    lpVertex++;


   lpInstruction = (LPD3DINSTRUCTION) ((LPD3DTLVERTEX)d3dExeBufDesc.lpData + d3debvc*3);
   lpInstruction->bOpcode = D3DOP_STATERENDER;
   lpInstruction->bSize = sizeof(D3DSTATE);
   lpInstruction->wCount = 1;
   lpInstruction++;
   lpState = (LPD3DSTATE)lpInstruction;

   lpState->drstRenderStateType = D3DRENDERSTATE_TEXTUREHANDLE;
   lpState->dwArg[0] = NULL;
   lpState++;
   
   lpInstruction = (LPD3DINSTRUCTION)lpState;
   lpInstruction->bOpcode = D3DOP_PROCESSVERTICES;
   lpInstruction->bSize   = sizeof(D3DPROCESSVERTICES);
   lpInstruction->wCount  = 1U;
   lpInstruction++;

   lpProcessVertices = (LPD3DPROCESSVERTICES)lpInstruction;
   lpProcessVertices->dwFlags    = D3DPROCESSVERTICES_COPY;
   lpProcessVertices->wStart     = 0U;
   lpProcessVertices->wDest      = 0U;
   lpProcessVertices->dwCount    = 4;
   lpProcessVertices->dwReserved = 0UL;
   lpProcessVertices++;

   
   lpInstruction = (LPD3DINSTRUCTION)lpProcessVertices;
   lpInstruction->bOpcode = D3DOP_LINE;
   lpInstruction->bSize   = sizeof(D3DLINE);
   lpInstruction->wCount  = 2;
   lpInstruction++;
   lpLine                 = (LPD3DLINE)lpInstruction;   
      
   lpLine->wV1    = 0;
   lpLine->wV2    = 1;   
   lpLine++;

   lpLine->wV1    = 2;
   lpLine->wV2    = 3;   
   lpLine++;
      
   lpInstruction = (LPD3DINSTRUCTION)lpLine;
   lpInstruction->bOpcode = D3DOP_EXIT;
   lpInstruction->bSize   = 0UL;
   lpInstruction->wCount  = 0U;

   lpd3dExecuteBuffer->Unlock( );
   
   hRes = lpd3dDevice->Execute(lpd3dExecuteBuffer, lpd3dViewport, D3DEXECUTE_UNCLIPPED);	


}


#endif

