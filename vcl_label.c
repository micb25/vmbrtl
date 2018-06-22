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

#ifndef VCL_LABEL_C
#define VCL_LABEL_C

#include <windows.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

LRESULT CALLBACK VCL_LabelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
void VCL_LabelUpdateFont(VCLLabelProperties *Prop, HWND hwnd);
void VCL_LabelUpdateAlign(VCLLabelProperties *Prop, HWND hwnd);
int iVCLLabelCount = 0;

WNDPROC *OldLabelProc;

extern HWND LastCreatedWindow;
void VCL_LabelAdjustBounds(VMBVar *VCLCom);

// -----------------------------------------------------------------------------
//  VCL_LabelCreate(char *) - erzeugt eine Label Komponente
// -----------------------------------------------------------------------------
HWND VCL_LabelCreate(char *Caption)
{
 HWND hWnd;
 hWnd = CreateWindow("STATIC", NULL,WS_CLIPSIBLINGS| SS_LEFTNOWORDWRAP | SS_NOTIFY | SS_LEFT | WS_VISIBLE | WS_CHILD, CW_USEDEFAULT, CW_USEDEFAULT, 75, 25, LastCreatedWindow, NULL, RTL_HINSTANCE, NULL);  
 if (hWnd != NULL)
 {
   OldLabelProc = (WNDPROC*)GetWindowLong(hWnd, GWL_WNDPROC);
   SetWindowLong(hWnd, GWL_WNDPROC, (LONG)&VCL_LabelProc);          
   SetWindowText(hWnd, Caption);  
   iVCLLabelCount++;
   return hWnd;
 } else {
   VCLError(306);
 }
}

// -----------------------------------------------------------------------------
//  VCL_LabelMethod(hwnd, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_LabelMethod(VMBVar *VCLCom, unsigned int Method)
{
     char *P1, *P2, *P3, *P4, *P5;
     VCLLabelProperties *Prop = (VCLLabelProperties*)VCLCom->Properties;
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
                            EnableWindow(VCLCom->hWnd, FALSE);
                            break;                                                     
                       case 5:
                            EnableWindow(VCLCom->hWnd, TRUE);
                            break;     
                       case 6:
                            Prop->Event_Clicked = 0;
                            Prop->Event_DblClicked = 0;
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
//  VCL_LabelMember(hwnd, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_LabelMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer1 = NULL;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     VCLLabelProperties *Prop = (VCLLabelProperties *)VCLCom->Properties;
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
                            if ( Prop->AutoSize )
                               VCL_LabelAdjustBounds(VCLCom);
                            else
                               RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE ); 
                            break; 
                       case 8:
                            iBuffer = atoi(szBuffer1);
                            SetParent(VCLCom->hWnd, (HWND)iBuffer);                     
                            break;  
                       case 9:
                            iBuffer = atoi(szBuffer1);
                            if (!iBuffer)                    
                            {
                             Prop->AutoSize = 0;
                            } else {
                             Prop->AutoSize = 1;
                             VCL_LabelAdjustBounds(VCLCom);
                            }
                            break;  
                       case 10:                    
                            strncpy(Prop->FontName, szBuffer1, 255);                        
                            VCL_LabelUpdateFont(Prop, VCLCom->hWnd);
                            if ( Prop->AutoSize )
                               VCL_LabelAdjustBounds(VCLCom);
                            break;                            
                       case 11:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontSize = iBuffer;
                            VCL_LabelUpdateFont(Prop, VCLCom->hWnd);
                            if ( Prop->AutoSize )
                               VCL_LabelAdjustBounds(VCLCom);                            
                            break;                                                        
                       case 12:
                            Prop->Color = atoi(szBuffer1);
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;    
                       case 13:
                            Prop->BkColor = atoi(szBuffer1);
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;  
                       case 14:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer)
                            {
                              Prop->Transparent = 1;                                   
                            } else {
                              Prop->Transparent = 0;
                            }
                            RedrawWindow(VCLCom->hWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
                            break;                             
                       case 15:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer)
                            {
                              Prop->Event_Clicked = 1;                                   
                            } else {
                              Prop->Event_Clicked = 0;
                            }
                            break;  
                       case 16:
                            iBuffer = atoi(szBuffer1);
                            if (iBuffer)
                            {
                              Prop->Event_DblClicked = 1;                                   
                            } else {
                              Prop->Event_DblClicked = 0;
                            }
                            break;                                                        
                       case 17:
                            iBuffer = atoi(szBuffer1);
                            Prop->FontStyle = iBuffer;
                            VCL_LabelUpdateFont(Prop, VCLCom->hWnd);
                            break; 
                       case 18:
                            iBuffer = atoi(szBuffer1);
                            Prop->Align = iBuffer;
                            VCL_LabelUpdateAlign(Prop, VCLCom->hWnd);
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
//  VCL_LabelGetMember(hwnd, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_LabelGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer = (char*)VMBAlloc(256);
     char TempBuffer[16];
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0; 
     VCLLabelProperties *Prop = (VCLLabelProperties *)VCLCom->Properties;
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
                            if (Prop->AutoSize)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;
                       case 10:
                            sprintf(szBuffer, "%s", (int)Prop->FontName);
                            break;                             
                       case 11:
                            sprintf(szBuffer, "%i", (int)Prop->FontSize);
                            break;    
                       case 12:
                            sprintf(szBuffer, "%i", (int)Prop->Color);
                            break; 
                       case 13:
                            sprintf(szBuffer, "%i", (int)Prop->BkColor);
                            break;                                                                                                                                                                       
                       case 14:
                            if (Prop->Transparent)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;  
                       case 15:
                            if (Prop->Event_Clicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;                            
                       case 16:
                            if (Prop->Event_DblClicked)
                            {
                               sprintf(szBuffer, "1");                        
                            } else {
                               sprintf(szBuffer, "0");                                   
                            }
                            break;  
                       case 17:
                            sprintf(szBuffer, "%i", (int)Prop->FontStyle);
                            break;
                       case 18:
                            sprintf(szBuffer, "%i", (int)Prop->Align);
                            break;   
                       case 19:
                            sprintf(szBuffer, "%i", (int)VCLCom->hWnd);
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
// VCL_LabelProc(int, int, int, int) - Callback für Labels
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_LabelProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     VMBVar *VCLCom;

     return  CallWindowProc( (WNDPROC)OldLabelProc, hwnd, uMsg, wParam, lParam);        
}

// -----------------------------------------------------------------------------
// VCL_LabelUpdateFont(PROP, HWND) - Verändert den Font des Labels
// -----------------------------------------------------------------------------
void VCL_LabelUpdateFont(VCLLabelProperties *Prop, HWND hwnd)
{
     DeleteObject(Prop->Font);
     Prop->Font = (HFONT)VMBCreateFont(Prop->FontName, Prop->FontSize, Prop->FontStyle);    
     SendMessage(hwnd, WM_SETFONT, (WPARAM)Prop->Font, (LPARAM)MAKELPARAM(TRUE, 0));
}

void VCL_LabelUpdateAlign(VCLLabelProperties *Prop, HWND hwnd)
{
     int iTemp = GetWindowLong(hwnd, GWL_STYLE);
     if (Prop->Align < 0 || Prop->Align > 2)
        Prop->Align = 0;
     
     iTemp = iTemp & ~( SS_LEFT | SS_CENTER | SS_RIGHT );
     
     switch (Prop->Align)
     {
            case 0:    
                 iTemp = iTemp | SS_LEFT;   
                 break;
            case 1:    
                 iTemp = iTemp | SS_CENTER;   
                 break;                 
            case 2:    
                 iTemp = iTemp | SS_RIGHT;   
                 break;                 
     }
             
     SetWindowLong(hwnd, GWL_STYLE, iTemp);
     RedrawWindow(hwnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
}

void VCL_LabelAdjustBounds(VMBVar *VCLCom)
{
    int iX, iY, iW, iH, i = 0, j = 0;
    char found = 0;
    char samecol = 0;
    SIZE dim; 
    RECT Rec;
    COLORREF OldBkColor, OldColor, TempCol;
    HBITMAP MyBit;

    HDC hDC = GetDC(VCLCom->hWnd);
    VCLLabelProperties *Prop = (VCLLabelProperties *)VCLCom->Properties;
    memset(&Rec, 0, sizeof(RECT));

    HFONT TempFont = SelectObject(hDC, Prop->Font);
    iX = GetWindowTextLength(VCLCom->hWnd);
    char *TextBuf = (char*)VMBAlloc(iX + 2);
    GetWindowText(VCLCom->hWnd, TextBuf, iX+1);    
    GetTextExtentPoint32(hDC, TextBuf, iX, &dim);
    DrawText(hDC, TextBuf, iX, &Rec, DT_CALCRECT | DT_EXPANDTABS );    

    hDC = CreateCompatibleDC(GetDC(VCLCom->hWnd));
    MyBit = CreateCompatibleBitmap(hDC, dim.cx + 5, Rec.bottom);
    SelectObject(hDC, MyBit);    
    TempFont = SelectObject(hDC, Prop->Font);
    
/*    OldBkColor = GetBkColor(hDC);
    OldColor = GetTextColor(hDC);
    
    if ( OldBkColor == OldColor )
    {
       samecol = 1; 
       if ( OldBkColor == 0x000000 )
          OldColor = 0xFFFFFF;
       else
          OldColor = 0x000000; 
          
       SetTextColor(hDC, OldColor);
    }*/

    
    SetTextColor(hDC, (COLORREF)RGB(255, 0, 0));
    SetBkColor(hDC, (COLORREF)RGB(0, 255, 0));
  /*  SelectObject(hDC, CreatePen(PS_SOLID, 1, (COLORREF)RGB(255, 0, 0)));*/
        
    Rec.right = dim.cx + 10;
    DrawText(hDC, TextBuf, iX, &Rec, DT_EXPANDTABS | DT_WORDBREAK );
    
    if ( dim.cx && Rec.bottom )
    {
        for ( i = dim.cx + 10; i > 0; i-- )
        {
            for ( j = Rec.bottom ; j > 0; j-- )
            {
                int p = GetPixel(hDC, i, j);
                if (  p != CLR_INVALID && p != 0x000000 )
                {
                   found = 1;
                   i++;
                   break;     
                }
            }
            if ( found )
               break;
        }
    }
    /*
    if ( samecol ) 
    {
        SetBkColor(hDC, (COLORREF) OldBkColor);
        SetTextColor(hDC, (COLORREF) OldBkColor);
    }
    SelectObject(hDC, TempFont);

    CancelDC(hDC);*/
    DeleteDC(hDC);

    VCL_GetCCoords(VCLCom->hWnd, &iX, &iY, &iW, &iH);
    if ( i == 0)
       i++;
    MoveWindow(VCLCom->hWnd, iX, iY, i, Rec.bottom, FALSE);
    TextBuf = VMBFree(TextBuf); 

    RedrawWindow(VCLCom->hWnd , NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );    
}

#endif

#endif
