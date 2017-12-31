void HLineTDGlass50( void )
{

   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  
	  cmp      xa16,0; 
	  jge L011 
	  

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm {       
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx
	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      
	  cmp      xb16,0; 
	  jge L021 
	  	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
	  

   } }

 L9: 
   __asm {	  
      mov      eax,edx                               
      mov      esi,ebx
      shr      eax,18
      shr      esi,18
      shl      esi,5
	  add      esi,eax	    	  

	  mov      si,word ptr[ebp + esi*2]	  
      and      esi,0x7bde
      mov      ax,word ptr [edi]
      and      eax,0x7bde
      add      eax,esi
      shr      eax,1
	  
	    add      edx,ctdx	  
	    add      ebx,ctdy	    

	  mov      word ptr[edi],ax
	  add      edi,2
           
      dec      ecx
      jnz      L9	  
	  pop ebp

   }
END_PAINT: ;

}






void HLineTDGlass25( void )
{

   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  
	  cmp      xa16,0; 
	  jge L011 
	  

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx
	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      
	  cmp      xb16,0; 
	  jge L021 
	  	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
	  

   } }

 L9: 
   __asm {	  
      mov      eax,edx                               
      mov      esi,ebx
      shr      eax,18
      shr      esi,18
      shl      esi,5
	  add      esi,eax	    	  

	  mov      ax,word ptr[ebp + esi*2]	  
      and      eax,0x739c
      shr      eax,2
	  mov      si,word ptr [edi]
      and      esi,0x739c
	  shr      esi,2
	  sub      si,word ptr [edi]
      sub      eax,esi      
	  
	    add      edx,ctdx	  
	    add      ebx,ctdy	    

	  mov      word ptr[edi],ax
	  add      edi,2
           
      dec      ecx
      jnz      L9	  
	  pop ebp

   }
END_PAINT: ;

}





void HLineTDGlass75( void )
{

   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  
	  cmp      xa16,0; 
	  jge L011 
	  

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx
	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      
	  cmp      xb16,0; 
	  jge L021 
	  	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
	  

   } }
      
 L9: 
   __asm {	  
      mov      eax,edx                               
      mov      esi,ebx
      shr      eax,18
      shr      esi,18
      shl      esi,5
	  add      esi,eax	    	  

	  mov      ax,word ptr[ebp + esi*2]	  
      and      eax,0x739c
      shr      eax,2
	  neg      ax
      add      ax,word ptr[ebp + esi*2]
	  	  
	  mov      si,word ptr [edi]
      and      esi,0x739c
	  shr      esi,2	  
      add      eax,esi
	  
	    add      edx,ctdx	  
	    add      ebx,ctdy	    

	  mov      word ptr[edi],ax
	  add      edi,2
           
      dec      ecx
      jnz      L9	  
	  pop ebp

   }
END_PAINT: ;

}











void HLineTBGlass25( void )
{

   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  
	  cmp      xa16,0; 
	  jge L011 
	  

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx
	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      
	  cmp      xb16,0; 
	  jge L021 
	  	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  push     ebp
      add      edi,esi
	  mov      ebp,lpTextureAddr

   } }

 L9: 
   __asm {	  
      mov      eax,edx                     
      mov      esi,ebx
      shr      eax,17
       and eax,63
      shr      esi,17
       and esi,63
      shl      esi,6
	  add      esi,eax	    	  

	  mov      ax,word ptr[ebp + esi*2]
      and      eax,0x739c*2
      shr      eax,3
	  mov      si,word ptr [edi]
      and      esi,0x739c
	  shr      esi,2
	  sub      si,word ptr [edi]
      sub      eax,esi      
	  
	    add      edx,ctdx	  
	    add      ebx,ctdy	    

	  mov      word ptr[edi],ax
	  add      edi,2
           
      dec      ecx
      jnz      L9	  
	  pop ebp

   }
END_PAINT: ;

}

















void HLineTxGOURAUD( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      dec edi
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      inc eax
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      
      lea      ebx,[offset DivTbl + ebx*4]      
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx

      mov      eax,ltb; 
      sub      eax,lta; 
      imul     ebx; 
      mov      cdlt,edx
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  mov      ebp,lta
	  cmp      xa16,0; 
	  jge L011 

	  mov  eax,cdlt;
	  imul xa16
	  sub  ebp,eax

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm {       
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      dec edi
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      inc eax
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx

	  mov      eax,lta; 
      sub      eax,ltb; 
      imul     ebx; 
      mov      cdlt,edx
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      mov      ebp,ltb
	  cmp      xb16,0; 
	  jge L021 
	  mov  eax,cdlt;
	  imul xb16
	  sub  ebp,eax
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr	  

   } }

   __asm sal ctdy,7
   __asm nop
   __asm shl ebx,7

L9: 
   __asm {	                    
      mov      esi,ebx      
        mov      eax,ebp	     
      and      esi,0x3F800000                
      add      esi,edx
      
      shr      esi,16
      
	  mov      ax,word ptr[esp + esi*2]
	  add      edx,ctdx      
            
	  mov      ax,word ptr[offset FadeTab + eax]	                            

	  add      ebx,ctdy

	  add      ebp,cdlt
      
	  mov      word ptr[edi], ax
      add      edi,2
      
           
      dec      ecx
      jnz      L9	     
      mov esp,_sp

   }
END_PAINT:
  __asm pop ebp  
      
    
} // HLineTxA //




void HLineTxB( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      dec      edi
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      inc      eax
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx

      mov      eax,ltb; 
      sub      eax,lta; 
      imul     ebx; 
      mov      cdlt,edx
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  mov      ebp,lta
	  cmp      xa16,0; 
	  jge L011 

	  mov  eax,cdlt;
	  imul xa16
	  sub  ebp,eax

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      dec      edi
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      inc      eax
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx

	  mov      eax,lta; 
      sub      eax,ltb; 
      imul     ebx; 
      mov      cdlt,edx
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      mov      ebp,ltb
	  cmp      xb16,0; 
	  jge L021 
	  mov  eax,cdlt;
	  imul xb16
	  sub  ebp,eax
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
	  

   } }

   __asm sal ctdy,6
   __asm nop
   __asm shl ebx,6

L9: 
   __asm {	                    
      mov      esi,ebx      
        mov      eax,ebp	     
      and      esi,0x3F800000
      add      esi,edx
      shr      esi,17
      
	  mov      ax,word ptr[esp + esi*2]
	  add      edx,ctdx      
            
	  mov      ax,word ptr[offset FadeTab + eax]	                            

	  add      ebx,ctdy

	  add      ebp,cdlt
      
      mov      word ptr[edi],ax
	  add      edi,2
           
      dec      ecx
      jnz      L9	     
      mov esp,_sp

   }
END_PAINT:
  __asm pop ebp  
      
    
} // HLineTxA //












void HLineTxC( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx

      mov      eax,ltb; 
      sub      eax,lta; 
      imul     ebx; 
      mov      cdlt,edx
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa
	  mov      ebp,lta
	  cmp      xa16,0; 
	  jge L011 

	  mov  eax,cdlt;
	  imul xa16
	  sub  ebp,eax

	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx

	  mov      eax,lta; 
      sub      eax,ltb; 
      imul     ebx; 
      mov      cdlt,edx
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb; 
      mov      ebp,ltb
	  cmp      xb16,0; 
	  jge L021 
	  mov  eax,cdlt;
	  imul xb16
	  sub  ebp,eax
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      
	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr

      } } 

      __asm {
          shl ebx,5
          nop
          sal ctdy,5      
        }
        
 L9: 
   __asm {	        
      mov      esi,ebx            
       mov      eax,ebp	     
      and      esi,0xFF800000
      add      esi,edx                   
      shr      esi,18           

	  mov      ax,word ptr[esp + esi*2]	  
	    add      edx,ctdx
	  mov      ax,word ptr[offset FadeTab + eax]	                      
	    add      ebx,ctdy
	    add      ebp,cdlt

	  mov      word ptr[edi],ax
	  add      edi,2
           
      dec      ecx
      jnz      L9	  
      mov esp,_sp

   }
END_PAINT:
  __asm pop ebp
} // HLineTxC //







void HLineTxModel( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      movsx    edi,word ptr[offset xa + 2]      
      movsx    ecx,word ptr[offset xb + 2]      
      mov      xa16,edi      
      mov      xb16,ecx

      sub      ecx,edi
      jz  END_PAINT
      
      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx
      jg       END_PAINT;                    
      cmp      xb16,edx
      jl       END_PAINT;

      mov      ebx,ecx
      cmp      ebx,1
      jle      L01      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa	  
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {      
      movsx  ecx,word ptr[offset xa + 2]      
      movsx  edi,word ptr[offset xb + 2]      
      mov      xa16,ecx
      mov      xb16,edi      
      
      sub      ecx,edi; 
      jz       END_PAINT

      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx;     
      jg       END_PAINT;  
      
      cmp      xa16,edx
      jl       END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle      L02
      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb;       
	  cmp      xb16,0; 
	  jge L021 	  
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr

   } }

   __asm {
       mov ebp, iModelFade 
       sub edi,2
       shl ctdy,8   
       shl ebx,8 
       xor eax,eax
       cmp OpacityMode,0
       jz L9

       test OpacityMode,sfTransparent
       jnz L11
       jmp L10
   }

//================ normal loop ==========================//
L9: 
   __asm {	  
      mov      esi,ebx        
          add      edi,2  
      and      esi,0xFF000000       
      add      esi,edx                   
      shr      esi,16 
	  
      mov      ax,word ptr[esp + esi*2]	        
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax

        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L9	  
      mov esp,_sp
      jmp END_PAINT
   }


//================ opacity loop ==========================//      
L10:
      __asm {	  
      mov      esi,ebx        
          add      edi,2  
      and      esi,0xFF000000       
      add      esi,edx                   
      shr      esi,16 
	  
      mov      ax,word ptr[esp + esi*2]	        
      cmp      eax,0
      jz       LSKIP
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax
      } LSKIP: __asm {
        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L10
      mov esp,_sp      
      jmp END_PAINT
   }


//================ opacity loop ==========================//      
L11:
      __asm {	  
      mov      esi,ebx        
          add      edi,2  
      and      esi,0xFF000000       
      add      esi,edx                   
      shr      esi,16 
	  
      mov      ax,word ptr[esp + esi*2]	        
      cmp      eax,0
      jz       LSKIP2
	  mov      ax,word ptr[ebp + eax]
      mov      si,word ptr[edi]
      and      eax,0x7bde
      and      esi,0x7bde
      add      eax,esi
      shr      eax,1
      mov      word ptr[edi], ax
      } LSKIP2: __asm {
        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L11
      mov esp,_sp         
   }


END_PAINT:
  __asm pop ebp
} // HLineTxModel //





void HLineTxModel2( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      movsx    edi,word ptr[offset xa + 2]      
      movsx    ecx,word ptr[offset xb + 2]      
      mov      xa16,edi      
      mov      xb16,ecx

      sub      ecx,edi
      jz  END_PAINT
      
      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx
      jg       END_PAINT;                    
      cmp      xb16,edx
      jl       END_PAINT;

      mov      ebx,ecx
      cmp      ebx,1
      jle      L01      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa	  
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {      
      movsx  ecx,word ptr[offset xa + 2]      
      movsx  edi,word ptr[offset xb + 2]      
      mov      xa16,ecx
      mov      xb16,edi      
      
      sub      ecx,edi; 
      jz       END_PAINT

      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx;     
      jg       END_PAINT;  
      
      cmp      xa16,edx
      jl       END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle      L02
      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb;       
	  cmp      xb16,0; 
	  jge L021 	  
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr

   } }

   __asm {
       mov ebp, iModelFade 
       sub edi,2
       shl ctdy,7
       shl ebx,7
       xor eax,eax
       cmp OpacityMode,0
       jnz L10
   }

//================ normal loop ==========================//
L9: 
   __asm {	  
      mov      esi,ebx        
          add      edi,2  
      and      esi,0xFF000000       
      add      esi,edx                   
      shr      esi,17
	  
      mov      ax,word ptr[esp + esi*2]	        
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax

        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L9	  
      mov esp,_sp
      jmp END_PAINT
   }


//================ opacity loop ==========================//
L10:
      __asm {	  
      mov      esi,ebx        
          add      edi,2  
      and      esi,0xFF000000       
      add      esi,edx                   
      shr      esi,17
	  
      mov      ax,word ptr[esp + esi*2]	        
      cmp      eax,0
      jz       LSKIP
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax
      } LSKIP: __asm {
        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L10
      mov esp,_sp      
   }

END_PAINT:
  __asm pop ebp
} // HLineTxModel //







void HLineTxModel3( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      movsx    edi,word ptr[offset xa + 2]      
      movsx    ecx,word ptr[offset xb + 2]      
      mov      xa16,edi      
      mov      xb16,ecx

      sub      ecx,edi
      jz  END_PAINT
      
      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx
      jg       END_PAINT;                    
      cmp      xb16,edx
      jl       END_PAINT;

      mov      ebx,ecx
      cmp      ebx,1
      jle      L01      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa	  
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {      
      movsx  ecx,word ptr[offset xa + 2]      
      movsx  edi,word ptr[offset xb + 2]      
      mov      xa16,ecx
      mov      xb16,edi      
      
      sub      ecx,edi; 
      jz       END_PAINT

      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx;     
      jg       END_PAINT;  
      
      cmp      xa16,edx
      jl       END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle      L02
      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb;       
	  cmp      xb16,0; 
	  jge L021 	  
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr

   } }

   __asm {
       mov ebp, iModelFade 
       sub edi,2
       shl ctdy,6
       shl ebx,6
       xor eax,eax
       cmp OpacityMode,0
       jnz L10
   }

//================ normal loop ==========================//
L9: 
   __asm {	  
      mov      esi,ebx
          add      edi,2
      and      esi,0xFF000000
      add      esi,edx
      shr      esi,18
	  
      mov      ax,word ptr[esp + esi*2]
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax

        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L9	  
      mov esp,_sp
      jmp END_PAINT
   }


//================ opacity loop ==========================//
L10:
      __asm {	  
      mov      esi,ebx        
          add      edi,2  
      and      esi,0xFF000000       
      add      esi,edx                   
      shr      esi,18
	  
      mov      ax,word ptr[esp + esi*2]	        
      cmp      eax,0
      jz       LSKIP
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax
      } LSKIP: __asm {
        add      edx,ctdx
	    add      ebx,ctdy
	                
      dec      ecx
      jnz      L10
      mov esp,_sp      
   }

END_PAINT:
  __asm pop ebp
} // HLineTxModel //





void HLineTxModelBMP( void )
{   
   _asm {
	  push ebp   
      movsx    edi,word ptr[offset xa + 2]
      movsx    ecx,word ptr[offset xb + 2]
      mov      xa16,edi
      mov      xb16,ecx

      sub      ecx,edi
      jz  END_PAINT
      
      mov      ebx,WinEX
      xor      edx,edx
      cmp      edi,ebx
      jg       END_PAINT;                    
      cmp      xb16,edx
      jl       END_PAINT;

      mov      ebx,ecx
      cmp      ebx,1
      jle      L01      
      mov      ebx,[offset DivTbl + ebx*4]
      mov      eax,128*256*256      
      imul     ebx
      mov      ctdx,edx
      
}L01:     __asm {
      
      mov      ebx,0
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdx;
	  imul  xa16
	  sub   ebx,eax	  

	  add ecx,xa16
	  jz END_PAINT
	  mov xa16,0    

} L011: 	  __asm {

	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT

} L012:      __asm { 

      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
   
       mov ebp, iModelFade 
       sub edi,2       
       //shl ebx,7
       xor eax,eax      
	   mov edx, ctdx
	  }
//================ opacity loop ==========================//
L10:
      __asm {	  
      mov      esi,ebx        
         add      edi,2        
      shr      esi,16
	  
      mov      ax,word ptr[esp + esi*2]	        
      cmp      eax,0
      jz       LSKIP
	  mov      ax,word ptr[ebp + eax]
      mov      word ptr[edi], ax
      } LSKIP: __asm {        
	    add      ebx,edx
	                
      dec      ecx
      jnz      L10
      mov esp,_sp      
   }

END_PAINT:
  __asm pop ebp
} 








void HLineTxModel25( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01
      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa	  
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb;       
	  cmp      xb16,0; 
	  jge L021 	  
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
   } }

   __asm mov ebp, iModelFade 
   __asm sub edi,2

L9: 
   __asm {	  
      mov      eax,edx                               
      mov      esi,ebx
      shr      eax,18
      and      esi,0xFFFC0000      
      shr      esi,12 
      add      edi,2
	  add      esi,eax	    	    
	  mov      ax,word ptr[esp + esi*2]
      cmp      eax,0
      je L10	    

      
      and      eax,0x739c*2
      shr      eax,3
	  mov      si,word ptr [edi]
      and      esi,0x739c
	  shr      esi,2
	  sub      si,word ptr [edi]
      sub      eax,esi            
	  
      mov      word ptr[edi], ax

   } L10: __asm {        
        add      edx,ctdx
	    add      ebx,ctdy
	     
           
      dec      ecx
      jnz      L9	  
      mov esp,_sp

   }
END_PAINT:
  __asm pop ebp
} // HLineTxModel //






void HLineTxModel50( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa	  
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb;       
	  cmp      xb16,0; 
	  jge L021 	  
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr

   } }

   __asm mov ebp, iModelFade 
   __asm sub edi,2

L9: 
   __asm {	  
      mov      eax,edx                               
      mov      esi,ebx
      shr      eax,18
      and      esi,0xFFFC0000      
      shr      esi,12
      add      edi,2
	  add      esi,eax	    	    
	  mov      ax,word ptr[esp + esi*2]	  
      cmp      eax,0
      je L10	    
      
      
      and      eax,0x7bde*2
      mov      si,word ptr [edi]
      shr      eax,1
      and      esi,0x7bde
      add      eax,esi
      shr      eax,1
	  
      mov      word ptr[edi], ax

   } L10: __asm {        
        add      edx,ctdx
	    add      ebx,ctdy
	     
           
      dec      ecx
      jnz      L9	  
      mov esp,_sp

   }
END_PAINT:
  __asm pop ebp
} // HLineTxModel //







void HLineTxModel75( void )
{
   __asm push ebp
   if ( xa < xb ) {
   _asm {
      mov      edi,xa
      sar         edi, 16    
      mov      xa16,edi
      mov      eax,xb; 
      sar         eax, 16
      mov      xb16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX; 
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xb16,ebx;  
      jl          END_PAINT;

      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L01

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txb; 
      sub      eax,txa; 
      imul     ebx; 
      mov      ctdx,edx

      mov      eax,tyb; 
      sub      eax,tya; 
      imul     ebx; 
      mov      ctdy,edx
      
}L01:     __asm {
      mov      ebx,tya
      mov      edx,txa	  
	  cmp      xa16,0; 
	  jge L011 
	  
	  mov   eax,ctdy;
	  imul  xa16
	  sub   ebx,eax

	  mov   eax,ctdx
	  imul  xa16
	  mov   edx,txa;
	  sub   edx,eax

	  add ecx,xa16
	  jz END_PAINT
	  mov      xa16,0    
} L011: 	  __asm {
	  mov eax,WinW
	  cmp xb16,eax
	  jl L012
      sub xb16,eax
	  sub ecx,xb16
      jz END_PAINT
} L012:  
	  	  
	  __asm { 
      mov      edi, Y1      
        mov    esi,xa16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr
           
     
   } }  else  { 
   __asm {
      mov      edi,xb
      sar         edi, 16    
      mov      xb16,edi
      mov      eax,xa; 
      sar         eax, 16
      mov      xa16,eax
      sub      eax,edi; 
      jz  END_PAINT
      mov      ecx,eax

      mov      ebx,WinEX;
      cmp      edi,ebx;     
      jg          END_PAINT;  
      mov      ebx,0; 
      cmp      xa16,ebx;  
      jl          END_PAINT;
      
      mov      ebx,ecx; 
      cmp      ebx,1; 
      jle         L02

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx,[ebx]
      mov      eax,txa; 
      sub      eax,txb; 
      imul     ebx; 
      mov      ctdx,edx
      
	  mov      eax,tya; 
      sub      eax,tyb; 
      imul     ebx; 
      mov      ctdy,edx	  
} L02:    __asm {
	  mov      ebx,tyb;
      mov      edx,txb;       
	  cmp      xb16,0; 
	  jge L021 	  
	  
	  mov  eax,ctdy;
	  imul xb16
	  sub  ebx,eax

	  mov  eax,ctdx
	  imul xb16
	  mov  edx,txb;
	  sub  edx,eax

	  add ecx,xb16
	  jz END_PAINT
	  mov      xb16,0  
}L021: 	  __asm{
	  mov eax,WinW
	  cmp xa16,eax
	  jl L022
      sub xa16,eax
	  sub ecx,xa16
      jz END_PAINT
}L022: 	  

      

	  __asm{
      mov      edi, Y1      
        mov    esi,xb16
      shl      edi, 11
        lea    esi,[esi*2]      
      add      edi, lpVideoBuf

	  mov      _sp,esp
      add      edi,esi
	  mov      esp,lpTextureAddr

   } }

   __asm mov ebp, iModelFade 
   __asm sub edi,2

L9: 
   __asm {	  
      mov      eax,edx                               
      mov      esi,ebx
      shr      eax,18
      and      esi,0xFFFC0000      
      shr      esi,12
      add      edi,2
	  add      esi,eax	    	    
	  mov      ax,word ptr[esp + esi*2]
      cmp      eax,0
      je L10	    

      
      and      eax,(0x739c*2)
      shr      eax,2
	  neg      eax
      add      ax,word ptr[esp + esi*2]
      shr      eax,1
	  	  
	  mov      si,word ptr [edi]
      and      esi,0x739c
	  shr      esi,2	  
      add      eax,esi           
	  
      mov      word ptr[edi], ax

   } L10: __asm {        
        add      edx,ctdx
	    add      ebx,ctdy
	     
           
      dec      ecx
      jnz      L9	  
      mov esp,_sp

   }
END_PAINT:
  __asm pop ebp
} // HLineTxModel //
























void DrawTexturedFace()
{   	
     __asm {            
      inc      dFacesCount
      mov      ebx, dword ptr[offset scrp + 4]
      mov      ecx, dword ptr[offset scrp + 4 + 32]
      mov      edx, dword ptr[offset scrp + 4 + 64]
     
      cmp      ebx,ecx
      jge L20   // == if BX<CX then ==}
	            // === if ebx<edx then v1:=0 else v1:=2;  ====}
      cmp      ebx,edx
      jge      L11
      mov      v1,0 
      jmp      L12 }

L11:  v1 = 2; 
L12: 
                //==  if ecx>=edx then v3:=1 else v3:=2;  ====}
   __asm {
      cmp      ecx,edx
      jl       L13
      mov      v3,1
      jmp      L14        }

L13:  v3 = 2;
L14:  
   __asm {
	  jmp      L30 }
L20:
               //=============================================================}
               //====  if ecx<edx then v1:=1 else v1:=2;  ====}
   __asm {
      cmp      ecx,edx
      jge      L21
      mov      v1,1 
      jmp      L22        }

L21:  v1 = 2;
L22:
        //====  if ebx>=edx then v3:=0 else v3:=2;  ====}
   __asm {
      cmp      ebx,edx
      jl       L23  
      mov      v3,0 
      jmp      L24       }

L23:  v3 = 2;
L24:
//==== End find v1,v3 ==================================================}


//        ;{=== find v2 ====}
L30:      

   __asm {
      mov      eax,0
      cmp      eax,v1
      je          L31
      cmp      eax,v3
      je          L31
      jmp      L39  }      
L31: 
   __asm {
      inc         eax
      cmp      eax,v1
      je          L32
      cmp      eax,v3
      je          L32
      jmp      L39  }      
L32:  
   __asm {  
	  inc      eax }
L39:      
   __asm { 
	  mov      v2,eax }

   __asm {
//================== fill x,y,tx,ty ====================//
      mov      ebx,v1
	  shl      ebx,5
	  mov      eax,dword ptr[offset scrp + ebx]
      mov      x1,eax
      mov      eax,dword ptr[offset scrp + 4 + ebx]
      mov      Y1,eax
      mov      eax,dword ptr[offset scrp + 8 + ebx]	  
      mov      tx1,eax      
      mov      eax,dword ptr[offset scrp + 12 + ebx]      
      mov      ty1,eax
      mov      eax,dword ptr[offset scrp + 16 + ebx]      
	   shl eax,16
	   add eax,8000h
      mov      lt1,eax
           

      mov      ebx,v2
	  shl      ebx,5
	  mov      eax,dword ptr[offset scrp + ebx]
      mov      x2,eax;
      mov      eax,dword ptr[offset scrp + 4 + ebx]
      mov      y2,eax
      mov      eax,dword ptr[offset scrp + 8 + ebx]	  
      mov      tx2,eax      
      mov      eax,dword ptr[offset scrp + 12 + ebx]      
      mov      ty2,eax
	  mov      eax,dword ptr[offset scrp + 16 + ebx]      
	   shl eax,16
	   add eax,8000h
      mov      lt2,eax
            
      
      mov      ebx,v3
	  shl      ebx,5
	  mov      eax,dword ptr[offset scrp + ebx]
      mov      x3,eax;
      mov      eax,dword ptr[offset scrp + 4 + ebx]
      mov      y3,eax			
      mov      eax,dword ptr[offset scrp + 8 + ebx]	  
      mov      tx3,eax      
      mov      eax,dword ptr[offset scrp + 12 + ebx]      
      mov      ty3,eax
	  mov      eax,dword ptr[offset scrp + 16 + ebx]      
	   shl eax,16
	   add eax,8000h
      mov      lt3,eax
      
      
      

//======================================== calc l1,l2,l3 =============//
      mov eax,y2
      sub eax,Y1
      mov l1,eax
      mov eax,y3
      sub eax,y2
      mov l2,eax
      mov eax,y3
      sub eax,Y1
      mov l3,eax

//======================================== calc dx,dtx,dty ===========}
//============================================= 1->2 =================}
      mov eax,tx1
      mov txa,eax
      mov txb,eax
      mov eax,ty1
      mov tya,eax
      mov tyb,eax
	  mov eax,lt1
      mov lta,eax
	  mov ltb,eax

      mov eax,x1
      shl eax,16
      add eax,8000h
      mov xa,eax
      mov xb,eax

      mov ebx,l1   
      cmp ebx,0   
      je  L101

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx, [ebx]

	  mov  eax,lt2
      sub  eax,lt1
      imul ebx
      mov  dlt1,edx

      mov eax,tx2
      sub eax,tx1
      imul ebx
      mov dtx1,edx

      mov eax,ty2
      sub eax,ty1
      imul ebx
      mov dty1,edx
      
      mov eax,x2
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx1,edx
   }

//;{============================================= 2->3 ===========}
L101:
   __asm {
      mov ebx,l2
      cmp ebx,0
      je L102

      lea      ebx,[offset DivTbl + ebx*4]
      mov ebx,[ebx]
      
	  mov  eax,lt3
      sub  eax,lt2
      imul ebx
      mov  dlt2,edx
	  
	  mov  eax,tx3
      sub  eax,tx2
      imul ebx
      mov  dtx2,edx

      mov  eax,ty3
      sub  eax,ty2
      imul ebx
      mov  dty2,edx

      mov eax,x3
      sub eax,x2
      sal eax,16
      imul ebx
      mov dx2,edx
   }

//;{============================================= 1->3 ===========}

L102:   
   __asm {
      mov ebx,l3
      cmp ebx,0
      je L103

      lea      ebx,[offset DivTbl + ebx*4]
      mov ebx,[ebx]

	  mov  eax,lt3
      sub  eax,lt1
      imul ebx
      mov  dlt3,edx
      
	  mov  eax,tx3
      sub  eax,tx1
      imul ebx
      mov  dtx3,edx

      mov  eax,ty3
      sub  eax,ty1
      imul ebx
      mov  dty3,edx

      mov eax,x3
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx3,edx
  }

L103:

L201:    
   _asm {
      mov eax,Y1
      cmp eax,y2
      jge L205
           
      cmp eax,0
      jl L203
      cmp eax,WinEY
      jg L400
      call HLineT
   }
L203:
   __asm {
	  mov eax,dlt3
      add lta,eax
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax
      mov eax,dx3
      add xa,eax
      
;{====}
	  mov eax,dlt1
      add ltb,eax
      mov eax,dtx1
      add txb,eax
      mov eax,dty1
      add tyb,eax
      mov eax,dx1
      add xb,eax
      inc Y1
      jmp L201
   }

//=============== 2->3 =================}
L205:
   __asm {
	  mov eax,lt2
      mov ltb,eax
      mov eax,tx2
      mov txb,eax
      mov eax,ty2
      mov tyb,eax
      mov eax,x2
      sal eax,16
      add eax,8000h
      mov xb,eax
     }

L301:    
   __asm {
      mov eax,Y1
      cmp eax,y3
      jge L400
      cmp eax,0
      jl L303
      cmp eax,WinEY
      jg L400
      call HLineT  }
L303:
   __asm {
	  mov eax,dlt3
      add lta,eax
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax
      mov eax,dx3
      add xa,eax

	  mov eax,dlt2
      add ltb,eax
      mov eax,dtx2
      add txb,eax
      mov eax,dty2
      add tyb,eax
      mov eax,dx2
      add xb,eax
      
      inc Y1;
      jmp L301
   } 

L400: ;
}







void DrawModelFace()
{   	
     __asm {            
      inc      dFacesCount
      mov      ebx, dword ptr[offset mscrp + 4]
      mov      ecx, dword ptr[offset mscrp + 4 + 16]
      mov      edx, dword ptr[offset mscrp + 4 + 32]
     
      cmp      ebx,ecx
      jge L20   // == if BX<CX then ==}
	            // === if ebx<edx then v1:=0 else v1:=2;  ====}
      cmp      ebx,edx
      jge      L11
      mov      v1,0 
      jmp      L12 }

L11:  v1 = 2; 
L12: 
                //==  if ecx>=edx then v3:=1 else v3:=2;  ====}
   __asm {
      cmp      ecx,edx
      jl       L13
      mov      v3,1
      jmp      L14        }

L13:  v3 = 2;
L14:  
   __asm {
	  jmp      L30 }
L20:
               //=============================================================}
               //====  if ecx<edx then v1:=1 else v1:=2;  ====}
   __asm {
      cmp      ecx,edx
      jge      L21
      mov      v1,1 
      jmp      L22        }

L21:  v1 = 2;
L22:
        //====  if ebx>=edx then v3:=0 else v3:=2;  ====}
   __asm {
      cmp      ebx,edx
      jl       L23  
      mov      v3,0 
      jmp      L24       }

L23:  v3 = 2;
L24:
//==== End find v1,v3 ==================================================}


//        ;{=== find v2 ====}
L30:      

   __asm {
      mov      eax,0
      cmp      eax,v1
      je          L31
      cmp      eax,v3
      je          L31
      jmp      L39  }      
L31: 
   __asm {
      inc         eax
      cmp      eax,v1
      je          L32
      cmp      eax,v3
      je          L32
      jmp      L39  }      
L32:  
   __asm {  
	  inc      eax }
L39:      
   __asm { 
	  mov      v2,eax }

   __asm {
//================== fill x,y,tx,ty ====================//      
      mov      ebx,v1
	  shl      ebx,4
	  mov      eax,dword ptr[offset mscrp + ebx]
      mov      x1,eax
      mov      eax,dword ptr[offset mscrp + 4 + ebx]
      mov      Y1,eax
      mov      eax,dword ptr[offset mscrp + 8 + ebx]	  
      mov      tx1,eax      
      mov      eax,dword ptr[offset mscrp + 12 + ebx]      
      mov      ty1,eax
                      
      mov      ebx,v2
	  shl      ebx,4
	  mov      eax,dword ptr[offset mscrp + ebx]
      mov      x2,eax;
      mov      eax,dword ptr[offset mscrp + 4 + ebx]
      mov      y2,eax
      mov      eax,dword ptr[offset mscrp + 8 + ebx]	  
      mov      tx2,eax      
      mov      eax,dword ptr[offset mscrp + 12 + ebx]      
      mov      ty2,eax
	  
      mov      ebx,v3
	  shl      ebx,4
	  mov      eax,dword ptr[offset mscrp + ebx]
      mov      x3,eax;
      mov      eax,dword ptr[offset mscrp + 4 + ebx]      
      mov      y3,eax			
      mov      eax,dword ptr[offset mscrp + 8 + ebx]	  
      mov      tx3,eax      
      mov      eax,dword ptr[offset mscrp + 12 + ebx]      
      mov      ty3,eax                                    
	       
      
      cmp y3,0
      jl L400
      
      

//======================================== calc l1,l2,l3 =============//
      mov eax,y2
      sub eax,Y1
      mov l1,eax
      mov eax,y3
      sub eax,y2
      mov l2,eax
      mov eax,y3
      sub eax,Y1
      mov l3,eax

//======================================== calc dx,dtx,dty ===========}
//============================================= 1->2 =================}
      mov eax,tx1
      mov txa,eax
      mov txb,eax
      mov eax,ty1
      mov tya,eax
      mov tyb,eax
	  

      mov eax,x1
      shl eax,16
      add eax,8000h
      mov xa,eax
      mov xb,eax

      mov ebx,l1   
      cmp ebx,0   
      je  L101

      lea      ebx,[offset DivTbl + ebx*4]
      mov      ebx, [ebx]	  

      mov eax,tx2
      sub eax,tx1
      imul ebx
      mov dtx1,edx

      mov eax,ty2
      sub eax,ty1
      imul ebx
      mov dty1,edx
      
      mov eax,x2
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx1,edx
   }

//;{============================================= 2->3 ===========}
L101:
   __asm {
      mov ebx,l2
      cmp ebx,0
      je L102

      lea      ebx,[offset DivTbl + ebx*4]
      mov ebx,[ebx]
      
	  	  
	  mov  eax,tx3
      sub  eax,tx2
      imul ebx
      mov  dtx2,edx

      mov  eax,ty3
      sub  eax,ty2
      imul ebx
      mov  dty2,edx

      mov eax,x3
      sub eax,x2
      sal eax,16
      imul ebx
      mov dx2,edx
   }

//;{============================================= 1->3 ===========}

L102:   
   __asm {
      mov ebx,l3
      cmp ebx,0
      je L103

      lea      ebx,[offset DivTbl + ebx*4]
      mov ebx,[ebx]

	        
	  mov  eax,tx3
      sub  eax,tx1
      imul ebx
      mov  dtx3,edx

      mov  eax,ty3
      sub  eax,ty1
      imul ebx
      mov  dty3,edx

      mov eax,x3
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx3,edx
  }

L103:

L201:    
   _asm {
      mov eax,Y1
      cmp eax,y2
      jge L205
           
      cmp eax,0
      jl L203
      cmp eax,WinEY
      jg L400
      call HLineT
   }
L203:
   __asm {	  
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax
      mov eax,dx3
      add xa,eax
      
;{====}	  
      mov eax,dtx1
      add txb,eax
      mov eax,dty1
      add tyb,eax
      mov eax,dx1
      add xb,eax
      inc Y1
      jmp L201
   }

//=============== 2->3 =================}
L205:
   __asm {	  
      mov eax,tx2
      mov txb,eax
      mov eax,ty2
      mov tyb,eax
      mov eax,x2
      sal eax,16
      add eax,8000h
      mov xb,eax
     }

L301:    
   __asm {
      mov eax,Y1
      cmp eax,y3
      jge L400
      cmp eax,0
      jl L303
      cmp eax,WinEY
      jg L400
      call HLineT  }
L303:
   __asm {	  
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax
      mov eax,dx3
      add xa,eax
	  
      mov eax,dtx2
      add txb,eax
      mov eax,dty2
      add tyb,eax
      mov eax,dx2
      add xb,eax
      
      inc Y1;
      jmp L301
   } 

L400: ;
}





void DrawModelFaces()
{        

LBEGIN:     
     __asm {            
      inc      dFacesCount
      
      mov      edi, Current
      shl      edi, 6
      add      edi, mptr
      add      edi, 0x4010
      mov      eax, [edi+0x2C]
      mov      Current, eax
      
      //mov      ebx, iModelBaseFade
      mov      eax, [edi + 0x24]
      /*test     eax, sfDark
      jz       DSKIP
      add      ebx, 12*256*256
     } DSKIP: __asm {*/
      and      eax, (0x0004+0x0008)
      mov      OpacityMode,eax
      //mov      iModelFade,ebx



    
      mov      eax, [edi+0]      
      mov      ebx, dword ptr[offset gScrp + 4 + eax*8]

      mov      eax, [edi+4]      
      mov      ecx, [offset gScrp + 4 + eax*8]

      mov      eax, [edi+8]      
      mov      edx, [offset gScrp + 4 + eax*8]
     
      cmp      ebx,ecx
      jge L20   // == if BX<CX then ==}
	            // === if ebx<edx then v1:=0 else v1:=2;  ====}
      cmp      ebx,edx
      jge      L11
      mov      v1,0 
      jmp      L12 }

L11:  v1 = 2; 
L12: 
                //==  if ecx>=edx then v3:=1 else v3:=2;  ====}
   __asm {
      cmp      ecx,edx
      jl       L13
      mov      v3,1
      jmp      L14        }

L13:  v3 = 2;
L14:  
   __asm {
	  jmp      L30 }
L20:
               //=============================================================}
               //====  if ecx<edx then v1:=1 else v1:=2;  ====}
   __asm {
      cmp      ecx,edx
      jge      L21
      mov      v1,1 
      jmp      L22        }

L21:  v1 = 2;
L22:
        //====  if ebx>=edx then v3:=0 else v3:=2;  ====}
   __asm {
      cmp      ebx,edx
      jl       L23  
      mov      v3,0 
      jmp      L24       }

L23:  v3 = 2;
L24:
//==== End find v1,v3 ==================================================}


//        ;{=== find v2 ====}
L30:      

   __asm {
      mov      eax,0
      cmp      eax,v1
      je          L31
      cmp      eax,v3
      je          L31
      jmp      L39  }      
L31: 
   __asm {
      inc         eax
      cmp      eax,v1
      je          L32
      cmp      eax,v3
      je          L32
      jmp      L39  }      
L32:  
   __asm {  
	  inc      eax }
L39:      
   __asm { 
	  mov      v2,eax }

   __asm {
//================== fill x,y,tx,ty ====================//      

      mov      ebx,v1

      mov      eax, [edi + ebx*4 + 12]
      mov      tx1, eax
      mov      eax, [edi + ebx*4 + 12+12]
      mov      ty1, eax

      mov      ebx,[edi + ebx*4]
	  shl      ebx,3

	  mov      eax,dword ptr[offset gScrp + ebx]
      mov      x1,eax
      mov      eax,dword ptr[offset gScrp + 4 + ebx]
      mov      Y1,eax 
      

      
      mov      ebx,v2

      mov      eax, [edi + ebx*4 + 12]
      mov      tx2, eax
      mov      eax, [edi + ebx*4 + 12+12]
      mov      ty2, eax

      mov      ebx,[edi + ebx*4]
	  shl      ebx,3
	  mov      eax,dword ptr[offset gScrp + ebx]
      mov      x2,eax
      mov      eax,dword ptr[offset gScrp + 4 + ebx]
      mov      y2,eax

      
      mov      ebx,v3

      mov      eax, [edi + ebx*4 + 12]
      mov      tx3, eax
      mov      eax, [edi + ebx*4 + 12+12]
      mov      ty3, eax

      mov      ebx,[edi + ebx*4]
	  shl      ebx,3
	  mov      eax,dword ptr[offset gScrp + ebx]
      mov      x3,eax
      mov      eax,dword ptr[offset gScrp + 4 + ebx]
      mov      y3,eax

      
      cmp y3,0
      jl L400
      
      

//======================================== calc l1,l2,l3 =============//
      mov eax,y2
      sub eax,Y1
      mov l1,eax
      mov eax,y3
      sub eax,y2
      mov l2,eax
      mov eax,y3
      sub eax,Y1
      mov l3,eax

//======================================== calc dx,dtx,dty ===========}
//============================================= 1->2 =================}
      mov eax,tx1
      mov txa,eax
      mov txb,eax
      mov eax,ty1
      mov tya,eax
      mov tyb,eax
	  

      mov eax,x1
      shl eax,16
      add eax,8000h
      mov xa,eax
      mov xb,eax

      mov ebx,l1   
      cmp ebx,0   
      jle  L101      
      mov      ebx, [offset DivTbl + ebx*4]	  

      mov eax,tx2
      sub eax,tx1
      imul ebx
      mov dtx1,edx

      mov eax,ty2
      sub eax,ty1
      imul ebx
      mov dty1,edx
      
      mov eax,x2
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx1,edx
   }

//;{============================================= 2->3 ===========}
L101:
   __asm {
      mov ebx,l2
      cmp ebx,0
      jle L102
      mov      ebx, [offset DivTbl + ebx*4]	  
      
	  	  
	  mov  eax,tx3
      sub  eax,tx2
      imul ebx
      mov  dtx2,edx

      mov  eax,ty3
      sub  eax,ty2
      imul ebx
      mov  dty2,edx

      mov eax,x3
      sub eax,x2
      sal eax,16
      imul ebx
      mov dx2,edx
   }

//;{============================================= 1->3 ===========}

L102:   
   __asm {
      mov ebx,l3
      cmp ebx,0
      jle L103      
      mov      ebx, [offset DivTbl + ebx*4]	  

	        
	  mov  eax,tx3
      sub  eax,tx1
      imul ebx
      mov  dtx3,edx

      mov  eax,ty3
      sub  eax,ty1
      imul ebx
      mov  dty3,edx

      mov eax,x3
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx3,edx
  }

L103:

L201:    
   _asm {
      mov eax,Y1
      cmp eax,y2
      jge L205
           
      cmp eax,0
      jl L203
      cmp eax,WinEY
      jg L400
      call HLineT
   }
L203:
   __asm {	  
      mov eax,dtx3
      add txa,eax      
      mov eax,dty3
      add tya,eax      
      mov eax,dx3
      add xa,eax
      
;{====}	  
      mov eax,dtx1
      add txb,eax
      mov eax,dty1
      add tyb,eax
      mov eax,dx1
      add xb,eax
      inc Y1
      jmp L201
   }

//=============== 2->3 =================}
L205:
   __asm {	  
      mov eax,tx2
      mov txb,eax
      mov eax,ty2
      mov tyb,eax
      mov eax,x2
      sal eax,16
      add eax,8000h
      mov xb,eax
     }

L301:    
   __asm {
      mov eax,Y1
      cmp eax,y3
      jge L400
      cmp eax,0
      jl L303
      cmp eax,WinEY
      jg L400
      call HLineT  }
L303:
   __asm {	  
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax
      mov eax,dx3
      add xa,eax
	  
      mov eax,dtx2
      add txb,eax
      mov eax,dty2
      add tyb,eax
      mov eax,dx2
      add xb,eax
      
      inc Y1;
      jmp L301
   } 

L400: 
   
   if (Current!=-1) goto LBEGIN;

}
































void RenderSkyLine(int y)
{   
  __asm {
	shl ctdx,8
    shl ctdy,8
    mov ebx,txa
	shl ebx,8
    mov edx,tya
	shl edx,8

	mov ecx,WinW		
	mov edi,y
	shl edi,11
	add edi,lpVideoBuf	

	push ebp
	mov _sp,esp
	mov ebp,ctdx
	mov esp,ctdy

} L0: __asm {
	mov eax,ebx
	mov esi,edx
	shr eax,24
	shr esi,24
	shl esi,8
	add esi,eax
	mov ax,word ptr[offset SkyPic + esi*2]	 
	mov      word ptr[edi],ax
	add      edi,2	
	
	add ebx, ebp
	add edx, esp		

	dec cx
	jnz L0
  	mov esp,_sp
	pop ebp 
	} 
}


void RenderSkyLineLo(int y)
{   
  __asm {
	shl ctdx,9
    shl ctdy,9
    mov ebx,txa
	shl ebx,8
    mov edx,tya
	shl edx,8

	mov ecx,WinW	
	shr ecx,1
	mov edi,y
	shl edi,11
	add edi,lpVideoBuf	

	push ebp
	mov _sp,esp
	mov ebp,ctdx
	mov esp,ctdy

} L0: __asm {
	mov eax,ebx
	mov esi,edx
	shr eax,24
	shr esi,24
	shl esi,8
	add esi,eax
	mov ax,word ptr[offset SkyPic + esi*2]	 
	mov      word ptr[edi],ax	
	mov      word ptr[edi+2],ax
	add      edi,4
	
	add ebx, ebp
	add edx, esp		

	dec cx
	jnz L0
  	mov esp,_sp
	pop ebp 
	} 
}




void RenderSkyLineFadeLo(int y, int FadeLevel)
{   
  __asm {
	shl ctdx,9
    shl ctdy,9
    mov ebx,txa
	shl ebx,8
    mov edx,tya
	shl edx,8

	mov ecx,WinW	
	shr ecx,1
	mov edi,y
	shl edi,11
	add edi,lpVideoBuf	

    mov eax,FadeLevel
    shl eax,15
    add eax,offset SkyFade

	push ebp
	mov _sp,esp
	mov ebp,ctdx
	mov esp,eax

} L0: __asm {
	mov eax,ebx
	mov esi,edx
	shr eax,25
	shr esi,25
	shl esi,7
	add esi,eax
	mov ax,word ptr[esp + esi*2]	 
	mov      word ptr[edi],ax
    mov      word ptr[edi+2],ax
	add      edi,4	  
	
	add ebx, ebp
	add edx, ctdy

	dec cx
	jnz L0
  	mov esp,_sp
	pop ebp 
	} 
}




void RenderSkyLineFade(int y, int FadeLevel)
{   
  __asm {
	shl ctdx,8
    shl ctdy,8
    mov ebx,txa
	shl ebx,8
    mov edx,tya
	shl edx,8

	mov ecx,WinW		
	mov edi,y
	shl edi,11
	add edi,lpVideoBuf	

    mov eax,FadeLevel
    shl eax,15
    add eax,offset SkyFade

	push ebp
	mov _sp,esp
	mov ebp,ctdx
	mov esp,eax

} L0: __asm {
	mov eax,ebx
	mov esi,edx
	shr eax,25
	shr esi,25
	shl esi,7
	add esi,eax
	mov ax,word ptr[esp + esi*2]	 
	mov      word ptr[edi],ax
	add      edi,2		
	
	add ebx, ebp
	add edx, ctdy

	dec cx
	jnz L0
  	mov esp,_sp
	pop ebp 
	} 
}






void DrawCorrectedTexturedFace()
{   	
     __asm {            
      inc      dFacesCount
      mov      ebx, dword ptr[offset scrp + 4]
      mov      ecx, dword ptr[offset scrp + 4 + 32]
      mov      edx, dword ptr[offset scrp + 4 + 64]
     
      cmp      ebx,ecx
      jge L20   // == if BX<CX then ==}
	            // === if ebx<edx then v1:=0 else v1:=2;  ====}
      cmp      ebx,edx
      jge      L11
      mov      v1,0 
      jmp      L12 }

L11:  v1 = 2; 
L12: 
                //==  if ecx>=edx then v3:=1 else v3:=2;  ====}
   __asm {
      cmp      ecx,edx
      jl       L13
      mov      v3,1
      jmp      L14        }

L13:  v3 = 2;
L14:  
   __asm {
	  jmp      L30 }
L20:
               //=============================================================}
               //====  if ecx<edx then v1:=1 else v1:=2;  ====}
   __asm {
      cmp      ecx,edx
      jge      L21
      mov      v1,1 
      jmp      L22        }

L21:  v1 = 2;
L22:
        //====  if ebx>=edx then v3:=0 else v3:=2;  ====}
   __asm {
      cmp      ebx,edx
      jl       L23  
      mov      v3,0 
      jmp      L24       }

L23:  v3 = 2;
L24:
//==== End find v1,v3 ==================================================}


//        ;{=== find v2 ====}
L30:      

   __asm {
      mov      eax,0
      cmp      eax,v1
      je          L31
      cmp      eax,v3
      je          L31
      jmp      L39  }      
L31: 
   __asm {
      inc         eax
      cmp      eax,v1
      je          L32
      cmp      eax,v3
      je          L32
      jmp      L39  }      
L32:  
   __asm {  
	  inc      eax }
L39:      
   __asm { 
	  mov      v2,eax }

   __asm {
//================== fill x,y,tx,ty ====================//
      mov      ebx,v1
	  shl      ebx,5
	  mov      eax,dword ptr[offset scrp + ebx]
      mov      x1,eax
      mov      eax,dword ptr[offset scrp + 4 + ebx]
      mov      Y1,eax
      mov      eax,dword ptr[offset scrp + 8 + ebx]	  
      mov      tx1,eax      
      mov      eax,dword ptr[offset scrp + 12 + ebx]      
      mov      ty1,eax
      mov      eax,dword ptr[offset scrp + 16 + ebx]      
	   shl eax,16
	   add eax,8000h
      mov      lt1,eax
      mov      eax,dword ptr[offset scrp + 20 + ebx]      
      mov      zdepth1,eax
      

      mov      ebx,v2
	  shl      ebx,5
	  mov      eax,dword ptr[offset scrp + ebx]
      mov      x2,eax;
      mov      eax,dword ptr[offset scrp + 4 + ebx]
      mov      y2,eax
      mov      eax,dword ptr[offset scrp + 8 + ebx]	  
      mov      tx2,eax      
      mov      eax,dword ptr[offset scrp + 12 + ebx]      
      mov      ty2,eax
	  mov      eax,dword ptr[offset scrp + 16 + ebx]      
	   shl eax,16
	   add eax,8000h
      mov      lt2,eax
      mov      eax,dword ptr[offset scrp + 20 + ebx]      
      mov      zdepth2,eax
            
      
      mov      ebx,v3
	  shl      ebx,5
	  mov      eax,dword ptr[offset scrp + ebx]
      mov      x3,eax;
      mov      eax,dword ptr[offset scrp + 4 + ebx]      
      mov      y3,eax			
      mov      eax,dword ptr[offset scrp + 8 + ebx]	  
      mov      tx3,eax      
      mov      eax,dword ptr[offset scrp + 12 + ebx]      
      mov      ty3,eax
	  mov      eax,dword ptr[offset scrp + 16 + ebx]      
	   shl eax,16
	   add eax,8000h
      mov      lt3,eax
      mov      eax,dword ptr[offset scrp + 20 + ebx]      
      mov      zdepth3,eax
      
           
//======================================== calc l1,l2,l3 =============//
      mov eax,y2
      sub eax,Y1
      mov l1,eax
      mov eax,y3
      sub eax,y2
      mov l2,eax
      mov eax,y3
      sub eax,Y1
      mov l3,eax

//======================================== calc dx,dtx,dty ===========}
//============================================= 1->2 =================}
      mov eax,tx1
      mov txa,eax
      mov txb,eax
      mov eax,ty1
      mov tya,eax
      mov tyb,eax
	  mov eax,lt1
      mov lta,eax
	  mov ltb,eax

      mov eax,x1
      shl eax,16
      add eax,8000h
      mov xa,eax
      mov xb,eax

      mov ebx,l1   
      cmp ebx,0   
      je  L101
      shl ebx,2    
      add      ebx,offset DivTbl
      mov      ebx, [ebx]

	  mov  eax,lt2
      sub  eax,lt1
      imul ebx
      mov  dlt1,edx

      mov eax,tx2
      sub eax,tx1
      imul ebx
      mov dtx1,edx

      mov eax,ty2
      sub eax,ty1
      imul ebx
      mov dty1,edx
      
      mov eax,x2
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx1,edx
   }
     k = (float)(zdepth2 - zdepth1) / (float)(zdepth1 + zdepth2);
     
     k = k*Soft_Persp_K;
     if (k>0.99f) k=0.99f;
     if (k<-0.99f) k= -0.99f;

     ddtx1 = (int)(dtx1 * 2 * k / l1);
     ddty1 = (int)(dty1 * 2 * k / l1);

     dtx1  = (int)(dtx1 * (1-k) * 0.995);     
     dty1  = (int)(dty1 * (1-k) * 0.995);

//;{============================================= 2->3 ===========}
L101:
   __asm {
      mov ebx,l2
      cmp ebx,0
      je L102
      shl ebx,2
      add      ebx, offset DivTbl
      mov ebx,[ebx]
      
	  mov  eax,lt3
      sub  eax,lt2
      imul ebx
      mov  dlt2,edx
	  
	  mov  eax,tx3
      sub  eax,tx2
      imul ebx
      mov  dtx2,edx

      mov  eax,ty3
      sub  eax,ty2
      imul ebx
      mov  dty2,edx

      mov eax,x3
      sub eax,x2
      sal eax,16
      imul ebx
      mov dx2,edx
   }
     
     k = (float)(zdepth3 - zdepth2) / (float)(zdepth2 + zdepth3 );
     
     k = k*Soft_Persp_K;
     if (k>0.99f) k=0.99f;
     if (k<-0.99f) k= -0.99f;

     ddtx2 = (int)(dtx2 * 2 * k / l2);
     ddty2 = (int)(dty2 * 2 * k / l2);
     dtx2  = (int)(dtx2 * (1-k) * 0.995);     
     dty2  = (int)(dty2 * (1-k) * 0.995);
//;{============================================= 1->3 ===========}

L102:   
   __asm {
      mov ebx,l3
      cmp ebx,0
      je L103
      shl ebx,2
      add      ebx, offset DivTbl
      mov ebx,[ebx]

	  mov  eax,lt3
      sub  eax,lt1
      imul ebx
      mov  dlt3,edx
      
	  mov  eax,tx3
      sub  eax,tx1
      imul ebx
      mov  dtx3,edx

      mov  eax,ty3
      sub  eax,ty1
      imul ebx
      mov  dty3,edx

      mov eax,x3
      sub eax,x1
      sal eax,16
      imul ebx
      mov dx3,edx
  }
     k = (float)(zdepth3 - zdepth1) / (float)(zdepth1 + zdepth3 );          
     k = k*Soft_Persp_K;
     if (k>0.99f) k=0.99f;
     if (k<-0.99f) k= -0.99f;

     ddtx3 = (int)(dtx3 * 2 * k / l3);
     ddty3 = (int)(dty3 * 2 * k / l3);
     dtx3  = (int)(dtx3 * (1-k) * 0.995);     
     dty3  = (int)(dty3 * (1-k) * 0.995);

L103:

L201:    
   _asm {
      mov eax,Y1
      cmp eax,y2
      jge L205
           
      cmp eax,0
      jl L203
      cmp eax,WinEY
      jg L400
      call HLineT
   }
L203:
   __asm {
	  mov eax,dlt3
      add lta,eax
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax

      mov eax,ddtx3
      add dtx3,eax
      mov eax,ddty3
      add dty3,eax

      mov eax,dx3
      add xa,eax
      
;{====}
	  mov eax,dlt1
      add ltb,eax
      mov eax,dtx1
      add txb,eax
      mov eax,dty1
      add tyb,eax

      mov eax,ddtx1
      add dtx1,eax
      mov eax,ddty1
      add dty1,eax

      mov eax,dx1
      add xb,eax
      inc Y1
      jmp L201
   }
     
     

//=============== 2->3 =================}
L205:
   __asm {
	  mov eax,lt2
      mov ltb,eax
      mov eax,tx2
      mov txb,eax
      mov eax,ty2
      mov tyb,eax
      mov eax,x2
      sal eax,16
      add eax,8000h
      mov xb,eax
     }

L301:    
   __asm {
      mov eax,Y1
      cmp eax,y3
      jge L400
      cmp eax,0
      jl L303
      cmp eax,WinEY
      jg L400
      call HLineT  }
L303:
   __asm {
	  mov eax,dlt3
      add lta,eax
      mov eax,dtx3
      add txa,eax
      mov eax,dty3
      add tya,eax

      mov eax,ddtx3
      add dtx3,eax
      mov eax,ddty3
      add dty3,eax

      mov eax,dx3
      add xa,eax

	  mov eax,dlt2
      add ltb,eax
      mov eax,dtx2
      add txb,eax
      mov eax,dty2
      add tyb,eax

      mov eax,ddtx2
      add dtx2,eax
      mov eax,ddty2
      add dty2,eax

      mov eax,dx2
      add xb,eax
      
      inc Y1;
      jmp L301
   } 

L400: ;
}
