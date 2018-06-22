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

#ifndef RTL_FLOAT_C
#define RTL_FLOAT_C
#include "main.h"
#include "rtl_mem.h"

char *PrecLabel = NULL;
int floatPrecision = 0;

void VMBSetFloatPrecision(int iPrec)
{
     if ( iPrec < 1 )
        iPrec = 1;
     if ( iPrec > 12 )
        iPrec = 12;
     floatPrecision = iPrec;
     if ( PrecLabel != NULL )
     {  
        PrecLabel = (char *)VMBFree(PrecLabel);         
     }
     PrecLabel = (char*)VMBAlloc(16);
     sprintf(PrecLabel, "%%.%if", iPrec);
}

int VMBGetFloatPrecision()
{
    return floatPrecision;    
}

char *VMBGetFloatStr(char *Float)
{
     char *newBuffer = (char *)VMBAlloc(32);
     double MyFloat = atof(Float), dTest;
     int iTest = (int)MyFloat;
     dTest = (double)iTest;
     if ( dTest == MyFloat )
     {
       sprintf(newBuffer, "%i", iTest);      
     } else {
       sprintf(newBuffer, PrecLabel, MyFloat);
     }
     return newBuffer;
}

char *VMBGetFloatFloat(double Float)
{
     char *newBuffer = (char *)VMBAlloc(32);
     double  dTest;
     int iTest = (int)Float;
     dTest = (double)iTest;
     if ( dTest == Float )
     {
       sprintf(newBuffer, "%i", iTest);      
     } else {
       sprintf(newBuffer, PrecLabel, Float);
     }
     return newBuffer;
}

#endif
 
