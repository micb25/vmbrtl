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

#ifndef RTL_IF_C
#define RTL_IF_C

#include <stdio.h>
#include "rtl_if.h"
#include "rtl_subs.h"
#include "rtl_mem.h"

VMBIF *FirstIf = NULL;

extern VMBSub* CurrentSub;
extern int SubLevel;

void VMB_InitIfs()
{
     FirstIf = (VMBIF*)VMBAlloc(sizeof(VMBIF));
}

VMBIF* VMBGetIf(char *IfCode)
{
       VMBIF *LastIF = FirstIf; 
       if ( LastIF == NULL )
       {
          return NULL;     
       }
       while ( LastIF->nextIF != NULL )
       {
             if (!VMBCodeCMP(LastIF->IfCode, IfCode) && LastIF->SubLevel == SubLevel )
             {
                break;                            
             }
             if ( LastIF->nextIF != NULL )
             {
                  LastIF = (VMBIF*)LastIF->nextIF;      
             } else {
                  break;         
             }
       } 
       if (VMBCodeCMP(LastIF->IfCode, IfCode) != 0 || LastIF->SubLevel != SubLevel )
          return 0;                           
       else
          return LastIF;      
}

VMBIF* VMBGetLastIf()
{
       VMBIF *LastIF = FirstIf; 
       if ( !LastIF )
          return NULL;     
          
       while ( LastIF->nextIF != NULL )
             LastIF = (VMBIF*)LastIF->nextIF;      
             
       return LastIF;      
}

#ifdef DEBUG
void VMBIfDump();
#endif

VMBIF* VMBAddIf(char *IfCode, unsigned int Pos)
{
       VMBIF *LastIF = FirstIf;
       VMBIF *TempIF = NULL;
       VMBIF *NewIF = NULL;
       char szBuffer[1024];
       
       if ( !FirstIf )
       {
          VMB_InitIfs();   
          LastIF = FirstIf;  
       }  
       
       TempIF = (VMBIF*)VMBGetIf(IfCode);
       if ( TempIF != NULL )
          return TempIF;            
        
       LastIF = (VMBIF*)VMBGetLastIf();
       if ( LastIF == NULL )
       {
          VMBIF *NewIF = (VMBIF*)VMBAlloc(sizeof(VMBIF));   
          FirstIf = LastIF = NewIF;
       }
       
       if ( !strcmp(LastIF->IfCode, "") )
       {
          NewIF = LastIF;     
          NewIF->prevIF = NULL;
          NewIF->nextIF = NULL;
       } else {
              
          NewIF = (VMBIF*)VMBAlloc(sizeof(VMBIF));              
          LastIF->nextIF = (char*)NewIF;      
          NewIF->prevIF = (char*)LastIF;
       }

       strncpy(NewIF->IfCode, IfCode, 4);
       NewIF->iPos = Pos;
       NewIF->iIfCode = VMBGetIntHexCode(IfCode);
       NewIF->isUsed = 0;
       NewIF->SubLevel = SubLevel;
       NewIF->wasExecuted = 0;
       NewIF->nextIF = NULL;

       if ( CurrentSub == NULL )
       {
          NewIF->isLocal = FALSE;
          NewIF->LocalSub = NULL;    
       } else {
          NewIF->isLocal = TRUE;
          NewIF->LocalSub = (void*)CurrentSub;
       }
       
       return NewIF;
}

void VMBDelIf(char *IfCode)
{
       VMBIF *LastIF = NULL;
//       VMBIF *LastButOne;
       
       char isFound = 0;
/*
       while ( LastIF->nextIF != NULL )
       {
             LastIF = (VMBIF*)LastIF->nextIF;      
       }
       if ( LastIF->prevIF != NULL )     
       {
           LastButOne = (VMBIF*)LastIF->prevIF;
           LastButOne->nextIF = NULL;
           LastIF = (VMBIF*)VMBFree(LastIF);       
       }
*/

       
       LastIF = VMBGetLastIf();
       if ( !LastIF )
          return;
          
       while ( LastIF != NULL )   
       {
             if ( !VMBCodeCMP(LastIF->IfCode, IfCode) && LastIF->SubLevel >= SubLevel )
             {
                VMBIF *prevIF = (VMBIF*)LastIF->prevIF;
                if ( prevIF == NULL )
                {
                     FirstIf = NULL;
                     LastIF = (VMBIF*)VMBFree(LastIF);
                     break;
                } else {
                     LastIF = (VMBIF*)VMBFree(LastIF);
                     LastIF = (VMBIF*)prevIF;
                     LastIF->nextIF = NULL;
                     continue;
                }
             }

             if ( LastIF->prevIF != NULL && LastIF->prevIF != (void*)LastIF )
             {
                    LastIF = (VMBIF*)LastIF->prevIF;
             } else {
                    break;       
             }
       }

/*
       if ( LastIF != NULL )
       {
           while ( LastIF != NULL )
           {
                 if ( LastIF->IfCode != NULL )
                 {
                    if (!strcmp(LastIF->IfCode, IfCode))     
                    {
                       isFound = 1;                         
                       break;                            
                    }
                 }  
                 if ( LastIF->nextIF != NULL )
                 {
                    LastIF = (VMBIF*)LastIF->nextIF;    
                 } else {
                    break;       
                 }
                 if ( LastIF->nextIF == LastIF->prevIF )
                 {
                    VMBCritical(666);
                 }
                 
           }
           if ( isFound == 1 )
           {
              LastButOne = (VMBIF*)LastIF->prevIF;
              VMBFree(LastIF);
              if ( LastButOne )
                 LastButOne->nextIF = NULL;
           }
       }
*/
}



void VMBIfClear()
{
       VMBIF* LastIF = FirstIf, *prevIF;
       if (FirstIf == NULL)
          return;
       LastIF = VMBGetLastIf();
       while ( LastIF->prevIF != NULL )
       {
             LastIF = (VMBIF*)LastIF->prevIF;
             VMBFree(LastIF->nextIF);
       }
       LastIF = (VMBIF*)VMBFree(LastIF);
       FirstIf = NULL;       
       VMB_InitIfs();
}

void VMBClearLocalIfs()
{
#if VERBOSE > 0
       VMBDebug(" -> %s();", __func__);
#endif     
       VMBIF* LastIF = FirstIf, *prevIF;
       char DelLast = 0;
       if (FirstIf == NULL)
          return;

  // Fehler: Versuch von nicht reservierten Speicher zu lesen!
       LastIF = VMBGetLastIf();
       if ( LastIF == NULL )
          return;
          
       while ( LastIF->prevIF != NULL )      
       {
               if ( LastIF->isLocal && LastIF->SubLevel >= SubLevel && LastIF != NULL )      
               {
                    LastIF = (VMBIF*)LastIF->prevIF;
                    LastIF->nextIF = (void*)VMBFree(LastIF->nextIF);
               }
               if ( LastIF->prevIF != NULL )
               {
                    LastIF = (VMBIF*)LastIF->prevIF;     
               } else {
                    break;         
               }
       }
       
       if ( LastIF->isLocal && LastIF->SubLevel == SubLevel )
       {
//          VMBDebug("asdf");     
       }

/*
      
       while ( LastIF != NULL )
       {
             VMBDebug("LOCAL: %i    SubLevel %d", (int)LastIF->isLocal, (int)LastIF->SubLevel);
             if ( LastIF->isLocal && LastIF->SubLevel >= SubLevel )
             {
                prevIF = (VMBIF*)LastIF->prevIF;
                prevIF->nextIF = LastIF->nextIF;
                DelLast = 1;
             }
             LastIF = (VMBIF*)LastIF->nextIF;
             if ( DelLast )
             {
                DelLast = 0;
                if ( LastIF )
                {
                    if ( LastIF->prevIF != NULL )
                    {
                        VMBFree(LastIF->prevIF);     
                    }
                } else {
                           
                }
             }
       }
       if ( LastIF != NULL )
       {
           if ( LastIF->isLocal && LastIF->SubLevel >= SubLevel )
           {
              VMBFree( LastIF );     
           }
       }    */ 
}

#ifdef DEBUG
void VMBIfDump()
{
 FILE *DumpFile;
 VMBIF* LastIF = FirstIf; 
 int i = 0;
 DumpFile = fopen("ifs.txt", "w");
 if (!DumpFile)
 {
  VMBDebug("VMBIfDump() failed.");
  return;              
 }      
 if ( LastIF == NULL )
 {
    fprintf(DumpFile, "FirstIF = NULL!");     
 } else {
    while ( 1 )    
    {
          i++;
          fprintf(DumpFile, "If[%i] @0x%08x = {\r\n", i, LastIF);      
          if ( LastIF != NULL )
          {
             fprintf(DumpFile, "\tIfCode = \"%s\"\r\n", LastIF->IfCode);     
             fprintf(DumpFile, "\twasExecuted = %i\r\n", (int)LastIF->wasExecuted);                  
             fprintf(DumpFile, "\tisLocal = %i\r\n", (int)LastIF->isLocal);                  
             fprintf(DumpFile, "\tSubLevel = %i\r\n", (int)LastIF->SubLevel);                               
             fprintf(DumpFile, "\r\n\tprevIF = 0x%08X\r\n", (int)LastIF->prevIF);                               
             fprintf(DumpFile, "\tnextIF = 0x%08X\r\n", (int)LastIF->nextIF);                                            
          }
          fprintf(DumpFile, "}\r\n");
          if ( LastIF == NULL )
             break;
          else
             LastIF = (VMBIF*)LastIF->nextIF;
    }
 }
 fclose(DumpFile);
}
#endif

#endif
