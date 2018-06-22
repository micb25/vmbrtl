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

#ifndef RTL_FOR_C
#define RTL_FOR_C

#include "rtl_for.h"
#include "rtl_mem.h"

VMBFor *FirstFor = NULL;
int CForC = 0;

void VMBAddFor(int forcode, char *varcode, char type, unsigned int CodePos)
{
#if VERBOSE > 0
     VMBDebug(" -> %s(); ", __func__);
#endif
     if (FirstFor == NULL)
     {
        FirstFor = (VMBFor *)VMBAlloc(sizeof(VMBFor)); 
#ifndef OPTIMIZESIZE        
        FirstFor->Code_Begin = 0;
        FirstFor->Code_Next = 0;
        FirstFor->prevFor = NULL;
        FirstFor->nextFor = NULL;
#endif        
        FirstFor->ForType = type;
        FirstFor->ForCode = forcode;
        FirstFor->Code_Begin = CodePos;
        strncpy(FirstFor->VarCode, varcode, 4);
     } else {   
        VMBFor *lastFor = FirstFor;
        while ( lastFor->nextFor != NULL )
        {
              lastFor = (VMBFor *)lastFor->nextFor;
        }            
        VMBFor *newFor = (VMBFor *)VMBAlloc(sizeof(VMBFor)); 
        newFor->ForCode = forcode;
        newFor->ForType = type;
        newFor->nextFor = NULL;
        newFor->prevFor = (int *)lastFor;
        newFor->Code_Begin = CodePos;
        lastFor->nextFor = (int *)newFor;
        strncpy(newFor->VarCode, varcode, 4);
     }
     CForC = forcode;
}

void VMBForClear()
{
#if VERBOSE > 0
     VMBDebug(" -> %s(); ", __func__);
#endif
     if (FirstFor)     
     {
        VMBFor *lastFor = FirstFor;
        while ( lastFor->nextFor != NULL )
        {
              lastFor = (VMBFor *)lastFor->nextFor;
        }  
        while ( lastFor->prevFor != NULL )
        {
              VMBFor *prev = (VMBFor*)lastFor->prevFor;
              VMBFree(lastFor);
              lastFor = (VMBFor *)prev;
        }  
        lastFor = (VMBFor *)VMBFree(lastFor);
        FirstFor = NULL;
     }
     CForC = 0;
}

VMBFor *VMBGetFor(int ForCode)
{
 if (FirstFor)       
 {
   VMBFor *ForSearch = FirstFor;
   
   while ( 1 )
   {
         if ( ForSearch->ForCode == ForCode )
         {
            return ForSearch;        
            break;     
         }
         if ( ForSearch->nextFor != NULL )
         {
           ForSearch = (VMBFor *)ForSearch->nextFor;
         } else {
           return NULL;  
           break;     
         }
   }
 } else {
   return NULL;       
 }
}

void VMBDelLastFor()
{
     if (FirstFor)     
     {
        VMBFor *CFor = FirstFor;
        while ( CFor->nextFor != NULL )                  
        {
              CFor = (VMBFor *) CFor->nextFor;
        }
        if (CFor->prevFor != NULL )
        {
           CFor = (VMBFor *)CFor->prevFor;
           VMBFree(CFor->nextFor);
           CFor->nextFor = NULL;
           CForC = CFor->ForCode;
        } else {
          if ( CFor == FirstFor)
          {
            FirstFor = (VMBFor*)VMBFree(FirstFor);
          } else {
            CFor = (VMBFor*)VMBFree(CFor);       
          }
        }
     }
}

#endif
