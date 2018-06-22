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
 
#ifndef RTL_MEM_C
#define RTL_MEM_C

#include "rtl_mem.h"

#ifdef DEBUG
long MemoryAllocated = 0;
extern char* RTL_Code;
extern char* RTL_CodeOrig;
#endif

// -----------------------------------------------------------------------------
// VMBAlloc(int) - reserviert Speicher
// -----------------------------------------------------------------------------
char *VMBAlloc(int size)
{
      char *tempBuffer = (char*)malloc(size);
      if (!tempBuffer)
       VMBCritical("Fehler beim Reservieren von Speicher!");
      ZeroMemory(tempBuffer, size);
#ifdef DEBUG
      MemoryAllocated++;
#endif      
      return tempBuffer;
}

// -----------------------------------------------------------------------------
// VMBFree(void *) - gibt den reservierten Speicher wieder frei
// -----------------------------------------------------------------------------
void *VMBFree(void *Buffer)
{
      if (Buffer != NULL)
      {   
#ifdef DEBUG                 
         MemoryAllocated--;        
         free(Buffer);
      } else {
#if VERBOSE > 1             
         VMBDebug("Freeing(NULL)!");
#endif
      }
#else
         free(Buffer);
      }
#endif
      return NULL;
}

#ifdef DEBUG
#ifndef FINAL
void VMBMemBench()
{
     int i;
     int t1 = GetTickCount();
     char *szBuffer;
     
     for ( i = 0; i < 10000000; i++ )
     {
           szBuffer = (char*)VMBAlloc(16);
           VMBFree(szBuffer);
     }
     
     VMBDebug("Time: %d", GetTickCount()-t1);
        
}
#endif
#endif

int VMBReadIntAt(int addr)
{
    int *address = (int *)addr;
    return *address;
}

char VMBReadCharAt(int addr)
{
     char *address = (char*)addr;
     return *address;
}

void VMBWriteCharTo(int addr, char ch)
{
     char *address = (char*)addr;
     *address = ch;
}

void VMBWriteIntTo(int addr, int in)
{
     int *address = (int*)addr;
     *address = in;
}

/*
int VMBCodeCMP(char *src1, char *src2)
{  
          return (int)strcmp(src1, src2);
}*/

#endif
 
