/***************************************************************************
 *   Versatile MB 2009 - C Runtime Library                                 *
 *   Copyright (C) 2002-2010 by Michael B�hme & embyte                     *
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

#ifndef VCL_BUTTON_C
#define VCL_BUTTON_C

#include <windows.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

LRESULT CALLBACK VCL_ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void VCL_ButtonUpdateFont(VCLButtonProperties *Prop, HWND hwnd);
int iVCLButtonCount = 0;

WNDPROC *OldButtonProc;

extern HWND LastCreatedWindow;

// -----------------------------------------------------------------------------
//  VCL_ButtonCreate(char *) - erzeugt eine Button Komponente
// -----------------------------------------------------------------------------
HWND VCL_ButtonCreate(char *Caption)
{
 HWND hWnd;
 hWnd = CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", NULL, BS_MULTILINE | WS_TABSTOP | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD, CW_USEDEFAULT, CW_USEDEFAULT, 75, 25, LastCreatedWindow, NULL, RTL_HINSTANCE, NULL);
 if (hWnd != NULL)
 {
   OldButtonProc = (WNDPROC*)GetWindowLong(hWnd, GWL_WNDPROC);
   SetWindowLong(hWnd, GWL_WNDPROC, (LONG)&VCL_ButtonProc);          
   SetWindowText(hWnd, Caption);  
   iVCLButtonCount++;
   return hWnd;
 } else {
   VCLError(306);       
 }
}

// -----------------------------------------------------------------------------
//  VCL_ButtonMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_ButtonMethod(VMBVar *VCLCom, unsigned int Method)
{
     char *P1, *P2, *P3, *P4, *P5;
     VCLButtonProperties *Prop = (VCLButtonProperties*)VCLCom->Properties;
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
                            SetFocus(VCLCom->hWnd);
                            break;                 
                       case 5:
                            EnableWindow(VCLCom->hWnd, FALSE);
                            break;                                                     
                       case 6:
                            EnableWindow(VCLCom->hWnd, TRUE);
                            break;     
                       case 7:
                            Prop->Event_Clicked = 0;
                            Prop->Event_RClicked = 0;
                            break; 
                       case 8:
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
                       case 9:
                            P1 = (char*)VMBParseOneString();
                            P2 = (char*)VMBParseOneString();
                            P3 = (char*)VMBParseOneString();
                            P4 = (char*)VMBParseOneString();                                                                                    
                            P5 = (char*)VMBParseOneString();                                                                                    
                            SetWindowText(VCLCom->hWnd, P1);
                            MoveWindow(VCLCom->hWnd, atoi(P2), atoi(P3), atoi(P4), atoi(P5), TRUE);
                            P1 = (char*)VMBFree(P1);
                            P2 = (char*)VMBFree(P2);
                            P3 = (char*)VMBFree(P3);
                            P4 = (char*)VMBFree(P4);                                                                                    
                            P5 = (char*)VMBFree(P5);                                                                                    
                            VMBPrevCode();
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
//  VCL_ButtonMember(hwnd, int) - �ndert einen Member 
// -----------------------------------------------------------------------------
void VCL_ButtonMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer1 = NULL;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     VCLButtonProperties *Prop = (VCLButtonProperties *)VCLCom->Properties;
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
                            if (!iBuffer)                    
                            {
                             EnableWindow(VCLCom->hWnd, 0);
                            } else {
                             EnableWindow(VCLCom->hWnd, 1);    
                            }
                            break;                                                      
                       case 7:
                            SetWindowText(VCLCom->hWnd, szBuffer1);
                            break; 
                       case 8:
                            iBuffer = atoi(szBuffer1);
                            SetParent(VCLCom->hWnd, (HWND)iBuffer);                     
                            break;    
                       case 11:                    
                            strncpy(Prop->FontName, szBuffer1, 255);                        
                            VCL_ButtonUpdateFont(Prop, VCLCom->hWnd);
                            break;                            
                       case 12:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontSize = iBuffer;
                            VCL_ButtonUpdateFont(Prop, VCLCom->hWnd);
                            break;                                                     
                       case 13:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer)
                            {
                              Prop->Event_Clicked = 1;                                   
                            } else {
                              Prop->Event_Clicked = 0;
                            }
                            break;   
                       case 14:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontStyle = iBuffer;
                            VCL_ButtonUpdateFont(Prop, VCLCom->hWnd);
                            break;
                       case 15:
                            iX = atoi(szBuffer1);
                            if (iX)
                            {
                              Prop->Event_RClicked = 1;                                   
                            } else {
                              Prop->Event_RClicked = 0;
                            }
                            break;   
                       case 16:
                            iX = atoi(szBuffer1);
                            if (iX)
                            {
                              iBuffer = GetWindowLong(VCLCom->hWnd, GWL_STYLE);                                 
                              iBuffer = iBuffer | BS_DEFPUSHBUTTON;
                              SetWindowLong(VCLCom->hWnd, GWL_STYLE, iBuffer);
                            } else {
                              iBuffer = GetWindowLong(VCLCom->hWnd, GWL_STYLE);                                 
                              iBuffer = iBuffer & ~( BS_DEFPUSHBUTTON );
                              SetWindowLong(VCLCom->hWnd, GWL_STYLE, iBuffer);
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
//  VCL_ButtonGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_ButtonGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     char TempBuffer[16];
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0; 
     VCLButtonProperties *Prop = (VCLButtonProperties *)VCLCom->Properties;
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
                            if (IsWindowEnabled(VCLCom->hWnd))
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                                                      
                       case 7:
                            szBuffer = (char*)VMBFree(szBuffer);
                            iX = GetWindowTextLength(VCLCom->hWnd);
                            szBuffer = (char*)VMBAlloc(iX+2);
                            GetWindowText(VCLCom->hWnd, szBuffer, iX+1);
                            break; 
                       case 8:
                            sprintf(szBuffer, "%i", (int)GetParent(VCLCom->hWnd));
                            break;
                       case 9:
                            sprintf(szBuffer, "%i", (int)VCLCom->hWnd);
                            break;
                       case 11:
                            sprintf(szBuffer, "%s", (int)Prop->FontName);
                            break;                             
                       case 12:
                            sprintf(szBuffer, "%i", (int)Prop->FontSize);
                            break;                             
                       case 13:
                            if (Prop->Event_Clicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;
                       case 14:
                            sprintf(szBuffer, "%i", (int)Prop->FontStyle);
                            break;                                                         
                       case 15:
                            if (Prop->Event_RClicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;     
                       case 16:
                            iBuffer = GetWindowLong(VCLCom->hWnd, GWL_STYLE);
                            if ( iBuffer & BS_DEFPUSHBUTTON )
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
// VCL_ButtonProc(int, int, int, int) - Callback f�r Buttons
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_ButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     VMBVar *VCLCom;
     
     switch(uMsg) {
         case WM_RBUTTONUP:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Button: WM_RBUTTONUP received!");
#endif                
              VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
              if ( VCLCom )
              {
                 if ( VCLCom->VarType == VARTYPE_BUTTON )    
                 {
                    VCLButtonProperties *Prop = (VCLButtonProperties*)VCLCom->Properties;
                    Prop->Event_RClicked = 1;
                    Prop->Event_RClickedX = LOWORD(lParam);
                    Prop->Event_RClickedY = HIWORD(lParam);  
                 }
              }
              return 0;
              break;              
     }
     return  CallWindowProc( (WNDPROC)OldButtonProc, hwnd, uMsg, wParam, lParam);        
}

// -----------------------------------------------------------------------------
// VCL_ButtonUpdateFont(PROP, HWND) - Ver�ndert den Font des Buttons
// -----------------------------------------------------------------------------
void VCL_ButtonUpdateFont(VCLButtonProperties *Prop, HWND hwnd)
{
     DeleteObject(Prop->Font);
     Prop->Font = (HFONT)VMBCreateFont(Prop->FontName, Prop->FontSize, Prop->FontStyle);    
     SendMessage(hwnd, WM_SETFONT, (WPARAM)Prop->Font, (LPARAM)MAKELPARAM(TRUE, 0)); 
}

#endif

#endif
