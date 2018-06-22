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

#ifndef VCL_RADIOBUTTON_C
#define VCL_RADIOBUTTON_C

#include <windows.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

LRESULT CALLBACK VCL_RadioButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void VCL_RadioButtonUpdateFont(VCLRadioButtonProperties *Prop, HWND hwnd);
int iVCLRadioButtonCount = 0;

/*
WNDPROC *OldRadioButtonProc;
*/
extern HWND LastCreatedWindow;

// -----------------------------------------------------------------------------
//  VCL_RadioButtonCreate(char *) - erzeugt eine RadioButton Komponente
// -----------------------------------------------------------------------------
HWND VCL_RadioButtonCreate(char *Caption)
{
 HWND hWnd;
 hWnd = CreateWindowEx(0, "BUTTON", NULL, BS_NOTIFY | BS_TEXT | BS_AUTORADIOBUTTON | WS_TABSTOP | WS_VISIBLE | WS_CHILD,  CW_USEDEFAULT, CW_USEDEFAULT, 120, 25, LastCreatedWindow, NULL, RTL_HINSTANCE, NULL);
 if (hWnd != NULL)
 {
/*   OldRadioButtonProc = (WNDPROC*)GetWindowLong(hWnd, GWL_WNDPROC);
   SetWindowLong(hWnd, GWL_WNDPROC, (LONG)&VCL_RadioButtonProc);          */
   SetWindowText(hWnd, Caption);  
   iVCLRadioButtonCount++;
   return hWnd;
 } else {
   VCLError(306);
 }
}

// -----------------------------------------------------------------------------
//  VCL_RadioButtonMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_RadioButtonMethod(VMBVar *VCLCom, unsigned int Method)
{
     char *P1, *P2, *P3, *P4, *P5;
     VCLRadioButtonProperties *Prop = (VCLRadioButtonProperties*)VCLCom->Properties;
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
                       case 8:
                            Prop->Event_Clicked = 0;
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
//  VCL_RadioButtonMember(hwnd, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_RadioButtonMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer1 = NULL;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     VCLRadioButtonProperties *Prop = (VCLRadioButtonProperties *)VCLCom->Properties;
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
                            VCL_RadioButtonUpdateFont(Prop, VCLCom->hWnd);
                            break;                            
                       case 12:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontSize = iBuffer;
                            VCL_RadioButtonUpdateFont(Prop, VCLCom->hWnd);
                            break;
                       case 13:
                            Prop->Color = atoi(szBuffer1);
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;
                       case 14:
                            Prop->BkColor = atoi(szBuffer1);
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;   
                       case 15:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer)
                            {
                              SendMessage(VCLCom->hWnd, BM_SETCHECK, (WPARAM)BST_CHECKED, 0);
                            } else {
                              SendMessage(VCLCom->hWnd, BM_SETCHECK, (WPARAM)BST_UNCHECKED, 0);
                            }
                            break;                                                      
                       case 16:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer)
                            {
                              Prop->Event_Clicked = 1;                                   
                            } else {
                              Prop->Event_Clicked = 0;
                            }
                            break;   
                       case 18:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontStyle = iBuffer;
                            VCL_RadioButtonUpdateFont(Prop, VCLCom->hWnd);
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
//  VCL_RadioButtonGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_RadioButtonGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     char TempBuffer[16];
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0; 
     VCLRadioButtonProperties *Prop = (VCLRadioButtonProperties *)VCLCom->Properties;
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
                            sprintf(szBuffer, "%i", (int)Prop->Color);
                            break;
                       case 14:  
                            sprintf(szBuffer, "%i", (int)Prop->BkColor);
                            break;                                                    
                       case 15:
                            iX = SendMessage(VCLCom->hWnd, BM_GETCHECK, 0, 0);
                            if ( iX == BST_CHECKED )
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                                                          
                       case 16:
                            if (Prop->Event_Clicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;
                       case 18:
                            sprintf(szBuffer, "%i", (int)Prop->FontStyle);
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

/*
// -----------------------------------------------------------------------------
// VCL_RadioButtonProc(int, int, int, int) - Callback für RadioButton
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_RadioButtonProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     VMBVar *VCLCom;
     
     return  CallWindowProc( (WNDPROC)OldRadioButtonProc, hwnd, uMsg, wParam, lParam);        
}
*/

// -----------------------------------------------------------------------------
// VCL_RadioButtonUpdateFont(PROP, HWND) - Verändert den Font der RadioButton
// -----------------------------------------------------------------------------
void VCL_RadioButtonUpdateFont(VCLRadioButtonProperties *Prop, HWND hwnd)
{
     DeleteObject(Prop->Font);
     Prop->Font = (HFONT)VMBCreateFont(Prop->FontName, Prop->FontSize, Prop->FontStyle);    
     SendMessage(hwnd, WM_SETFONT, (WPARAM)Prop->Font, (LPARAM)MAKELPARAM(TRUE, 0)); 
}

#endif

#endif
