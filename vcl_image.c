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

#ifndef VCL_IMAGE_C
#define VCL_IMAGE_C

#define _WIN32_IE 0x500

#include <windows.h>
#include <commctrl.h>
#include <ole2.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

LRESULT CALLBACK VCL_ImageProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void VCL_ImageUpdate(VMBVar *VCLCom);
int iVCLImageCount = 0;

WNDPROC *OldImageProc;

extern HWND LastCreatedWindow;

// -----------------------------------------------------------------------------
//  VCL_ImageCreate(char *) - erzeugt eine Image Komponente
// -----------------------------------------------------------------------------
HWND VCL_ImageCreate(char *Caption)
{
 HWND hWnd;
 hWnd = CreateWindow( WC_LISTVIEW, "",  WS_CHILD | WS_VISIBLE | LVS_NOSCROLL | LVS_NOCOLUMNHEADER | LVS_OWNERDATA | LVS_SINGLESEL , CW_USEDEFAULT, CW_USEDEFAULT, 200, 200, LastCreatedWindow, NULL, RTL_HINSTANCE, NULL);
 if (hWnd != NULL)
 {
   OldImageProc = (WNDPROC*)GetWindowLong(hWnd, GWL_WNDPROC);
   SetWindowLong(hWnd, GWL_WNDPROC, (LONG)&VCL_ImageProc);          
   iVCLImageCount++;
   return hWnd;
 } else {
   VCLError(306);
 }
}

// -----------------------------------------------------------------------------
//  VCL_ImageMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_ImageMethod(VMBVar *VCLCom, unsigned int Method)
{
     char *P1, *P2, *P3, *P4, *P5;
     RECT MyRec;
     VCLImageProperties *Prop = (VCLImageProperties*)VCLCom->Properties;
     switch(Method) 
        {
                       case 1:
                            ShowWindow(VCLCom->hWnd, SW_SHOWNORMAL);
                            break;
                       case 2:
                            ShowWindow(VCLCom->hWnd, SW_HIDE);
                            break;
                       case 3:
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;                                                                
                       case 4:
                            P1 = (char*)VMBParseOneString();
                            P2 = (char*)VMBParseOneString();
                            P3 = (char*)VMBParseOneString();
                            P4 = (char*)VMBParseOneString();            
                            MoveWindow(VCLCom->hWnd, atoi(P1), atoi(P2), atoi(P3), atoi(P4), TRUE);
                            P1 = (char*)VMBFree(P1);
                            P2 = (char*)VMBFree(P2);
                            P3 = (char*)VMBFree(P3);
                            P4 = (char*)VMBFree(P4);                                                                                    
                            VMBPrevCode();
                            break;    
                       case 5:
                            Prop->Event_Clicked = 0;
                            Prop->Event_ClickedX = 0;
                            Prop->Event_ClickedY = 0;
                            Prop->Event_RClicked = 0;
                            Prop->Event_RClickedX = 0;
                            Prop->Event_RClickedY = 0;                            
                            break;                                                                                                                                     

                       default:
#ifdef DEBUG                               
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VCLError(305);
                            break;               
        }
}

// -----------------------------------------------------------------------------
//  VCL_ImageMember(hwnd, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_ImageMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer1 = NULL;
     char *P1;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     VCLImageProperties *Prop = (VCLImageProperties *)VCLCom->Properties;
     szBuffer1 = (char *)VMBParseOneString();
        switch(Method) 
        {
                       case 1:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);                        
                            MoveWindow(VCLCom->hWnd, iBuffer, iY, iW, iH, TRUE);                   
                            break;
                       case 2:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(VCLCom->hWnd, iX, iBuffer, iW, iH, TRUE);                                                
                            break;                            
                       case 3:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(VCLCom->hWnd, iX, iY, iBuffer, iH, TRUE);  
                            break;   
                       case 4:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(VCLCom->hWnd, iX, iY, iW, iBuffer, TRUE);  
                            break;  
                       case 5:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             ShowWindow(VCLCom->hWnd, SW_HIDE);
                            } else {
                             ShowWindow(VCLCom->hWnd, SW_SHOW);    
                            }
                            break;                              
                       case 6:
                            iBuffer = atoi(szBuffer1);
                            SetParent(VCLCom->hWnd, (HWND)iBuffer);                     
                            break;    
                       case 8:
                            Prop->BkColor = atoi(szBuffer1);
                            VCL_ImageUpdate(VCLCom);
                            break;  
                       case 9:
                            P1 = (char*)VMBAlloc(VMBStrLen(szBuffer1)+2);
                            if ( Prop->File != NULL )
                               VMBFree(Prop->File);
                            strncpy(P1, szBuffer1, VMBStrLen(szBuffer1));
                            Prop->File = P1;
                            VCL_ImageUpdate(VCLCom);                            
                            break;                                                        
                       case 10:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             Prop->Event_Clicked = 0;
                            } else {
                             Prop->Event_Clicked = 1;
                            }
                            break;  
                       case 11:
                            Prop->Event_ClickedX = atoi(szBuffer1);
                            break;    
                       case 12:
                            Prop->Event_ClickedY = atoi(szBuffer1);
                            break;                                                       
                       case 13:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             Prop->Event_Clicked = 0;
                            } else {
                             Prop->Event_Clicked = 1;
                            }
                            break;  
                       case 14:
                            Prop->Event_RClickedX = atoi(szBuffer1);
                            break;    
                       case 15:
                            Prop->Event_RClickedY = atoi(szBuffer1);
                            break; 
                       case 16:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             Prop->Tiled = 0;                             
                            } else {
                             Prop->Tiled = 1;
                             Prop->Offset = 0;
                            }
                            VCL_ImageUpdate(VCLCom);                            
                            break; 
                       case 17:
                            Prop->XOffset = atoi(szBuffer1);
                            VCL_ImageUpdate(VCLCom);                            
                            break;    
                       case 18:
                            Prop->YOffset = atoi(szBuffer1);
                            VCL_ImageUpdate(VCLCom);                            
                            break; 
                       case 19:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             Prop->Offset = 0;
                            } else {
                             Prop->Tiled = 0;
                             Prop->Offset = 1;
                            }
                            VCL_ImageUpdate(VCLCom);                            
                            break;   
                       case 20:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             Prop->Transparent = 0;
                            } else {
                             Prop->Transparent = 1;
                            }
                            VCL_ImageUpdate(VCLCom);                            
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
//  VCL_ImageGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_ImageGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     char TempBuffer[16];
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0; 
     VCLImageProperties *Prop = (VCLImageProperties *)VCLCom->Properties;
        switch(Method) 
        {
                       case 1:
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iX);
                            break;  
                       case 2:
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iY);
                            break;  
                       case 3:
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iW);
                            break; 
                       case 4:
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iH);
                            break;  
                       case 5:                            
                            if (IsWindowVisible(VCLCom->hWnd))
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;    
                       case 6:
                            sprintf(szBuffer, "%i", (int)GetParent(VCLCom->hWnd));
                            break;
                       case 7:
                            sprintf(szBuffer, "%i", (int)VCLCom->hWnd);
                            break;
                       case 8:  
                            sprintf(szBuffer, "%i", (int)Prop->BkColor);
                            break;                                                                                                             
                       case 9:  
                            
                            if ( Prop->File )
                            {
                               sprintf(szBuffer, "%s", Prop->File );
                            } else {
                              *szBuffer = '\0';
                            }
                            break;  
                       case 10:                            
                            if ( Prop->Event_Clicked )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;  
                       case 11:
                            sprintf(szBuffer, "%i", (int)Prop->Event_ClickedX);
                            break;                                                       
                       case 12:
                            sprintf(szBuffer, "%i", (int)Prop->Event_ClickedY);
                            break;                                                       
                       case 13:                            
                            if ( Prop->Event_RClicked )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break; 
                       case 14:
                            sprintf(szBuffer, "%i", (int)Prop->Event_RClickedX);
                            break;                                                       
                       case 15:
                            sprintf(szBuffer, "%i", (int)Prop->Event_RClickedY);
                            break;  
                       case 16:                            
                            if ( Prop->Tiled )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                                                                         
                       case 17:
                            sprintf(szBuffer, "%i", (int)Prop->XOffset);
                            break;                                                       
                       case 18:
                            sprintf(szBuffer, "%i", (int)Prop->YOffset);
                            break;   
                       case 19:                            
                            if ( Prop->Offset )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                
                       case 20:                            
                            if ( Prop->Transparent )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
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

// -----------------------------------------------------------------------------
// VCL_ImageProc(int, int, int, int) - Callback für Image
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_ImageProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     VMBVar *VCLCom;
     switch ( uMsg )
     {   
         case WM_RBUTTONUP:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Image: WM_RBUTTONUP received!");
#endif                
              VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
              if ( VCLCom )
              {
                 if ( VCLCom->VarType == VARTYPE_IMAGE )    
                 {
                    VCLImageProperties *Prop = (VCLImageProperties*)VCLCom->Properties;
                    Prop->Event_RClicked = 1;
                    Prop->Event_RClickedX = LOWORD(lParam);
                    Prop->Event_RClickedY = HIWORD(lParam);  
                 }
              }
              return 0;
              break;  
         case WM_LBUTTONUP:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Image: WM_LBUTTONUP received!");
#endif                
              VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
              if ( VCLCom )
              {
                 if ( VCLCom->VarType == VARTYPE_IMAGE )    
                 {
                    VCLImageProperties *Prop = (VCLImageProperties*)VCLCom->Properties;
                    Prop->Event_Clicked = 1;
                    Prop->Event_ClickedX = LOWORD(lParam);
                    Prop->Event_ClickedY = HIWORD(lParam);  
                 }
              }
              return 0;
              break; 
         case WM_ERASEBKGND:
              //return 0;
              break;              
     }
     
     return  CallWindowProc( (WNDPROC)OldImageProc, hwnd, uMsg, wParam, lParam);        
}

// -----------------------------------------------------------------------------
// VCL_ImageUpdate(VMBVar *) - aktualisiert eine Image Komponente
// -----------------------------------------------------------------------------
void VCL_ImageUpdate(VMBVar *VCLCom)
{
     if ( VCLCom  && VCLCom->Properties )
     {
          VCLImageProperties *Prop = (VCLImageProperties*)VCLCom->Properties;
          
          if ( Prop->Picture == NULL )
          {
             Prop->Picture = (void*)VMBAlloc(sizeof(LVBKIMAGE)+1);     
          }
          
          if ( Prop->File == NULL )
          {
             Prop->File = (char*)VMBAlloc(32);
          }
          
          LVBKIMAGE *Picture = (LVBKIMAGE*)Prop->Picture;
          
          Picture->ulFlags = LVBKIF_SOURCE_URL;
          Picture->pszImage = Prop->File;
          
          if ( Prop->Tiled )
             Picture->ulFlags |= LVBKIF_STYLE_TILE;
          else
             Picture->ulFlags |= LVBKIF_STYLE_NORMAL;
          
          SendMessage(VCLCom->hWnd, LVM_SETBKIMAGE, 0, (LPARAM)(LPLVBKIMAGE) Picture);    
          if ( Prop->Transparent )
             SendMessage(VCLCom->hWnd, LVM_SETBKCOLOR, 0, (LPARAM)CLR_NONE);    
          else
             SendMessage(VCLCom->hWnd, LVM_SETBKCOLOR, 0, (LPARAM)Prop->BkColor);   
     }
}

#endif

#endif
