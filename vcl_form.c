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
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"


const char VCL_FORMCLASSNAME[] = "VMBWindow";
int VCL_FORMINIT = 0;
WNDCLASS VCL_FORMCLASS;
LRESULT CALLBACK VCL_FORMWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
HWND VCL_FormCreate(char *WindowName, char *Caption);
HWND LastCreatedWindow = NULL;

int iVCLFormsCount = 0;

// -----------------------------------------------------------------------------
//  VCL_FORM_Init() - initialisiert die VCL_Forms
// -----------------------------------------------------------------------------
void VCL_FORM_Init()
{
#if VERBOSE > 1
 VMBDebug(" -> %s();", __func__);
#endif
 VCL_FORMINIT = 1;     
 VCL_FORMCLASS.style = CS_DBLCLKS;
 VCL_FORMCLASS.hInstance = RTL_HINSTANCE;
 VCL_FORMCLASS.hCursor = LoadCursor(NULL, IDC_ARROW);
 VCL_FORMCLASS.hIcon = LoadIcon(RTL_HINSTANCE, "A");
 VCL_FORMCLASS.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
 VCL_FORMCLASS.lpfnWndProc = (WNDPROC)VCL_FORMWndProc;
 VCL_FORMCLASS.lpszMenuName = NULL;
 VCL_FORMCLASS.lpszClassName = VCL_FORMCLASSNAME;
 RegisterClass(&VCL_FORMCLASS);
}

// -----------------------------------------------------------------------------
//  VCL_FormCreate(char *, char *) - erzeugt eine Form Komponente
// -----------------------------------------------------------------------------
HWND VCL_FormCreate(char *WindowName, char *Caption)
{
 HWND hWnd;
 hWnd = CreateWindow(WindowName,WindowName, WS_MAXIMIZEBOX | WS_MINIMIZEBOX | WS_SIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 640, 480, NULL, NULL, RTL_HINSTANCE, NULL);
 if (hWnd != NULL)
 {
   SetWindowText(hWnd, Caption);    
   LastCreatedWindow = hWnd;
   iVCLFormsCount++;
   return hWnd;
 } else {
   VCLError(306);
 }
}

BOOL CALLBACK DrawChildWindows(HWND hwnd, LPARAM lParam)
{
#ifdef SHOWWINDOWMESSAGES             
             VMBDebug("DrawChildWindows(0x%08X)", hwnd);
#endif
#ifdef FORM_DOUBLEBUFFERED
     // RedrawWindow(hwnd, NULL, 0, RDW_INVALIDATE );
//      InvalidateRect(GetParent(hwnd), NULL, TRUE);
     // SendMessage(hwnd, WM_ERASEBKGND, (WPARAM)(HDC) lParam, 0 );   
//      SendMessage(hwnd, WM_PAINT, (WPARAM)(HDC) lParam, 0 );   
      //VMBProcessMessages();
      
//      RedrawWindow(hwnd, NULL, 0, RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW );
#else
//      RedrawWindow(hwnd, NULL, 0, RDW_FRAME	| RDW_ERASE | RDW_INVALIDATE | RDW_UPDATENOW );
      SendMessage(hwnd, WM_PAINT, 0, 0);
#endif   
}

// -----------------------------------------------------------------------------
//  VCL_FORMWndProc(int, int, int, int) - Callback für VCL_Forms
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_FORMWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     RECT Re;
     PAINTSTRUCT PS;
     HBRUSH Br;
     HDC hdcMem,hDC;
     HBITMAP hbmMem, hbmOld;
     VMBVar *VCLCom;
     
     switch(uMsg) {
        case WM_DESTROY:
             return 0;
             break;
        case WM_COMMAND:          
             VCLCom = (VMBVar*)VMBFindVarByHWND((HWND) lParam );
             if (VCLCom)
             {                        
                if ( VCLCom->VarType == VARTYPE_BUTTON )
                {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("Button: WM_CLICK received!");
#endif                       
                   VCLButtonProperties *Prop = (VCLButtonProperties *)VCLCom->Properties;    
                   Prop->Event_Clicked = 1;                   
                }  else
                if ( VCLCom->VarType == VARTYPE_CHECKBOX && HIWORD(wParam) == BN_CLICKED )
                {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("CheckBox: WM_CLICK received!");
#endif                       
                   VCLCheckBoxProperties *Prop = (VCLCheckBoxProperties *)VCLCom->Properties;    
                   Prop->Event_Clicked = 1;                   
                }  else                
                if ( VCLCom->VarType == VARTYPE_RADIOBUTTON && HIWORD(wParam) == BN_CLICKED )
                {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("RadioButton: WM_CLICK received!");
#endif                       
                   VCLRadioButtonProperties *Prop = (VCLRadioButtonProperties *)VCLCom->Properties;    
                   Prop->Event_Clicked = 1;                   
                }  else                                    
                if ( VCLCom->VarType == VARTYPE_EDIT )
                {
                   if ( HIWORD(wParam) == EN_CHANGE )
                   {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("Edit: EN_CHANGED received!");
#endif                       
                         VCLEditProperties *Prop = (VCLEditProperties *)VCLCom->Properties;    
                         Prop->Event_Changed = 1;                   
                   }
                } else  
                if ( VCLCom->VarType == VARTYPE_GROUPBOX )
                {
//                   if ( HIWORD(wParam) == EN_CHANGE )
//                   {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("GroupBox: WM_COMMAND received!");
#endif                       
//                         VCLTextBoxProperties *Prop = (VCLTextBoxProperties *)VCLCom->Properties;    
//                         Prop->Event_Changed = 1;                   
//                   }
                } else                 
                if ( VCLCom->VarType == VARTYPE_TEXTBOX )
                {
                   if ( HIWORD(wParam) == EN_CHANGE )
                   {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("TextBox: EN_CHANGED received!");
#endif                       
                         VCLTextBoxProperties *Prop = (VCLTextBoxProperties *)VCLCom->Properties;    
                         Prop->Event_Changed = 1;                   
                   }
                } else                  
                if ( VCLCom->VarType == VARTYPE_LABEL )
                {
                   if ( HIWORD(wParam) == STN_CLICKED )
                   {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("Label: STN_CLICKED received!");
#endif                       
                         VCLLabelProperties *Prop = (VCLLabelProperties *)VCLCom->Properties;    
                         Prop->Event_Clicked = 1;                   
                   } else
                   if ( HIWORD(wParam) == STN_DBLCLK )
                   {
#ifdef SHOWWINDOWMESSAGES
                   VMBDebug("Label: STN_DBLCLK received!");
#endif                       
                         VCLLabelProperties *Prop = (VCLLabelProperties *)VCLCom->Properties;    
                         Prop->Event_DblClicked = 1;                   
                   }
                }                      
             }
             break;                
        case WM_CLOSE:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Form: WM_CLOSE received!");
#endif               
             VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
             if (VCLCom)
             {
                VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                if (Prop->canClose)
                {
                    Prop->Event_Closed = 1;
                    ShowWindow(hwnd, SW_HIDE);
                }
                return 0;
             }             
             break;
        case WM_CTLCOLOREDIT:    
             VCLCom = (VMBVar*)VMBFindVarByHWND((HWND)lParam);
             if ( VCLCom != NULL )
             {
                 if ( VCLCom->VarType == VARTYPE_EDIT )
                 { 
                     VCLEditProperties *Prop = (VCLEditProperties *)VCLCom->Properties;   
                     SetBkColor((HDC)wParam, Prop->BkColor); 
                     SetTextColor((HDC)wParam, Prop->Color);
                     return (LRESULT)Prop->Background;
                 }                 
                 if ( VCLCom->VarType == VARTYPE_TEXTBOX )
                 { 
                     VCLTextBoxProperties *Prop = (VCLTextBoxProperties *)VCLCom->Properties;   
                     SetBkColor((HDC)wParam, Prop->BkColor); 
                     SetTextColor((HDC)wParam, Prop->Color);
                     return (LRESULT)Prop->Background;
                 }                 
                 
             }
             break;
        case WM_CTLCOLORSTATIC:    
             VCLCom = (VMBVar*)VMBFindVarByHWND((HWND)lParam);
             if ( VCLCom != NULL )
             {
                 if ( VCLCom->VarType == VARTYPE_LABEL )
                 { 
                     VCLLabelProperties *Prop = (VCLLabelProperties *)VCLCom->Properties;   
                     
                     SetTextColor((HDC)wParam, Prop->Color);
                     if ( Prop->Transparent )
                     {
                        SetBkMode(GetDC(VCLCom->hWnd), TRANSPARENT);     
                        return (LRESULT)GetStockObject(HOLLOW_BRUSH);
                     } else {
                        SetBkColor((HDC)wParam, Prop->BkColor);     
                        HBRUSH hBrush = CreateSolidBrush(Prop->BkColor);                            
                        SetBkMode(GetDC(VCLCom->hWnd), OPAQUE);                                 
                        return (LRESULT)hBrush;
                     }
                     
                 }
                 if ( VCLCom->VarType == VARTYPE_CHECKBOX )
                 { 
                     VCLCheckBoxProperties *Prop = (VCLCheckBoxProperties *)VCLCom->Properties;   
                     SetBkColor((HDC)wParam, Prop->BkColor); 
                     SetTextColor((HDC)wParam, Prop->Color);
                     HBRUSH hBrush = CreateSolidBrush(Prop->BkColor);                          
                     return (LRESULT)hBrush;
                 } 
                 if ( VCLCom->VarType == VARTYPE_RADIOBUTTON )
                 { 
                     VCLRadioButtonProperties *Prop = (VCLRadioButtonProperties *)VCLCom->Properties;   
                     SetBkColor((HDC)wParam, Prop->BkColor); 
                     SetTextColor((HDC)wParam, Prop->Color);
                     HBRUSH hBrush = CreateSolidBrush(Prop->BkColor);                          
                     return (LRESULT)hBrush;
                 }                 
             }
             break;  
             
        case WM_LBUTTONUP:
             VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
             if (VCLCom)
             {
                if ( VCLCom->VarType == VARTYPE_FORM ) 
                {        
#ifdef SHOWWINDOWMESSAGES
                    VMBDebug("Form: WM_LBUTTONUP received!");
#endif                                        
                    VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                    Prop->Event_Clicked = 1;
                    Prop->Event_ClickedX = LOWORD(lParam);
                    Prop->Event_ClickedY = HIWORD(lParam);                
                    return 0;
                }
             }             
             break;
        case WM_RBUTTONUP:
             VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
             if (VCLCom)
             {
                if ( VCLCom->VarType == VARTYPE_FORM ) 
                {         
#ifdef SHOWWINDOWMESSAGES
                    VMBDebug("Form: WM_RBUTTONUP received!");
#endif                                         
                    VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                    Prop->Event_RClicked = 1;
                    Prop->Event_RClickedX = LOWORD(lParam);
                    Prop->Event_RClickedY = HIWORD(lParam);                
                    return 0;
                }
             }             
             break;               
        case WM_MOVE:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Form: WM_MOVE received!");
#endif               
             VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
             if (VCLCom)
             {      
                if ( VCLCom->VarType == VARTYPE_FORM )
                {    
                    VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                    Prop->Event_Moved = 1;
                    return 0;
                }
             }   
             return 0;      
             break;        
#ifdef FORM_DOUBLEBUFFERED             
        case WM_ERASEBKGND:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Form: WM_ERASEBKGND received!");
#endif              
             return 1;
             break;
#endif             
        case WM_PAINT:
#ifdef SHOWWINDOWMESSAGES             
             VMBDebug("Form: WM_PAINT received!");
#endif
             GetClientRect(hwnd, &Re);
             hDC = BeginPaint(hwnd, &PS);                                     
#ifdef FORM_DOUBLEBUFFERED
             hdcMem = CreateCompatibleDC(hDC);
             hbmMem = CreateCompatibleBitmap(hDC, Re.right, Re.bottom);
             hbmOld = SelectObject(hdcMem, hbmMem);             
#endif
             VCLCom = (VMBVar *)VMBFindVarByHWND(hwnd);
             if (VCLCom)
             {
                VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                Br = Prop->Background;
             } else {
                Br = GetSysColorBrush((int)COLOR_BTNFACE);                     
             }
#ifdef FORM_DOUBLEBUFFERED                 
             FillRect(hdcMem, &Re, Br);
#else
             FillRect(hDC, &Re, Br);
             DeleteDC(hDC);
#endif
//                 DeleteObject(Br);
#ifndef FORM_DOUBLEBUFFERED             
             //EnumChildWindows(hwnd, &DrawChildWindows, 1);               
#endif   
#ifdef FORM_DOUBLEBUFFERED
             //EnumChildWindows(hwnd, &DrawChildWindows, (INT)(HDC)hdcMem);               
             BitBlt(hDC, Re.left, Re.top, Re.right-Re.left, Re.bottom-Re.top, hdcMem, 0, 0, SRCCOPY);             
             SelectObject(hdcMem, hbmOld);
             DeleteObject(hbmMem);
             DeleteDC(hdcMem);
#endif
             EndPaint(hwnd, &PS); 
          
             VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
             if ( VCLCom )
             {      
                if ( VCLCom->VarType == VARTYPE_FORM )
                {    
                    VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                    Prop->Event_Painted = 1;
                    return 0;
                }
             }                               
             return 0;          
             break; 

     }
     return DefWindowProc(hwnd, uMsg, wParam, lParam);        
}

// -----------------------------------------------------------------------------
//  VCL_ChangeLayeredProperties(hwnd) - Setzt AlphaBlend/Transparent Eigenschaften
// -----------------------------------------------------------------------------
void VCL_ChangeLayeredProperties(HWND hwnd)
{
          VMBVar *VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
          if ( VCLCom )
          {
             if ( VCLCom->Properties )     
             {
                VCLFormProperties *Properties = (VCLFormProperties *)VCLCom->Properties;
                int iStyle = GetWindowLong(hwnd, GWL_EXSTYLE);     
                if ( !Properties->isTransparent && !Properties->isAlphaBlending )
                {
                   SetWindowLong(hwnd, GWL_EXSTYLE, iStyle & !WS_EX_LAYERED );
                   RedrawWindow(hwnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                } else {
                   int iFlags = 0;
                   SetWindowLong(hwnd, GWL_EXSTYLE, iStyle | WS_EX_LAYERED );
                   if ( Properties->isAlphaBlending )
                   {
                      iFlags = iFlags | 2;     
                   }
                   if ( Properties->isTransparent )
                   {
                      iFlags = iFlags | 1;     
                   }                   
                   VMBSetLayeredWindowAttributes(hwnd, Properties->TransparentColor, Properties->AlphaValue, iFlags);
                }
             }
          }
}

void VCL_FormSetClientRect(HWND hwnd, int Width, int Height)
{
  RECT R, R2;
  BOOL Menu;

  SetRect(&R, 0, 0, Width, Height);

  if ( GetMenu(hwnd) != 0 )
   Menu = TRUE;
  else
   Menu = FALSE;


  AdjustWindowRectEx(&R, GetWindowLong(hwnd, GWL_STYLE), Menu, GetWindowLong(hwnd, GWL_EXSTYLE));
  GetWindowRect(hwnd, &R2);
  MoveWindow(hwnd, R2.left, R2.top, R.right - R.left, R.bottom - R.top, TRUE);
}

// -----------------------------------------------------------------------------
//  VCL_FormSetIcons(hwnd, int) - setzt die Symbole eines Fensters
// -----------------------------------------------------------------------------
void VCL_FormSetIcons(HWND hwnd, int i)
{
     DWORD cStyle = GetWindowLong(hwnd, GWL_STYLE);
     cStyle = cStyle & ~(WS_SYSMENU | WS_MAXIMIZEBOX | WS_MINIMIZEBOX);
     if ( i & 1 )     
        cStyle = cStyle | WS_SYSMENU;
     if ( i & 2 )
        cStyle = cStyle | WS_MAXIMIZEBOX;
     if ( i & 4 )
        cStyle = cStyle | WS_MINIMIZEBOX;
     SetWindowLong(hwnd, GWL_STYLE, cStyle);
     SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME);
     RedrawWindow(hwnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
}

// -----------------------------------------------------------------------------
//  VCL_FormSetCursor(hwnd, int) - setzt den Cursor für ein Fenster
// -----------------------------------------------------------------------------
void VCL_FormSetCursor(HWND hwnd, int i)
{
     int iCursors[] = {(int)IDC_ARROW, (int)IDC_APPSTARTING, (int)IDC_WAIT, (int)IDC_CROSS, (int)IDC_IBEAM, (int)IDC_NO, (int)IDC_SIZE, (int)IDC_SIZEALL, (int)IDC_SIZENESW, (int)IDC_SIZENS, (int)IDC_SIZENWSE, (int)IDC_SIZEWE, (int)IDC_UPARROW, (int)IDC_ICON};
     if (i < 0 || i > 13)
        i = 0;
     SetClassLong(hwnd, GCL_HCURSOR, (LONG)LoadCursor(0, MAKEINTRESOURCE(iCursors[i])));
     VMBVar *VCLCom = (VMBVar*)VMBFindVarByHWND(hwnd);
}

// -----------------------------------------------------------------------------
//  VCL_FormSetStyle(hwnd, int) - setzt den Style eines Fensters
// -----------------------------------------------------------------------------
void VCL_FormSetStyle(HWND hwnd, int i)
{
     DWORD cStyle = GetWindowLong(hwnd, GWL_STYLE);
     DWORD xStyle = GetWindowLong(hwnd, GWL_EXSTYLE);
     cStyle = cStyle & ~(WS_CAPTION | WS_THICKFRAME | WS_POPUP | WS_BORDER );
     xStyle = xStyle & ~(WS_EX_TOOLWINDOW);     
     if (i < 0 || i > 4)
        i = 0;     
     switch(i)
     {
              case 0:
                    cStyle = cStyle | WS_CAPTION | WS_THICKFRAME;
                    break;
              case 1:
                    cStyle = cStyle | WS_CAPTION | WS_BORDER;
                    break;
              case 2:
                    cStyle = cStyle | WS_POPUP;
                    break;
              case 3:
                    xStyle = xStyle | WS_EX_TOOLWINDOW;
                    cStyle = cStyle | WS_CAPTION | WS_THICKFRAME;
                    break;
              case 4:
                    xStyle = xStyle | WS_EX_TOOLWINDOW;
                    cStyle = cStyle | WS_CAPTION | WS_BORDER;
                    break;
                     
     }           
     SetWindowLong(hwnd, GWL_STYLE, cStyle);
     SetWindowPos(hwnd, HWND_TOP, 0, 0, 0, 0, SWP_NOZORDER|SWP_NOMOVE|SWP_NOSIZE|SWP_NOACTIVATE|SWP_DRAWFRAME);
     RedrawWindow(hwnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
}

// -----------------------------------------------------------------------------
//  VCL_GetCoords(hwnd, int*, int*, int*, int*) - liest die Koordinaten eines Fensters
// -----------------------------------------------------------------------------
void VCL_GetCoords(HWND hWnd, int *iX, int *iY, int *iW, int *iH)
{
     RECT rClient, rParent;
     GetWindowRect(hWnd, &rParent);    
     if ( iX ) 
     {
        *iX = rParent.left;
     }
     if ( iY )
     {
        *iY = rParent.top;    
     }
     if ( iH )
     {
        *iH = rParent.bottom - rParent.top;
     }
     if ( iW )
     {
        *iW = rParent.right - rParent.left;   
     }
}

// -----------------------------------------------------------------------------
//  VCL_GetCCoords(hwnd, int*, int*, int*, int*) - liest die Koordinaten eines Fensters
// -----------------------------------------------------------------------------
void VCL_GetCCoords(HWND hWnd, int *iX, int *iY, int *iW, int *iH)
{
     RECT rClient, rParent;
     POINT XY;
     BOOL blubb;
     GetClientRect(hWnd, &rParent); 
     GetWindowRect(hWnd, &rClient);          
     XY.x = rClient.left;
     XY.y = rClient.top;     
     blubb = ScreenToClient(GetParent(hWnd), &XY);
     if ( iX ) 
     {
        *iX = XY.x;
     }
     if ( iY )
     {
        *iY = XY.y;
     }
     if ( iH )
     {
        *iH = rParent.bottom;
     }
     if ( iW )
     {
        *iW = rParent.right;   
     }
}

// -----------------------------------------------------------------------------
//  VCL_Center(hwnd) - zentriert ein Fenster
// -----------------------------------------------------------------------------
void VCL_Center(HWND hWnd)
{
 RECT rClient, rParent;
 HWND Parent = GetParent(hWnd);
 if (Parent == NULL)
 {
  Parent = GetDesktopWindow();           
 }
 if (Parent != NULL && hWnd != NULL);
 {
            GetWindowRect(Parent, &rParent);
            GetClientRect(hWnd,   &rClient);
            rClient.left = (rParent.right - rClient.right ) / 2;
            rClient.top  = (rParent.bottom - rClient.bottom ) / 2;            
            SetWindowPos(hWnd, HWND_TOPMOST, rClient.left, rClient.top, rClient.right, rClient.bottom, SWP_NOZORDER | SWP_NOSIZE);
 }
 
}

// -----------------------------------------------------------------------------
//  VCL_FormMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_FormMethod(HWND hWnd, unsigned int Method)
{
#if VERBOSE > 2
    VMBDebug(" -> %s(%08x, %04i)", __func__, hWnd, Method);
#endif     
     VMBVar *VCLCom = (VMBVar*)VMBFindVarByHWND(hWnd);
     if (!VCLCom)
     {
        VCLError(306);            
     }     
     char *P1, *P2, *P3, *P4, *P5;
     VCLFormProperties *Prop = (VCLFormProperties*)VCLCom->Properties;
     if ( hWnd != NULL )
     {
        switch(Method) 
        {
                       case 1:
                            ShowWindow(hWnd, SW_SHOWNORMAL);
                            break;
                       case 2:
                            ShowWindow(hWnd, SW_HIDE);
                            break;
                       case 3:
                            VCL_Center(hWnd);
                            break;
                       case 4:
                            ShowWindow(hWnd, SW_MINIMIZE);         
                            break;
                       case 5:
                            ShowWindow(hWnd, SW_MAXIMIZE);         
                            break;                 
                       case 6: 
                            RedrawWindow(hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;
                       case 7: 
                            SetForegroundWindow(hWnd);
                            SetFocus(hWnd);
                            break;
                       case 8: 
                            EnableWindow(hWnd, FALSE);          
                            break;
                       case 9: 
                            EnableWindow(hWnd, FALSE);          
                            break;   
                       case 10:
                            Prop->Event_Clicked = 0;
                            Prop->Event_Closed = 0;
                            Prop->Event_RClicked = 0;
                            Prop->Event_ClickedX = 0;
                            Prop->Event_ClickedY = 0;
                            Prop->Event_RClickedX = 0;
                            Prop->Event_RClickedY = 0;
                            Prop->Event_Moved = 0;
                            Prop->Event_Painted = 0;
                            break;              
                       case 11:
                            P1 = (char*)VMBParseOneString();
                            P2 = (char*)VMBParseOneString();
                            P3 = (char*)VMBParseOneString();
                            P4 = (char*)VMBParseOneString();                                                                                    
                            MoveWindow(hWnd, atoi(P1), atoi(P2), atoi(P3), atoi(P4), TRUE);
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
                            SetWindowText(hWnd, P1);                                                              
                            MoveWindow(hWnd, atoi(P2), atoi(P3), atoi(P4), atoi(P5), TRUE);
                            P1 = (char*)VMBFree(P1);
                            P2 = (char*)VMBFree(P2);
                            P3 = (char*)VMBFree(P3);
                            P4 = (char*)VMBFree(P4);  
                            P5 = (char*)VMBFree(P5);                                                                                  
                            VMBPrevCode();
                            break;  
                       case 13:
                            P1 = (char*)VMBParseOneString();
                            P2 = (char*)VMBParseOneString();
                            P3 = (char*)VMBAlloc(32);
                            sprintf(P3, "%d", GetPixel(GetDC(hWnd), atoi(P1), atoi(P2)));
                            VMBPushRStack(P3, VMBStrLen(P3));
                            P1 = (char*)VMBFree(P1);
                            P2 = (char*)VMBFree(P2);
                            P3 = (char*)VMBFree(P3);
                            VMBPrevCode();
                            break; 
                       case 14:
                            P1 = (char*)VMBParseOneString();
                            P2 = (char*)VMBParseOneString();
                            P3 = (char*)VMBParseOneString();                            
                            SetPixel(Prop->hDC, atoi(P1), atoi(P2), (COLORREF)atoi(P3));
                            P1 = (char*)VMBFree(P1);
                            P2 = (char*)VMBFree(P2);
                            P3 = (char*)VMBFree(P3);                            
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
}
 
// -----------------------------------------------------------------------------
//  VCL_FormMember(hwnd, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_FormMember(HWND hWnd, unsigned int Method)
{
     char *szBuffer1 = NULL;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     VMBVar *VCLCom;
     VCLFormProperties *Properties;
#if VERBOSE > 2
    VMBDebug(" -> %s(%08x, %04i)", __func__, hWnd, Method);
#endif     
     VCLCom = (VMBVar*)VMBFindVarByHWND(hWnd);
     if (!VCLCom)
     {
        VCLError(300);            
     }
     Properties = (VCLFormProperties *)VCLCom->Properties;
     szBuffer1 = (char *)VMBParseOneString();
     if ( hWnd != NULL )
     {
        switch(Method) 
        {
                       case 1:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(hWnd, iBuffer, iY, iW, iH, TRUE);                                                        
                            break;
                       case 2:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(hWnd, iX, iBuffer, iW, iH, TRUE);                            
                            break;                            
                       case 3:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(hWnd, iX, iY, iBuffer, iH, TRUE);                            
                            break;   
                       case 4:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            MoveWindow(hWnd, iX, iY, iW, iBuffer, TRUE);                            
                            break;  
                       case 5:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             ShowWindow(hWnd, SW_HIDE);
                            } else {
                             ShowWindow(hWnd, SW_SHOW);    
                            }
                            break;                              
                       case 6:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             EnableWindow(hWnd, 0);
                            } else {
                             EnableWindow(hWnd, 1);    
                            }
                            break;                                                      
                       case 7:
                            SetWindowText(hWnd, szBuffer1);
                            break;    
                       case 8:
                            iBuffer = atoi(szBuffer1);
                            VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
                            Prop->BkColor = iBuffer;   
                            DeleteObject(Prop->Background);
                            Prop->Background  = CreateSolidBrush(Prop->BkColor);
                            RedrawWindow(hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );                                         
                            break;        
                       case 9:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->isTransparent = 0;   
                            } else {
                               Properties->isTransparent = 1;
                            }
                            VCL_ChangeLayeredProperties(hWnd);                            
                            break;   
                       case 10:
                            Properties->TransparentColor = atoi(szBuffer1);                               
                            VCL_ChangeLayeredProperties(hWnd);                            
                            break;
                       case 11:
                            Properties->Cursor = atoi(szBuffer1);
                            VCL_FormSetCursor(hWnd, (int)Properties->Cursor);
                            break;
                       case 13:
                            Properties->Style = atoi(szBuffer1);
                            VCL_FormSetStyle(hWnd, (int)Properties->Style);
                            break;
                       case 15:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer < 0)
                               iBuffer = 0;
                            if (iBuffer > 7)
                               iBuffer = 7;
                            VCL_FormSetIcons(hWnd, iBuffer);                     
                            break;                                                                                                                               
                       case 17:
                            iBuffer = atoi(szBuffer1);
                            SetParent(hWnd, (HWND)iBuffer);                     
                            break; 
                       case 18:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->Event_Clicked = 0;   
                            } else {
                               Properties->Event_Clicked = 1;
                            }
                            break;                               
                       case 19:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->isAlphaBlending = 0;   
                            } else {
                               Properties->isAlphaBlending = 1;
                            }
                            VCL_ChangeLayeredProperties(hWnd);
                            break;   
                       case 20:
                            Properties->AlphaValue = atoi(szBuffer1);                               
                            VCL_ChangeLayeredProperties(hWnd);
                            break;   
                       case 21:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->canClose = 0;   
                            } else {
                               Properties->canClose = 1;
                            }
                            VCL_ChangeLayeredProperties(hWnd);
                            break;    
                       case 22:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->Event_Closed = 0;   
                            } else {
                               Properties->Event_Closed = 1;
                            }
                            break; 
                       case 23:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->Event_RClicked = 0;   
                            } else {
                               Properties->Event_RClicked = 1;
                            }
                            break;   
                       case 24:
                            Properties->Event_ClickedX = atoi(szBuffer1);
                            break;       
                       case 25:
                            Properties->Event_ClickedX = atoi(szBuffer1);
                            break;                                                                                                                                                                                   
                       case 26:
                            Properties->Event_RClickedX = atoi(szBuffer1);
                            break;                               
                       case 27:
                            Properties->Event_RClickedY = atoi(szBuffer1);
                            break;                               
                       case 28:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->Event_Moved = 0;   
                            } else {
                               Properties->Event_Moved = 1;
                            }
                            break;   
                       case 29:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(hWnd, &iX, &iY, &iW, &iH);
                            VCL_FormSetClientRect(hWnd, iBuffer, iH);
                            break;   
                       case 30:
                            iBuffer = atoi(szBuffer1);
                            VCL_GetCCoords(hWnd, &iX, &iY, &iW, &iH);
                            VCL_FormSetClientRect(hWnd, iW, iBuffer);                            
                            break; 
                       case 31:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                               Properties->Event_Painted = 0;   
                            } else {
                               Properties->Event_Painted = 1;
                            }
                            break;                                                                                   
                       default:
#ifdef DEBUG                              
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VCLError(301);
                            break;               
        }
     }
     VMBNextCode();
     szBuffer1 = (char *)VMBFree(szBuffer1);
}

// -----------------------------------------------------------------------------
//  VCL_FormGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_FormGetMember(HWND hWnd, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     char TempBuffer[16];
     VMBVar *VCLCom;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
#if VERBOSE > 2
    VMBDebug(" -> %s(%08x, %04i)", __func__, hWnd, Method);
#endif     
     VCLCom = (VMBVar*)VMBFindVarByHWND(hWnd);
     if (!VCLCom)
     {
        VCLError(302);            
     }
     VCLFormProperties *Prop = (VCLFormProperties *)VCLCom->Properties;
     if ( hWnd != NULL )
     {
        switch(Method) 
        {
                       case 1:
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iX);
                            break;  
                       case 2:
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iY);
                            break;  
                       case 3:
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iW);
                            break; 
                       case 4:
                            VCL_GetCoords(hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iH);
                            break;  
                       case 5:                            
                            if (IsWindowVisible(hWnd))
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;    
                       case 6:                            
                            if (IsWindowEnabled(hWnd))
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                                                      
                       case 7:
                            szBuffer = (char*)VMBFree(szBuffer);
                            iX = GetWindowTextLength(hWnd);
                            szBuffer = (char*)VMBAlloc(iX+2);
                            GetWindowText(hWnd, szBuffer, iX+1);
                            break;
                       case 8:
                            sprintf(szBuffer, "%i", Prop->BkColor);
                            break;  
                       case 9:
                            if (Prop->isTransparent)
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;    
                       case 10:
                            sprintf(szBuffer, "%i", Prop->TransparentColor);
                            break;  
                       case 11:
                            sprintf(szBuffer, "%i", Prop->Cursor);
                            break;     
                       case 13:
                            sprintf(szBuffer, "%i", Prop->Style);
                            break;                              
                       case 15:
                            sprintf(szBuffer, "%i", Prop->Icons);
                            break;                              
                       case 16:
                            sprintf(szBuffer, "%i", hWnd);
                            break; 
                       case 17:                            
                            sprintf(szBuffer, "%i", GetParent(hWnd));
                            break;   
                       case 18:
                            if ( Prop->Event_Clicked )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                                                       
                       case 19:                            
                            if (Prop->isAlphaBlending)
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                             
                       case 20:
                            sprintf(szBuffer, "%i", Prop->AlphaValue);
                            break;  
                       case 21:                            
                            if (Prop->canClose)
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;  
                       case 22:
                            if ( Prop->Event_Closed )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;   
                       case 23:
                            if ( Prop->Event_RClicked )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break; 
                       case 24:                            
                            sprintf(szBuffer, "%i", Prop->Event_ClickedX);
                            break;           
                       case 25:                            
                            sprintf(szBuffer, "%i", Prop->Event_ClickedY);
                            break;           
                       case 26:                            
                            sprintf(szBuffer, "%i", Prop->Event_RClickedX);
                            break;
                       case 27:                            
                            sprintf(szBuffer, "%i", Prop->Event_RClickedY);
                            break;                                                                                                                                                                                                                                                                                                                                                                      
                       case 28:
                            if ( Prop->Event_Moved )
                            {
                               sprintf(szBuffer, "1");
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;   
                       case 29:
                            VCL_GetCCoords(hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iW);
                            break; 
                       case 30:
                            VCL_GetCCoords(hWnd, &iX, &iY, &iW, &iH);
                            sprintf(szBuffer, "%i", iH);
                            break;  
                       case 31:
                            if ( Prop->Event_Painted )
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
     }
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     szBuffer = (char*)VMBFree(szBuffer);
}


#endif
