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

#ifndef RTL_GOTO_C
#define RTL_GOTO_C

#include <stdio.h>
#include "rtl_goto.h"

VMBGoto *FirstGoto = NULL;

VMBGoto *VMBGetGoto(char *GotoCode)
{
    VMBGoto *MyGoto;
    if ( FirstGoto ) 
    {
         MyGoto = FirstGoto;
         while ( MyGoto )
         {
               if ( !VMBCodeCMP(GotoCode, MyGoto->Code) )
               {
                  return MyGoto;
               }
               MyGoto = (VMBGoto*)MyGoto->nextGoto;
         }
         return NULL;
    } 
    return NULL;
}

void VMBAddGoto(char *Code, char* Addr)
{
    if ( !FirstGoto )
    {
       FirstGoto = (VMBGoto*)VMBAlloc(sizeof(VMBGoto));     
       memcpy(FirstGoto->Code, Code, 4);
       FirstGoto->iCode = Addr;
    } else {
       VMBGoto *MyGoto = FirstGoto;
       while ( MyGoto->nextGoto )
       {
             MyGoto = (VMBGoto*)MyGoto->nextGoto;
       }
       MyGoto->nextGoto = (void*)VMBAlloc(sizeof(VMBGoto));
       MyGoto = (VMBGoto*)MyGoto->nextGoto;
       memcpy(MyGoto->Code, Code, 4);
       MyGoto->iCode = Addr;
    }
}

#endif
