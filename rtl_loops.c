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

#ifndef RTL_LOOP_C
#define RTL_LOOP_C

#include "rtl_loops.h"

int LoopTypes[LOOPMAX];
int LabelLoopLevel[LOOPMAX];
int CL = 0;

void VMBLoopError()
{
     VMBPanic(56);     
     VMBExit(1);     
}

void VMBLoopsInc()
{
     if ( CL >= LOOPMAX)
     {
        VMBLoopError();
        return;
     }
     CL++;
}

void VMBLoopsDec()
{
     if ( CL <= 0)
     {
        VMBLoopError();
        return;
     }
     CL--;
}

int VMBGetCurrentLoopType()
{
    return LoopTypes[CL];
}

void VMBSetCurrentLoopType(int LT)
{
     LoopTypes[CL] = LT;
}

void VMBAddLoop(int LT)
{
     VMBLoopsInc();
     VMBSetCurrentLoopType(LT);     
}

void VMBInitLoopTypes()
{
/*     int i;
     for (i=0;i<LOOPMAX;i++)     
     {
         LoopTypes[i] = LOOPTYPE_UNKNOWN;
         LabelLoopLevel[i] = 0;
     }*/
     memset(&LoopTypes[0], 0, sizeof(LoopTypes));
     memset(&LabelLoopLevel[0], 0, sizeof(LabelLoopLevel));
     
}

#endif
