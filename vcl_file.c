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

#ifdef VCL

#ifndef VCL_FILE_C
#define VCL_FILE_C
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

// -----------------------------------------------------------------------------
//  VCL_FileMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_FileMethod(VMBVar *VCLCom, unsigned int Method)
{
     unsigned int iTemp;
     char *szBuffer;
     VCLFileProperties *Prop = (VCLFileProperties*)VCLCom->Properties;     
     switch(Method) 
        {
                       case 1:
                            if ( Prop->FileName )
                            {
                               iTemp = (int)CreateFile(Prop->FileName, 0, 0, NULL, CREATE_ALWAYS, FILE_ATTRIBUTE_NORMAL, NULL);     
                               if ( iTemp != (int)INVALID_HANDLE_VALUE )
                               {
                                       CloseHandle((HANDLE)iTemp);
                               }
                            }
                            break;
                       case 2:
                            if ( Prop->FileName )
                            {
                               if ( Prop->File )
                               {
                                  if ( Prop->Opened )
                                  {
                                     Prop->Opened = 0;
                                     fclose(Prop->File);     
                                  }     
                                  Prop->File = NULL;
                               }     
                               DeleteFile(Prop->FileName);
                            }
                       case 5:
                            if ( Prop->File )
                            {
                              if ( Prop->Opened )   
                              {
                                szBuffer = (char*)VMBAlloc(2049);   
                                fgets(szBuffer, 2048, Prop->File);   
                                int iLength = VMBStrLen(szBuffer);
                                if ( iLength > 2 )
                                {
                                    if ( * ( szBuffer + iLength - 1 ) == '\n' )
                                      * ( szBuffer + iLength - 1 ) = '\0';
                                    if ( * ( szBuffer + iLength - 2 ) == '\r' )
                                      * ( szBuffer + iLength - 2 ) = '\0';                                  
                                }
                                VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
                                szBuffer = (char*)VMBFree(szBuffer);
                              } else {
                                VMBPushRStack("", 0);
                              }
                            } else {
                              VMBPushRStack("", 0);
                            }
                            break;
                       case 6:
                            szBuffer = (char*)VMBParseOneString();  
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {                                  
                                  fputs(szBuffer, Prop->File);     
                               }
                            }
                            szBuffer = (char*)VMBFree(szBuffer);
                            VMBPrevCode();                            
                            break;
                       case 7:
                            szBuffer = (char*)VMBParseOneString();  
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {                                  
                                  fputs(szBuffer, Prop->File);     
                                  fputs("\n", Prop->File);                                       
                               }
                            }
                            szBuffer = (char*)VMBFree(szBuffer);
                            VMBPrevCode();                            
                            break;
                       case 8:
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {
                                  int *iT = (int*)VMBAlloc(6);
                                  szBuffer = (char*)VMBAlloc(16);
                                  fread(iT, 4, 1, Prop->File);
                                  sprintf(szBuffer, "%i", *iT);     
                                  VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
                                  iT = (int*)VMBFree(iT);
                                  szBuffer = (char*)VMBFree(szBuffer);                                  
                               } else {
                                  VMBPushRStack("0", 1);       
                               }
                            } else {
                               VMBPushRStack("0", 1);
                            }                                                        
                            break;
                       case 9:
                            szBuffer = (char*)VMBParseOneString();
                            if ( Prop->File )
                            {
                               if ( Prop->Opened ) 
                               {
                                  int *iT = (int*)VMBAlloc(6);
                                  *iT = (int) atoi(szBuffer);
                                  fwrite(iT, 4, 1, Prop->File);
                                  iT = (int*)VMBFree(iT);
                               }     
                            }
                            VMBPrevCode();
                            szBuffer = (char*)VMBFree(szBuffer);
                            break; 
                       case 10:
                            if ( Prop->FileName )
                            {
                               if ( Prop->File )     
                               {
                                  if ( Prop->Opened )     
                                  {
                                     fclose(Prop->File);                                          
                                     Prop->Opened = 0;
                                  }
                                  Prop->File = NULL;
                               }
                               Prop->File = fopen(Prop->FileName, "r");
                               if ( Prop->File )
                               {
                                  Prop->Opened = 1;     
                               }
                            }
                            break;
                       case 11:
                            if ( Prop->FileName )
                            {
                               if ( Prop->File )     
                               {
                                  if ( Prop->Opened )     
                                  {
                                     fclose(Prop->File);                                          
                                     Prop->Opened = 0;
                                  }
                                  Prop->File = NULL;
                               }
                               Prop->File = fopen(Prop->FileName, "w");
                               if ( Prop->File )
                               {
                                  Prop->Opened = 1;     
                               }
                            }                            
                            break;
                       case 12:
                            if ( Prop->FileName )
                            {
                               if ( Prop->File )     
                               {
                                  if ( Prop->Opened )     
                                  {
                                     fclose(Prop->File);                                          
                                     Prop->Opened = 0;
                                  }
                                  Prop->File = NULL;
                               }
                               Prop->File = fopen(Prop->FileName, "rw");
                               if ( Prop->File )
                               {
                                  Prop->Opened = 1;     
                               }
                            }                            
                            break;
                       case 13:
                            if ( Prop->FileName )
                            {
                               if ( Prop->File )     
                               {
                                  if ( Prop->Opened )     
                                  {
                                     fclose(Prop->File);                                          
                                     Prop->Opened = 0;
                                  }
                                  Prop->File = NULL;
                               }
                               Prop->File = fopen(Prop->FileName, "a");
                               if ( Prop->File )
                               {
                                  Prop->Opened = 1;     
                               }
                            }                            
                            break;
                       case 14:
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {
                                  szBuffer = (char*)VMBAlloc(6);
                                  sprintf(szBuffer, "%i", fgetc(Prop->File) & 0xFF);     
                                  VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
                                  szBuffer = (char*)VMBFree(szBuffer);
                               } else {
                                  VMBPushRStack("0", 1);       
                               }
                            } else {
                               VMBPushRStack("0", 1);
                            }                            
                            break;
                       case 15:
                            szBuffer = (char*)VMBParseOneString();  
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {                                  
                                  fputc(atoi(szBuffer) & 0xFF, Prop->File);     
                               }
                            }
                            szBuffer = (char*)VMBFree(szBuffer);
                            VMBPrevCode();
                            break;
                       case 17:
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {
                                  fclose(Prop->File);     
                                  Prop->Opened = 0;
                               }
                            }
                            break;                                                                                                  
                       case 18:
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )     
                               {
                                  short *iT = (short*)VMBAlloc(4);
                                  szBuffer = (char*)VMBAlloc(16);
                                  fread(iT, 2, 1, Prop->File);
                                  sprintf(szBuffer, "%i", (int)(short)*iT);     
                                  VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
                                  iT = (short*)VMBFree(iT);
                                  szBuffer = (char*)VMBFree(szBuffer);                                  
                               } else {
                                  VMBPushRStack("0", 1);       
                               }
                            } else {
                               VMBPushRStack("0", 1);
                            }                                                        
                            break;  
                       case 19:
                            szBuffer = (char*)VMBParseOneString();
                            if ( Prop->File )
                            {
                               if ( Prop->Opened ) 
                               {
                                  short *iT = (short*)VMBAlloc(4);
                                  *iT = (short) atoi(szBuffer) & 0xFFFF;
                                  fwrite(iT, 2, 1, Prop->File);
                                  iT = (short*)VMBFree(iT);
                               }     
                            }
                            VMBPrevCode();
                            szBuffer = (char*)VMBFree(szBuffer);
                            break;                          
                       default:
#ifdef DEBUG                               
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VCLError(316);
                            break;               
        }
}

// -----------------------------------------------------------------------------
//  VCL_FileMember(hwnd, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_FileMember(VMBVar *VCLCom, unsigned int Method)
{
      VCLFileProperties *Prop = (VCLFileProperties *)VCLCom->Properties;
      char *szBuffer1 = (char *)VMBParseOneString();
      switch(Method) 
        {
                       case 1:
                            if ( Prop->FileName )
                               Prop->FileName = (char*)VMBFree(Prop->FileName);
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )
                               {
                                  fclose(Prop->File);     
                                  Prop->Opened = 0;
                               }   
                               Prop->File = NULL;
                            }
                            Prop->FileName = (char*)VMBAlloc(VMBStrLen(szBuffer1)+1);
                            strncpy(Prop->FileName, szBuffer1, VMBStrLen(szBuffer1));
                            break;
                       case 3:
                            if ( Prop->File )
                            {
                               if ( Prop->Opened )
                               {
                                  fseek(Prop->File, atoi(szBuffer1), SEEK_SET);     
                               }     
                            }
                            break;                              
                       default:
#ifdef DEBUG                               
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VCLError(301);
                            break;               
        }
     VMBNextCode();
     szBuffer1 = (char *)VMBFree(szBuffer1);
}

// -----------------------------------------------------------------------------
//  VCL_FileGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_FileGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     unsigned int FilePos;
     VCLFileProperties *Prop = (VCLFileProperties *)VCLCom->Properties;
        switch(Method) 
        {
                       case 1:
                             // TODO!!!
                            if ( Prop->FileName )
                               sprintf(szBuffer, "%s", Prop->FileName);
                            else
                               sprintf(szBuffer, ""); 
                            break;  
                       case 2:
                            if ( Prop->File )
                            {
                               FilePos = ftell(Prop->File);
                               fseek(Prop->File, 0, SEEK_END);
                               sprintf(szBuffer, "%i", ftell(Prop->File));
                               fseek(Prop->File, FilePos, SEEK_SET);
                            } else {
                               sprintf(szBuffer, "0");     
                            }
                       case 3:
                            if ( Prop->File )
                               sprintf(szBuffer, "%i", ftell(Prop->File));
                            else
                               sprintf(szBuffer, "0"); 
                            break;
                       case 4:
                            if ( Prop->Opened )
                               sprintf(szBuffer, "1");
                            else
                               sprintf(szBuffer, "0"); 
                            break;                            
                       case 5:
                            if ( Prop->File )
                            {
                               if ( feof(Prop->File) )
                               {
                                sprintf(szBuffer, "1");     
                               } else {
                                sprintf(szBuffer, "0");        
                               }
                            }
                            else
                               sprintf(szBuffer, "0"); 
                            break;                            

                       default: 
#ifdef DEBUG                                
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VCLError(303);
                            break;  
        }
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     szBuffer = (char*)VMBFree(szBuffer);
}

#endif

#endif
