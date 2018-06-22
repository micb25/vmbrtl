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
 
#ifndef RTL_STACK_C
#define RTL_STACK_C

#include <stdio.h>
#include <stdlib.h>

#include "rtl_stack.h"
#include "rtl_error.h"
#include "rtl_mem.h"
#include "main.h"

int CS = 0;
VMBStack *FirstStack = NULL;
VMBStack *CurrentStack = NULL;

void VMBStackDump();

// -----------------------------------------------------------------------------
// VMBStackError() - meldet einen Fehler in der Stack-Engine
// -----------------------------------------------------------------------------
void VMBStackError()
{
     VMBPanic(41);
}

// -----------------------------------------------------------------------------
// VMBNextStack() - springt zum nächsten Stack
// -----------------------------------------------------------------------------
void VMBNextStack()
{
 if (CS < 256)     
 {
    CS++;
    if (CurrentStack->nextStack == NULL)
       {                                
        VMBStack *TempStack;
        TempStack = (VMBStack *)VMBAlloc(sizeof(VMBStack));
        TempStack->prevStack = (char *)CurrentStack;
        TempStack->nextStack = NULL;
        TempStack->StackElements = 0;
        TempStack->Content = (char **)VMBAlloc(64 * sizeof(char*));
        TempStack->ContentSize = (int **)VMBAlloc(64 * sizeof(int *));
        CurrentStack->nextStack = (char *)TempStack;        
       }
    CurrentStack = (VMBStack *)CurrentStack->nextStack;    
 } else {
    VMBStackError();
 }
}

// -----------------------------------------------------------------------------
// VMBPrevStack() - springt zum vorrigen Stack
// -----------------------------------------------------------------------------
void VMBPrevStack()
{
 if (CS > 0)     
 {
    CS--;
    if (CurrentStack->prevStack == NULL)
       VMBStackError();
    CurrentStack = (VMBStack *)CurrentStack->prevStack;    
 } else {
       VMBStackError();
 }
}

// -----------------------------------------------------------------------------
// VMBPushStack(char *) - Speichert einen String auf dem Stack
// -----------------------------------------------------------------------------
void VMBPushStack(char *str)
{
     int iSize = 0;
     
#if VERBOSE > 3
     VMBDebug(" -> %s(%s);", __func__ , str);
#endif       
     
     if (!CurrentStack)
        VMBStackError();
        
     if (!CurrentStack->Content)
        VMBStackError();

     if (CurrentStack->StackElements > 63 )
        VMBStackError();
        
     iSize = VMBStrLen(str)+1;
     CurrentStack->Content[CurrentStack->StackElements] = (char *)VMBAlloc(iSize);
     CurrentStack->ContentSize[CurrentStack->StackElements] = (int*)(int)iSize;
     strncpy(CurrentStack->Content[CurrentStack->StackElements], str, iSize);
     CurrentStack->StackElements++;
}

// -----------------------------------------------------------------------------
// VMBPushStack(char *) - Speichert einen String auf dem Stack
// -----------------------------------------------------------------------------
void VMBPushStackWOFree(char *str, int size)
{
     int iSize = 0;
     
#if VERBOSE > 3
     VMBDebug(" -> %s(%s);", __func__ , str);
#endif       
     
     if (!CurrentStack)
        VMBStackError();
        
     if (!CurrentStack->Content)
        VMBStackError();

     if (CurrentStack->StackElements > 63 )
        VMBStackError();
        
     CurrentStack->Content[CurrentStack->StackElements] = (char *)str;
     CurrentStack->ContentSize[CurrentStack->StackElements] = (int*)(int)size;
     CurrentStack->StackElements++;
}

// -----------------------------------------------------------------------------
// VMBPushStack2(char *, ...) - Speichert einen String auf dem Stack
// -----------------------------------------------------------------------------
void VMBPushStack2(const char * fmt, ...)
{
     char *szBuffer = (char *)VMBAlloc(2048);
     int iSize = 0;
     va_list va_args;
     
     va_start(va_args, fmt);     
     vsprintf(szBuffer, fmt, va_args);
     va_end(va_args); 
     
#if VERBOSE > 3
     VMBDebug(" -> %s(%s);", __func__ , szBuffer);
#endif       
     
     if (!CurrentStack)
        VMBStackError();
        
     if (!CurrentStack->Content)
        VMBStackError();

     if (CurrentStack->StackElements > 63 )
        VMBStackError();
        
     iSize = VMBStrLen(szBuffer)+1;
     CurrentStack->Content[CurrentStack->StackElements] = (char *)VMBAlloc(iSize);
     CurrentStack->ContentSize[CurrentStack->StackElements] = (int*)(int)iSize;
     strncpy(CurrentStack->Content[CurrentStack->StackElements], szBuffer, iSize);
     CurrentStack->StackElements++;

     VMBFree(szBuffer);
}

// -----------------------------------------------------------------------------
// VMBPushStack2(char *, ...) - Speichert einen String auf dem Stack
// -----------------------------------------------------------------------------
void VMBPushStackInt(int i)
{
     char *szBuffer = (char*)VMBAlloc(32);
     int iSize = 0;
     va_list va_args;
     
     sprintf(szBuffer, "%i", i);
     
#if VERBOSE > 3
     VMBDebug(" -> %s(%s);", __func__ , szBuffer);
#endif       
     
     if (!CurrentStack)
        VMBStackError();
        
     if (!CurrentStack->Content)
        VMBStackError();

     if (CurrentStack->StackElements > 63 )
        VMBStackError();
        
     iSize = VMBStrLen(szBuffer)+1;
     CurrentStack->Content[CurrentStack->StackElements] = (char *)szBuffer;
     CurrentStack->ContentSize[CurrentStack->StackElements] = (int*)(int)32;
//     strncpy(CurrentStack->Content[CurrentStack->StackElements], szBuffer, iSize);
     CurrentStack->StackElements++;

     //VMBFree(szBuffer);
}

// -----------------------------------------------------------------------------
// VMBClearStack() - löscht den aktuellen Stackinhalt
// -----------------------------------------------------------------------------
void VMBClearStack()
{
#if VERBOSE > 2
 VMBDebug(" -> %s();", __func__ );
#endif  
     if (CurrentStack == NULL)
        return;
     if ((unsigned)CurrentStack->StackElements > 64)
     {
        VMBStackError();
        return;
     }
     while (CurrentStack->StackElements)
     {
           CurrentStack->StackElements--;
           if ( CurrentStack->ContentSize[CurrentStack->StackElements] != 0 )
           {
              VMBFree(CurrentStack->Content[CurrentStack->StackElements]);
              //CurrentStack->ContentSize[CurrentStack->StackElements] = 0;                   
//              VMBFree(CurrentStack->ContentSize[CurrentStack->StackElements]);                   
           }
     }
}

// -----------------------------------------------------------------------------
// VMBGetStack() - liest den aktuellen Stackinhalt aus
// -----------------------------------------------------------------------------
char *VMBGetStack()
{
     if ( CurrentStack->StackElements > 1 )
     {
       int i, BufferSize = 0;
       char *Buffer;
       for (i = 0; i < CurrentStack->StackElements; i++)
           BufferSize += (int)CurrentStack->ContentSize[i];
       if ( BufferSize )
       {
              Buffer = (char *)VMBAlloc(BufferSize);
              for (i = 0; i < CurrentStack->StackElements; i++)              
              {
                    strcat(Buffer, CurrentStack->Content[i]);
              }
              return Buffer;
       } 
     } else
     if ( CurrentStack->StackElements == 1 )
     {
          char *Buffer = (char *)VMBAlloc((int)CurrentStack->ContentSize[0]);
          memcpy(Buffer, CurrentStack->Content[0], (int)CurrentStack->ContentSize[0]);
          return Buffer;        
     }
     return NULL;       
}

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBStackDump() - speichert den Inhalt des Stacks übersichtlich in einer Datei
// -----------------------------------------------------------------------------
void VMBStackDump()
{
     char DumpFileName[] = "stack.txt\0";
     FILE *DumpFile;
     int SI = CS, i = 0, c = 0;     
     VMBStack *TempStack = NULL;
     
     DumpFile = fopen(DumpFileName, "w");
     if (!DumpFile)
     {
            VMBDebug("VMBStackDump() failed.");       
            return;              
     }
     
     TempStack = FirstStack;
     while ( 1 )          
     {
           fprintf(DumpFile, "VMBStack(%d) @ 0x%08X = {\r\n   prevStack = 0x%08X\r\n   nextStack = 0x%08X\r\n   StackElements = %d\r\n", c++, TempStack, TempStack->prevStack, TempStack->nextStack, TempStack->StackElements);
           for (i = 0; i < TempStack->StackElements; i++)
           {
            fprintf(DumpFile, "   ContentSize[%d] = %d\r\n   Content[%d] = \"%s\"\r\n", i, TempStack->ContentSize[i], i, TempStack->Content[i]);
           }
           fprintf(DumpFile, "}\r\n");
           if (TempStack->nextStack == NULL)
              break;
           TempStack = (VMBStack *)TempStack->nextStack;
     }
     
     fclose(DumpFile);
     VMBDebug("Stack wrote to %s.", DumpFileName);
}
#endif

// -----------------------------------------------------------------------------
// VMBInitStack() - initialisiert den Stack
// -----------------------------------------------------------------------------
void VMBInitStack()
{
#if VERBOSE > 1     
     VMBDebug(" -> %s();", __func__ );
#endif  
     CS = 0;
     
     FirstStack = (VMBStack *)VMBAlloc(sizeof(VMBStack));
     FirstStack->Content = (char **)VMBAlloc(64 * sizeof(char*));
     FirstStack->ContentSize = (int **)VMBAlloc(64 * sizeof(int *));     
     CurrentStack = FirstStack;   
     
}

// -----------------------------------------------------------------------------
// VMBResetStack() - reinitialisiert den Stack
// -----------------------------------------------------------------------------
void VMBResetStack()
{
     VMBStack *TempStack;
#if VERBOSE > 1     
     VMBDebug(" -> %s();", __func__ );
#endif  
     TempStack = CurrentStack;
     while (TempStack->nextStack != NULL)
       TempStack = (VMBStack*)TempStack->nextStack;
     CurrentStack = TempStack;
     while ( 1 )
     {
        VMBClearStack();
        if (TempStack->prevStack == NULL)   
         break;
        TempStack = (VMBStack*)TempStack->prevStack;
        VMBFree(TempStack->nextStack);
     }  
     VMBFree(FirstStack);
     CurrentStack = FirstStack = NULL;
     VMBInitStack();
}

#endif
 
