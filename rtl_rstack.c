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
 
#ifndef RTL_RSTACK_C
#define RTL_RSTACK_C

#include "main.h"
#include "rtl_mem.h"

#include "rtl_rstack.h"
#include "rtl_error.h"

int CRS = 0;
char *ReturnStack[MAX_RSTACK];

// -----------------------------------------------------------------------------
// VMBRStackError() - meldet einen Fehler in der RStack Engine
// -----------------------------------------------------------------------------
void VMBRStackError()
{
     VMBPanic(42);  
}

// -----------------------------------------------------------------------------
// VMBNextRStack() - springt zum nächsten ReturnStack
// -----------------------------------------------------------------------------
void VMBNextRStack()
{
     if (CRS > MAX_RSTACK)     
     {
             VMBRStackError();
     } else {
             CRS++;       
     }     
}

// -----------------------------------------------------------------------------
// VMBPrevRStack() - springt zum vorrigen ReturnStack
// -----------------------------------------------------------------------------
void VMBPrevRStack()
{
     if (CRS < 0)     
     {
             VMBRStackError();
     } else {
             CRS--;       
     }
}

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBGetRStack() - gibt den Inhalt des RStacks zurück
// -----------------------------------------------------------------------------
void VMBRStackDump()
{
     char DumpFileName[] = "rstack.txt\0";
     FILE *DumpFile;
     int i = 0, c = 0;     
          
     DumpFile = fopen(DumpFileName, "w");
     if (!DumpFile)
     {
            VMBDebug("VMBRStackDump() failed.");       
            return;              
     }
     
     fprintf(DumpFile, "RStack @ 0x%08X\r\n", &ReturnStack);
     fprintf(DumpFile, "CRS = %i\r\n", CRS);

     for (i=0;i<MAX_RSTACK;i++)
         {
               fprintf(DumpFile, "RStack[%d] @ 0x%08X = \"%s\"\r\n", i, &ReturnStack[i] , ReturnStack[i] );
         }
     
     fclose(DumpFile);
     VMBDebug("Stack wrote to %s.", DumpFileName);     
}
#endif

// -----------------------------------------------------------------------------
// VMBPushRStack(char *, char *) - schreibt etwas auf den ReturnStack
// -----------------------------------------------------------------------------
void VMBPushRStack(char *Value, int iSize)
{

#if VERBOSE > 2
     VMBDebug(" -> %s(%s, %i);", __func__ , Value, iSize);    
#endif

     VMBNextRStack();
     isRStackUsed = TRUE;
     if (ReturnStack[CRS] != NULL)
     {
        VMBFree(ReturnStack[CRS]);
     }
     ReturnStack[CRS] = (char *)VMBAlloc( iSize + 2 );        
     if ( iSize > 0 )
          memcpy(ReturnStack[CRS], Value, iSize);
}

// -----------------------------------------------------------------------------
// VMBGetRStack() - gibt den Inhalt des RStacks zurück
// -----------------------------------------------------------------------------
char *VMBGetRStack()
{
#if VERBOSE > 2
     VMBDebug(" -> %s();", __func__ );    
#endif
     if (ReturnStack[CRS] != NULL)     
     {
            char *Buffer = ReturnStack[CRS];
            VMBPrevRStack();   
            return Buffer;
     } else {
            VMBPrevRStack();
            return NULL;       
     }
}

// -----------------------------------------------------------------------------
// VMBInitRStack() - Initialisiert den ReturnStack
// -----------------------------------------------------------------------------
void VMBInitRStack()
{
     int i;
#if VERBOSE > 1
     VMBDebug(" -> %s();", __func__ );    
#endif
     for (i=0;i<MAX_RSTACK;i++)
         {
               ReturnStack[i] = NULL;
         }
     isRStackUsed = FALSE;
     CRS = 0;
}

// -----------------------------------------------------------------------------
// VMBResetRStack() - reinitialisiert den ReturnStack
// -----------------------------------------------------------------------------
void VMBResetRStack()
{
     int i;
#if VERBOSE > 1     
     VMBDebug(" -> %s();", __func__ );
#endif     
     for (i=0;i<MAX_RSTACK;i++)
         {
               if (ReturnStack[i] != NULL)
               {
                  ReturnStack[i] = (char*)VMBFree(ReturnStack[i]);                   
               }
         }
     VMBInitRStack();    
}

#endif
 
