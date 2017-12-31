#include "Hunt.h"
#include "stdio.h"

BOOL NewPhase;

#define fx_DIE    0




//======= new ============//
#define PIG_WALK   0
#define PIG_RUN    1
#define PIG_IDLE1  2 
#define PIG_IDLE2  3
#define PIG_IDLE3  4
#define PIG_DIE    5
#define PIG_SLP    6

#define DIM_FLY    0
#define DIM_FLYP   1
#define DIM_FALL   2
#define DIM_DIE    3

#define BRO_WALK   0
#define BRO_RUN    1
#define BRO_IDLE1  2 
#define BRO_IDLE2  3
#define BRO_IDLE3  4
#define BRO_DIE    5
#define BRO_SLP    6

#define DIA_WALK   0
#define DIA_RUN    1
#define DIA_SWIM   1
#define DIA_IDLE1  2
#define DIA_IDLE2  3
#define DIA_JUMP   4
#define DIA_SLIDE  5
#define DIA_EAT    6
#define DIA_DIE    7
#define DIA_SLP    8

#define DER_WALK   0
#define DER_RUN    1
#define DER_IDLE1  2 
#define DER_IDLE2  3
#define DER_DIE    4
#define DER_SLP    5

#define MAM_WALK   0
#define MAM_RUN    1
#define MAM_IDLE1  2 
#define MAM_IDLE2  3
#define MAM_DIE    4
#define MAM_SLP    5


// 

int CurDino;

void SetNewTargetPlace(TCharacter *cptr, float R);


void ProcessPrevPhase(TCharacter *cptr)
{
   cptr->PPMorphTime+=TimeDt;
   if (cptr->PPMorphTime > PMORPHTIME) cptr->PrevPhase = cptr->Phase;

   cptr->PrevPFTime+=TimeDt;
   cptr->PrevPFTime %= cptr->pinfo->Animation[cptr->PrevPhase].AniTime;
}


void ActivateCharacterFx(TCharacter *cptr)
{
	if (cptr->AI) //== not hunter ==//
	 if (UNDERWATER) return;
	int fx = cptr->pinfo->Anifx[cptr->Phase];
	if (fx==-1) return;

	if ( VectorLength(SubVectors(PlayerPos, cptr->pos) ) > 68 * 256) return;

    AddVoice3d(cptr->pinfo->SoundFX[fx].length,
               cptr->pinfo->SoundFX[fx].lpData,
               cptr->pos.x,cptr->pos.y,cptr->pos.z);	    
}


void ResetCharacter(TCharacter *cptr)
{
	 cptr->AI     = DinoInfo[cptr->CType].AI;
     cptr->pinfo  = &ChInfo[cptr->CType];
	 cptr->State  =  0;
	 cptr->StateF =  0;
	 cptr->Phase  =  0;
     cptr->FTime  =  0;
	 cptr->PrevPhase   = 0;
	 cptr->PrevPFTime  = 0;
	 cptr->PPMorphTime = 0;
	 cptr->beta    =  0;
	 cptr->gamma   =  0;
	 cptr->tggamma =  0;
	 cptr->bend    =  0;
	 cptr->rspeed  =  0;
	 cptr->AfraidTime = 0;
	 cptr->BloodTTime = 0;
	 cptr->BloodTime  = 0;

	 cptr->lookx = (float)f_cos(cptr->alpha);
     cptr->lookz = (float)f_sin(cptr->alpha);
	 
	 cptr->Health = DinoInfo[cptr->CType].Health0;
	 if (OptAgres>128) cptr->Health= (cptr->Health*OptAgres)/128;

	 cptr->scale =  (float)(DinoInfo[cptr->CType].Scale0 + rRand(DinoInfo[cptr->CType].ScaleA)) / 1000.f;
}


void AddDeadBody(TCharacter *cptr, int phase)
{
   if (!MyHealth) return;

   if (ExitTime) 
	  AddMessage("Transportation cancelled.");  
   ExitTime = 0;

   OPTICMODE = FALSE;
   BINMODE = FALSE;
   Characters[ChCount].CType = 0;
   Characters[ChCount].alpha = CameraAlpha;
   ResetCharacter(&Characters[ChCount]);

   int v = rRand(3);
   if (phase != HUNT_BREATH)
      AddVoicev(fxScream[r].length, fxScream[r].lpData, 256);

   Characters[ChCount].Health = 0;
   MyHealth = 0;
   if (cptr) {
	 float pl = 170;
	 //if (cptr->AI==AI_SPINO) pl = 200.f;
	 //if (cptr->AI==AI_CERAT) pl = 320.f;	 
     Characters[ChCount].pos.x = cptr->pos.x + cptr->lookx * pl * cptr->scale;
     Characters[ChCount].pos.z = cptr->pos.z + cptr->lookz * pl * cptr->scale;
	 Characters[ChCount].pos.y = GetLandQH(Characters[ChCount].pos.x, Characters[ChCount].pos.z);
   } else {
	 Characters[ChCount].pos.x = PlayerX;
     Characters[ChCount].pos.z = PlayerZ;
	 Characters[ChCount].pos.y = PlayerY;
   }
   
   Characters[ChCount].Phase = phase;
   Characters[ChCount].PrevPhase = phase;

   ActivateCharacterFx(&Characters[ChCount]);
   

   DemoPoint.pos = Characters[ChCount].pos;	          
   DemoPoint.DemoTime = 1;		 	 
   DemoPoint.CIndex = ChCount;

   ChCount++;
}



float AngleDifference(float a, float b)
{
 a-=b;
 a = (float)fabs(a);
 if (a > pi) a = 2*pi - a;
 return a;
}

float CorrectedAlpha(float a, float b)
{
	float d = (float)fabs(a-b);
	if (d<pi) return (a+b)/2;
	     else d = (a+pi*2-b);

	if (d<0) d+=2*pi;
	if (d>2*pi) d-=2*pi;
	return d;
}

void ThinkY_Beta_Gamma(TCharacter *cptr, float blook, float glook, float blim, float glim)
{
    cptr->pos.y = GetLandH(cptr->pos.x, cptr->pos.z);

    //=== beta ===//
    float hlook  = GetLandH(cptr->pos.x + cptr->lookx * blook, cptr->pos.z + cptr->lookz * blook);
    float hlook2 = GetLandH(cptr->pos.x - cptr->lookx * blook, cptr->pos.z - cptr->lookz * blook);    
	DeltaFunc(cptr->beta, (hlook2 - hlook) / (blook * 3.2f), TimeDt / 800.f);

    if (cptr->beta > blim) cptr->beta = blim;
    if (cptr->beta <-blim) cptr->beta =-blim;

    //=== gamma ===//
    hlook = GetLandH(cptr->pos.x + cptr->lookz * glook, cptr->pos.z - cptr->lookx*glook);
    hlook2 = GetLandH(cptr->pos.x - cptr->lookz * glook, cptr->pos.z + cptr->lookx*glook);
    cptr->tggamma =(hlook - hlook2) / (glook * 3.2f);
    if (cptr->tggamma > glim) cptr->tggamma = glim;
    if (cptr->tggamma <-glim) cptr->tggamma =-glim;      
/*
	if (DEBUG) cptr->tggamma = 0;
	if (DEBUG) cptr->beta    = 0;
	*/
}




int CheckPlaceCollisionP(Vector3d &v)
{
   int ccx = (int)v.x / 256;
   int ccz = (int)v.z / 256;

   if (ccx<4 || ccz<4 || ccx>1008 || ccz>1008) return 1;

   int F = (FMap[ccz][ccx-1] | FMap[ccz-1][ccx] | FMap[ccz-1][ccx-1] |
	        FMap[ccz][ccx] | 
		    FMap[ccz+1][ccx] | FMap[ccz][ccx+1] | FMap[ccz+1][ccx+1]);

   if (F & (fmWater + fmNOWAY)) return 1;


   float h = GetLandH(v.x, v.z);
   v.y = h;

   float hh = GetLandH(v.x-164, v.z-164); if ( fabs(hh-h) > 160 ) return 1;
         hh = GetLandH(v.x+164, v.z-164); if ( fabs(hh-h) > 160 ) return 1;
		 hh = GetLandH(v.x-164, v.z+164); if ( fabs(hh-h) > 160 ) return 1;
		 hh = GetLandH(v.x+164, v.z+164); if ( fabs(hh-h) > 160 ) return 1;
  
   for (int z=-2; z<=2; z++)
    for (int x=-2; x<=2; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
		if (MObjects[ob].info.Radius<10) continue;
        float CR = (float)MObjects[ob].info.Radius + 64;
        
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;
        
        float r = (float) sqrt( (ox-v.x)*(ox-v.x) + (oz-v.z)*(oz-v.z) );
        if (r<CR) return 1;
	  }

   return 0;
}




int CheckPlaceCollision(Vector3d &v, BOOL wc, BOOL mc)
{
   int ccx = (int)v.x / 256;
   int ccz = (int)v.z / 256;

   if (ccx<4 || ccz<4 || ccx>1018 || ccz>1018) return 1;

   if (wc)
   if ( (FMap[ccz][ccx-1] | FMap[ccz-1][ccx] | FMap[ccz-1][ccx-1] |
	     FMap[ccz][ccx] | 
		 FMap[ccz+1][ccx] | FMap[ccz][ccx+1] | FMap[ccz+1][ccx+1]) & fmWater)
	   return 1;


   float h = GetLandH(v.x, v.z);
   if (! (FMap[ccz][ccx] & fmWater) )
     if (fabs(h - v.y) > 64) return 1;
   
   v.y = h;

   float hh = GetLandH(v.x-64, v.z-64); if ( fabs(hh-h) > 100 ) return 1;
         hh = GetLandH(v.x+64, v.z-64); if ( fabs(hh-h) > 100 ) return 1;
		 hh = GetLandH(v.x-64, v.z+64); if ( fabs(hh-h) > 100 ) return 1;
		 hh = GetLandH(v.x+64, v.z+64); if ( fabs(hh-h) > 100 ) return 1;

  if (mc)
   for (int z=-2; z<=2; z++)
    for (int x=-2; x<=2; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
		if (MObjects[ob].info.Radius<10) continue;
        float CR = (float)MObjects[ob].info.Radius + 64;
        
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;
        
        float r = (float) sqrt( (ox-v.x)*(ox-v.x) + (oz-v.z)*(oz-v.z) );
        if (r<CR) return 1;
	  }

   return 0;
}






int CheckPlaceCollision2(Vector3d &v, BOOL wc)
{
   int ccx = (int)v.x / 256;
   int ccz = (int)v.z / 256;      

   if (ccx<4 || ccz<4 || ccx>1018 || ccz>1018) return 1;

   if (wc)
   if ( (FMap[ccz][ccx-1] | FMap[ccz-1][ccx] | FMap[ccz-1][ccx-1] |
	     FMap[ccz][ccx] | 
		 FMap[ccz+1][ccx] | FMap[ccz][ccx+1] | FMap[ccz+1][ccx+1]) & fmWater)
	   return 1;

   float h = GetLandH(v.x, v.z);
   /*if (! (FMap[ccz][ccx] & fmWater) )
     if (fabs(h - v.y) > 64) return 1;*/
   v.y = h;

   float hh = GetLandH(v.x-64, v.z-64); if ( fabs(hh-h) > 100 ) return 1;
         hh = GetLandH(v.x+64, v.z-64); if ( fabs(hh-h) > 100 ) return 1;
		 hh = GetLandH(v.x-64, v.z+64); if ( fabs(hh-h) > 100 ) return 1;
		 hh = GetLandH(v.x+64, v.z+64); if ( fabs(hh-h) > 100 ) return 1;

   return 0;
}



int CheckPossiblePath(TCharacter *cptr, BOOL wc, BOOL mc)
{
  Vector3d p = cptr->pos;
  float lookx = (float)f_cos(cptr->tgalpha);
  float lookz = (float)f_sin(cptr->tgalpha);
  int c=0;
  for (int t=0; t<20; t++) {    
    p.x+=lookx * 64.f;
	p.z+=lookz * 64.f;
    if (CheckPlaceCollision(p, wc, mc)) c++;
  }
  return c;
}


void LookForAWay(TCharacter *cptr, BOOL wc, BOOL mc)
{
  float alpha = cptr->tgalpha;
  float dalpha = 15.f;
  float afound = alpha;
  int maxp = 16;
  int curp;

  if (!CheckPossiblePath(cptr, wc, mc)) { cptr->NoWayCnt=0; return; }
  
  cptr->NoWayCnt++;
  for (int i=0; i<12; i++) {
	cptr->tgalpha = alpha+dalpha*pi/180.f;
	curp=CheckPossiblePath(cptr, wc, mc) + (i>>1);
	if (!curp) return;
	if (curp<maxp) {
		maxp = curp;
        afound = cptr->tgalpha;
	}

	cptr->tgalpha = alpha-dalpha*pi/180.f;
	curp=CheckPossiblePath(cptr, wc, mc) + (i>>1);
	if (!curp) return;
	if (curp<maxp) {
		maxp = curp;
        afound = cptr->tgalpha;
	}

	dalpha+=15.f;
  }
 
  cptr->tgalpha = afound;
}




BOOL ReplaceCharacterForward(TCharacter *cptr)
{
  float al = CameraAlpha + (float)siRand(2048) / 2048.f;
  float sa = (float)f_sin(al);
  float ca = (float)f_cos(al);
  Vector3d p;
  p.x = PlayerX + sa * (ctViewR+rRand(10))*256;
  p.z = PlayerZ - ca * (ctViewR+rRand(10))*256;
  p.y = GetLandH(p.x, p.z); 

  if (p.x <  16*256) return FALSE;
  if (p.z <  16*256) return FALSE;
  if (p.x >1000*256) return FALSE;
  if (p.z >1000*256) return FALSE;
  
  if (CheckPlaceCollisionP(p)) return FALSE;

  cptr->State = 0;
  cptr->pos = p;  
  //cptr->tgx = cptr->pos.x + siRand(2048);
  //cptr->tgz = cptr->pos.z + siRand(2048);	 
  SetNewTargetPlace(cptr, 2048);  

  if (cptr->AI==AI_ARCHEO) //===== dimor ========//
	  cptr->pos.y+=1048.f;
  return TRUE;
}


void Characters_AddSecondaryOne(int ctype)
{
	 if (ChCount>64) return;
     Characters[ChCount].CType = ctype;
	 int tr = 0;
replace1:
	 tr++;
	 if (tr>128) return;
	 Characters[ChCount].pos.x = PlayerX + siRand(20040);
     Characters[ChCount].pos.z = PlayerZ + siRand(20040);
	 Characters[ChCount].pos.y = GetLandH(Characters[ChCount].pos.x, 
		                                  Characters[ChCount].pos.z);

	 if (CheckPlaceCollisionP(Characters[ChCount].pos)) goto replace1;

	 if ( fabs(Characters[ChCount].pos.x - PlayerX) + 
	      fabs(Characters[ChCount].pos.z - PlayerZ) < 256 * 40 )
		  goto replace1;		  

     Characters[ChCount].tgx = Characters[ChCount].pos.x;
     Characters[ChCount].tgz = Characters[ChCount].pos.z;
	 Characters[ChCount].tgtime = 0;

	 ResetCharacter(&Characters[ChCount]);     
	 ChCount++;
}



void MoveCharacter(TCharacter *cptr, float dx, float dz, BOOL wc, BOOL mc)
{
	//return;    //000000000000000000000000000000000000000000000
	//000000000000000000000000000000000000000000000
//000000000000000000000000000000000000000000000//000000000000000000000000000000000000000000000//000000000000000000000000000000000000000000000


   Vector3d p = cptr->pos;

   if (CheckPlaceCollision2(p, wc)) {
	 cptr->pos.x+=dx / 2; 
	 cptr->pos.z+=dz / 2; 
	 return;
   }
   
   p.x+=dx;
   p.z+=dz;
   
   if (!CheckPlaceCollision2(p, wc)) {
	   cptr->pos = p; 
	   return;
   }

   p = cptr->pos;
   p.x+=dx/2;   
   p.z+=dz/2;
   if (!CheckPlaceCollision2(p, wc)) cptr->pos = p; 	   
   p = cptr->pos;

   p.x+=dx/4;
   //if (!CheckPlaceCollision2(p)) cptr->pos = p; 	      
   p.z+=dz/4;
   //if (!CheckPlaceCollision2(p)) cptr->pos = p; 	   
   cptr->pos = p;      
}




void MoveCharacter2(TCharacter *cptr, float dx, float dz)
{
	cptr->pos.x+=dx; 
    cptr->pos.z+=dz; 
}




void SetNewTargetPlace(TCharacter *cptr, float R)
{
	 Vector3d p;
	 int tr = 0;
replace:
     p.x = cptr->pos.x + siRand((int)R); if (p.x<512) p.x = 512; if (p.x>1018*256) p.x = 1018*256;
     p.z = cptr->pos.z + siRand((int)R); if (p.z<512) p.z = 512; if (p.z>1018*256) p.z = 1018*256;	    
	 p.y = GetLandH(p.x, p.z);
	 tr++;
	 if (tr<128) 
	  if ( fabs(p.x - cptr->pos.x) + fabs(p.z - cptr->pos.z) < R / 2.f) goto replace;
	 	 
	 R+=512;

	 if (tr<256)
	 if (CheckPlaceCollisionP(p)) goto replace;

	 cptr->tgtime = 0;
	 cptr->tgx = p.x;
	 cptr->tgz = p.z;
}



void SetNewTargetPlace_Brahi(TCharacter *cptr, float R)
{
	 Vector3d p;
	 int tr = 0;
replace:
     p.x = cptr->pos.x + siRand((int)R); if (p.x<512) p.x = 512; if (p.x>1018*256) p.x = 1018*256;
     p.z = cptr->pos.z + siRand((int)R); if (p.z<512) p.z = 512; if (p.z>1018*256) p.z = 1018*256;	 
	 tr++;
	 if (tr<16)
	   if ( fabs(p.x - cptr->pos.x) + fabs(p.z - cptr->pos.z) < R / 2.f) goto replace;	 	 

	 p.y = GetLandH(p.x, p.z);
	 float wy = GetLandUpH(p.x, p.z) - p.y;

	 if (tr<128) {
	  if (wy > 400) goto replace;
	  if (wy < 200) goto replace;
	 }

	 cptr->tgtime = 0;
	 cptr->tgx = p.x;
	 cptr->tgz = p.z;
}



void AnimateHuntDead(TCharacter *cptr)
{    
  
	//if (!cptr->FTime) ActivateCharacterFx(cptr);
	
	ProcessPrevPhase(cptr);
	BOOL NewPhase = FALSE;
	
	cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {		
		NewPhase = TRUE;		
		if (cptr->Phase==2)
			cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;	
		else
			cptr->FTime = 0;		

		if (cptr->Phase==1) {			
			cptr->FTime=0;
			cptr->Phase=2;
		}

		ActivateCharacterFx(cptr);
	}
        

    float h = GetLandH(cptr->pos.x, cptr->pos.z);
	DeltaFunc(cptr->pos.y, h, TimeDt / 5.f);

	if (cptr->Phase==2)
		if (cptr->pos.y > h+3) {
			cptr->FTime = 0;
			//MessageBeep(0xFFFFFFFF);
		}

	
	  if (cptr->pos.y < h + 256) {
        //=== beta ===//
		  float blook = 256;
        float hlook  = GetLandH(cptr->pos.x + cptr->lookx * blook, cptr->pos.z + cptr->lookz * blook);
        float hlook2 = GetLandH(cptr->pos.x - cptr->lookx * blook, cptr->pos.z - cptr->lookz * blook);    
	    DeltaFunc(cptr->beta, (hlook2 - hlook) / (blook * 3.2f), TimeDt / 1800.f);

        if (cptr->beta > 0.4f) cptr->beta = 0.4f;
        if (cptr->beta <-0.4f) cptr->beta =-0.4f;

        //=== gamma ===//
		  float glook = 256;
        hlook = GetLandH(cptr->pos.x + cptr->lookz * glook, cptr->pos.z - cptr->lookx*glook);
        hlook2 = GetLandH(cptr->pos.x - cptr->lookz * glook, cptr->pos.z + cptr->lookx*glook);
        cptr->tggamma =(hlook - hlook2) / (glook * 3.2f);
        if (cptr->tggamma > 0.4f) cptr->tggamma = 0.4f;
        if (cptr->tggamma <-0.4f) cptr->tggamma =-0.4f;      
		DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1800.f);	
	  }
 

	  TCharacter *cptr2 = &Characters[DemoPoint.CIndex];	
}











void AnimateDiatrDead(TCharacter *cptr)
{    

  if (cptr->Phase!=DIA_DIE && cptr->Phase!=DIA_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }    

    cptr->FTime = 0;
    cptr->Phase = DIA_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = DIA_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}












void AnimatePigDead(TCharacter *cptr)
{    

  if (cptr->Phase!=PIG_DIE && cptr->Phase!=PIG_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }
    
    cptr->FTime = 0;
    cptr->Phase = PIG_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = PIG_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;   

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);   
     
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}









void AnimateDimorDead(TCharacter *cptr)
{    

  if (cptr->Phase!=DIM_FALL && cptr->Phase!=DIM_DIE) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }    

    cptr->FTime = 0;
    cptr->Phase = DIM_FALL;
	cptr->rspeed = 0;
	ActivateCharacterFx(cptr);
	return;
  }    
    
  ProcessPrevPhase(cptr);

  cptr->FTime+=TimeDt;
  if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)	 
    if (cptr->Phase == DIM_DIE)
       cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
    else
       cptr->FTime%= cptr->pinfo->Animation[cptr->Phase].AniTime;
  

//======= movement ===========//
   if (cptr->Phase == DIM_DIE)
     DeltaFunc(cptr->vspeed, 0, TimeDt / 400.f);          
   else
	 DeltaFunc(cptr->vspeed, 0, TimeDt / 1200.f);

   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;

   if (cptr->Phase == DIM_FALL) {
	   float wh = GetLandUpH(cptr->pos.x, cptr->pos.z);
	   float lh = GetLandH  (cptr->pos.x, cptr->pos.z);
	   BOOL OnWater = (wh > lh);
	   if (OnWater)
		   if (cptr->pos.y>=wh && (cptr->pos.y+cptr->rspeed * TimeDt / 1024) <wh) {
			   AddWCircle(cptr->pos.x+siRand(128), cptr->pos.z+siRand(128), 2.0);
			   AddWCircle(cptr->pos.x+siRand(128), cptr->pos.z+siRand(128), 2.5);
			   AddWCircle(cptr->pos.x+siRand(128), cptr->pos.z+siRand(128), 3.0);
			   AddWCircle(cptr->pos.x+siRand(128), cptr->pos.z+siRand(128), 3.5);
			   AddWCircle(cptr->pos.x+siRand(128), cptr->pos.z+siRand(128), 3.0);
		   }
	   cptr->pos.y+=cptr->rspeed * TimeDt / 1024;
	   cptr->rspeed-=TimeDt*2.56f;

	   if (cptr->pos.y<lh) {
		   cptr->pos.y = lh;
		   if (OnWater) {
			   AddElements(cptr->pos.x+siRand(128), lh, cptr->pos.z+siRand(128), 4, 10);
			   AddElements(cptr->pos.x+siRand(128), lh, cptr->pos.z+siRand(128), 4, 10);
			   AddElements(cptr->pos.x+siRand(128), lh, cptr->pos.z+siRand(128), 4, 10);
		   }

		   if (cptr->PPMorphTime>128) {
            cptr->PrevPhase = cptr->Phase;
            cptr->PrevPFTime  = cptr->FTime;
            cptr->PPMorphTime = 0; }

		   cptr->Phase = DIM_DIE;
		   cptr->FTime = 0;
		   ActivateCharacterFx(cptr);
	   }
   }    else    {
     ThinkY_Beta_Gamma(cptr, 140, 126, 0.6f, 0.5f);	 
     DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);	 
   }   
}









void AnimateBronDead(TCharacter *cptr)
{    

  if (cptr->Phase!=BRO_DIE && cptr->Phase!=BRO_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }     

    cptr->FTime = 0;
    cptr->Phase = BRO_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = BRO_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;   

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);   
     
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}



void AnimateHogDead(TCharacter *cptr)
{    

  if (cptr->Phase!=BRO_DIE && cptr->Phase!=BRO_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }     

    cptr->FTime = 0;
    cptr->Phase = BRO_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = BRO_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;   

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);   
     
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}



void AnimateRhinoDead(TCharacter *cptr)
{    

  if (cptr->Phase!=BRO_DIE && cptr->Phase!=BRO_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }     

    cptr->FTime = 0;
    cptr->Phase = BRO_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = BRO_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;   

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);   
     
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}



void AnimateSmiloDead(TCharacter *cptr)
{    

  if (cptr->Phase!=BRO_DIE && cptr->Phase!=BRO_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }     

    cptr->FTime = 0;
    cptr->Phase = BRO_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = BRO_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;   

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);   
     
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}










void AnimateDeerDead(TCharacter *cptr)
{    

  if (cptr->Phase!=DER_DIE && cptr->Phase!=DER_SLP) {    
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = cptr->Phase;
     cptr->PrevPFTime  = cptr->FTime;
     cptr->PPMorphTime = 0; }
    
    cptr->FTime = 0;
    cptr->Phase = DER_DIE;   
	ActivateCharacterFx(cptr);
  } else {    
    ProcessPrevPhase(cptr);

    cptr->FTime+=TimeDt;      
    if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime)
		if (Tranq) {
			cptr->FTime=0;
			cptr->Phase = DER_SLP;
			ActivateCharacterFx(cptr);
		} else
        cptr->FTime = cptr->pinfo->Animation[cptr->Phase].AniTime-1;
  }

//======= movement ===========//
   DeltaFunc(cptr->vspeed, 0, TimeDt / 800.f);          
   cptr->pos.x+=cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+=cptr->lookz * cptr->vspeed * TimeDt;   

   ThinkY_Beta_Gamma(cptr, 100, 96, 0.6f, 0.5f);   
     
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1600.f);
}















































void AnimateDiatr(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;


TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );    

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 108;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 108;
   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   if (cptr->State==2) { if (cptr->Phase!=DIA_JUMP) NewPhase=TRUE; cptr->State=1; cptr->Phase=DIA_RUN;}

   
   if (GetLandUpH(cptr->pos.x, cptr->pos.z) - GetLandH(cptr->pos.x, cptr->pos.z) > 140 * cptr->scale)
	   cptr->StateF |= csONWATER; else
	   cptr->StateF &= (!csONWATER);

   if (cptr->Phase == DIA_EAT) goto NOTHINK;
   
//============================================//   
   if (!MyHealth) cptr->State = 0;
   if (cptr->State) {        	 		
	   if (pdist > ctViewR*140+OptAgres/8) {
		   	nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	        NormVector(nv, 2048.f);
            cptr->tgx = cptr->pos.x - nv.x;
            cptr->tgz = cptr->pos.z - nv.z;
			cptr->tgtime = 0;
			cptr->AfraidTime-=TimeDt;
			if (cptr->AfraidTime<=0) {
			 cptr->AfraidTime=0; cptr->State = 0; 			 
			}			
	   } else {
            cptr->tgx = PlayerX;
            cptr->tgz = PlayerZ;			
			cptr->tgtime = 0;
	   }
     
	 if (!(cptr->StateF & csONWATER))
	  if (pdist<1324 * cptr->scale && pdist>900 * cptr->scale) 
	   if (AngleDifference(cptr->alpha, FindVectorAlpha(playerdx, playerdz)) < 0.2f)
	    cptr->Phase = DIA_JUMP;

	 if (pdist<300) 
	  if (fabs(PlayerY - cptr->pos.y - 120) < 256) { 	    
		  if (!(cptr->StateF & csONWATER)) {
	        cptr->vspeed/= 8.0f;
	        cptr->State = 1;
	        cptr->Phase = DIA_EAT;	   
		 }
	   
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }
   }

   if (!cptr->State) {    
	cptr->AfraidTime = 0;
	if (tdist<456) {
       SetNewTargetPlace(cptr, 8048.f);	   	
	   goto TBEGIN; }
   }

NOTHINK:
   if (pdist<2048) cptr->NoFindCnt = 0;
   if (cptr->NoFindCnt) cptr->NoFindCnt--; else 
   {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   	  
	if (cptr->State && pdist>1648) { 
		cptr->tgalpha += (float)f_sin(RealTime/824.f) / 4.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
	}

   LookForAWay(cptr, FALSE, TRUE);
   if (cptr->NoWayCnt>12) { cptr->NoWayCnt=0; cptr->NoFindCnt = 16 + rRand(20); }


   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);
   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;   
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
   
   if (cptr->Phase==DIA_EAT)  goto ENDPSELECT; 
   if (NewPhase && _Phase==DIA_JUMP) { cptr->Phase = DIA_RUN; goto ENDPSELECT; }   

   if (cptr->Phase== DIA_JUMP) goto ENDPSELECT;    
   
   if (NewPhase) 
	 if (!cptr->State)
      if (rRand(128)>110) {
			cptr->Phase = DIA_IDLE1 + rRand(1);		    
            goto ENDPSELECT;
	  } else cptr->Phase = DIA_WALK; else cptr->Phase = DIA_RUN;
            
   if (cptr->Phase!=DIA_IDLE1 && cptr->Phase!=DIA_IDLE2)
   if (!cptr->State) cptr->Phase=DIA_WALK; else
        if (fabs(cptr->tgalpha - cptr->alpha)<1.0 ||
            fabs(cptr->tgalpha - cptr->alpha)>2*pi-1.0) 
                cptr->Phase = DIA_RUN; else cptr->Phase=DIA_WALK;

   if (cptr->StateF & csONWATER) cptr->Phase = DIA_SWIM;
   if (cptr->Slide>40) cptr->Phase = DIA_SLIDE;
   
   
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {     
    //==== set proportional FTime for better morphing =//       
   if (MORPHP)
    if (_Phase<=3 && cptr->Phase<=3)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;

   if (cptr->Phase==DIA_JUMP  || cptr->Phase==DIA_EAT ||
	   cptr->Phase==DIA_IDLE1 || cptr->Phase==DIA_IDLE2) goto SKIPROT;
   
   if (drspd > 0.02)	
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.6f + drspd*1.2f;
                                else currspeed =-0.6f - drspd*1.2f;
     else currspeed = 0;
   if (cptr->AfraidTime) currspeed*=2.5;
   
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->StateF & csONWATER) || cptr->Phase==DIA_WALK) currspeed/=1.4f;
         
   if (cptr->AfraidTime) DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 160.f);
                    else DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 180.f);

   tgbend = drspd/3;
   if (tgbend>pi/5) tgbend = pi/5;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 800.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 600.f);

   
   rspd=cptr->rspeed * TimeDt / 1024.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:

   //======= set slide mode ===========//
   if (!cptr->Slide && cptr->vspeed>0.6 && cptr->Phase!=DIA_JUMP)
     if (AngleDifference(cptr->tgalpha, cptr->alpha)>pi*2/3.f) {         
         cptr->Slide = (int)(cptr->vspeed*700.f);
         cptr->slidex = cptr->lookx;
         cptr->slidez = cptr->lookz;         
         cptr->vspeed = 0;
     }



//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == DIA_RUN ) curspeed = 0.640*2;
   if (cptr->Phase == DIA_JUMP) curspeed = 0.600*2;
   if (cptr->Phase == DIA_WALK) curspeed = 0.224*2;
   if (cptr->Phase == DIA_SWIM) curspeed = 0.300*2;
   if (cptr->Phase == DIA_EAT)  curspeed = 0.0f;

   if (cptr->Phase == DIA_RUN && cptr->Slide) {
    curspeed /= 8;
    if (drspd > pi / 2.f) curspeed=0; else 
    if (drspd > pi / 4.f) curspeed*=2.f - 4.f*drspd / pi; 
   } else 
    if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   
   DeltaFunc(cptr->vspeed, curspeed, TimeDt / 500.f);    
   
   if (cptr->Phase==DIA_JUMP) cptr->vspeed = 1.1f;

   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt * cptr->scale,
	                   cptr->lookz * cptr->vspeed * TimeDt * cptr->scale, FALSE, TRUE);

   
//========== slide ==============//
   if (cptr->Slide) {
     MoveCharacter(cptr, cptr->slidex * cptr->Slide / 600.f * TimeDt * cptr->scale,
		                 cptr->slidez * cptr->Slide / 600.f * TimeDt * cptr->scale, FALSE, TRUE);     

     cptr->Slide-=TimeDt;
     if (cptr->Slide<0) cptr->Slide=0;
   }
    

//============ Y movement =================//   
   if (cptr->StateF & csONWATER) {
    cptr->pos.y = GetLandUpH(cptr->pos.x, cptr->pos.z) - 160 * cptr->scale;
    cptr->beta/=2;
    cptr->tggamma=0;    
   } else {    	
	ThinkY_Beta_Gamma(cptr, 98, 84, 0.4f, 0.3f);           	
   }

   //=== process to tggamma ===//   
   if (cptr->Phase==DIA_WALK) cptr->tggamma+= cptr->rspeed / 9.0f;
                         else cptr->tggamma+= cptr->rspeed / 6.0f;
   if (cptr->Phase==DIA_JUMP) cptr->tggamma=0;

   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 1624.f);


//==================================================//   
  
}





























void AnimatePig(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   float playerdx = PlayerX - cptr->pos.x;
   float playerdz = PlayerZ - cptr->pos.z;
   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    	
	if (!cptr->AfraidTime) {
	 if (pdist<2048.f) cptr->AfraidTime = (5 + rRand(5)) * 1024; 
	
	if (!cptr->AfraidTime)
  	 if (pdist > 4096.f) { 
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 2048.f);
		goto TBEGIN; }
	}


	nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	NormVector(nv, 2048.f);
    cptr->tgx = cptr->pos.x - nv.x;
    cptr->tgz = cptr->pos.z - nv.z;
	cptr->tgtime = 0;
   }

   if (pdist>(ctViewR+20)*256) 
	   if (ReplaceCharacterForward(cptr)) goto TBEGIN;


   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;
	 if (pdist<812.f) { 
		 cptr->State = 1; 
		 cptr->AfraidTime = (5 + rRand(5)) * 1024;
		 cptr->Phase = PIG_RUN;
		 goto TBEGIN;  }
	 

	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 2048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//        

   if (cptr->NoFindCnt) cptr->NoFindCnt--;
                   else cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 8 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }   
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == PIG_IDLE1 || cptr->Phase == PIG_IDLE2 || cptr->Phase == PIG_IDLE3) {
			 if (rRand(128)>96) cptr->Phase = PIG_WALK;
			               else cptr->Phase = PIG_IDLE1 + rRand(2);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 120) cptr->Phase=PIG_IDLE1; else cptr->Phase=PIG_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = PIG_RUN; 
	                    else cptr->Phase = PIG_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == PIG_IDLE1 || cptr->Phase == PIG_IDLE2 || cptr->Phase == PIG_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.8f + drspd*1.4f;
                                else currspeed =-0.8f - drspd*1.4f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==PIG_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 260.f);

   tgbend = drspd/2;
   if (tgbend>pi/2) tgbend = pi/2;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 800.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 400.f);

   
   rspd=cptr->rspeed * TimeDt / 1024.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == PIG_RUN ) curspeed = 0.6f;   
   if (cptr->Phase == PIG_WALK) curspeed = 0.2f;
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 64, 32, 0.7f, 0.4f);         
   if (cptr->Phase==PIG_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}















void AnimateBron(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   //float playerdx = PlayerX - cptr->pos.x;
   //float playerdz = PlayerZ - cptr->pos.z;

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 300 * cptr->scale;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 300 * cptr->scale;

   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    
    if (pdist < 6000) cptr->AfraidTime = 8000;

	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	if (pdist > 256*16+OptAgres/8) {
	  nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	  NormVector(nv, 2048.f);
      cptr->tgx = cptr->pos.x - nv.x;
      cptr->tgz = cptr->pos.z - nv.z;
	  cptr->tgtime = 0;
	} else {
		cptr->tgx = PlayerX;
        cptr->tgz = PlayerZ;					
		cptr->tgtime = 0;
	}
   }

   if (MyHealth)
   if (pdist<300) 
	 if (fabs(PlayerY - cptr->pos.y - 160) < 256) { 	    	   		 	        
	   cptr->State = 0;	        		 
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }

   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;	 
	 
	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;   
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);       
    if (cptr->AfraidTime) 
      if (pdist>12*256) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 48 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) {
			 if (rRand(128) > 64 && cptr->Phase == BRO_IDLE3) 
			      cptr->Phase = BRO_WALK;
			 else cptr->Phase = BRO_IDLE1 + rRand(2);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 124) cptr->Phase=BRO_IDLE1; else cptr->Phase=BRO_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = BRO_RUN; 
	                    else cptr->Phase = BRO_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.2f + drspd*1.0f;
                                else currspeed =-0.2f - drspd*1.0f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==BRO_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/3.5f;
   if (tgbend>pi/2.f) tgbend = pi/2.f;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1600.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1200.f);

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == BRO_RUN ) curspeed = 0.768*2;   
   if (cptr->Phase == BRO_WALK) curspeed = 0.168*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.3f);         
   if (cptr->Phase==BRO_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}






void AnimateBear(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   //float playerdx = PlayerX - cptr->pos.x;
   //float playerdz = PlayerZ - cptr->pos.z;

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 300 * cptr->scale;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 300 * cptr->scale;

   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {
    if (pdist < 6000) cptr->AfraidTime = 8000;

	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	if (pdist > 256*20+OptAgres/8) {
	  nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	  NormVector(nv, 2048.f);
      cptr->tgx = cptr->pos.x - nv.x;
      cptr->tgz = cptr->pos.z - nv.z;
	  cptr->tgtime = 0;
	} else {
		cptr->tgx = PlayerX;
        cptr->tgz = PlayerZ;					
		cptr->tgtime = 0;
	}
   }

   if (MyHealth)
   if (pdist<300) 
	 if (fabs(PlayerY - cptr->pos.y - 160) < 256) { 	    	   		 	        
	   cptr->State = 0;	        		 
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }

   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;	 
	 
	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 48 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) {
			 if (rRand(128) > 64 && cptr->Phase == BRO_IDLE3) 
			      cptr->Phase = BRO_WALK;
			 else cptr->Phase = BRO_IDLE1 + rRand(2);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 124) cptr->Phase=BRO_IDLE1; else cptr->Phase=BRO_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = BRO_RUN; 
	                    else cptr->Phase = BRO_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.2f + drspd*1.0f;
                                else currspeed =-0.2f - drspd*1.0f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==BRO_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/3.5f;
   if (tgbend>pi/2.f) tgbend = pi/2.f;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1600.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1200.f);

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == BRO_RUN ) curspeed = 0.896*2;   
   if (cptr->Phase == BRO_WALK) curspeed = 0.240*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.3f);         
   if (cptr->Phase==BRO_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}




void AnimateWolf(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   //float playerdx = PlayerX - cptr->pos.x;
   //float playerdz = PlayerZ - cptr->pos.z;

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 300 * cptr->scale;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 300 * cptr->scale;

   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (!MyHealth) cptr->State = 0;
   if (cptr->State) {        	 		
	   if (pdist > ctViewR*140+OptAgres/8) {
		   	nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	        NormVector(nv, 2048.f);
            cptr->tgx = cptr->pos.x - nv.x;
            cptr->tgz = cptr->pos.z - nv.z;
			cptr->tgtime = 0;
			cptr->AfraidTime-=TimeDt;
			if (cptr->AfraidTime<=0) {
			 cptr->AfraidTime=0; cptr->State = 0; 			 
			}			
	   } else {
            cptr->tgx = PlayerX;
            cptr->tgz = PlayerZ;			
			cptr->tgtime = 0;
	   }
   }


   if (MyHealth)
   if (pdist<300) 
	 if (fabs(PlayerY - cptr->pos.y - 160) < 256) { 	    	   		 	        
	   cptr->State = 0;	        		 
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }

   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;
	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   }
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) 
      if (pdist>12*256) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 48 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) {
			 if (rRand(128) > 64) 
			      cptr->Phase = BRO_WALK;
			 else 
                  cptr->Phase = BRO_IDLE1 + rRand(2);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 124) cptr->Phase=BRO_IDLE1 + rRand(2); else cptr->Phase=BRO_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = BRO_RUN; 
	                    else cptr->Phase = BRO_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.4f + drspd*1.5f;
                                else currspeed =-0.4f - drspd*1.5f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==BRO_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/3.5f;
   if (tgbend>pi/2.f) tgbend = pi/2.f;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1600.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1200.f);

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == BRO_RUN ) curspeed = 1.024*2;   
   if (cptr->Phase == BRO_WALK) curspeed = 0.160*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.3f);         
   if (cptr->Phase==BRO_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}




void AnimateHog(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   //float playerdx = PlayerX - cptr->pos.x;
   //float playerdz = PlayerZ - cptr->pos.z;

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 300 * cptr->scale;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 300 * cptr->scale;

   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    
    if (pdist < 6000) cptr->AfraidTime = 8000;

	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	if (pdist > 256*20+OptAgres/8) {
	  nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	  NormVector(nv, 2048.f);
      cptr->tgx = cptr->pos.x - nv.x;
      cptr->tgz = cptr->pos.z - nv.z;
	  cptr->tgtime = 0;
	} else {
		cptr->tgx = PlayerX;
        cptr->tgz = PlayerZ;					
		cptr->tgtime = 0;
	}
   }

   if (MyHealth)
   if (pdist<300) 
	 if (fabs(PlayerY - cptr->pos.y - 160) < 256) { 	    	   		 	        
	   cptr->State = 0;	        		 
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }

   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;	 
	 
	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) 
      if (pdist>12*256) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 48 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) {
			 if (rRand(128) > 64 && cptr->Phase == BRO_IDLE3) 
			      cptr->Phase = BRO_WALK;
			 else cptr->Phase = BRO_IDLE1 + rRand(2);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 124) cptr->Phase=BRO_IDLE1; else cptr->Phase=BRO_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = BRO_RUN; 
	                    else cptr->Phase = BRO_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.3f + drspd*1.4f;
                                else currspeed =-0.3f - drspd*1.4f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==BRO_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/3.5f;
   if (tgbend>pi/2.f) tgbend = pi/2.f;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1600.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1200.f);

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == BRO_RUN ) curspeed = 0.576*2;   
   if (cptr->Phase == BRO_WALK) curspeed = 0.112*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.3f);         
   if (cptr->Phase==BRO_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}




void AnimateRhino(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   //float playerdx = PlayerX - cptr->pos.x;
   //float playerdz = PlayerZ - cptr->pos.z;

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 300 * cptr->scale;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 300 * cptr->scale;

   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    
    if (pdist < 6000) cptr->AfraidTime = 8000;

	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	if (pdist > 256*18+OptAgres/8) {
	  nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	  NormVector(nv, 2048.f);
      cptr->tgx = cptr->pos.x - nv.x;
      cptr->tgz = cptr->pos.z - nv.z;
	  cptr->tgtime = 0;
	} else {
		cptr->tgx = PlayerX;
        cptr->tgz = PlayerZ;					
		cptr->tgtime = 0;
	}
   }

   if (MyHealth)
   if (pdist<300) 
	 if (fabs(PlayerY - cptr->pos.y - 160) < 256) { 	    	   		 	        
	   cptr->State = 0;	        		 
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }

   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;	 
	 
	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) 
       if (pdist>12*256) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 48 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) {
			 if (rRand(128) > 64 && cptr->Phase == BRO_IDLE3) 
			      cptr->Phase = BRO_WALK;
			 else cptr->Phase = BRO_IDLE1 + rRand(2);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 124) cptr->Phase=BRO_IDLE1; else cptr->Phase=BRO_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = BRO_RUN; 
	                    else cptr->Phase = BRO_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.3f + drspd*1.2f;
                                else currspeed =-0.3f - drspd*1.2f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==BRO_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/3.5f;
   if (tgbend>pi/2.f) tgbend = pi/2.f;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1600.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1200.f);

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == BRO_RUN ) curspeed = 0.576*2;   
   if (cptr->Phase == BRO_WALK) curspeed = 0.168*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.3f);         
   if (cptr->Phase==BRO_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}





void AnimateSmilo(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   //float playerdx = PlayerX - cptr->pos.x;
   //float playerdz = PlayerZ - cptr->pos.z;

   float playerdx = PlayerX - cptr->pos.x - cptr->lookx * 300 * cptr->scale;
   float playerdz = PlayerZ - cptr->pos.z - cptr->lookz * 300 * cptr->scale;

   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    
    if (pdist < 6000) cptr->AfraidTime = 8000;

	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	if (pdist > 256*16+OptAgres/8) {
	  nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	  NormVector(nv, 2048.f);
      cptr->tgx = cptr->pos.x - nv.x;
      cptr->tgz = cptr->pos.z - nv.z;
	  cptr->tgtime = 0;
	} else {
		cptr->tgx = PlayerX;
        cptr->tgz = PlayerZ;					
		cptr->tgtime = 0;
	}
   }

   if (MyHealth)
   if (pdist<300) 
	 if (fabs(PlayerY - cptr->pos.y - 160) < 256) { 	    	   		 	        
	   cptr->State = 0;	        		 
	   AddDeadBody(cptr, HUNT_EAT);	   
	 }

   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;	 
	 
	 if (tdist<456) {      	   
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;   
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) 
       if (pdist>12*256) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>8) { cptr->NoWayCnt=0; cptr->NoFindCnt = 48 + rRand(80); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) {
			 cptr->Phase = BRO_WALK;
			 goto ENDPSELECT;
		   }
		   if (rRand(128) > 124) cptr->Phase=BRO_IDLE1 + rRand(2); else cptr->Phase=BRO_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = BRO_RUN; 
	                    else cptr->Phase = BRO_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=1 && cptr->Phase<=1)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == BRO_IDLE1 || cptr->Phase == BRO_IDLE2 || cptr->Phase == BRO_IDLE3) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.3f + drspd*1.5f;
                                else currspeed =-0.3f - drspd*1.5f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==BRO_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/3.5f;
   if (tgbend>pi/2.f) tgbend = pi/2.f;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1600.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 1200.f);

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == BRO_RUN ) curspeed = 1.024*2;   
   if (cptr->Phase == BRO_WALK) curspeed = 0.320*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.3f);         
   if (cptr->Phase==BRO_WALK) cptr->tggamma+= cptr->rspeed / 12.0f;
                         else cptr->tggamma+= cptr->rspeed / 8.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}


























void AnimateDeer(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   float playerdx = PlayerX - cptr->pos.x;
   float playerdz = PlayerZ - cptr->pos.z;
   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    
    
	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	NormVector(nv, 2048.f);
    cptr->tgx = cptr->pos.x - nv.x;
    cptr->tgz = cptr->pos.z - nv.z;
	cptr->tgtime = 0;
   }


   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;
	 if (pdist<1024.f) { 
		 cptr->State = 1; 
		 cptr->AfraidTime = (6 + rRand(8)) * 1024;
		 cptr->Phase = DER_RUN;
		 goto TBEGIN;  }
	 

	 if (tdist<456) {      
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<2048) cptr->NoFindCnt = 0;   
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>12) { cptr->NoWayCnt=0; cptr->NoFindCnt = 32 + rRand(60); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == DER_IDLE1 || cptr->Phase == DER_IDLE2) {
			 if (rRand(128) > 64 && cptr->Phase == DER_IDLE2) 
			      cptr->Phase = DER_WALK;
			 else cptr->Phase = DER_IDLE1 + rRand(1);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 120) cptr->Phase=DER_IDLE1; else cptr->Phase=DER_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = DER_RUN; 
	                    else cptr->Phase = DER_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=2 && cptr->Phase<=2)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == DER_IDLE1 || cptr->Phase == DER_IDLE2) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.2f + drspd*1.0f;
                                else currspeed =-0.2f - drspd*1.0f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==DER_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/2.f;
   if (tgbend>pi/3.f) tgbend = pi/3.f;

   tgbend*= SGN(currspeed);
   DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 2000.f);
   

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == DER_RUN ) curspeed = 0.768*2;   
   if (cptr->Phase == DER_WALK) curspeed = 0.160*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.4f);         
   if (cptr->Phase==DER_WALK) cptr->tggamma+= cptr->rspeed / 16.0f;
                         else cptr->tggamma+= cptr->rspeed / 10.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}








void AnimateMamm(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
   if (cptr->AfraidTime) cptr->AfraidTime = max(0, cptr->AfraidTime - TimeDt);
   if (cptr->State==2) { NewPhase=TRUE; cptr->State=1; }

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   float playerdx = PlayerX - cptr->pos.x;
   float playerdz = PlayerZ - cptr->pos.z;
   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//
   
   if (cptr->State) {    
    
	if (!cptr->AfraidTime) {
		cptr->State = 0; 
		SetNewTargetPlace(cptr, 8048.f);
		goto TBEGIN; 
	}

	nv.x = playerdx; nv.z = playerdz; nv.y = 0;
	NormVector(nv, 2048.f);
    cptr->tgx = cptr->pos.x - nv.x;
    cptr->tgz = cptr->pos.z - nv.z;
	cptr->tgtime = 0;
   }


   //======== exploring area ===============//
   if (!cptr->State) {
	 cptr->AfraidTime = 0;
	 if (pdist<1024.f) { 
		 cptr->State = 1; 
		 cptr->AfraidTime = (6 + rRand(8)) * 1024;
		 cptr->Phase = MAM_RUN;
		 goto TBEGIN;  }
	 

	 if (tdist<456) {      
       SetNewTargetPlace(cptr, 8048.f);
	   goto TBEGIN; } 	 	
   } 
   
   
//============================================//   
   if (pdist<3048) cptr->NoFindCnt = 0;
   if (cptr->NoFindCnt) cptr->NoFindCnt--;
   else {
    cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);   
    if (cptr->AfraidTime) { 
		cptr->tgalpha += (float)f_sin(RealTime/1024.f) / 3.f;
		if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
        if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
	}
   }
   

   LookForAWay(cptr, TRUE, TRUE);
   if (cptr->NoWayCnt>12) { cptr->NoWayCnt=0; cptr->NoFindCnt = 32 + rRand(60); }

   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }
              
   if (NewPhase) 
	   if (!cptr->State) {
		   if (cptr->Phase == MAM_IDLE1 || cptr->Phase == MAM_IDLE2) {
			 if (rRand(128) > 64 && cptr->Phase == MAM_IDLE2) 
			      cptr->Phase = MAM_WALK;
			 else cptr->Phase = MAM_IDLE1 + rRand(1);
			 goto ENDPSELECT;
		   }		   
		   if (rRand(128) > 120) cptr->Phase=MAM_IDLE1; else cptr->Phase=MAM_WALK; 
	   } else
	   if (cptr->AfraidTime) cptr->Phase = MAM_RUN; 
	                    else cptr->Phase = MAM_WALK; 	
      
ENDPSELECT:

//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {         
    if (_Phase<=2 && cptr->Phase<=2)
        cptr->FTime = _FTime * cptr->pinfo->Animation[cptr->Phase].AniTime / cptr->pinfo->Animation[_Phase].AniTime + 64;
    else if (!NewPhase) cptr->FTime = 0;

    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   

      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;


   if (cptr->Phase == MAM_IDLE1 || cptr->Phase == MAM_IDLE2) goto SKIPROT;
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.2f + drspd*1.0f;
                                else currspeed =-0.2f - drspd*1.0f;
     else currspeed = 0;
   
   if (cptr->AfraidTime) currspeed*=1.5;
   if (dalpha > pi) currspeed*=-1;
   if ((cptr->State & csONWATER) || cptr->Phase==MAM_WALK) currspeed/=1.4f;
      
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 400.f);

   tgbend = drspd/2.f;
   if (tgbend>pi/3.f) tgbend = pi/3.f;

   tgbend*= SGN(currspeed);
   DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 2000.f);
   

   
   rspd=cptr->rspeed * TimeDt / 612.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

SKIPROT:   

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == MAM_RUN ) curspeed = 0.640*2;   
   if (cptr->Phase == MAM_WALK) curspeed = 0.320*2;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

//========== process speed =============//
   curspeed*=cptr->scale;
   if (curspeed>cptr->vspeed) DeltaFunc(cptr->vspeed, curspeed, TimeDt / 1024.f);    
                         else DeltaFunc(cptr->vspeed, curspeed, TimeDt / 256.f);    
      
   MoveCharacter(cptr, cptr->lookx * cptr->vspeed * TimeDt,
	                   cptr->lookz * cptr->vspeed * TimeDt, TRUE, TRUE);      

   ThinkY_Beta_Gamma(cptr, 128, 64, 0.6f, 0.4f);         
   if (cptr->Phase==MAM_WALK) cptr->tggamma+= cptr->rspeed / 16.0f;
                         else cptr->tggamma+= cptr->rspeed / 10.0f;   
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}



















void AnimateDimor(TCharacter *cptr)
{
   NewPhase = FALSE;
   int _Phase = cptr->Phase;
   int _FTime = cptr->FTime;
   float _tgalpha = cptr->tgalpha;
      

TBEGIN:
   float targetx = cptr->tgx;
   float targetz = cptr->tgz;
   float targetdx = targetx - cptr->pos.x;
   float targetdz = targetz - cptr->pos.z;

   float tdist = (float)sqrt( targetdx * targetdx + targetdz * targetdz );       

   float playerdx = PlayerX - cptr->pos.x;
   float playerdz = PlayerZ - cptr->pos.z;
   float pdist = (float)sqrt( playerdx * playerdx + playerdz * playerdz );    
   

   //=========== run away =================//   

   if (pdist>(ctViewR+20)*256)
	   if (ReplaceCharacterForward(cptr)) goto TBEGIN;


   //======== exploring area ===============//   
	 if (tdist<1024) {      
       SetNewTargetPlace(cptr, 4048.f);
	   goto TBEGIN; } 	 	
      
   
//============================================//        

   
   cptr->tgalpha = CorrectedAlpha(FindVectorAlpha(targetdx, targetdz), cptr->alpha);//FindVectorAlpha(targetdx, targetdz);      
   if (cptr->tgalpha < 0) cptr->tgalpha+=2*pi;
   if (cptr->tgalpha > 2*pi) cptr->tgalpha-=2*pi;
  
//===============================================//

   ProcessPrevPhase(cptr);   

//======== select new phase =======================//
   cptr->FTime+=TimeDt;
   
   if (cptr->FTime >= cptr->pinfo->Animation[cptr->Phase].AniTime) {       
       cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;
       NewPhase = TRUE; }   
              
   if (NewPhase) {
	   if (cptr->Phase == DIM_FLY) 
		   if (cptr->pos.y > GetLandH(cptr->pos.x, cptr->pos.z) + 2800) 
			   cptr->Phase = DIM_FLYP;
		   else ;
			   else
	   if (cptr->Phase == DIM_FLYP) 
		   if (cptr->pos.y < GetLandH(cptr->pos.x, cptr->pos.z) + 1800) 
			   cptr->Phase = DIM_FLY;
   }
      



//====== process phase changing ===========//
   if ( (_Phase != cptr->Phase) || NewPhase)
	 if ( (rand() & 1023) > 980 )
	   ActivateCharacterFx(cptr);	   
   
   if (_Phase != cptr->Phase) {
    if (!NewPhase) cptr->FTime = 0;   
    if (cptr->PPMorphTime>128) {
     cptr->PrevPhase = _Phase;
     cptr->PrevPFTime  = _FTime;
     cptr->PPMorphTime = 0; }
   }
   
  
   cptr->FTime %= cptr->pinfo->Animation[cptr->Phase].AniTime;   
      
   
   //========== rotation to tgalpha ===================//
   
   float rspd, currspeed, tgbend;
   float dalpha = (float)fabs(cptr->tgalpha - cptr->alpha);   
   float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd;

   
   if (drspd > 0.02)
    if (cptr->tgalpha > cptr->alpha) currspeed = 0.6f + drspd*1.2f;
                                else currspeed =-0.6f - drspd*1.2f;
     else currspeed = 0;
      
   if (dalpha > pi) currspeed*=-1;         
   DeltaFunc(cptr->rspeed, currspeed, (float)TimeDt / 460.f);

   tgbend = drspd/2.f;
   if (tgbend>pi/2) tgbend = pi/2;

   tgbend*= SGN(currspeed);
   if (fabs(tgbend) > fabs(cptr->bend)) DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 800.f);
                                   else DeltaFunc(cptr->bend, tgbend, (float)TimeDt / 400.f);

   
   rspd=cptr->rspeed * TimeDt / 1024.f;
   if (drspd < fabs(rspd)) cptr->alpha = cptr->tgalpha;
                      else cptr->alpha+=rspd;
   

   if (cptr->alpha > pi * 2) cptr->alpha-= pi * 2;
   if (cptr->alpha < 0     ) cptr->alpha+= pi * 2;

//========== movement ==============================//
   cptr->lookx = (float)f_cos(cptr->alpha);
   cptr->lookz = (float)f_sin(cptr->alpha);

   float curspeed = 0;
   if (cptr->Phase == DIM_FLY ) curspeed = 1.5f;   
   if (cptr->Phase == DIM_FLYP) curspeed = 1.3f;   
      
   if (drspd > pi / 2.f) curspeed*=2.f - 2.f*drspd / pi;

   if (cptr->Phase == DIM_FLY)
    DeltaFunc(cptr->pos.y, GetLandH(cptr->pos.x, cptr->pos.z)+4048, TimeDt / 6.f);
   else
	DeltaFunc(cptr->pos.y, GetLandH(cptr->pos.x, cptr->pos.z), TimeDt / 16.f);
   
   
   if (cptr->pos.y < GetLandH(cptr->pos.x, cptr->pos.z) + 236)
	   cptr->pos.y = GetLandH(cptr->pos.x, cptr->pos.z) + 256;
   
   

//========== process speed =============//
   curspeed*=cptr->scale;
   DeltaFunc(cptr->vspeed, curspeed, TimeDt / 2024.f);    
      
   cptr->pos.x+= cptr->lookx * cptr->vspeed * TimeDt;
   cptr->pos.z+= cptr->lookz * cptr->vspeed * TimeDt;	                   
   
   cptr->tggamma = cptr->rspeed / 4.0f;
   if (cptr->tggamma > pi / 6.f) cptr->tggamma = pi / 6.f;
   if (cptr->tggamma <-pi / 6.f) cptr->tggamma =-pi / 6.f;
   DeltaFunc(cptr->gamma, cptr->tggamma, TimeDt / 2048.f);
}





void AnimateCharacters()
{
  //if (!RunMode) return;

  TCharacter *cptr;
  if (TrophyMode) return;

  for (CurDino=0; CurDino<ChCount; CurDino++) {
   cptr = &Characters[CurDino];   
   if (cptr->StateF == 0xFF) continue;
   cptr->tgtime+=TimeDt;

   if (cptr->tgtime > 30*1000) SetNewTargetPlace(cptr, 2048);
   if (cptr->Health)
	   if (cptr->BloodTTime) {
		   cptr->BloodTTime-=TimeDt;
		   if (cptr->BloodTTime<0) cptr->BloodTTime=0;

		   float k = (20000.f + cptr->BloodTTime) / 90000.f;
		   if (k>1.5) k = 1.5;
		   cptr->BloodTime+=(int)((float)TimeDt * k);
		   if (cptr->BloodTime>600) {
			   cptr->BloodTime=rRand(228);
			   AddBloodTrail(cptr);
			   if (rRand(128) > 96) AddBloodTrail(cptr);
		   }
	   }


   switch (cptr->AI) {    
	case AI_PIG    : if (cptr->Health) AnimatePig(cptr); else AnimatePigDead(cptr);
		    break;	
    case AI_ARCHEO : if (cptr->Health) AnimateDimor(cptr); else AnimateDimorDead(cptr);
		    break;
    case AI_BIGFOOT: 
		    break;	
    case AI_POACHER: ;
	        break;


    case AI_BRONT: if (cptr->Health) AnimateBron(cptr);    else AnimateBronDead(cptr);
		    break; 
    case AI_HOG  : if (cptr->Health) AnimateHog(cptr);    else AnimateHogDead(cptr);
		    break; 
    case AI_WOLF : if (cptr->Health) AnimateWolf(cptr);    else AnimateBronDead(cptr);
            break;
    case AI_RHINO: if (cptr->Health) AnimateRhino(cptr);   else AnimateRhinoDead(cptr);
		    break; 
    case AI_DIATR: if (cptr->Health) AnimateDiatr(cptr);   else AnimateDiatrDead(cptr);
		    break;
    case AI_DEER : if (cptr->Health) AnimateDeer(cptr);   else AnimateDeerDead(cptr);
		    break; 
    case AI_SMILO: if (cptr->Health) AnimateSmilo(cptr);   else AnimateSmiloDead(cptr);
		    break; 
    case AI_MAMM : if (cptr->Health) AnimateMamm(cptr);   else AnimateDeerDead(cptr);
		    break; 
    case AI_BEAR : if (cptr->Health) AnimateBear(cptr);   else AnimateBronDead(cptr);
		    break; 

            /*
    case AI_ANKY : if (cptr->Health) AnimateAnky(cptr);   else AnimateAnkyDead(cptr);
		    break; 
    case AI_STEGO: if (cptr->Health) AnimateSteg(cptr);   else AnimateStegDead(cptr);
		    break;

    case AI_ALLO : if (cptr->Health) AnimateRaptor(cptr); else AnimateRaptorDead(cptr);
		    break; 
	case AI_CHASM: if (cptr->Health) AnimateTric(cptr);   else AnimateTricDead(cptr);
		    break;

	case AI_VELO : if (cptr->Health) AnimateVelo(cptr);   else AnimateVeloDead(cptr);
		    break;
    case AI_SPINO: if (cptr->Health) AnimateSpin(cptr);   else AnimateSpinDead(cptr);
		    break;
	case AI_CERAT: if (cptr->Health) AnimateCera(cptr);   else AnimateCeraDead(cptr);
		    break;    
    case AI_TREX : if (cptr->Health) AnimateTRex(cptr);  else AnimateTRexDead(cptr);
		    break;
            */
    case 0       : AnimateHuntDead(cptr); break;			 	
   }
  }   
}  




void MakeNoise(Vector3d pos, float range)
{  
  for (int c=0; c<ChCount; c++) {
	TCharacter *cptr = &Characters[c];
	if (!cptr->Health) continue;
	float l = VectorLength(SubVectors(cptr->pos, pos));
  	if (l>range) continue;


	
	cptr->AfraidTime = (int) (10.f + (range-l) / 256.f) * 1024;
    cptr->State = 2;
	cptr->NoFindCnt = 0;
	
  }
}


void CheckAfraid()
{   
   if (!MyHealth) return;
   if (TrophyMode) return;

   Vector3d ppos, plook, clook, wlook, rlook;
   ppos = PlayerPos;

   if (DEBUG || UNDERWATER || ObservMode) return;

   plook.y = 0;
   plook.x = (float) f_sin(CameraAlpha);
   plook.z = (float)-f_cos(CameraAlpha);     

   wlook = Wind.nv;
   
   float kR, kwind, klook, kstand;
   
   float kmask  = 1.0f;
   float kskill = 1.0f;
   float kscent = 1.0f;

   if (CamoMode)  kmask *=1.5;
   if (ScentMode) kscent*=1.5;   

   for (int c=0; c<ChCount; c++) {
	 TCharacter *cptr = &Characters[c];
	 if (!cptr->Health) continue;
	 if (cptr->AI<10) continue;
	 if (cptr->AfraidTime || cptr->State==1) continue;

	 rlook = SubVectors(ppos, cptr->pos);
	 kR = VectorLength( rlook ) / 256.f / (32.f + ctViewR/2);
	 NormVector(rlook, 1.0f);

	 kR *= 2.5f / (float)(1.5+OptSens/128.f );
	 if (kR > 3.0f) continue;
     
	 clook.x = cptr->lookx;
	 clook.y = 0;
	 clook.z = cptr->lookz;

	 MulVectorsScal(wlook, rlook, kwind); kwind*=Wind.speed / 10;
	 MulVectorsScal(clook, rlook, klook); klook*=-1.f;

	 if (HeadY > 180) kstand = 0.7f; else kstand = 1.2f;

	 //============= reasons ==============//

	 float kALook  = kR * ((klook+3.f) / 3.f) * kstand * kmask;
	 if (klook>0.3) kALook*= 2.0;
	 if (klook>0.8) kALook*= 2.0;
	 kALook/=DinoInfo[cptr->CType].LookK;
	 if (kALook < 1.0)
	  if (TraceLook(cptr->pos.x, cptr->pos.y+220, cptr->pos.z,
		            PlayerX, PlayerY+HeadY/2.f, PlayerZ) ) kALook*=1.3f;
     
	 if (kALook < 1.0)
	  if (TraceLook(cptr->pos.x, cptr->pos.y+220, cptr->pos.z,
		            PlayerX, PlayerY+HeadY, PlayerZ) )   kALook = 2.0;
	 kALook *= (1.f + (float)ObjectsOnLook / 6.f);

	/*
	  if (kR<1.0f) {
		  char t[32];       
	   wsprintf(t,"%d", ObjectsOnLook);
	   AddMessage(t);
	   kALook = 20.f;
	  }
	  */ 
	 
	 float kASmell = kR * ((kwind+2.0f) / 2.0F) * ((klook+3.f) / 3.f) * kscent;
     if (kwind>0) kASmell*= 2.0;
	 kASmell/=DinoInfo[cptr->CType].SmellK;
	  	 	 
	 float kRes = min (kALook, kASmell);

     if (kRes < 1.0) {				
         /*
        MessageBeep(0xFFFFFFFF);        
		char t[128];
		if (kALook<kASmell)
		 sprintf(t, "LOOK: KR: %f  Tr: %d  K: %f", kR, ObjectsOnLook, kALook);
		else
	     sprintf(t, "SMELL: KR: %f  Tr: %d  K: %f", kR, ObjectsOnLook, kASmell);
		AddMessage(t); 
        */

		kRes = min (kRes, kR);
		cptr->AfraidTime = (int)(1.0 / (kRes+0.1) * 10.f * 1000.f);
		cptr->State = 2;
		cptr->NoFindCnt = 0;
	 }
   }
}





void PlaceTrophy()
{
	ChCount = 0;
	
	for (int c=0; c<24; c++) {
	 if (!TrophyRoom.Body[c].ctype) continue;
     Characters[ChCount].CType = TrophyRoom.Body[c].ctype;
	 	 
	 if (c<6) Characters[ChCount].alpha = pi/2; else
	  if (c<12) Characters[ChCount].alpha = pi; else
	   if (c<18) Characters[ChCount].alpha = pi*3/2; else
	               Characters[ChCount].alpha = 0;

	 ResetCharacter(&Characters[ChCount]);

	 Characters[ChCount].State = c;
	 Characters[ChCount].scale = TrophyRoom.Body[c].scale;
	 Characters[ChCount].pos.x = LandingList.list[c].x * 256.f + 128.f;
	 Characters[ChCount].pos.z = LandingList.list[c].y * 256.f + 128.f;
	 
	 Characters[ChCount].pos.y = GetLandH(Characters[ChCount].pos.x, 
		                                  Characters[ChCount].pos.z);
	 ChCount++;
	}
}



void PlaceCharacters()
{
	int c, tr;
  	ChCount = 0;
	PrintLog("Placing...");
//return;
	for (c=10; c<30; c++)
		if ( (TargetDino & (1<<c))>0 ) { TargetCall=c; break; }
	    
	
    //======== lohs =========//

   int MC = 5 + OptDens/80;
   if (OptDayNight==2) MC/=2;
   
   tr = 0;

   for (c=0; c<MC; c++) {
     Characters[ChCount].CType = 1 + c % 2;
replace1:
	 Characters[ChCount].pos.x = PlayerX + siRand(10040);
     Characters[ChCount].pos.z = PlayerZ + siRand(10040);
	 Characters[ChCount].pos.y = GetLandH(Characters[ChCount].pos.x, 
		                                  Characters[ChCount].pos.z);
	 tr++;
	 if (tr>10240) break;

	 if (CheckPlaceCollisionP(Characters[ChCount].pos)) goto replace1;

	 ResetCharacter(&Characters[ChCount]);     

	 if (Characters[ChCount].AI==AI_ARCHEO)
		 Characters[ChCount].pos.y+=2048.f; 

     Characters[ChCount].tgx = Characters[ChCount].pos.x;
     Characters[ChCount].tgz = Characters[ChCount].pos.z;
	 Characters[ChCount].tgtime = 0;

	 
	 ChCount++;
   }

   

   int TC = 0;
   int TDi[10];
   TDi[0] = 10;
   for (c=10; c<20; c++)
	   if (TargetDino & (1<<c)) TDi[TC++]=c;
      
   MC = 8 + OptDens/30 + rRand(6);
   tr = 0;
   
   //======== main =========//
   for (c=0; c<MC; c++) {        	 
	   
	 if ((c<4) || (!TargetDino)) Characters[ChCount].CType = AI_to_CIndex[10] + rRand(6); else
     if (c<10) Characters[ChCount].CType = AI_to_CIndex[ TDi[c % (TC)] ];
          else Characters[ChCount].CType = AI_to_CIndex[ TDi[rRand(TC-1)] ];
     //Characters[ChCount].CType = AI_to_CIndex[ AI_BRONT+1];
	 //Characters[ChCount].CType = AI_to_CIndex[10] + 7;//rRand(3);     
replace2:
	 Characters[ChCount].pos.x = 512*256 + siRand(50*256)*10;
     Characters[ChCount].pos.z = 512*256 + siRand(50*256)*10;
	 Characters[ChCount].pos.y = GetLandH(Characters[ChCount].pos.x, 
		                                  Characters[ChCount].pos.z);
	 tr++;
	 if (tr>10240) break;

	 if ( fabs(Characters[ChCount].pos.x - PlayerX) + 
	      fabs(Characters[ChCount].pos.z - PlayerZ) < 256 * 40 )
		  goto replace2;		  
	 
	 if (CheckPlaceCollisionP(Characters[ChCount].pos)) goto replace2;

     Characters[ChCount].tgx = Characters[ChCount].pos.x;
     Characters[ChCount].tgz = Characters[ChCount].pos.z;
	 Characters[ChCount].tgtime = 0;

	 ResetCharacter(&Characters[ChCount]);     
	 ChCount++;
   }

   PrintLog("\n");
   DemoPoint.DemoTime = 0;
}








void CreateChMorphedModel(TCharacter *cptr)
{   
   TAni *aptr  = &cptr->pinfo->Animation[cptr->Phase];   
   TAni *paptr = &cptr->pinfo->Animation[cptr->PrevPhase];   

   int CurFrame, SplineD, PCurFrame, PSplineD;
   float scale = cptr->scale;

   CurFrame = ( (aptr->FramesCount-1) * cptr->FTime * 256) / aptr->AniTime;
   SplineD = CurFrame & 0xFF;
   CurFrame = (CurFrame>>8);


   BOOL PMorph = (cptr->Phase != cptr->PrevPhase) && (cptr->PPMorphTime < PMORPHTIME) && (MORPHP);

   if (PMorph) {
      PCurFrame = ( (paptr->FramesCount-1) * cptr->PrevPFTime * 256) / paptr->AniTime;
      PSplineD = PCurFrame & 0xFF;
      PCurFrame = (PCurFrame>>8);      
   }

  

   if (!MORPHA) { SplineD = 0; PSplineD = 0; }

   float k1, k2, pk1, pk2, pmk1, pmk2;

   k2 = (float)(SplineD) / 256.f;
   k1 = 1.0f - k2;
   k1/=8.f; k2/=8.f;

   if (PMorph) {
     pk2 = (float)(PSplineD) / 256.f;
     pk1 = 1.0f - pk2;
     pk1/=8.f; pk2/=8.f;
     pmk1 = (float)cptr->PPMorphTime / PMORPHTIME;
     pmk2 = 1.f - pmk1;
   }

   int VCount = cptr->pinfo->mptr->VCount;
   short int* adptr  =  aptr->aniData + CurFrame*VCount*3;
   short int* padptr = paptr->aniData + PCurFrame*VCount*3;

   float sb = (float)f_sin(cptr->beta) * scale;
   float cb = (float)f_cos(cptr->beta) * scale;
   float sg = (float)f_sin(cptr->gamma);
   float cg = (float)f_cos(cptr->gamma);

   for (int v=0; v<VCount; v++) {
    float x = *(adptr+v*3+0) * k1 + *(adptr+(v+VCount)*3+0) * k2;
    float y = *(adptr+v*3+1) * k1 + *(adptr+(v+VCount)*3+1) * k2;
    float z = - (*(adptr+v*3+2) * k1 + *(adptr+(v+VCount)*3+2) * k2);

    if (PMorph) {
      float px = *(padptr+v*3+0) * pk1 + *(padptr+(v+VCount)*3+0) * pk2;
      float py = *(padptr+v*3+1) * pk1 + *(padptr+(v+VCount)*3+1) * pk2;
      float pz = - (*(padptr+v*3+2) * pk1 + *(padptr+(v+VCount)*3+2) * pk2);

      x = x*pmk1 + px* pmk2;
      y = y*pmk1 + py* pmk2;
      z = z*pmk1 + pz* pmk2;
    }
    

    float zz = z;
    float xx = cg * x - sg * y;
    float yy = cg * y + sg * x;


    //float fi = (z / 400) * (cptr->bend / 1.5f);
    float fi;
    if (z>0) {
        fi = z / 240.f; 
        if (fi>1.f) fi=1.f;
    } else {
        fi = z / 380.f;
        if (fi<-1.f) fi=-1.f;
    }
    
    fi*=cptr->bend;
    
	float bendc = (float)f_cos(fi);
    float bends = (float)f_sin(fi);
    
    float bx = bendc * xx - bends * zz;
    float bz = bendc * zz + bends * xx;
    zz = bz;
    xx = bx;
    
    cptr->pinfo->mptr->gVertex[v].x = xx * scale;
    cptr->pinfo->mptr->gVertex[v].y = cb * yy - sb * zz;
    cptr->pinfo->mptr->gVertex[v].z = cb * zz + sb * yy;
   }   
}



void CreateMorphedModel(TModel* mptr, TAni *aptr, int FTime, float scale)
{
  int CurFrame = ((aptr->FramesCount-1) * FTime * 256) / aptr->AniTime;

  int SplineD = CurFrame & 0xFF;
  CurFrame = (CurFrame>>8);

  float k2 = (float)(SplineD) / 256.f;
  float k1 = 1.0f - k2;
  k1*=scale/8.f; 
  k2*=scale/8.f;
 
  int VCount = mptr->VCount;
  short int* adptr = &(aptr->aniData[CurFrame*VCount*3]);
  for (int v=0; v<VCount; v++) {
   mptr->gVertex[v].x = *(adptr+v*3+0) * k1 + *(adptr+(v+VCount)*3+0) * k2;
   mptr->gVertex[v].y = *(adptr+v*3+1) * k1 + *(adptr+(v+VCount)*3+1) * k2;
   mptr->gVertex[v].z =- *(adptr+v*3+2) * k1 - *(adptr+(v+VCount)*3+2) * k2;
  }
}




void CreateMorphedObject(TModel* mptr, TVTL &vtl, int FTime)
{
  int CurFrame = ((vtl.FramesCount-1) * FTime * 256) / vtl.AniTime;

  int SplineD = CurFrame & 0xFF;
  CurFrame = (CurFrame>>8);

  float k2 = (float)(SplineD) / 256.f;
  float k1 = 1.0f - k2;
  k1/=8.f; k2/=8.f;
 
  int VCount = mptr->VCount;
  short int* adptr = &(vtl.aniData[CurFrame*VCount*3]);
  for (int v=0; v<VCount; v++) {
   mptr->gVertex[v].x = *(adptr+v*3+0) * k1 + *(adptr+(v+VCount)*3+0) * k2;
   mptr->gVertex[v].y = *(adptr+v*3+1) * k1 + *(adptr+(v+VCount)*3+1) * k2;
   mptr->gVertex[v].z =- *(adptr+v*3+2) * k1 - *(adptr+(v+VCount)*3+2) * k2;
  }
}