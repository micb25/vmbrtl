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

#ifndef RTL_REPEAT_C
#define RTL_REPEAT_C

#include "rtl_mem.h"
#include "rtl_repeat.h"

/*
typedef struct {
       char RepCode[5];
       short iRepCode;
       int iPos;
       char wasExecuted;
       char isUsed;
       
       char *prevREPEAT;
       char *nextREPEAT;
} VMBREPEAT;

*/

VMBREPEAT *FirstRep = NULL;

void VMB_InitReps()
{
     FirstRep = (VMBREPEAT*)VMBAlloc(sizeof(VMBREPEAT)+1);
}

VMBREPEAT* VMBAddRep(char *RepCode, unsigned int Pos)
{
       VMBREPEAT *LastRep = FirstRep;
       while ( LastRep->nextREPEAT != NULL )
       {
             LastRep = (VMBREPEAT*)LastRep->nextREPEAT;      
       }
       VMBREPEAT *NewRep = (VMBREPEAT*)VMBAlloc(sizeof(VMBREPEAT)+1);       
       LastRep->nextREPEAT = (char*)NewRep;      
       strncpy(NewRep->RepCode, RepCode, 4);
       NewRep->iPos = Pos;
       NewRep->iRepCode = VMBGetIntHexCode(RepCode);
       NewRep->isUsed = 0;
       NewRep->wasExecuted = 0;
       NewRep->nextREPEAT = NULL;
       NewRep->prevREPEAT = (char*)LastRep;
       return NewRep;
}

void VMBDelLastRep()
{
       VMBREPEAT *LastRep = FirstRep;
       VMBREPEAT *LastButOne;
       while ( LastRep->nextREPEAT != NULL )
       {
             LastRep = (VMBREPEAT*)LastRep->nextREPEAT;      
       }     
       LastButOne = (VMBREPEAT*)LastRep->prevREPEAT;
       LastButOne->nextREPEAT = NULL;
       LastRep = (VMBREPEAT*)VMBFree(LastRep);       
}

VMBREPEAT* VMBGetLastRep()
{
       VMBREPEAT *LastRep = FirstRep; 
       if ( LastRep == NULL )
       {
          return NULL;     
       }
       while ( LastRep->nextREPEAT != NULL )
       {
             LastRep = (VMBREPEAT*)LastRep->nextREPEAT;      
       } 
       return LastRep;      
}

VMBREPEAT* VMBGetRep(char *RepCode)
{
       VMBREPEAT *LastRep = FirstRep; 
       if ( LastRep == NULL )
       {
          return NULL;     
       }
       while ( LastRep->nextREPEAT != NULL )
       {
             if (!VMBCodeCMP(LastRep->RepCode, RepCode))
             {
                break;                            
             }
             LastRep = (VMBREPEAT*)LastRep->nextREPEAT;      
       } 
       if (VMBCodeCMP(LastRep->RepCode, RepCode) != 0)
          return 0;                           
       else
          return LastRep;      
}

void VMBRepClear()
{
       VMBREPEAT* LastRep = FirstRep, *prevREP;
       if (FirstRep == NULL)
          return;
       LastRep = (VMBREPEAT*)VMBGetLastRep();
       while ( LastRep->prevREPEAT != NULL )
       {
             LastRep = (VMBREPEAT*)LastRep->prevREPEAT;
             VMBFree(LastRep->nextREPEAT);
       }
       LastRep = (VMBREPEAT*)VMBFree(LastRep);
       FirstRep = NULL;       
       VMB_InitReps();
}

#endif

