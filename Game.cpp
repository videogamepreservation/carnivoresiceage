#include "Hunt.h"

bool ShowFaces = true;

void UploadGeometry()
{
	int x,y,xx,yy;
    byte temp;
    
	AudioFCount = 0;

	int MaxView = 20;
    int HalfView = (int)(MaxView/2)+1;    

	for (x = 0; x < MaxView; x++)
     for (y = 0; y < MaxView; y++)
	 {
	  xx = (x - HalfView)*2; 
	  yy = (y - HalfView)*2;
	  data[AudioFCount].x1 = (CCX+xx) * 256 - CameraX; 
	  data[AudioFCount].y1 = HMap[CCY+yy][CCX+xx] * ctHScale - CameraY; 
	  data[AudioFCount].z1 = (CCY+yy) * 256 - CameraZ; 

	  xx = ((x+1) - HalfView)*2; 
	  yy = (y - HalfView)*2;
	  data[AudioFCount].x2 = (CCX+xx) * 256 - CameraX; 
	  data[AudioFCount].y2 = HMap[CCY+yy][CCX+xx] * ctHScale - CameraY; 
	  data[AudioFCount].z2 = (CCY+yy) * 256 - CameraZ; 

	  xx = ((x+1) - HalfView)*2; 
	  yy = ((y+1) - HalfView)*2;
	  data[AudioFCount].x3 = (CCX+xx) * 256 - CameraX; 
	  data[AudioFCount].y3 = HMap[CCY+yy][CCX+xx] * ctHScale - CameraY; 
	  data[AudioFCount].z3 = (CCY+yy) * 256 - CameraZ; 

	  xx = (x - HalfView)*2; 
	  yy = ((y+1) - HalfView)*2;
	  data[AudioFCount].x4 = (CCX+xx) * 256 - CameraX; 
	  data[AudioFCount].y4 = HMap[CCY+yy][CCX+xx] * ctHScale - CameraY; 
	  data[AudioFCount].z4 = (CCY+yy) * 256 - CameraZ; 

	  AudioFCount++;
	 }

	 if (ShowFaces) 
	 {   wsprintf(logt,"Audio_UpdateGeometry: %i faces uploaded\n", AudioFCount); PrintLog(logt);

		 ShowFaces = false;}
}


void SetupRes()
{
	if (!HARD3D)
		if (OptRes>5) OptRes=5;
    if (OptRes==0) { WinW = 320; WinH=240; }
	if (OptRes==1) { WinW = 400; WinH=300; }
	if (OptRes==2) { WinW = 512; WinH=384; }
	if (OptRes==3) { WinW = 640; WinH=480; }
	if (OptRes==4) { WinW = 800; WinH=600; }
	if (OptRes==5) { WinW =1024; WinH=768; }		
	if (OptRes==6) { WinW =1280; WinH=1024; }
	if (OptRes==7) { WinW =1600; WinH=1200; }	
}


float GetLandOH(int x, int y)
{
  return (float)(HMapO[y][x]) * ctHScale;
}


float GetLandOUH(int x, int y)
{
  if (FMap[y][x] & fmReverse)
   return (float)((int)(HMap[y][x+1]+HMap[y+1][x])/2.f)*ctHScale;             
                else
   return (float)((int)(HMap[y][x]+HMap[y+1][x+1])/2.f)*ctHScale;
}



float GetLandUpH(float x, float y)
{ 
	
   int CX = (int)x / 256;
   int CY = (int)y / 256;

   if (!(FMap[CY][CX] & fmWaterA)) return GetLandH(x,y);
   
   return (float)(WaterList[ WMap[CY][CX] ].wlevel * ctHScale);

}


float GetLandH(float x, float y)
{ 
   int CX = (int)x / 256;
   int CY = (int)y / 256;
   
   int dx = (int)x % 256;
   int dy = (int)y % 256; 

   int h1 = HMap[CY][CX];
   int h2 = HMap[CY][CX+1];
   int h3 = HMap[CY+1][CX+1];
   int h4 = HMap[CY+1][CX];


   if (FMap[CY][CX] & fmReverse) {
     if (256-dx>dy) h3 = h2+h4-h1;
               else h1 = h2+h4-h3;
   } else {
     if (dx>dy) h4 = h1+h3-h2;
           else h2 = h1+h3-h4;
   }

   float h = (float)
	   (h1   * (256-dx) + h2 * dx) * (256-dy) +
	   (h4   * (256-dx) + h3 * dx) * dy;

   return  (h / 256.f / 256.f) * ctHScale;	      
}



float GetLandLt(float x, float y)
{ 
   int CX = (int)x / 256;
   int CY = (int)y / 256;
   
   int dx = (int)x % 256;
   int dy = (int)y % 256; 

   int h1 = LMap[CY][CX];
   int h2 = LMap[CY][CX+1];
   int h3 = LMap[CY+1][CX+1];
   int h4 = LMap[CY+1][CX]; 

   float h = (float)
	   (h1   * (256-dx) + h2 * dx) * (256-dy) +
	   (h4   * (256-dx) + h3 * dx) * dy;

   return  (h / 256.f / 256.f);
}



float GetLandLt2(float x, float y)
{ 
   int CX = ((int)x / 512)*2 - CCX;
   int CY = ((int)y / 512)*2 - CCY;
   
   int dx = (int)x % 512;
   int dy = (int)y % 512; 

   int h1 = VMap[CY+128][CX+128].Light;
   int h2 = VMap[CY+128][CX+2+128].Light;
   int h3 = VMap[CY+2+128][CX+2+128].Light;
   int h4 = VMap[CY+2+128][CX+128].Light; 

   float h = (float)
	   (h1   * (512-dx) + h2 * dx) * (512-dy) +
	   (h4   * (512-dx) + h3 * dx) * dy;

   return  (h / 512.f / 512.f);
}



void CalcModelGroundLight(TModel *mptr, float x0, float z0, int FI)
{
	float ca = (float)f_cos(FI * pi / 2);
	float sa = (float)f_sin(FI * pi / 2);
	for (int v=0; v<mptr->VCount; v++) {
		float x = mptr->gVertex[v].x * ca + mptr->gVertex[v].z * sa + x0;
		float z = mptr->gVertex[v].z * ca - mptr->gVertex[v].x * sa + z0;
        mptr->VLight[0][v] = GetLandLt2(x, z) - 128;		
	}
}


BOOL PointOnBound(float &H, float px, float py, float cx, float cy, float oy, TBound *bound, int angle)
{
	px-=cx;
	py-=cy; 
	
	float ca = (float) f_cos(angle*pi / 2.f);
	float sa = (float) f_sin(angle*pi / 2.f);	

	BOOL _on = FALSE;
	H=-10000;

	for (int o=0; o<8; o++) {
		
		if (bound[o].a<0) continue;
		if (bound[o].y2 + oy > PlayerY + 128) continue;        
		
        float a,b;
	    float ccx = bound[o].cx*ca + bound[o].cy*sa;
	    float ccy = bound[o].cy*ca - bound[o].cx*sa;

	    if (angle & 1) {
         a = bound[o].b;
	     b = bound[o].a;
		} else {
	     a = bound[o].a;
	     b = bound[o].b;
		}

	    if ( ( fabs(px - ccx) < a) &&  (fabs(py - ccy) < b) ) 
		{
		      _on=TRUE;			  
			  if (H < bound[o].y2) H = bound[o].y2;			  
		}
	}

	return _on;
}



BOOL PointUnBound(float &H, float px, float py, float cx, float cy, float oy, TBound *bound, int angle)
{
	px-=cx;
	py-=cy; 
	
	float ca = (float) f_cos(angle*pi / 2.f);
	float sa = (float) f_sin(angle*pi / 2.f);	

	BOOL _on = FALSE;
	H=+10000;

	for (int o=0; o<8; o++) {
		
		if (bound[o].a<0) continue;
		if (bound[o].y1 + oy < PlayerY + 128) continue;        
		
        float a,b;
	    float ccx = bound[o].cx*ca + bound[o].cy*sa;
	    float ccy = bound[o].cy*ca - bound[o].cx*sa;

	    if (angle & 1) {
         a = bound[o].b;
	     b = bound[o].a;
		} else {
	     a = bound[o].a;
	     b = bound[o].b;
		}

	    if ( ( fabs(px - ccx) < a) &&  (fabs(py - ccy) < b) ) 
		{
		      _on=TRUE;			  
			  if (H > bound[o].y1) H = bound[o].y1;
		}
	}

	return _on;
}





float GetLandCeilH(float CameraX, float CameraZ)
{
  float h,hh;
  
   h = GetLandH(CameraX, CameraZ) + 20480;

   int ccx = (int)CameraX / 256;
   int ccz = (int)CameraZ / 256;

   for (int z=-4; z<=4; z++)
    for (int x=-4; x<=4; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
		
        float CR = (float)MObjects[ob].info.Radius - 1.f;
                
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;

		float LandY = GetLandOH(ccx+x, ccz+z);

		if (!(MObjects[ob].info.flags & ofBOUND)) {
         if (MObjects[ob].info.YLo + LandY > h) continue;
         if (MObjects[ob].info.YLo + LandY < PlayerY+100) continue;         
		}

        float r = CR+1;

		if (MObjects[ob].info.flags & ofBOUND)
		{
			float hh;
			if (PointUnBound(hh, CameraX, CameraZ, ox, oz, LandY, MObjects[ob].bound, ((FMap[ccz+z][ccx+x] >> 2) & 3)  ) )
				if (h > LandY + hh) h = LandY + hh;
		} else {
		 if (MObjects[ob].info.flags & ofCIRCLE)
		   r = (float) sqrt( (ox-CameraX)*(ox-CameraX) + (oz-CameraZ)*(oz-CameraZ) );
		 else
		   r = (float) max( fabs(ox-CameraX) , fabs(oz-CameraZ) );
		 
		 if (r<CR) h = MObjects[ob].info.YLo + LandY;
		}
        
   }
  return h;
}



float GetLandQH(float CameraX, float CameraZ)
{
  float h,hh;
  
   h = GetLandH(CameraX, CameraZ);
   hh = GetLandH(CameraX-90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX+90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX-90.f, CameraZ+90.f); if (hh>h) h=hh; 
   hh = GetLandH(CameraX+90.f, CameraZ+90.f); if (hh>h) h=hh;

   hh = GetLandH(CameraX+128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX-128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ+128.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ-128.f); if (hh>h) h=hh;

   int ccx = (int)CameraX / 256;
   int ccz = (int)CameraZ / 256;

   for (int z=-4; z<=4; z++)
    for (int x=-4; x<=4; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
		
        float CR = (float)MObjects[ob].info.Radius - 1.f;
                
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;

		float LandY = GetLandOH(ccx+x, ccz+z);

		if (!(MObjects[ob].info.flags & ofBOUND)) {
         if (MObjects[ob].info.YHi + LandY < h) continue;
         if (MObjects[ob].info.YHi + LandY > PlayerY+128) continue;
         //if (MObjects[ob].info.YLo + LandY > PlayerY+256) continue;
		}

        float r = CR+1;

		if (MObjects[ob].info.flags & ofBOUND)
		{
			float hh;
			if (PointOnBound(hh, CameraX, CameraZ, ox, oz, LandY, MObjects[ob].bound, ((FMap[ccz+z][ccx+x] >> 2) & 3)  ) )
				if (h < LandY + hh) h = LandY + hh;
		} else {
		 if (MObjects[ob].info.flags & ofCIRCLE)
		   r = (float) sqrt( (ox-CameraX)*(ox-CameraX) + (oz-CameraZ)*(oz-CameraZ) );
		 else
		   r = (float) max( fabs(ox-CameraX) , fabs(oz-CameraZ) );
		 
		 if (r<CR) h = MObjects[ob].info.YHi + LandY;
		}
        
   }
  return h;
}


float GetLandHObj(float CameraX, float CameraZ)
{
   float h;   

   h = 0;

   int ccx = (int)CameraX / 256;
   int ccz = (int)CameraZ / 256;

   for (int z=-3; z<=3; z++)
    for (int x=-3; x<=3; x++) 
      if (OMap[ccz+z][ccx+x]!=255) {
        int ob = OMap[ccz+z][ccx+x];
        float CR = (float)MObjects[ob].info.Radius - 1.f;
        
        float oz = (ccz+z) * 256.f + 128.f;
        float ox = (ccx+x) * 256.f + 128.f;

        if (MObjects[ob].info.YHi + GetLandOH(ccx+x, ccz+z) < h) continue;
        if (MObjects[ob].info.YLo + GetLandOH(ccx+x, ccz+z) > PlayerY+256) continue;
        float r;
		if (MObjects[ob].info.flags & ofCIRCLE) 		
		  r = (float) sqrt( (ox-CameraX)*(ox-CameraX) + (oz-CameraZ)*(oz-CameraZ) );
		else
		  r = (float) max( fabs(ox-CameraX) , fabs(oz-CameraZ) );

        if (r<CR) 
            h = MObjects[ob].info.YHi + GetLandOH(ccx+x, ccz+z);
   }

  return h;
}


float GetLandQHNoObj(float CameraX, float CameraZ)
{
  float h,hh;
  
   h = GetLandH(CameraX, CameraZ);
   hh = GetLandH(CameraX-90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX+90.f, CameraZ-90.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX-90.f, CameraZ+90.f); if (hh>h) h=hh; 
   hh = GetLandH(CameraX+90.f, CameraZ+90.f); if (hh>h) h=hh;

   hh = GetLandH(CameraX+128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX-128.f, CameraZ); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ+128.f); if (hh>h) h=hh;
   hh = GetLandH(CameraX, CameraZ-128.f); if (hh>h) h=hh;
   
   return h;
}


void ProcessCommandLine()
{
  for (int a=0; a<__argc; a++) {
     LPSTR s = __argv[a];
     if (strstr(s,"x=")) { PlayerX = (float)atof(&s[2])*256.f; LockLanding = TRUE; }
     if (strstr(s,"y=")) { PlayerZ = (float)atof(&s[2])*256.f; LockLanding = TRUE; }          
	 if (strstr(s,"reg=")) TrophyRoom.RegNumber = atoi(&s[4]); 
     if (strstr(s,"prj=")) strcpy(ProjectName, (s+4)); 
	 if (strstr(s,"din=")) TargetDino = (atoi(&s[4])*1024);
	 if (strstr(s,"wep=")) WeaponPres = atoi(&s[4]);	 
	 if (strstr(s,"dtm=")) OptDayNight  = atoi(&s[4]);

	 if (strstr(s,"-debug"))   DEBUG = TRUE;
	 if (strstr(s,"-double"))  DoubleAmmo = TRUE;
	 if (strstr(s,"-radar"))   RadarMode = TRUE;
	 if (strstr(s,"-tranq")) Tranq = TRUE;
	 if (strstr(s,"-observ")) ObservMode = TRUE;
  } 
}




void AddWCircle(float x, float z, float scale)
{
   WCircles[WCCount].pos.x = x;
   WCircles[WCCount].pos.z = z;
   WCircles[WCCount].pos.y = GetLandUpH(x, z);
   WCircles[WCCount].FTime = 0;   
   WCircles[WCCount].scale = scale;
   WCCount++;
}


void AddShipTask(int cindex)
{
  TCharacter *cptr = &Characters[cindex];

  BOOL TROPHYON  = (GetLandUpH(cptr->pos.x, cptr->pos.z) - GetLandH(cptr->pos.x, cptr->pos.z) < 100) && 
	               (!Tranq);

  if (TROPHYON) {
     ShipTask.clist[ShipTask.tcount] = cindex;
     ShipTask.tcount++;
	 AddVoicev(ShipModel.SoundFX[3].length,
		       ShipModel.SoundFX[3].lpData, 256);
  }

  //===== trophy =======//
  SYSTEMTIME st;
  GetLocalTime(&st);
  int t=0;
  for (t=0; t<23; t++)
	  if (!TrophyRoom.Body[t].ctype) break;

  float score = (float)DinoInfo[Characters[cindex].CType].BaseScore;

  if (TrophyRoom.Last.success>1)
	  score*=(1.f + TrophyRoom.Last.success / 10.f);

  if (!(TargetDino & (1<<Characters[cindex].AI)) ) score/=2.f;

  if (Tranq    ) score *= 1.25f;
  if (RadarMode) score *= 0.70f;
  if (ScentMode) score *= 0.80f;
  if (CamoMode ) score *= 0.85f;
  TrophyRoom.Score+=(int)score;


  if (!Tranq) {
   TrophyTime = 20 * 1000;  
   TrophyBody = t;  
   TrophyRoom.Body[t].ctype  = Characters[cindex].CType;
   TrophyRoom.Body[t].scale  = Characters[cindex].scale;
   TrophyRoom.Body[t].weapon = CurrentWeapon;
   TrophyRoom.Body[t].score  = (int)score;
   TrophyRoom.Body[t].phase  = (RealTime & 3);
   TrophyRoom.Body[t].time = (st.wHour<<10) + st.wMinute;
   TrophyRoom.Body[t].date = (st.wYear<<20) + (st.wMonth<<10) + st.wDay;
   TrophyRoom.Body[t].range = VectorLength( SubVectors(Characters[cindex].pos, PlayerPos) ) / 64.f;
   PrintLog("Trophy added: ");
   PrintLog(DinoInfo[Characters[cindex].CType].Name);
   PrintLog("\n");
  }  
}



void InitShip(int cindex)
{
	TCharacter *cptr = &Characters[cindex];

	Ship.DeltaY = 2048.f + DinoInfo[cptr->CType].ShDelta * cptr->scale;

	Ship.pos.x = PlayerX - 90*256; 
	if (Ship.pos.x < 256) Ship.pos.x = PlayerX + 90*256; 
	Ship.pos.z = PlayerZ - 90*256;
	if (Ship.pos.z < 256) Ship.pos.z = PlayerZ + 90*256; 
	Ship.pos.y = GetLandUpH(Ship.pos.x, Ship.pos.z)  + Ship.DeltaY + 1024;

	Ship.tgpos.x = cptr->pos.x;
	Ship.tgpos.z = cptr->pos.z;
    Ship.tgpos.y = GetLandUpH(Ship.tgpos.x, Ship.tgpos.z)  + Ship.DeltaY;
	Ship.State = 0;

	Ship.retpos = Ship.pos;
	Ship.cindex = cindex;
	Ship.FTime = 0;
}



void HideWeapon()
{
  TWeapon *wptr = &Weapon;
  if (UNDERWATER && !wptr->state) return;
  if (ObservMode || TrophyMode) return;
  
  if (wptr->state == 0) {   
	  if (!ShotsLeft[CurrentWeapon]) return;
	  if (WeapInfo[CurrentWeapon].Optic) OPTICMODE = TRUE;
      AddVoicev(wptr->chinfo[CurrentWeapon].SoundFX[0].length,
                wptr->chinfo[CurrentWeapon].SoundFX[0].lpData, 256);
      wptr->FTime = 0;
      wptr->state = 1;
	  BINMODE = FALSE;
	  MapMode = FALSE;
	  wptr->shakel = 0.2f;
      return;
  }

  if (wptr->state!=2 || wptr->FTime!=0) return;
  AddVoicev(wptr->chinfo[CurrentWeapon].SoundFX[2].length,
            wptr->chinfo[CurrentWeapon].SoundFX[2].lpData, 256);
  wptr->state = 3;
  wptr->FTime = 0;
  OPTICMODE = FALSE;
  return ;
}








void InitGameInfo()
{
	for (int c=0; c<32; c++) {
		DinoInfo[c].Scale0 = 800;
		DinoInfo[c].ScaleA = 600;
		DinoInfo[c].ShDelta = 0;
	}
/*
    WeapInfo[0].Name = "Shotgun";
	WeapInfo[0].Power = 1.5f;
	WeapInfo[0].Prec  = 1.1f;
	WeapInfo[0].Loud  = 0.3f;
	WeapInfo[0].Rate  = 1.6f;
	WeapInfo[0].Shots = 6;

	WeapInfo[1].Name = "X-Bow";
	WeapInfo[1].Power = 1.1f;
	WeapInfo[1].Prec  = 0.7f;
	WeapInfo[1].Loud  = 1.9f;
	WeapInfo[1].Rate  = 1.2f;
	WeapInfo[1].Shots = 8;

    WeapInfo[2].Name = "Sniper Rifle";
	WeapInfo[2].Power = 1.0f;
	WeapInfo[2].Prec  = 1.8f;
	WeapInfo[2].Loud  = 0.6f;
	WeapInfo[2].Rate  = 1.0f;
	WeapInfo[2].Shots = 6;


	
	
	DinoInfo[ 0].Name = "Moschops";
	DinoInfo[ 0].Health0 = 2;
	DinoInfo[ 0].Mass = 0.15f;

    DinoInfo[ 1].Name = "Galimimus";
	DinoInfo[ 1].Health0 = 2;
	DinoInfo[ 1].Mass = 0.1f;

	DinoInfo[ 2].Name = "Dimorphodon";
    DinoInfo[ 2].Health0 = 1;
	DinoInfo[ 2].Mass = 0.05f;

	DinoInfo[ 3].Name = "Dimetrodon";
    DinoInfo[ 3].Health0 = 2;
	DinoInfo[ 3].Mass = 0.22f;	


	DinoInfo[ 5].Name = "Parasaurolophus";
	DinoInfo[ 5].Mass = 1.5f;
	DinoInfo[ 5].Length = 5.8f;
	DinoInfo[ 5].Radius = 320.f;
	DinoInfo[ 5].Health0 = 5;
	DinoInfo[ 5].BaseScore = 6;
	DinoInfo[ 5].SmellK = 0.8f; DinoInfo[ 4].HearK = 1.f; DinoInfo[ 4].LookK = 0.4f;
	DinoInfo[ 5].ShDelta = 48;

	DinoInfo[ 6].Name = "Pachycephalosaurus";
	DinoInfo[ 6].Mass = 0.8f;
	DinoInfo[ 6].Length = 4.5f;
	DinoInfo[ 6].Radius = 280.f;
	DinoInfo[ 6].Health0 = 4;
	DinoInfo[ 6].BaseScore = 8;
	DinoInfo[ 6].SmellK = 0.4f; DinoInfo[ 5].HearK = 0.8f; DinoInfo[ 5].LookK = 0.6f;
	DinoInfo[ 6].ShDelta = 36;

	DinoInfo[ 7].Name = "Stegosaurus";
    DinoInfo[ 7].Mass = 7.f;
	DinoInfo[ 7].Length = 7.f;
	DinoInfo[ 7].Radius = 480.f;
	DinoInfo[ 7].Health0 = 5;
	DinoInfo[ 7].BaseScore = 7;
	DinoInfo[ 7].SmellK = 0.4f; DinoInfo[ 6].HearK = 0.8f; DinoInfo[ 6].LookK = 0.6f;
	DinoInfo[ 7].ShDelta = 128;

	DinoInfo[ 8].Name = "Allosaurus";
	DinoInfo[ 8].Mass = 0.5;
	DinoInfo[ 8].Length = 4.2f;
	DinoInfo[ 8].Radius = 256.f;
	DinoInfo[ 8].Health0 = 3;
	DinoInfo[ 8].BaseScore = 12;
	DinoInfo[ 8].Scale0 = 1000;
	DinoInfo[ 8].ScaleA = 600;
	DinoInfo[ 8].SmellK = 1.0f; DinoInfo[ 7].HearK = 0.3f; DinoInfo[ 7].LookK = 0.5f;
	DinoInfo[ 8].ShDelta = 32;
	DinoInfo[ 8].DangerCall = TRUE;

	DinoInfo[ 9].Name = "Chasmosaurus";
	DinoInfo[ 9].Mass = 3.f;
	DinoInfo[ 9].Length = 5.0f;
	DinoInfo[ 9].Radius = 400.f;	
	DinoInfo[ 9].Health0 = 8;
	DinoInfo[ 9].BaseScore = 9;
	DinoInfo[ 9].SmellK = 0.6f; DinoInfo[ 8].HearK = 0.5f; DinoInfo[ 8].LookK = 0.4f;
	//DinoInfo[ 8].ShDelta = 148;
	DinoInfo[ 9].ShDelta = 108;

	DinoInfo[10].Name = "Velociraptor";
	DinoInfo[10].Mass = 0.3f;
	DinoInfo[10].Length = 4.0f;
	DinoInfo[10].Radius = 256.f;
	DinoInfo[10].Health0 = 3;
	DinoInfo[10].BaseScore = 16;
	DinoInfo[10].ScaleA = 400;
	DinoInfo[10].SmellK = 1.0f; DinoInfo[ 9].HearK = 0.5f; DinoInfo[ 9].LookK = 0.4f;
	DinoInfo[10].ShDelta =-24;
	DinoInfo[10].DangerCall = TRUE;

	DinoInfo[11].Name = "T-Rex";
    DinoInfo[11].Mass = 6.f;
	DinoInfo[11].Length = 12.f;
	DinoInfo[11].Radius = 400.f;
	DinoInfo[11].Health0 = 1024;
	DinoInfo[11].BaseScore = 20;
	DinoInfo[11].SmellK = 0.85f; DinoInfo[10].HearK = 0.8f; DinoInfo[10].LookK = 0.8f;
	DinoInfo[11].ShDelta = 168;
	DinoInfo[11].DangerCall = TRUE;

	DinoInfo[ 4].Name = "Brahiosaurus";
    DinoInfo[ 4].Mass = 9.f;
	DinoInfo[ 4].Length = 12.f;
	DinoInfo[ 4].Radius = 400.f;
	DinoInfo[ 4].Health0 = 1024;
	DinoInfo[ 4].BaseScore = 0;
	DinoInfo[ 4].SmellK = 0.85f; DinoInfo[16].HearK = 0.8f; DinoInfo[16].LookK = 0.8f;
	DinoInfo[ 4].ShDelta = 168;
	DinoInfo[ 4].DangerCall = FALSE;
*/
	LoadResourcesScript();
}




void InitEngine()
{
    //DEBUG        = TRUE;

	WATERANI     = TRUE;
	NODARKBACK   = TRUE;
    LoDetailSky  = TRUE;
    CORRECTION   = TRUE;
	FOGON        = TRUE;
	FOGENABLE    = TRUE;
    Clouds       = TRUE;   
    SKY          = TRUE;
    GOURAUD      = TRUE;   
    MODELS       = TRUE;
    TIMER        = DEBUG;
    BITMAPP      = FALSE;
    MIPMAP       = TRUE;
    NOCLIP       = FALSE;
    CLIP3D       = TRUE;

    
    SLOW         = FALSE;
	LOWRESTX     = FALSE;
    MORPHP       = TRUE;
    MORPHA       = TRUE;

	_GameState = 0;

	RadarMode    = FALSE;

	fnt_BIG = CreateFont(
        23, 10, 0, 0,
        600, 0,0,0,
#ifdef __rus
		RUSSIAN_CHARSET,		
#else
        ANSI_CHARSET,
#endif				
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);	




    fnt_Small = CreateFont(
        14, 5, 0, 0,
        100, 0,0,0,
#ifdef __rus
		RUSSIAN_CHARSET,		
#else
        ANSI_CHARSET,
#endif		        
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);	


    fnt_Midd  = CreateFont(
        16, 7, 0, 0,
        550, 0,0,0,
#ifdef __rus
		RUSSIAN_CHARSET,		
#else
        ANSI_CHARSET,
#endif		        
        OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, DEFAULT_PITCH | FF_SWISS, NULL);	


    Heap = HeapCreate( 0, 60000000, 64000000 );
    if( Heap == NULL ) {
      MessageBox(hwndMain,"Error creating heap.","Error",IDOK);     
      return; }

    Textures[255] = (TEXTURE*) _HeapAlloc(Heap, 0, sizeof(TEXTURE));

    WaterR = 10;
    WaterG = 38;
    WaterB = 46;
    WaterA = 10;
	TargetDino = 1<<10;
	TargetCall = 10;
	WeaponPres = 1;
    MessageList.timeleft = 0;

	InitGameInfo();    
	        
    CreateVideoDIB();
    CreateFadeTab();	
    CreateDivTable();
    InitClips();
   
    TrophyRoom.RegNumber=0;
	
    PlayerX = (ctMapSize / 3) * 256;
	PlayerZ = (ctMapSize / 3) * 256;    

    ProcessCommandLine();
	

    switch (OptDayNight) {
   case 0:
	   SunShadowK = 0.7f;
	   Sun3dPos.x = - 4048;
       Sun3dPos.y = + 2048;
       Sun3dPos.z = - 4048;
	   break;
   case 1:
	   SunShadowK = 0.5f;
	   Sun3dPos.x = - 2048;
       Sun3dPos.y = + 4048;
       Sun3dPos.z = - 2048;
	   break;
   case 2:
	   SunShadowK = -0.7f;
	   Sun3dPos.x = + 3048;
       Sun3dPos.y = + 3048;
       Sun3dPos.z = + 3048;
	   break;
   }

	LoadTrophy();
	
	ProcessCommandLine();    

	
	
    
    //ctViewR  = 72;
	//ctViewR1 = 28;
	//ctViewRM = 24;
	ctViewR  = 42 + (int)(OptViewR / 8)*2;
	ctViewR1 = 28;
	ctViewRM = 24;
	
    Soft_Persp_K = 1.5f;
    HeadY = 220;

	FogsList[0].fogRGB = 0x000000;
	FogsList[0].YBegin = 0;
	FogsList[0].Transp = 000;
	FogsList[0].FLimit = 000;

	FogsList[127].fogRGB = 0x00504000;
    FogsList[127].Mortal = FALSE;
	FogsList[127].Transp = 460;
	FogsList[127].FLimit = 200;

	FillMemory( FogsMap, sizeof(FogsMap), 0);
	PrintLog("Init Engine: Ok.\n");
}





void ShutDownEngine()
{
   ReleaseDC(hwndMain,hdcMain);   
}



void ProcessSyncro()
{
   RealTime = timeGetTime();
   srand( (unsigned) RealTime );
   if (SLOW) RealTime/=4;
   TimeDt = RealTime - PrevTime;
   if (TimeDt<0) TimeDt = 10;
   if (TimeDt>10000) TimeDt = 10;
   if (TimeDt>1000) TimeDt = 1000;
   PrevTime = RealTime;
   Takt++;
   if (!PAUSE)
    if (MyHealth) MyHealth+=TimeDt*4;
   if (MyHealth>MAX_HEALTH) MyHealth = MAX_HEALTH;   
}






void AddBloodTrail(TCharacter *cptr)
{
	if (BloodTrail.Count>508) {
	  memcpy(&BloodTrail.Trail[0], &BloodTrail.Trail[1], 510*sizeof(TBloodP));
	  BloodTrail.Count--;
	}
    BloodTrail.Trail[BloodTrail.Count].LTime = 210000;
    BloodTrail.Trail[BloodTrail.Count].pos = cptr->pos;
	BloodTrail.Trail[BloodTrail.Count].pos.x+=siRand(32);
	BloodTrail.Trail[BloodTrail.Count].pos.z+=siRand(32);
	BloodTrail.Trail[BloodTrail.Count].pos.y = 
		GetLandH(BloodTrail.Trail[BloodTrail.Count].pos.x,
		         BloodTrail.Trail[BloodTrail.Count].pos.z)+4;
	BloodTrail.Count++;
}





void MakeCall()
{
   if (!TargetDino) return;
   if (UNDERWATER) return;
   if (ObservMode || TrophyMode) return;
   if (CallLockTime) return;
   
   CallLockTime=1024*3;
   
   NextCall+=(RealTime % 2)+1;
   NextCall%=3;

   AddVoicev(fxCall[TargetCall-10][NextCall].length,  
	         fxCall[TargetCall-10][NextCall].lpData, 256);

   float dmin = 512*256;
   int ai = -1;

   for (int c=0; c<ChCount; c++) {
	 TCharacter *cptr = &Characters[c];

	 if (DinoInfo[AI_to_CIndex[TargetCall] ].DangerCall)
		 if (cptr->AI<10) {
			 cptr->State=2;
			 cptr->AfraidTime = (10 + rRand(5)) * 1024; 
		 }

	 if (cptr->AI!=TargetCall) continue;
	 if (cptr->AfraidTime) continue;
	 if (cptr->State) continue;

	 float d = VectorLength(SubVectors(PlayerPos, cptr->pos));
	 if (d < ctViewR * 400) {
	  if (rRand(128) > 32)
	    if (d<dmin) { dmin = d; ai = c; }
	  cptr->tgx = PlayerX + siRand(1800);
	  cptr->tgz = PlayerZ + siRand(1800);
	 }
   }

   if (ai!=-1) {
	   answpos = SubVectors(Characters[ai].pos, PlayerPos);
       answpos.x/=-3.f; answpos.y/=-3.f; answpos.z/=-3.f;
       answpos = SubVectors(PlayerPos, answpos);
	   answtime = 2000 + rRand(2000);	   
	   answcall = TargetCall;
   }
	              
}



DWORD ColorSum(DWORD C1, DWORD C2) 
{
	DWORD R,G,B;
	R = min(255, ((C1>> 0) & 0xFF) + ((C2>> 0) & 0xFF));
	G = min(255, ((C1>> 8) & 0xFF) + ((C2>> 8) & 0xFF));
	B = min(255, ((C1>>16) & 0xFF) + ((C2>>16) & 0xFF));
	return R + (G<<8) + (B<<16);
}


#define partBlood   1
#define partWater   2
#define partGround  3
#define partBubble  4

void AddElements(float x, float y, float z, int etype, int cnt)
{
	if (ElCount > 30) {
		memcpy(&Elements[0], &Elements[1], (ElCount-1) * sizeof(TElements));
		ElCount--;
	}

	Elements[ElCount].EDone  = 0;
	Elements[ElCount].Type = etype;
	Elements[ElCount].ECount = min(30, cnt);
	int c;

	switch (etype) {
	case partBlood:		
#ifdef _d3d
		Elements[ElCount].RGBA = 0xE0600000;
	    Elements[ElCount].RGBA2= 0x20300000;	    
#else
		Elements[ElCount].RGBA = 0xE0000060;
	    Elements[ElCount].RGBA2= 0x20000030;		
#endif
		break;

	case partGround:		
#ifdef _d3d
	    Elements[ElCount].RGBA = 0xF0F09E55;
	    Elements[ElCount].RGBA2= 0x10F09E55;
#else
	    Elements[ElCount].RGBA = 0xF0559EF0;
	    Elements[ElCount].RGBA2= 0x10559EF0;
#endif
		break;


    case partBubble:		
		c = WaterList[ WMap[ (int)z / 256][ (int)x / 256] ].fogRGB;
#ifdef _d3d
		c = ColorSum( ((c & 0xFEFEFE)>>1) , 0x152020);
#else
		c = ColorSum( ((c & 0xFEFEFE)>>1) , 0x202015);
#endif
		Elements[ElCount].RGBA = 0x70000000 + (ColorSum(c, ColorSum(c,c)));
	    Elements[ElCount].RGBA2= 0x40000000 + (ColorSum(c, c));		
		break;

    case partWater:
		c = WaterList[ WMap[ (int)z / 256][ (int)x / 256] ].fogRGB;
#ifdef _d3d
		c = ColorSum( ((c & 0xFEFEFE)>>1) , 0x152020);
#else
		c = ColorSum( ((c & 0xFEFEFE)>>1) , 0x202015);
#endif			
		Elements[ElCount].RGBA  = 0xB0000000 + ( ColorSum(c, ColorSum(c,c)) );
		Elements[ElCount].RGBA2 = 0x40000000 + (c);		
		break;
	}
			
	Elements[ElCount].RGBA  = conv_xGx(Elements[ElCount].RGBA);
    Elements[ElCount].RGBA2 = conv_xGx(Elements[ElCount].RGBA2);	
		
	float al = siRand(128) / 128.f * pi / 4.f;
	float ss = f_sin(al);
	float cc = f_cos(al);

	for (int e=0; e<Elements[ElCount].ECount; e++) {
		Elements[ElCount].EList[e].pos.x = x;
		Elements[ElCount].EList[e].pos.y = y;
		Elements[ElCount].EList[e].pos.z = z;
		Elements[ElCount].EList[e].R = 6 + rRand(5);
		Elements[ElCount].EList[e].Flags = 0;
		float v;

		switch (etype) {
	       case partBlood:
				v = e * 6 + rRand(96) + 220;
				Elements[ElCount].EList[e].speed.x =ss*ca*v + siRand(32);
				Elements[ElCount].EList[e].speed.y =cc * (v * 3);
				Elements[ElCount].EList[e].speed.z =ss*sa*v + siRand(32);		
				break;
		   case partGround:
			    Elements[ElCount].EList[e].speed.x =siRand(52)-sa*64;
				Elements[ElCount].EList[e].speed.y =rRand(100) + 600 + e * 20;
				Elements[ElCount].EList[e].speed.z =siRand(52)+ca*64;
				break;
		   case partWater:
			    Elements[ElCount].EList[e].speed.x =siRand(32);
				Elements[ElCount].EList[e].speed.y =rRand(80) + 400 +  e * 40;
				Elements[ElCount].EList[e].speed.z =siRand(32);		
				break;
		   case partBubble:
			    Elements[ElCount].EList[e].speed.x =siRand(40);
				Elements[ElCount].EList[e].speed.y =rRand(140) + 20;
				Elements[ElCount].EList[e].speed.z =siRand(40);		
				break;
		}
	}

	ElCount++;
}


void MakeShot(float ax, float ay, float az,
              float bx, float by, float bz)
{
  int sres;  
  if (!WeapInfo[CurrentWeapon].Fall)
   sres = TraceShot(ax, ay, az, bx, by, bz);
  else {
	 Vector3d dl;
	 float dy = 40.f * ctViewR / 36.f;
	 dl.x = (bx-ax) / 3;
	 dl.y = (by-ay) / 3;
	 dl.z = (bz-az) / 3;
	 bx = ax + dl.x;
	 by = ay + dl.y - dy / 2;
	 bz = az + dl.z;
	 sres = TraceShot(ax, ay, az, bx, by, bz);
	 if (sres!=-1) goto ENDTRACE;
	 ax = bx; ay = by; az = bz;

	 bx = ax + dl.x;
	 by = ay + dl.y - dy * 3;
	 bz = az + dl.z;
	 sres = TraceShot(ax, ay, az, bx, by, bz);
	 if (sres!=-1) goto ENDTRACE;
	 ax = bx; ay = by; az = bz;

	 bx = ax + dl.x;
	 by = ay + dl.y - dy * 5;
	 bz = az + dl.z;
	 sres = TraceShot(ax, ay, az, bx, by, bz);
	 if (sres!=-1) goto ENDTRACE;
	 ax = bx; ay = by; az = bz;
  }

ENDTRACE:
  if (sres==-1) return;  

  int mort = (sres & 0xFF00) && (Characters[ShotDino].Health);
  sres &= 0xFF;

  if (sres == tresGround) 
	  AddElements(bx, by, bz, partGround, 6 + WeapInfo[CurrentWeapon].Power*4);
  if (sres == tresModel) 
	  AddElements(bx, by, bz, partGround, 6 + WeapInfo[CurrentWeapon].Power*4);  

    
  if (sres == tresWater) {
	  AddElements(bx, by, bz, partWater, 4 + WeapInfo[CurrentWeapon].Power*3);  
	  //AddElements(bx, GetLandH(bx, bz), bz, partBubble);  
	  AddWCircle(bx, bz, 1.2);
	  AddWCircle(bx, bz, 1.2);
  }

  
  if (sres!=tresChar) return;
  AddElements(bx, by, bz, partBlood, 4 + WeapInfo[CurrentWeapon].Power*4);  
  if (!Characters[ShotDino].Health) return;

//======= character damage =========//
  

  if (mort) Characters[ShotDino].Health = 0; 
       else Characters[ShotDino].Health-=WeapInfo[CurrentWeapon].Power;
  if (Characters[ShotDino].Health<0) Characters[ShotDino].Health=0;

    
   if (!Characters[ShotDino].Health) {	 	 
	   if (Characters[ShotDino].AI>=10) {
	   TrophyRoom.Last.success++;
	   AddShipTask(ShotDino);	   
	   } 	   

	 if (Characters[ShotDino].AI<10) 
	     Characters_AddSecondaryOne(Characters[ShotDino].CType);
     	 
	}
	else {
	 Characters[ShotDino].AfraidTime = 60*1000;
	 if (Characters[ShotDino].State==0)
        Characters[ShotDino].State = 2;	 
	 
	 Characters[ShotDino].BloodTTime+=90000;
	   
	}   
   
}


void RemoveCharacter(int index)
{
  if (index==-1) return;
  memcpy( &Characters[index], &Characters[index+1], (255 - index) * sizeof(TCharacter) );
  ChCount--;
   
  if (DemoPoint.CIndex > index) DemoPoint.CIndex--;

  for (int c=0; c<ShipTask.tcount; c++) 
	 if (ShipTask.clist[c]>index) ShipTask.clist[c]--;
}


void AnimateShip()
{
  if (Ship.State==-1) {
	  SetAmbient3d(0,0, 0,0,0);
	  if (!ShipTask.tcount) return; 
	  InitShip(ShipTask.clist[0]);
	  memcpy(&ShipTask.clist[0], &ShipTask.clist[1], 250*4);
	  ShipTask.tcount--;
	  return;
  }

  SetAmbient3d(ShipModel.SoundFX[0].length, 
	           ShipModel.SoundFX[0].lpData, 
			   Ship.pos.x, Ship.pos.y, Ship.pos.z);

  int _TimeDt = TimeDt;

//====== get up/down time acceleration ===========//
  if (Ship.FTime) {
	int am = ShipModel.Animation[0].AniTime;
	if (Ship.FTime < 500) _TimeDt = TimeDt * (Ship.FTime + 48) / 548;
    if (am-Ship.FTime < 500) _TimeDt = TimeDt * (am-Ship.FTime + 48) / 548;
	if (_TimeDt<2) _TimeDt=2;
  }
//===================================

  float L  = VectorLength( SubVectors(Ship.tgpos, Ship.pos) );  
  float L2 = sqrt ( (Ship.tgpos.x - Ship.pos.x) * (Ship.tgpos.x - Ship.pos.x) +
	                (Ship.tgpos.x - Ship.pos.x) * (Ship.tgpos.x - Ship.pos.x) );

  Ship.pos.y+=0.3f*(float)f_cos(RealTime / 256.f);

  

  Ship.tgalpha    = FindVectorAlpha(Ship.tgpos.x - Ship.pos.x, Ship.tgpos.z - Ship.pos.z);
  float currspeed;
  float dalpha = (float)fabs(Ship.tgalpha - Ship.alpha); 
  float drspd = dalpha; if (drspd>pi) drspd = 2*pi - drspd; 


//====== fly more away if I near =============//
  if (Ship.State)
   if (Ship.speed>1)
   if (L<4000)
	if (VectorLength(SubVectors(PlayerPos, Ship.pos))<(ctViewR+2)*256) {
		Ship.tgpos.x += (float)f_cos(Ship.alpha) * 256*6.f;
		Ship.tgpos.z += (float)f_sin(Ship.alpha) * 256*6.f;
		Ship.tgpos.y = GetLandUpH(Ship.tgpos.x, Ship.tgpos.z) + Ship.DeltaY;		
        Ship.tgpos.y = max(Ship.tgpos.y, GetLandUpH(Ship.pos.x, Ship.pos.z) + Ship.DeltaY);
	}
//==============================//



//========= animate down ==========//
  if (Ship.State==3) {
	Ship.FTime+=_TimeDt;
	if (Ship.FTime>=ShipModel.Animation[0].AniTime) {
	  Ship.FTime=ShipModel.Animation[0].AniTime-1;
      Ship.State=2;
	  AddVoicev(ShipModel.SoundFX[4].length,
		       ShipModel.SoundFX[4].lpData, 256);
	  AddVoice3d(ShipModel.SoundFX[1].length, ShipModel.SoundFX[1].lpData,
		        Ship.pos.x, Ship.pos.y, Ship.pos.z);   
	}
	return;
  }

  
//========= get body on board ==========//
  if (Ship.State) {
	  if (Ship.cindex!=-1) {
	    DeltaFunc(Characters[Ship.cindex].pos.y, Ship.pos.y-650 - (Ship.DeltaY-2048), _TimeDt / 3.f);
	    DeltaFunc(Characters[Ship.cindex].beta,  0, TimeDt / 4048.f);
	    DeltaFunc(Characters[Ship.cindex].gamma, 0, TimeDt / 4048.f);
	  }
	
	if (Ship.State==2) {
	  Ship.FTime-=_TimeDt;
	  if (Ship.FTime<0) Ship.FTime=0;

	  if (Ship.FTime==0)
		  if (fabs(Characters[Ship.cindex].pos.y - (Ship.pos.y-650 - (Ship.DeltaY-2048))) < 1.f) {
		      Ship.State = 1;		  	  
			  AddVoicev(ShipModel.SoundFX[5].length,
		               ShipModel.SoundFX[5].lpData, 256);
		  	  AddVoice3d(ShipModel.SoundFX[2].length, ShipModel.SoundFX[2].lpData,
		        Ship.pos.x, Ship.pos.y, Ship.pos.z);   
		  }
      return;
	}
  }
//=====================================//

  
//====== speed ===============//  
  float vspeed = 1.f + L / 128.f;
  if (vspeed > 24) vspeed = 24;
  if (Ship.State) vspeed = 24;
  if (fabs(dalpha) > 0.4) vspeed = 0.f;
  float _s = Ship.speed;
  if (vspeed>Ship.speed) DeltaFunc(Ship.speed, vspeed, TimeDt / 200.f);
                    else Ship.speed = vspeed;

  if (Ship.speed>0 && _s==0) 
	  AddVoice3d(ShipModel.SoundFX[2].length, ShipModel.SoundFX[2].lpData,
		        Ship.pos.x, Ship.pos.y, Ship.pos.z);   

//====== fly ===========//
  float l = TimeDt * Ship.speed / 16.f;   
  
  if (fabs(dalpha) < 0.4)
  if (l<L) {
   if (l>L2) l = L2 * 0.5f;
   if (L2<0.1) l = 0;
   Ship.pos.x += (float)f_cos(Ship.alpha)*l;
   Ship.pos.z += (float)f_sin(Ship.alpha)*l;
  } else {   
   if (Ship.State) {
	 Ship.State = -1;
	 RemoveCharacter(Ship.cindex);
	 return;
   } else {
	 Ship.pos = Ship.tgpos;
	 Ship.State = 3;
	 Ship.FTime = 1;
	 Ship.tgpos = Ship.retpos;
	 Ship.tgpos.y = GetLandUpH(Ship.tgpos.x, Ship.tgpos.z) + Ship.DeltaY;		
     Ship.tgpos.y = max(Ship.tgpos.y, GetLandUpH(Ship.pos.x, Ship.pos.z) + Ship.DeltaY);
	 Characters[Ship.cindex].StateF = 0xFF;	 
	 AddVoice3d(ShipModel.SoundFX[1].length, ShipModel.SoundFX[1].lpData,
		        Ship.pos.x, Ship.pos.y, Ship.pos.z);   
   }
  }

//======= y movement ============//
  float h = GetLandUpH(Ship.pos.x, Ship.pos.z);
  DeltaFunc(Ship.pos.y, Ship.tgpos.y, TimeDt / 4.f);
  if (Ship.pos.y < h + 1024) {
	  if (Ship.State)
		if (Ship.cindex!=-1) 
	      Characters[Ship.cindex].pos.y+= h + 1024 - Ship.pos.y;
	  Ship.pos.y = h + 1024;
  }
  
  

//======= rotation ============//
  
  if (Ship.tgalpha > Ship.alpha) currspeed = 0.1f + (float)fabs(drspd)/2.f;
                            else currspeed =-0.1f - (float)fabs(drspd)/2.f;     
							 
  if (fabs(dalpha) > pi) currspeed=-currspeed;
  
         
  DeltaFunc(Ship.rspeed, currspeed, (float)TimeDt / 420.f);
  
  float rspd=Ship.rspeed * TimeDt / 1024.f;
  if (fabs(drspd) < fabs(rspd)) { Ship.alpha = Ship.tgalpha; Ship.rspeed/=2; }
  else {
	Ship.alpha+=rspd;
	if (Ship.State) 
	 if (Ship.cindex!=-1)
		Characters[Ship.cindex].alpha+=rspd;
  }

  if (Ship.alpha<0) Ship.alpha+=pi*2;
  if (Ship.alpha>pi*2) Ship.alpha-=pi*2;

//======== move body ===========//
  if (Ship.State) {
	  if (Ship.cindex!=-1) {
	   Characters[Ship.cindex].pos.x = Ship.pos.x;
	   Characters[Ship.cindex].pos.z = Ship.pos.z;
	  }
	 if (L>1000) Ship.tgpos.y+=TimeDt / 12.f;
  } else {
    Ship.tgpos.x = Characters[Ship.cindex].pos.x;
	Ship.tgpos.z = Characters[Ship.cindex].pos.z;
	Ship.tgpos.y = GetLandUpH(Ship.tgpos.x, Ship.tgpos.z) + Ship.DeltaY;		
    Ship.tgpos.y = max(Ship.tgpos.y, GetLandUpH(Ship.pos.x, Ship.pos.z) + Ship.DeltaY);
  }

  
  
}



void ProcessTrophy()
{
	TrophyBody = -1;

	for (int c=0; c<ChCount; c++) {
		Vector3d p = Characters[c].pos;
		p.x+=Characters[c].lookx * 256*2.5f;
		p.z+=Characters[c].lookz * 256*2.5f;

		if (VectorLength( SubVectors(p, PlayerPos) ) < 148)
			TrophyBody = c;
	}

	if (TrophyBody==-1) return;

	TrophyBody = Characters[TrophyBody].State;
}



void RespawnSnow(int s, BOOL rand)
{
  Snow[s].pos.x = PlayerX + siRand(12*256);
  Snow[s].pos.z = PlayerZ + siRand(12*256);
  Snow[s].hl = GetLandUpH(Snow[s].pos.x, Snow[s].pos.z);
  Snow[s].ftime = 0;
  if (rand) Snow[s].pos.y = Snow[s].hl + 256+rRand(12*256);
       else Snow[s].pos.y = Snow[s].hl + (8+rRand(5))*256;
}




void AnimateElements()
{	
for (int eg=0; eg<ElCount; eg++) {	

	if  (Elements[eg].Type == partGround)	 {
		int a1 = Elements[eg].RGBA >> 24; a1-=TimeDt/4; if (a1<0) a1=0; Elements[eg].RGBA = (Elements[eg].RGBA  & 0x00FFFFFF) + (a1<<24);
		int a2 = Elements[eg].RGBA2>> 24; a2-=TimeDt/4; if (a2<0) a2=0; Elements[eg].RGBA2= (Elements[eg].RGBA2 & 0x00FFFFFF) + (a2<<24);
		if (a1 == 0 && a2==0) Elements[eg].ECount = 0;
	}

	if  (Elements[eg].Type == partWater)
	  if (Elements[eg].EDone == Elements[eg].ECount)
		 Elements[eg].ECount = 0;

	if  (Elements[eg].Type == partBubble)
	  if (Elements[eg].EDone == Elements[eg].ECount)
		 Elements[eg].ECount = 0;
   
	if  (Elements[eg].Type == partBlood)
	 if ((Takt & 3)==0)
	  if (Elements[eg].EDone == Elements[eg].ECount) {
		int a1 = Elements[eg].RGBA >> 24; a1--; if (a1<0) a1=0; Elements[eg].RGBA = (Elements[eg].RGBA  & 0x00FFFFFF) + (a1<<24);
		int a2 = Elements[eg].RGBA2>> 24; a2--; if (a2<0) a2=0; Elements[eg].RGBA2= (Elements[eg].RGBA2 & 0x00FFFFFF) + (a2<<24);
		if (a1 == 0 && a2==0) Elements[eg].ECount = 0;		
	}



//====== remove finished process =========//
	if (!Elements[eg].ECount) {
		memcpy(&Elements[eg], &Elements[eg+1], (ElCount+1-eg) * sizeof(TElements));
		ElCount--;
		eg--;
		continue;
	}


	for (int e=0; e<Elements[eg].ECount; e++) {
	   if (Elements[eg].EList[e].Flags) continue;	  		
       Elements[eg].EList[e].pos.x+=Elements[eg].EList[e].speed.x * TimeDt / 1000.f;
	   Elements[eg].EList[e].pos.y+=Elements[eg].EList[e].speed.y * TimeDt / 1000.f;
	   Elements[eg].EList[e].pos.z+=Elements[eg].EList[e].speed.z * TimeDt / 1000.f;

	   float h;	   
	   h = GetLandUpH(Elements[eg].EList[e].pos.x, Elements[eg].EList[e].pos.z);
	   BOOL OnWater = GetLandH(Elements[eg].EList[e].pos.x, Elements[eg].EList[e].pos.z) < h;

	   switch (Elements[eg].Type) {
	   case partBubble:	   
	     Elements[eg].EList[e].speed.y += 2.0 * 256 * TimeDt / 1000.f;
		 if (Elements[eg].EList[e].speed.y > 824) Elements[eg].EList[e].speed.y = 824;
		 if (Elements[eg].EList[e].pos.y > h) {		  
	      AddWCircle(Elements[eg].EList[e].pos.x, Elements[eg].EList[e].pos.z, 0.6);
		  Elements[eg].EDone++;
		  Elements[eg].EList[e].Flags = 1;		  
		  if (OnWater) Elements[eg].EList[e].pos.y-= 10240;		  
		 }
		  break;
	   
	   default:	   
		Elements[eg].EList[e].speed.y -= 9.8 * 256 * TimeDt / 1000.f;	   
	    if (Elements[eg].EList[e].pos.y < h) {		  
	      if (OnWater) AddWCircle(Elements[eg].EList[e].pos.x, Elements[eg].EList[e].pos.z, 0.6);
		  Elements[eg].EDone++;
		  Elements[eg].EList[e].Flags = 1;		  
		  if (OnWater) Elements[eg].EList[e].pos.y-= 10240;
		          else Elements[eg].EList[e].pos.y = h + 4;
		}
	      break;
		
		} //== switch ==//

		 } // for(e) //
	   } // for(eg) //

     for (int b=0; b<BloodTrail.Count; b++) {
       BloodTrail.Trail[b].LTime-=TimeDt;
	   if (BloodTrail.Trail[b].LTime<=0) {
		   memcpy(&BloodTrail.Trail[b], &BloodTrail.Trail[b+1], (511-b)*sizeof(TBloodP));
		   BloodTrail.Count--;
		   b--;
	   }
	 }

	 if (!SNOW) return;     
	 while (SnCount<2000) {
		 RespawnSnow(SnCount, TRUE);
		 SnCount++;
	 }

     nv = Wind.nv;
	 NormVector(nv, (4+Wind.speed)*4*TimeDt/1000);

	 for (int s=0; s<SnCount; s++) {
	 
	   if ( (fabs(Snow[s].pos.x - PlayerX) > 14*256) ||
		    (fabs(Snow[s].pos.z - PlayerZ) > 14*256) ) {
		   Snow[s].pos.x = PlayerX + siRand(12*256);
           Snow[s].pos.z = PlayerZ + siRand(12*256);
		   Snow[s].pos.y = Snow[s].pos.y - Snow[s].hl;
           Snow[s].hl = GetLandUpH(Snow[s].pos.x, Snow[s].pos.z);
		   Snow[s].pos.y+=Snow[s].hl;
	   }

	   if (!Snow[s].ftime) {
	     float v = ( ((RealTime + s * 23) % 800) - 400) * TimeDt / 16000;
	     Snow[s].pos.x+=ca*v;
	     Snow[s].pos.z+=sa*v;
	   
	     Snow[s].pos = AddVectors(Snow[s].pos, nv);	   
	     Snow[s].hl = GetLandUpH(Snow[s].pos.x, Snow[s].pos.z);
		 Snow[s].pos.y-=TimeDt*192/1000.f;  
		 if (Snow[s].pos.y < Snow[s].hl+8) {
			 Snow[s].pos.y = Snow[s].hl+8;
			 Snow[s].ftime = 1;
		 }
	   } else {
		   Snow[s].ftime+=TimeDt;
		   Snow[s].pos.y-=TimeDt*3/1000.f; 
		   if (Snow[s].ftime>2000)  RespawnSnow(s, FALSE);
	   }

	 }
}


void AnimateProcesses()
{
  AnimateElements();

  if ((Takt & 63)==0)  {
   float al2 = CameraAlpha + siRand(60) * pi / 180.f;
   float c2 = f_cos(al2);
   float s2 = f_sin(al2);
   float l = 1024 + rRand(3120);
   float xx = CameraX + s2 * l;
   float zz = CameraZ - c2 * l;
   if (GetLandUpH(xx,zz) > GetLandH(xx,zz)+256) 
	  AddElements(xx, GetLandH(xx,zz), zz, 4, 6 + rRand(6));
  }

  if (Takt & 1) {
   Wind.alpha+=siRand(16) / 4096.f;
   Wind.speed+=siRand(400) / 6400.f;
  }

  if (Wind.speed< 4.f) Wind.speed=4.f;
  if (Wind.speed>18.f) Wind.speed=18.f;

  Wind.nv.x = (float) f_sin(Wind.alpha);
  Wind.nv.z = (float)-f_cos(Wind.alpha);
  Wind.nv.y = 0.f;


  if (answtime) {
	  answtime-=TimeDt;
	  if (answtime<=0) {
		  answtime = 0;
		  int r = rRand(128) % 3;
		  AddVoice3d(fxCall[answcall-10][r].length,  fxCall[answcall-10][r].lpData,
		          answpos.x, answpos.y, answpos.z);				  
	  }
  }



  if (CallLockTime) {
	  CallLockTime-=TimeDt;
	  if (CallLockTime<0) CallLockTime=0;
  }

  CheckAfraid();
  AnimateShip();
  if (TrophyMode)
      ProcessTrophy();
	  
  for (int w=0; w<WCCount; w++) {
	  if (WCircles[w].scale > 1)
        WCircles[w].FTime+=(int)(TimeDt*3 / WCircles[w].scale);
	  else
		WCircles[w].FTime+=TimeDt*3;
     if (WCircles[w].FTime >= 2000) {         
       memcpy(&WCircles[w], &WCircles[w+1], sizeof(TWCircle) * (WCCount+1-w) );
       w--;
       WCCount--;
     }
  }



  if (ExitTime) {
	ExitTime-=TimeDt;
	if (ExitTime<=0) {		
        TrophyRoom.Total.time   +=TrophyRoom.Last.time;
	    TrophyRoom.Total.smade  +=TrophyRoom.Last.smade;
		TrophyRoom.Total.success+=TrophyRoom.Last.success;
		TrophyRoom.Total.path   +=TrophyRoom.Last.path;		
				  
	    if (MyHealth) SaveTrophy();
				 else LoadTrophy();				
        DoHalt("");
	}
  }
}



void RemoveCurrentTrophy()
{	
    int p = 0;
	if (!TrophyMode) return;
	if (!TrophyRoom.Body[TrophyBody].ctype) return;

	PrintLog("Trophy removed: ");
	PrintLog(DinoInfo[TrophyRoom.Body[TrophyBody].ctype].Name);
	PrintLog("\n");

	for (int c=0; c<TrophyBody; c++)
		if (TrophyRoom.Body[c].ctype) p++;


	TrophyRoom.Body[TrophyBody].ctype = 0;

	if (TrophyMode) {
	  memcpy(&Characters[p],  
	         &Characters[p+1], 
		     (250-p) * sizeof(TCharacter) );
	  ChCount--;
	}

	TrophyTime = 0;
	TrophyBody = -1;
}


void LoadTrophy()
{
    int pr = TrophyRoom.RegNumber;
	FillMemory(&TrophyRoom, sizeof(TrophyRoom), 0);
	TrophyRoom.RegNumber = pr;
	DWORD l;
	char fname[128];
	int rn = TrophyRoom.RegNumber;
	wsprintf(fname, "trophy0%d.sav", TrophyRoom.RegNumber);
	HANDLE hfile = CreateFile(fname, GENERIC_READ, FILE_SHARE_READ, NULL, OPEN_EXISTING, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile==INVALID_HANDLE_VALUE) {
		PrintLog("===> Error loading trophy!\n");
		return;
	}
	ReadFile(hfile, &TrophyRoom, sizeof(TrophyRoom), &l, NULL);

	ReadFile(hfile, &OptAgres, 4, &l, NULL);
	ReadFile(hfile, &OptDens , 4, &l, NULL);
	ReadFile(hfile, &OptSens , 4, &l, NULL);

	ReadFile(hfile, &OptRes, 4, &l, NULL);
	ReadFile(hfile, &FOGENABLE, 4, &l, NULL);
	ReadFile(hfile, &OptText , 4, &l, NULL);
	ReadFile(hfile, &OptViewR, 4, &l, NULL);
	ReadFile(hfile, &SHADOWS3D, 4, &l, NULL);
	ReadFile(hfile, &OptMsSens, 4, &l, NULL);
	ReadFile(hfile, &OptBrightness, 4, &l, NULL);


	ReadFile(hfile, &KeyMap, sizeof(KeyMap), &l, NULL);
	ReadFile(hfile, &REVERSEMS, 4, &l, NULL);

	ReadFile(hfile, &ScentMode, 4, &l, NULL);
	ReadFile(hfile, &CamoMode, 4, &l, NULL);
	ReadFile(hfile, &RadarMode, 4, &l, NULL);
	ReadFile(hfile, &Tranq    , 4, &l, NULL);
	ReadFile(hfile, &OPT_ALPHA_COLORKEY, 4, &l, NULL);

	ReadFile(hfile, &OptSys  , 4, &l, NULL);
	ReadFile(hfile, &OptSound , 4, &l, NULL);
	ReadFile(hfile, &OptRender, 4, &l, NULL);
	

	SetupRes();

	CloseHandle(hfile);	 
	TrophyRoom.RegNumber = rn;
	PrintLog("Trophy Loaded.\n");
//	TrophyRoom.Score = 299;
}




void SaveTrophy()
{
	DWORD l;
	char fname[128];
	wsprintf(fname, "trophy0%d.sav", TrophyRoom.RegNumber);

	int r = TrophyRoom.Rank;
	TrophyRoom.Rank = 0;
	if (TrophyRoom.Score >= 100) TrophyRoom.Rank = 1;
	if (TrophyRoom.Score >= 300) TrophyRoom.Rank = 2;

    
    HANDLE hfile = CreateFile(fname, GENERIC_WRITE, FILE_SHARE_READ, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);
	if (hfile == INVALID_HANDLE_VALUE) {
		PrintLog("==>> Error saving trophy!\n");
		return;
	}
	WriteFile(hfile, &TrophyRoom, sizeof(TrophyRoom), &l, NULL);

    WriteFile(hfile, &OptAgres, 4, &l, NULL);
	WriteFile(hfile, &OptDens , 4, &l, NULL);
	WriteFile(hfile, &OptSens , 4, &l, NULL);

	WriteFile(hfile, &OptRes, 4, &l, NULL);
	WriteFile(hfile, &FOGENABLE, 4, &l, NULL);
	WriteFile(hfile, &OptText , 4, &l, NULL);
	WriteFile(hfile, &OptViewR, 4, &l, NULL);
	WriteFile(hfile, &SHADOWS3D, 4, &l, NULL);
	WriteFile(hfile, &OptMsSens, 4, &l, NULL);
	WriteFile(hfile, &OptBrightness, 4, &l, NULL);

	WriteFile(hfile, &KeyMap, sizeof(KeyMap), &l, NULL);
	WriteFile(hfile, &REVERSEMS, 4, &l, NULL);	

	WriteFile(hfile, &ScentMode, 4, &l, NULL);
	WriteFile(hfile, &CamoMode , 4, &l, NULL);
	WriteFile(hfile, &RadarMode, 4, &l, NULL);
	WriteFile(hfile, &Tranq    , 4, &l, NULL);
	WriteFile(hfile, &OPT_ALPHA_COLORKEY, 4, &l, NULL);

	WriteFile(hfile, &OptSys   , 4, &l, NULL);
	WriteFile(hfile, &OptSound , 4, &l, NULL);
	WriteFile(hfile, &OptRender, 4, &l, NULL);
	CloseHandle(hfile);
	PrintLog("Trophy Saved.\n");
}

