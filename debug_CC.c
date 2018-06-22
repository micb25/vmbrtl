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

#ifdef DEBUGGER

#ifndef DEBUG_CC_C
#define DEBUG_CC_C

#include <windows.h>
#include <commctrl.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

HWND CC_Wnd = NULL;
HWND CC_LV = NULL;
HWND CC_hList = NULL;
HWND CC_hSrc = NULL;
HWND CC_hSrcView = NULL;
HWND CC_WndCC = NULL;
HWND CC_WndCCLabel = NULL;
HWND CC_MemAllocs = NULL;
HWND CC_WndSubLevel = NULL;
HWND CC_SRC = NULL;
HWND CC_Step = NULL;

HFONT MyFont = NULL;
extern int SubLevel;
extern int MemoryAllocated;
extern char isParsing;
extern char isStepMode;

unsigned int iItems = 0;
unsigned int CurrentSRCLine = 0;

const char DbgWndNameCC[] = "VMBDbgWndCC";


LRESULT CALLBACK DEBUG_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

HWND CreateLabelEditPair(char *Name, int top)
{
     HWND Label, Edit;
     HFONT MyFont  = (HFONT)VMBCreateFont("Courier New", 10, 0);    
     Edit = CreateWindow("EDIT", NULL, ES_READONLY | WS_VISIBLE | WS_BORDER | ES_LEFT	| WS_VISIBLE | WS_CHILD, 120, top, 250, 21, CC_Wnd, NULL, RTL_HINSTANCE, NULL);                            
     SendMessage(Edit, WM_SETFONT, (WPARAM)MyFont, (LPARAM)MAKELPARAM(TRUE, 0));    
     Label = CreateWindow("STATIC", Name, SS_LEFT | SS_SIMPLE | WS_VISIBLE | WS_CHILD, 5, top+2, 85, 21, CC_Wnd, NULL, RTL_HINSTANCE, NULL);
     SendMessage(Label, WM_SETFONT, (WPARAM)MyFont, (LPARAM)MAKELPARAM(TRUE, 0));    
     return Edit;
     
}

HWND CreateLabelEditPairWindow(char *Name, int top)
{
     HWND Label, Edit;
     HFONT MyFont  = (HFONT)VMBCreateFont("Courier New", 10, 0);    
     Edit = CreateWindow("EDIT", NULL, ES_READONLY | WS_VISIBLE | WS_BORDER | ES_LEFT	| WS_VISIBLE | WS_CHILD, 120, top, 250, 21, CC_Wnd, NULL, RTL_HINSTANCE, NULL);                            
     SendMessage(Edit, WM_SETFONT, (WPARAM)MyFont, (LPARAM)MAKELPARAM(TRUE, 0));    
     Label = CreateWindow("STATIC", Name, SS_LEFT | SS_SIMPLE | WS_VISIBLE | WS_CHILD, 5, top+2, 85, 21, CC_Wnd, NULL, RTL_HINSTANCE, NULL);
     SendMessage(Label, WM_SETFONT, (WPARAM)MyFont, (LPARAM)MAKELPARAM(TRUE, 0));    
     return Edit;
     
}

#ifdef CODEWINDOW
void UpdateCC(char *Str)
{
     if ( CC_WndCC != NULL )
     {
         char *szBuffer = (char*)VMBAlloc(16);
         if ( Str != NULL )                   
           SetWindowText(CC_WndCC, Str);
         else
           SetWindowText(CC_WndCC, "NULL");       
         sprintf(szBuffer, "%i", SubLevel);
         SetWindowText(CC_WndSubLevel, szBuffer);
         sprintf(szBuffer, "%i", MemoryAllocated);
         SetWindowText(CC_MemAllocs, szBuffer);         
         VMBFree(szBuffer);
         
         SendMessage(CC_SRC, EM_SETSEL, RTL_Code - RTL_CodeOrig, RTL_Code - RTL_CodeOrig + 4);         
         RedrawWindow(CC_Wnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
     }
}
#endif

void DEBUGGER_Init()
{
     WNDCLASS wc;
     RECT Desktop;
     RECT Wn;
     LVCOLUMN Column;
     
     ZeroMemory(&wc, sizeof(WNDCLASS));
     wc.style = 0;
     wc.hInstance = RTL_HINSTANCE;
     wc.hCursor = LoadCursor(NULL, IDC_ARROW);
     wc.hIcon = LoadIcon(RTL_HINSTANCE, "A");
     wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
     wc.lpfnWndProc = (WNDPROC)DEBUG_Proc;
     wc.lpszMenuName = NULL;
     wc.lpszClassName = DbgWndNameCC;
     
     if (RegisterClass(&wc) == 0)
      VMBPanic(13);
      
     MyFont = (HFONT)VMBCreateFont("Courier New", 8, 0); 
     GetClientRect(GetDesktopWindow(), &Desktop);     

#ifdef CODEWINDOW      
     CC_Wnd = CreateWindowEx(WS_EX_TOOLWINDOW, DbgWndNameCC, "VMB Debugger", WS_MINIMIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 400, 850, NULL, NULL, RTL_HINSTANCE, NULL);     
     SetWindowPos(CC_Wnd, HWND_TOPMOST, Desktop.right -  401, 0,0, 0, SWP_NOSIZE | SWP_NOZORDER);
    
     CC_WndCC = CreateLabelEditPair("CC:", 10);
     CC_WndSubLevel = CreateLabelEditPair("SUBLEVEL:", 36);     
     CC_MemAllocs = CreateLabelEditPair("MEMALLOCS:", 62);
             
     CC_SRC = CreateWindow("EDIT", NULL, WS_VSCROLL | ES_READONLY | ES_MULTILINE | ES_NOHIDESEL | WS_VISIBLE | WS_BORDER | ES_LEFT	| WS_VISIBLE | WS_CHILD, 5, 90, 380, 710, CC_Wnd, NULL, RTL_HINSTANCE, NULL);                                   
     SetWindowText(CC_SRC, RTL_Code);
     SendMessage(CC_SRC, WM_SETFONT, (WPARAM)(HFONT)VMBCreateFont("Courier New", 8, 0), (LPARAM)MAKELPARAM(TRUE, 0));                    
#endif

     // Variablen Fenster     
     CC_LV = CreateWindowEx(WS_EX_TOOLWINDOW, DbgWndNameCC, "globale Variablen", WS_SIZEBOX | WS_MINIMIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU, 0, 0, 600, 400, NULL, NULL, RTL_HINSTANCE, NULL);          
     GetClientRect(CC_LV, &Wn);
     CC_hList = CreateWindowEx(WS_EX_CLIENTEDGE, WC_LISTVIEW, "", WS_VISIBLE | WS_CHILD | LVS_REPORT | LVS_SHOWSELALWAYS, 0, 0, Wn.right-Wn.left, Wn.bottom-Wn.top, CC_LV,/*(HMENU)ID_LISTVIEW*/ NULL, RTL_HINSTANCE, NULL);
     SendMessage(CC_hList, /*LVM_SETEXTENDEDLISTVIEWSTYLE*/ 0x1000 + 54, 0, /*LVS_EX_FULLROWSELECT*/ 32 );

     ZeroMemory(&Column, sizeof(Column));
     Column.mask = LVCF_TEXT | LVCF_WIDTH | LVCF_SUBITEM;
     Column.cx = 250;
     Column.pszText = "Inhalt";
     SendMessage(CC_hList, LVM_INSERTCOLUMN, 0, (LPARAM)&Column);
     Column.cx = 80;     
     Column.pszText = "Adresse";
     SendMessage(CC_hList, LVM_INSERTCOLUMN, 0, (LPARAM)&Column);
     Column.cx = 80;     
     Column.pszText = "Typ";
     SendMessage(CC_hList, LVM_INSERTCOLUMN, 0, (LPARAM)&Column);
     Column.cx = 100;     
     Column.pszText = "Variable";
     SendMessage(CC_hList, LVM_INSERTCOLUMN, 0, (LPARAM)&Column);

     // Quellcode Fenster
     CC_hSrc = CreateWindowEx(WS_EX_TOOLWINDOW, DbgWndNameCC, "Quellcode", WS_TABSTOP | WS_MINIMIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU , Desktop.right - 600, 0, 600, 450, NULL, NULL, RTL_HINSTANCE, NULL);           
     GetClientRect(CC_hSrc, &Wn);
     CC_hSrcView = CreateWindowEx(WS_EX_CLIENTEDGE, "LISTBOX", "", LBS_DISABLENOSCROLL	| LBS_HASSTRINGS | WS_VISIBLE |  WS_BORDER | WS_CHILD , 0, 0, Wn.right, Wn.bottom-30, CC_hSrc, NULL, RTL_HINSTANCE, NULL);           
     SendMessage(CC_hSrcView, WM_SETFONT, (WPARAM)MyFont, (LPARAM)MAKELPARAM(TRUE, 0));                          	
     ShowScrollBar(CC_hSrcView, SB_BOTH, 1);
     CC_Step = CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", "Step",  WS_GROUP | WS_TABSTOP | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD , 5, Wn.bottom-33, 85, 25, CC_hSrc, NULL, RTL_HINSTANCE, NULL );
     SendMessage(CC_Step, WM_SETFONT, (WPARAM)MyFont, (LPARAM)MAKELPARAM(TRUE, 0));                          	

#ifdef CODEWINDOW           	
     UpdateCC("NULL");
     ShowWindow(CC_Wnd, SW_SHOW);
#endif
     
     ShowWindow(CC_LV, SW_SHOWNORMAL);     
     ShowWindow(CC_hSrc, SW_SHOWNORMAL);          
}

BOOL CALLBACK DrawChildWindow(HWND hwnd, LPARAM lParam)
{
     InvalidateRect(hwnd, NULL, TRUE);
     UpdateWindow(hwnd);
}

LRESULT CALLBACK DEBUG_Proc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     RECT Re;
     PAINTSTRUCT PS;
     HBRUSH Br;
     HDC hdcMem,hDC;
     HBITMAP hbmMem, hbmOld;
     switch(uMsg) {
        case WM_DESTROY:
             return 0;
             break;
        case WM_CLOSE:
             return 0;
             break;
        case WM_COMMAND:
             if ( (HWND) lParam == CC_Step ) 
             {
                 isStopped = FALSE;  
                 if ( !isParsing )   
                    MessageBox(CC_hSrc, "Das Programm wurde nicht gestartet!", "Fehler!", 16);
             }
             return 1;
             break;                             
        case WM_SIZING:
             if ( hwnd == CC_LV )
             {
                  RECT *MyRec = (LPRECT)lParam;
                  MoveWindow(CC_hList, 0, 0, MyRec->right - MyRec->left, MyRec->bottom - MyRec->top, 0);      
                  InvalidateRect(CC_hList, NULL, 0);
                  UpdateWindow(CC_hList);
             }
             return 1;
             break;
     }
     return DefWindowProc(hwnd, uMsg, wParam, lParam);        
}

void DEBUGGER_AddVar(VMBVar* MyVar)
{
     if ( MyVar )      
     {
        LVITEM NewItem;
        ZeroMemory(&NewItem, sizeof(LVITEM));
        
        NewItem.mask = LVIF_TEXT;
        NewItem.iItem = iItems;
        MyVar->iItem = iItems++;
        NewItem.cchTextMax = 255;
        NewItem.iSubItem = 0;
        NewItem.pszText = MyVar->DbgName;
        SendMessage(CC_hList, LVM_INSERTITEM, 0, (LPARAM)&NewItem);

        char *Buffer = (char*)VMBAlloc(32);
        sprintf(Buffer, "0x%08X", (int)MyVar);
        NewItem.iSubItem = 2;
        NewItem.pszText = Buffer;
        SendMessage(CC_hList, LVM_SETITEM, 0, (LPARAM)&NewItem);
        Buffer = (char*)VMBFree(Buffer);    
     }
}

void DEBUGGER_UpdateVarContent(VMBVar *MyVar)
{
     if ( MyVar )     
     {
        LVITEM NewItem;  
        ZeroMemory(&NewItem, sizeof(LVITEM));
        
        if ( MyVar->isLocal )
           return;
        
        NewItem.mask = LVIF_TEXT;
        NewItem.iItem = MyVar->iItem;
        if ( MyVar->VarType < VARTYPE_ARRAY ) 
        { 
            char *szBuffer = NULL;
            
            NewItem.iSubItem = 1;
            switch ( MyVar->VarType )
            {
                   case VARTYPE_INT:
                        NewItem.pszText = "Integer";
                        break;
                   case VARTYPE_STR:
                        NewItem.pszText = "String";
                        break;
                   case VARTYPE_BOOL:
                        NewItem.pszText = "Boolean";
                        break;
                   case VARTYPE_FLOAT:
                        NewItem.pszText = "Float";
                        break;
                   case VARTYPE_FORM:
                        NewItem.pszText = "Form";
                        break;
                   case VARTYPE_BUTTON:
                        NewItem.pszText = "Button";
                        break;
                   case VARTYPE_LABEL:
                        NewItem.pszText = "Label";
                        break;
                   case VARTYPE_TIMER:
                        NewItem.pszText = "Timer";
                        break;
                   case VARTYPE_FILE:
                        NewItem.pszText = "File";
                        break;      
                   case VARTYPE_EDIT:
                        NewItem.pszText = "Edit";
                        break;                                  
                   case VARTYPE_TEXTBOX:
                        NewItem.pszText = "Textbox";
                        break;                    
                   default:
                        NewItem.pszText = "Unbekannt";
                        break;       
            }
            SendMessage(CC_hList, LVM_SETITEM, 0, (LPARAM)&NewItem);            
            
            NewItem.cchTextMax = 255;
            NewItem.iSubItem = 3;
            if ( MyVar->VarType == VARTYPE_STR )
            {
               if ( MyVar->VarContent )
               {  
                  NewItem.pszText = MyVar->VarContent;     
               } else {
                  NewItem.pszText = "";       
               }
               SendMessage(CC_hList, LVM_SETITEM, 0, (LPARAM)&NewItem);
            } else
            if ( MyVar->VarType == VARTYPE_INT )
            {
               szBuffer = (char*)VMBAlloc(32);
               sprintf(szBuffer, "%i", *MyVar->VarContent);
               NewItem.pszText = szBuffer;     
               SendMessage(CC_hList, LVM_SETITEM, 0, (LPARAM)&NewItem);
               szBuffer = (char*)VMBFree(szBuffer);
            } 
            if ( MyVar->VarType == VARTYPE_BOOL )
            {
               szBuffer = (char*)VMBAlloc(32);
               if ( MyVar->VarContent == NULL )
                  NewItem.pszText = "FALSE";     
               else
                  NewItem.pszText = "TRUE";     
               SendMessage(CC_hList, LVM_SETITEM, 0, (LPARAM)&NewItem);
               szBuffer = (char*)VMBFree(szBuffer);
            } 
            
            UpdateWindow(CC_LV);    
        }
        
     }
}

void DEBUGGER_VarListReset()
{
     iItems = 0;
     SendMessage(CC_hList, /*LVM_DELETEALLITEMS*/ 0x1000 + 9, 0, 0);     
}

#endif

#endif

