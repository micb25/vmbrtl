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

#ifndef RTL_SUBS_C
#define RTL_SUBS_C

#include "rtl_subs.h"
#include "rtl_mem.h"

VMBSub *FirstSub = NULL;

void VMB_InitSubs()
{
     FirstSub = (VMBSub*)VMBAlloc(sizeof(VMBSub));
}

char VMBisSubDefined(char *SubCode)
{
     char Result = FALSE;
     VMBSub *SearchSub = FirstSub;
     if ( SearchSub )
     {
        if ( SearchSub->isDefined )  
        {
            if ( !VMBCodeCMP(SubCode, SearchSub->Code) ) 
            {
               return TRUE;     
            }
            do {
                if ( !SearchSub->isDefined )                
                   break;
                if ( !VMBCodeCMP(SubCode, SearchSub->Code) )
                {
                   Result = TRUE;
                   break;     
                }
                if ( SearchSub->nextSub != NULL )
                {
                  SearchSub = (VMBSub*)SearchSub->nextSub;
                } else {
                  break;       
                }
            } while ( SearchSub != NULL );
        }
     }
     return Result;
}

VMBSub* VMBGetNewSub()
{
        VMBSub *NewSub = FirstSub;
        VMBSub *TempSub = NULL;        
        if ( !NewSub->isDefined )
           return NewSub;
        while ( NewSub->nextSub != NULL )
        {
              if ( !NewSub->isDefined)      
                 return NewSub;
              NewSub = (VMBSub*)NewSub->nextSub;
        }
        TempSub = (VMBSub*)VMBAlloc(sizeof(VMBSub));
        NewSub->nextSub = TempSub;
        TempSub->prevSub = NewSub;
        return TempSub;
}

VMBSub* VMBGetSubByID(int iSubCode)
{
        VMBSub *NewSub = FirstSub;
        if ( !NewSub->isDefined )
           return NULL;
        do {
            if ( !NewSub->isDefined )
            {
               return NULL;     
            }
            if ( NewSub->iCode == iSubCode )
            {
               return NewSub;     
            }
            if ( NewSub->nextSub != NULL )
            {
               NewSub = (VMBSub*)NewSub->nextSub;
            } else {
               return NULL;       
            }
        } while ( 1 );
}

VMBSub* VMBGetSubByCode(char *SubCode)
{
        VMBSub *NewSub = FirstSub;
        if ( !NewSub->isDefined )
           return NULL;
        do {
            if ( !NewSub->isDefined )
            {
               return NULL;     
            }
            if ( !VMBCodeCMP(NewSub->Code, SubCode) )
            {
               return NewSub;     
            }
            if ( NewSub->nextSub != NULL )
            {
               NewSub = (VMBSub*)NewSub->nextSub;
            } else {
               return NULL;       
            }
        } while ( 1 );
}

#ifdef DEBUG
void VMBSubDump()
{
     FILE *DumpFile = fopen("subs.txt", "w");     
     VMBSub *MySub = FirstSub;
     int i;
     
     if ( DumpFile )
     {
        if ( MySub != NULL )  
        {
            do {
               fprintf(DumpFile, "Sub(@%08X) = {\r\n", (int)MySub);
               fprintf(DumpFile, "\tisDefined = %i\r\n", (int)MySub->isDefined);
               if ( MySub->isDefined )
               {
                   fprintf(DumpFile, "\tCode = \"%s\"\r\n", (char*)MySub->Code);                                       
                   fprintf(DumpFile, "\tiCode = %04i\r\n", MySub->iCode);                                       
                   fprintf(DumpFile, "\tisExtern = %i\r\n", (int)MySub->isExtern);     
                   fprintf(DumpFile, "\tParameters = %i\r\n", (int)MySub->Parameters);     
                   if ( MySub->Parameters > 0)
                   {
                      for ( i=0; i < MySub->Parameters; i++ )
                      {
//                          fprintf(DumpFile, "\tParamTypes[%i] = %i\r\n", i+1, (int)MySub->ParamTypes[i]);     
//                          fprintf(DumpFile, "\tParamNames[%i] = \"%s\"\r\n", i+1, MySub->ParamNames[i]);                               
//                          fprintf(DumpFile, "\tParamContent[%i] = \"%s\"\r\n", i+1, MySub->ParamContent[i]);                                                         
                      }
                   }
               }               
               fprintf(DumpFile, "\tprevSub = %08X\r\n", (int)MySub->prevSub);               
               fprintf(DumpFile, "\tnextSub = %08X\r\n", (int)MySub->nextSub);
               fprintf(DumpFile, "}\r\n\r\n");
               if ( MySub->nextSub )
               {
                 MySub = (VMBSub*)MySub->nextSub;
               } else {
                 break;       
               }
            } while ( 1 );
        }
        VMBDebug("Subs wrote to subs.txt");
        fclose(DumpFile);     
     }
     
}
        
#endif

#endif
