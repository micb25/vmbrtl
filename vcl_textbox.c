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

#ifndef VCL_TEXTBOX_C
#define VCL_TEXTBOX_C

#include <windows.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"


LRESULT CALLBACK VCL_TextBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void VCL_TextBoxUpdateFont(VCLTextBoxProperties *Prop, HWND hwnd);
int iVCLTextBoxCount = 0;

WNDPROC *OldTextBoxProc;

extern HWND LastCreatedWindow;

// -----------------------------------------------------------------------------
//  VCL_TextBoxCreate(char *) - erzeugt eine TextBox Komponente
// -----------------------------------------------------------------------------
HWND VCL_TextBoxCreate(char *Caption)
{
 HWND hWnd;
 hWnd = CreateWindowEx(WS_EX_CLIENTEDGE, "EDIT", NULL, WS_VISIBLE | WS_CHILD | WS_HSCROLL | WS_VSCROLL | ES_MULTILINE | ES_AUTOVSCROLL | ES_AUTOHSCROLL | ES_WANTRETURN , CW_USEDEFAULT, CW_USEDEFAULT, 200, 60, LastCreatedWindow, NULL, RTL_HINSTANCE, NULL); 
 if (hWnd != NULL)
 {
   OldTextBoxProc = (WNDPROC*)GetWindowLong(hWnd, GWL_WNDPROC);
   SetWindowLong(hWnd, GWL_WNDPROC, (LONG)&VCL_TextBoxProc);          
   SetWindowText(hWnd, Caption);  
   iVCLTextBoxCount++;
   return hWnd;
 } else {
   VCLError(315);
 }
}

// -----------------------------------------------------------------------------
//  VCL_TextBoxMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_TextBoxMethod(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = NULL;
     char *rBuffer = NULL;
     short *Length = NULL;
     FILE *File;
     int iTemp;
     char *P1, *P2, *P3, *P4, *P5;
     VCLTextBoxProperties *Prop = (VCLTextBoxProperties*)VCLCom->Properties;     
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
                            Prop->Event_ClickedX = 0;
                            Prop->Event_ClickedY = 0;                            
                            Prop->Event_Changed = 0;
                            Prop->Event_RClicked = 0;
                            Prop->Event_RClickedX = 0;
                            Prop->Event_RClickedY = 0;                                                        
                            break; 
                       case 8:
                            szBuffer = (char*)VMBParseOneString();
                            rBuffer = (char*)VMBAlloc(0xFFFF);
                            Length = (short*)(void*)rBuffer;
                            *Length = 0xFFFF;
                            iTemp = SendMessage(VCLCom->hWnd, EM_GETLINE, atoi(szBuffer) - 1, (WPARAM)rBuffer);
                            if ( iTemp )
                               VMBPushRStack(rBuffer, VMBStrLen(rBuffer));
                            else
                               VMBPushRStack("", 0); 
                            szBuffer = (char*)VMBFree(szBuffer);
                            rBuffer = (char*)VMBFree(rBuffer);
                            VMBPrevCode();
                            break;  
                       case 9:
                            szBuffer = (char*)VMBParseOneString();
                            File = fopen(szBuffer, "w");
                            if (File == NULL)
                            {
                             VMBPushRStack("0", 1);                               
                            } else {
                                 iTemp = GetWindowTextLength(VCLCom->hWnd) + 1;
                                 char *rBuffer = (char*)VMBAlloc(iTemp + 1);
                                 GetWindowText(VCLCom->hWnd, rBuffer, iTemp);                
                                 fputs(rBuffer, File);
                                 fclose(File);
                                 VMBPushRStack("1", 1);
                                 rBuffer = (char*)VMBFree(rBuffer);
                            }     
                            szBuffer = (char*)VMBFree(szBuffer);              
                            VMBPrevCode();
                            break;
                       case 10:
                            szBuffer = (char*)VMBParseOneString();
                            File = fopen(szBuffer, "r");
                            if (File == NULL)
                            {
                             VMBPushRStack("0", 1);                               
                            } else {
                                 fseek(File , 0, SEEK_END);
                                 iTemp = ftell(File);
                                 fseek(File,  0, SEEK_SET);
                                 rBuffer = (char*)VMBAlloc(iTemp+2);
                                 fread(rBuffer, iTemp, 1, File);                                 
                                 fclose(File);
                                 SetWindowText(VCLCom->hWnd, rBuffer);
                                 VMBPushRStack("1", 1);
                                 rBuffer = (char*)VMBFree(rBuffer);
                            }     
                            szBuffer = (char*)VMBFree(szBuffer);              
                            VMBPrevCode();                            
                            break;    
                       case 11:
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
                       case 12:
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
                            VCLError(316);
                            break;               
        }
}

// -----------------------------------------------------------------------------
//  VCL_TextBoxMember(hwnd, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_TextBoxMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer1 = NULL;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     char cChar;
     VCLTextBoxProperties *Prop = (VCLTextBoxProperties *)VCLCom->Properties;
     szBuffer1 = (char *)VMBParseOneString();
        switch(Method) 
        {
                       case 1:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);        
                            iW += GetSystemMetrics(SM_CYBORDER) * 4;
                            iH += GetSystemMetrics(SM_CXBORDER) * 4;                                            
                            MoveWindow(VCLCom->hWnd, iBuffer, iY, iW, iH, TRUE);                   
                            break;
                       case 2:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            iW += GetSystemMetrics(SM_CYBORDER) * 4;
                            iH += GetSystemMetrics(SM_CXBORDER) * 4;
                            MoveWindow(VCLCom->hWnd, iX, iBuffer, iW, iH, TRUE);                                                
                            break;                        
                       case 3:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            iH += GetSystemMetrics(SM_CYBORDER) * 4;
                            MoveWindow(VCLCom->hWnd, iX, iY, iBuffer, iH, TRUE);  
                            break;   
                       case 4:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            iW += GetSystemMetrics(SM_CXBORDER) * 4;
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
                            Prop->FontSize = atoi(szBuffer1);
                            VCL_TextBoxUpdateFont(Prop, VCLCom->hWnd);
                            break;                            
                       case 12:
                            Prop->BkColor = atoi(szBuffer1);
                            DeleteObject(Prop->Background);
                            Prop->Background = CreateSolidBrush(Prop->BkColor);
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;   
                       case 13:
                            Prop->Color = atoi(szBuffer1);
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;   
                       case 14:
                            iX = atoi(szBuffer1);
                            if (iX)
                            {
                              Prop->Event_Clicked = 1;                                   
                            } else {
                              Prop->Event_Clicked = 0;
                            }
                            break;   
                       case 15:
                            iX = atoi(szBuffer1);
                            if (iX)
                            {
                              Prop->Event_Changed = 1;                                   
                            } else {
                              Prop->Event_Changed = 0;
                            }
                            break;   
                       case 16:                    
                            strncpy(Prop->FontName, szBuffer1, 255);                        
                            VCL_TextBoxUpdateFont(Prop, VCLCom->hWnd);
                            break;                                                       
                       case 17:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontStyle = iBuffer;
                            VCL_TextBoxUpdateFont(Prop, VCLCom->hWnd);
                            break;                                                     
                       case 18:
                            iX = atoi(szBuffer1);
                            if (iX)
                            {
                              Prop->Event_RClicked = 1;                                   
                            } else {
                              Prop->Event_RClicked = 0;
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
//  VCL_TextBoxGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_TextBoxGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     char TempBuffer[16];
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0; 
     char cChar;
     VCLTextBoxProperties *Prop = (VCLTextBoxProperties *)VCLCom->Properties;
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
                            iW += GetSystemMetrics(SM_CXBORDER) * 4;   
                            sprintf(szBuffer, "%i", iW);
                            break; 
                       case 4:
                            VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
                            iH += GetSystemMetrics(SM_CYBORDER) * 4;   
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
                            sprintf(szBuffer, "%i", (int)Prop->FontSize);
                            break;     
                       case 12:
                            sprintf(szBuffer, "%i", (int)Prop->BkColor);
                            break; 
                       case 13:
                            sprintf(szBuffer, "%i", (int)Prop->Color);
                            break;                                                                                   
                       case 14:
                            if (Prop->Event_Clicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                                                         
                       case 15:
                            if (Prop->Event_Changed)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                            
                       case 16:
                            sprintf(szBuffer, "%s", (int)Prop->FontName);
                            break;   
                       case 17:
                            sprintf(szBuffer, "%i", (int)Prop->FontStyle);
                            break; 
                       case 18:
                            if (Prop->Event_RClicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;
                       case 19:
                            sprintf(szBuffer, "%i", SendMessage(VCLCom->hWnd, EM_GETLINECOUNT, 0, 0));          
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
// VCL_TextBoxProc(int, int, int, int) - Callback für TextBoxs
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_TextBoxProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     VMBVar *VCLCom;
     
     switch(uMsg) {
         case WM_RBUTTONUP:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("TextBox: WM_RBUTTONUP received!");
#endif                
              VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
              if ( VCLCom )
              {
                 if ( VCLCom->VarType == VARTYPE_TEXTBOX )    
                 {
                    VCLTextBoxProperties *Prop = (VCLTextBoxProperties*)VCLCom->Properties;
                    Prop->Event_RClicked = 1;
                    Prop->Event_RClickedX = LOWORD(lParam);
                    Prop->Event_RClickedY = HIWORD(lParam);  
                 }
              }
              break;                        
         case WM_LBUTTONUP:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("TextBox: WM_LBUTTONUP received!");
#endif                
              VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
              if ( VCLCom )
              {
                 if ( VCLCom->VarType == VARTYPE_TEXTBOX )    
                 {
                    VCLTextBoxProperties *Prop = (VCLTextBoxProperties*)VCLCom->Properties;
                    Prop->Event_Clicked = 1;
                    Prop->Event_ClickedX = LOWORD(lParam);
                    Prop->Event_ClickedY = HIWORD(lParam);  
                 }
              }
              break;             
     }
     return  CallWindowProc( (WNDPROC)OldTextBoxProc, hwnd, uMsg, wParam, lParam);        
}

// -----------------------------------------------------------------------------
// VCL_TextBoxUpdateFont(PROP, HWND) - Verändert den Font des TextBoxs
// -----------------------------------------------------------------------------
void VCL_TextBoxUpdateFont(VCLTextBoxProperties *Prop, HWND hwnd)
{
     DeleteObject(Prop->Font);
     Prop->Font = (HFONT)VMBCreateFont(Prop->FontName, Prop->FontSize, Prop->FontStyle);    
     SendMessage(hwnd, WM_SETFONT, (WPARAM)Prop->Font, (LPARAM)MAKELPARAM(TRUE, 0)); 
}

#endif

#endif
