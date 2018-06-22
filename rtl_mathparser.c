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

#include <windows.h>
#include <stdio.h>
#include "./fpar/fpar.h"

/*
char cC;
char *Expr;

unsigned int iBLevel = 0;

int math_isDigit(char C)
{
    if ( C >= 48 && C <= 57 )
      return 1;
    else
      return 0;       
}

double math_getNum(char *Str)
{
      char szBuffer[32];
      double dRes = 0.0;
      int isKomma = 0;
      int iLength = 0;
      char *orig = Str;
      
      if (!math_isDigit(*Str))
      {
         return 0.0;
      }
      while ( math_isDigit(*(Str++)) == 1 )
      {
       iLength++;      
      }
           VMBDebug("%c", *Str);      
      if ( *(--Str) == '.' )
      {
            iLength++; Str++;
            while ( math_isDigit(*(Str++)) )
            {
              iLength++;      
            }            
      }
      
      if ( iLength < 32 )
      {
            ZeroMemory(&szBuffer, sizeof(szBuffer));
            strncpy((char*)&szBuffer, orig, iLength);
            dRes = (double)strtod(szBuffer, NULL);
            return dRes;
      } else {
        return 0.0;       
      }
}

int math_isOp(char C)
{
    switch (C)
    {
           case '+':
           case '-':
           case '/':
           case '*':
                return 1;
                break;
           default:
                return 0;
                break;
    }
}

int math_isPlusMinus(C)
{
    switch (C)
    {
           case '+':
           case '-':
                return 1;
                break;
           default:
                return 0;
                break;
    }    
}

int math_isMulDiv(C)
{
    switch (C)
    {
           case '*':
           case '/':
                return 1;
                break;
           default:
                return 0;
                break;
    }    
}

double VMBParseMathString(char *Str)
{
       VMBDebug("  -> %s()", __func__);
       double iResult;

       while (*Str != '\0')
       {
           if ( *Str == '(' )
           {
              Str++;
              iResult = VMBParseMathString(Str);
              if ( *Str == ')' )
              {
               Str++;
               return iResult;     
              }
           } else 
           if ( *Str == ')')
           {
               return iResult;
           } else
           if ( math_isDigit(*Str) )
           {
               iResult = math_getNum(Str);    
               VMBDebug("digit: %.10f", iResult);
           } else 
           if ( math_isOp(*Str) )
           {
                VMBDebug("op found!");
                 if ( math_isPlusMinus(*Str) )
                 {
                    if ( *Str == '-')
                    {
                       Str++;    
                       iResult = iResult - math_getNum(Str);  
                       Str++;
                    } else {
                       Str++;
                       iResult = iResult + math_getNum(Str);    
                       Str++;
                    }                     
                 }
           } else 
           {
             VMBDebug("syntax error: unknown char '%c'", *Str);
             break;       
           }
       }
       VMBDebug("return %.10f", iResult);
       return iResult;
//       VMBDebug("getNum: %f", (float)math_getNum(Str));
} */

char *SkipWhite(char *Str)
{
     if (Str == NULL)
        return NULL;        
      while (*Str == ' ' || *Str == '\t')
      { Str++;  }
      return Str;
}

void VMBMathParse(char *ParamStart, double *Result)
{
 int iLength = 0;
 int oldMem;
 node_t *n;

 ParamStart = SkipWhite(ParamStart);
 iLength = VMBStrLen(ParamStart);
 
#if VERBOSE > 0
 VMBDebug("%s() -> ParamStart = \"%s\"", __func__, ParamStart);
#endif 
 
 if ( iLength == 0 )
 {
       return;
 }

 n = fpar_parserw(ParamStart);
 if ( n != NULL)
 {
    calc_value(n);
    *Result = n->value;
    free_node_tree(n);
 } else {
    *Result = 0;
 }
 return;
}
 
