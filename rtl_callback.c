/***************************************************************************
 *   Versatile MB 2009 - C Runtime Library                                 *
 *   Copyright (C) 2002-2010 by Michael Böhme & embyte                     *
 *   http://www.v-mb.de | webmaster@v-mb.de                                *
 *                                                                         *
 *   This program is free software; you can redistribute it and/or modify  *
 *   it under the terms of the GNU General Public License as published by  *
 *   the Free Software Foundation; either version 2 of the License, or     *
 *   (at your option) any later version.                                   *
 *                                                                         *
 *   This program is distributed in the hope that it will be useful,       *
 *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
 *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
 *   GNU General Public License for more details.                          *
 *                                                                         *
 *   You should have received a copy of the GNU General Public License     *
 *   along with this program; if not, write to the                         *
 *   Free Software Foundation, Inc.,                                       *
 *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
 ***************************************************************************/
 
#include "build_options.h"

#ifndef RTL_CALLBACK_C
#define RTL_CALLBACK_C

#include <windows.h>
/*
#define EXPORT __declspec(dllexport)

extern int VMBExCall(int Addr, int iCount, int Stack);
extern double VMBExFCall(int Addr, int iCount, int Stack);

void VMBCallBack(unsigned int param)
{
#ifdef DEBUG
     VMBDebug("Callback %08X", param);
#endif     
}

void VMBTestFunc()
{
    __asm__(" mov  $_VMBCallBack, %eax"  "\n" 
        " push $0xDEADBEEF" "\n"
        " call *%eax" "\n"
        " pop %eax");
}

void *VMBCreateCallBack(int CID)
{
   char *test;   
   void (*NewFunc)() = (void (*)())VirtualAlloc(NULL, 32, MEM_COMMIT, PAGE_EXECUTE_READWRITE);     
   
   memcpy(NewFunc, &VMBTestFunc, 32);
   test = (char*)NewFunc;
   
   test += 9;  // ggf anpassen
   
   *test++ = (char)   ( CID & 0x000000FF        );
   *test++ = (char) ( ( CID & 0x0000FF00) / 256  );
   *test++ = (char) ( ( CID & 0x00FF0000) / (256*256) );           
   *test++ = (char) ( ( CID & 0xFF000000) / (256*256*256) );
   
   return NewFunc;
}*/

/*
void VMBExternTest()
{
     char Str1[] = "Es funktioniert!\0";
     char Str2[] = "StackTest!\0";
     int i;
     
     int *Stack = (int*)VMBAlloc(256);
     
     i = 0;
     *(Stack + i ) = (int)0;
     i = 4;
     *(Stack + i ) = (int)&Str1;
     i = 8;
     *(Stack + i ) = (int)&Str2;
     i = 12;
     *(Stack + i )= (int)32;     
     
     HINSTANCE hLib = LoadLibrary("user32.dll");
     if ( hLib != NULL )
     {
          FARPROC MsgBox = GetProcAddress(hLib, "MessageBoxA");
          if ( MsgBox != NULL )
          {
             VMBExCall((int)MsgBox, 4, (int)Stack);
          }
     }     
     VMBFree(Stack);
}*/

#endif
