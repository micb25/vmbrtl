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

#ifndef RTL_DBGWIN_C
#define RTL_DBGWIN_C


#define _WIN32_IE 0x500


#include <windows.h>
#include <commctrl.h>
#include <ole2.h>

#include "main.h"
#include "rtl_dbgwin.h"
#include "rtl_mem.h"
#include "rtl_vars.h"

#ifdef DEBUG

HWND l_hwnd;

void VMBTestFunc();

extern int ASMDemo(int i);

extern int MemoryAllocated;

extern char isTerminated;

extern HWND LastCreatedWindow;

const char DbgWndName[] = "VMBDbgWnd";

const char DbgHelp[] = "DEBUG COMMANDS:\r\n" \
                       "CC\t\tshows the current code\r\n" \
                       "CLS\t\tclears the screen\r\n" \
                       "DUMPCMDS\tdumps all commands into a file\r\n" \
                       "DUMPVARS\tdumps all variables into a file\r\n" \
                       "DUMPRSTACK\tdumps the return stack into a file\r\n" \
                       "DUMPSTACK\tdumps the stack into a file\r\n" \
                       "EXEC\t\texecutes a script with debug commands\r\n" \
                       "HELP\t\tshows the help\r\n" \
                       "LOAD\t\tloads VMBC code from a file\r\n" \
                       "PARSE\t\tparses the loaded vmbc code\r\n" \
                       "QUIT\t\tterminates the RTL\r\n" \
                       "RESET\t\tresets the whole RTL\r\n" \
                       "RUNMODE\t\texecutes the code without interrupts\r\n" \
                       "SAVELOG\t\tstores the debug output in a file\r\n" \
                       "SHOWCODE\tshows the loaded VMBC code\r\n" \
                       "STEP\t\texecutes the next code\r\n" \
                       "STEPMODE\texecutes the code step by step";

char blink = 255;

WNDPROC *OldImgProc = NULL;

LRESULT CALLBACK RTL_ImageProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     int i;
     switch(uMsg) {
                  
                  case WM_LBUTTONUP:
                       MessageBox(0, "LCLICK", "asdf", 32);
                       //return 0;
                       break;

     }
     
     return CallWindowProc( (WNDPROC)OldImgProc, hwnd, uMsg, wParam, lParam);   ;  
}

// -----------------------------------------------------------------------------
// VMBInitDebugWnd() - initialisiert das Debug Fenster
// -----------------------------------------------------------------------------
void VMBInitDebugWnd()
{
     HFONT hfont;
     int i;
     
     ZeroMemory(&wc, sizeof(WNDCLASS));
     wc.style = 0;
     wc.hInstance = RTL_HINSTANCE;
     wc.hCursor = LoadCursor(NULL, IDC_ARROW);
     wc.hIcon = LoadIcon(RTL_HINSTANCE, "A");
     wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
     wc.lpfnWndProc = (WNDPROC)RTL_DebugWndProc;
     wc.lpszMenuName = NULL;
     wc.lpszClassName = DbgWndName;
     
     if (RegisterClass(&wc) == 0)
      VMBCritical("interner Fehler #54");
     
     hDbgWnd = CreateWindow(DbgWndName, "CRTL - DEBUG Window", /* 0x10CC0800 */ WS_MINIMIZEBOX | WS_CAPTION | WS_BORDER | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 465, 650, NULL, NULL, RTL_HINSTANCE, NULL); 
          
     if (hDbgWnd == NULL)
      VMBCritical("Runtime-Fehler!\r\nDebug-Fenster konnte nicht erzeugt werden!");
          
     /* Log */
     hDbgEdit = CreateWindowEx(0x200, "EDIT", "CRTL - Debug Console\r\n--------------------", WS_HSCROLL	| WS_VSCROLL | WS_CHILD | ES_AUTOHSCROLL	| ES_MULTILINE | ES_LEFT | /* ES_READONLY	| */WS_VISIBLE, 8, 8, 441, 571, hDbgWnd, NULL, RTL_HINSTANCE, NULL);
	 hfont = CreateFont( -MulDiv( 8, GetDeviceCaps( GetDC(hDbgWnd), LOGPIXELSY), 72), 0, 0, 0, FW_LIGHT, 0, 0, 0, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FF_MODERN | FIXED_PITCH, "Courier New" );
     SendMessage(hDbgEdit, WM_SETFONT, (WPARAM)hfont, (LPARAM)MAKELPARAM(TRUE, 0));     
     SendMessage(hDbgEdit, EM_SETREADONLY, (WPARAM)(BOOL)TRUE, (LPARAM)0);
     
     /* Eingabezeile */
     hDbgLine = CreateWindowEx(0x200, "EDIT", NULL, 0x50010000 | WS_TABSTOP , 8, 586, 360, 22, hDbgWnd, NULL, RTL_HINSTANCE, NULL);
     oldWndProc = (WNDPROC)GetWindowLong(hDbgLine, GWL_WNDPROC);
     SetWindowLong(hDbgLine, GWL_WNDPROC, (LONG)&RTL_InputWndProc);
              
     /* Ok Button */
     hOk = CreateWindowEx(WS_EX_WINDOWEDGE, "BUTTON", NULL, WS_TABSTOP | BS_PUSHBUTTON | WS_VISIBLE | WS_CHILD , 376, 586, 73, 22, hDbgWnd, (HMENU)ID_OK, RTL_HINSTANCE, NULL);
     SendMessage(hOk, WM_SETTEXT, 0, (LPARAM)"OK");

//#ifndef FINAL          
     ShowWindow(hDbgWnd, SW_SHOWNORMAL); 
     SetFocus(hDbgLine);  
//#endif


/*
    
    OldImgProc = (WNDPROC*)GetWindowLong(l_hwnd, GWL_WNDPROC);   
    SetWindowLong(l_hwnd, GWL_WNDPROC, (DWORD)&RTL_ImageProc);    
    
    SendMessage(l_hwnd, LVM_SETBKCOLOR, 0, 255);
    
    LVBKIMAGE IBBkImg;
    ZeroMemory(&IBBkImg,sizeof(LVBKIMAGE));

    
    IBBkImg.ulFlags = LVBKIF_SOURCE_URL;
    
    //if you want the image not to have a tile, remove the style
    //LVBKIF_STYLE_TILE
    
    IBBkImg.pszImage="C:\\v.png";
    //you can replace C:\\my_image.bmp with any working image URL
    //from the Internet.
    //Try codeguru logo http://www.codeguru.com/img/logo.gif
    OleInitialize(NULL); //Initialize the OLE libraries
    //now load image
    SendMessage (l_hwnd,LVM_SETBKIMAGE,0,(LPARAM)(LPLVBKIMAGE) &IBBkImg);*/
}



// -----------------------------------------------------------------------------
// ParseCMD(char *) - verarbeitet einen Konsolenbefehl
// -----------------------------------------------------------------------------
void ParseCCMD(char *szBuffer)
{    
      int iIndex = 0;
      char *ParamStart = szBuffer;
    
      if (szBuffer == NULL)
         return;
      char *szCMD = (char *)VMBAlloc(strlen(szBuffer) + 1);
      while ( ( *(szBuffer+iIndex) != ' '  ) && ( *(szBuffer+iIndex) != '\t' ) && 
              ( *(szBuffer+iIndex) != '\0' ) && ( *(szBuffer+iIndex) != '\r' ) && 
              ( *(szBuffer+iIndex) != '\n' ) )
            iIndex++;            
      strncpy(szCMD, szBuffer, iIndex);
      ParamStart = szBuffer + iIndex + 1;
      VMBUpperCase(szCMD);
      if (!strcmp(szCMD, ""))
      {
           // NOP              
      }
      else if (!strcmp(szCMD, "CLS"))
      {
       SetWindowText(hDbgEdit, "");                   
      }  
      else if (!strcmp(szCMD, "MEMUSED"))
      {
       VMBDebug("%i times Memory allocated...", (int)MemoryAllocated);                
      }        
#ifndef FINAL        
      else if (!strcmp(szCMD, "LOAD"))
      {
       FILE *TempFile = NULL;
       TempFile = fopen(ParamStart, "r");
       if (!TempFile)
       {
        VMBDebug("can't load source %s!", ParamStart);              
       } else {
        VMBResetVars();              
        VMBResetStack();     
        VMBLoadSource(ParamStart);      
       }
      }
#endif      
      else if (!strcmp(szCMD, "EXEC"))
      {
       FILE *TempFile = NULL;
       char *TempLine;
       TempLine = VMBAlloc(256);
       TempFile = fopen(ParamStart, "r");
       if (!TempFile)
       {
        VMBDebug("can't execute script %s!", ParamStart);              
       } else {
         while (fgets(TempLine, 255, TempFile) > 0)
         {
               if ( TempLine[strlen(TempLine) - 1] == '\n' ) 
                  TempLine[strlen(TempLine) - 1] = '\0';
               VMBDebug("> %s", TempLine);
               ParseCCMD(TempLine);
               ZeroMemory(TempLine, 255);
         }
       }
       VMBFree(TempLine);
      }
      else if (!strcmp(szCMD, "SHOWCODE"))
      {
       VMBDebug("CODE:\r\n%s", RTL_CodeOrig);      
      }
      else if (!strcmp(szCMD, "BREAK"))
      {
       isTerminated = TRUE;
      }      
      else if (!strcmp(szCMD, "HELP"))
      {
       VMBDebug(DbgHelp);      
      }      
      else if (!strcmp(szCMD, "HIDE"))
      {
       ShowWindow(hDbgWnd, SW_HIDE);
      }         
      else if (!strcmp(szCMD, "DUMPVARS"))
      {
       VMBVarDump();        
      }      
      else if (!strcmp(szCMD, "DUMPCMDS"))
      {
       VMBCMDDump();        
      }      
      else if (!strcmp(szCMD, "DUMPSTACK")) 
      {
       VMBStackDump();
      }
      else if (!strcmp(szCMD, "DUMPRSTACK"))
      {
       VMBRStackDump();     
      }
      else if (!strcmp(szCMD, "PARSE"))
      {
       VMBParse();     
      }
      else if (!strcmp(szCMD, "RESET"))
      {
       VMBRTLReset();     
      }
      else if (!strcmp(szCMD, "RUNMODE"))
      {
       isStepMode = FALSE;
       VMBDebug("switched to non-step mode.");
      } 
      else if (!strcmp(szCMD, "STEPMODE"))
      {
       isStepMode = TRUE;
       VMBDebug("switched to step mode.");
      }      
      else if (!strcmp(szCMD, "STEP"))
      {
       isStopped = FALSE;
      }            
      else if (!strcmp(szCMD, "QUIT")) 
      {
       ExitProcess(0);
      }  
      else if (!strcmp(szCMD, "CC"))
      {
       char szTemp[5];
       if (RTL_Code != NULL)
       {
              VMBGetCurrentCode(&szTemp);
              VMBDebug("CurrentCode: %s", szTemp);     
       } else {
              VMBDebug("no VMBC code loaded.");       
       }
      }
      else if (!strcmp(szCMD, "SAVELOG"))
      {
       FILE *logfile;
       logfile = fopen("log.txt", "w");
       if (logfile == NULL)
       {
        VMBDebug("can't write log file!");
       } else {
         int i = GetWindowTextLength(hDbgEdit) + 1;
         char *szTemp = (char*)VMBAlloc(i + 1);
         GetWindowText(hDbgEdit, szTemp, i);                
         fputs(szTemp, logfile);
         fclose(logfile);
         VMBDebug("%i Bytes written to log.txt", i);
       }
      }
      else if (!strcmp(szCMD, "MATH")) 
      {
           double MyFl;
           VMBMathParse(ParamStart, &MyFl);
           VMBDebug("Result: %.10f", MyFl);
      }        
#ifndef FINAL      
/*      else if (!strcmp(szCMD, "TESTCALL"))
      {
           void (*NewFunc)() = (void *)VMBCreateCallBack(0x12345678);
           NewFunc();
           NewFunc = VMBFree(&NewFunc);
      }*/
      else if (!strcmp(szCMD, "DUMPSUBS"))
      {
            VMBSubDump();     
      }
#endif      
      else if (!strcmp(szCMD, "ERROR"))
      {
           VMBCritical("Es ist ein Fehler aufgetreten!");
      }
#ifndef FINAL  
      else if (!strcmp(szCMD, "EXETEST"))
      {
           VMBLoadCodeFromExe();     
      }    
      else if (!strcmp(szCMD, "BENCHMARK"))
      {
       VMBMemBench();        
      }      
#endif
      else 
      {
       VMBDebug("unknown command: %s", szCMD);     
      }
      szCMD = VMBFree(szCMD);
}

// -----------------------------------------------------------------------------
// RTL_DebugWndProc(int, int, int, int) - Callback für das Debug Fenster
// -----------------------------------------------------------------------------
LRESULT CALLBACK RTL_DebugWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     int i;
     switch(uMsg) {
        case WM_DESTROY:
             PostQuitMessage(0);
             return 0;
             break;
        case WM_CLOSE:
             ExitProcess(0);
             return 0;
             break;
        case WM_HOTKEY:
             switch (HIWORD(lParam))
             {
                    case VK_F12:
                         ExitProcess(0);
                         break;
             }
             break;
        case WM_ACTIVATE:
             if ( LOWORD(wParam) != WA_INACTIVE )
              SetFocus(hDbgLine);
             return 0;
             break;
        case WM_TIMER:
             if ( wParam == ERRORTIMERID )
             {
                blink = !blink;     
                if ( blink )
                {
                    SetClassLong(hDbgWnd, GCL_HBRBACKGROUND, (LONG) CreateSolidBrush(RGB(0xFF, 0x00, 0x00)));
                }  else {                        
                    SetClassLong(hDbgWnd, GCL_HBRBACKGROUND, (LONG) (HBRUSH)(COLOR_BTNFACE+1));
                }
                RedrawWindow(hDbgWnd, NULL, 0, RDW_ERASE | RDW_INVALIDATE );
             }
             break;
        case WM_COMMAND:
             if (LOWORD(wParam) == ID_OK)
             {
                char inputBuffer[512];
                ZeroMemory(&inputBuffer, sizeof(inputBuffer));
                GetWindowText(hDbgLine, inputBuffer, sizeof(inputBuffer));                
                SetWindowText(hDbgLine, "");                
                VMBDebug(">%s", inputBuffer);
                ParseCCMD(inputBuffer);                
                SetFocus(hDbgLine);
                return 0;                   
             } else 
             if ( (HWND) lParam == l_hwnd )
             {
                MessageBox(0, "asdf", "asdf", 32);
             }
             break;
        case WM_CTLCOLOREDIT :
             if ( (HWND)lParam == hDbgLine )
             {
                 SetBkColor((HDC)wParam, RGB(0xB7, 0xFF, 0xB7)); 
                 HBRUSH hBrush = CreateSolidBrush(RGB(0xB7, 0xFF, 0xB7));
                 return (LRESULT)hBrush;
             }
             return 0;
             break;
     }
     return DefWindowProc(hwnd, uMsg, wParam, lParam);        
}

// -----------------------------------------------------------------------------
// RTL_InputWndProc(int, int, int, int) - Callback für die Eingabezeile
// -----------------------------------------------------------------------------
LRESULT CALLBACK RTL_InputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
          RECT Re;
          PAINTSTRUCT PS;
          HBRUSH Br;
          int i;
          switch (uMsg)
          {
           case WM_KEYUP:
             if (wParam == VK_UP )
             {
              SetWindowText(hDbgLine, inputBuffer);           
             }
             if (wParam == VK_DOWN )
             {
              SetWindowText(hDbgLine, "");           
             }
             if (wParam == VK_F1 )
             {
              VMBDebug(">%s", "help");
              ParseCCMD("help");                
              SetFocus(hDbgLine);               
             }
             if (wParam == VK_F7 )
             {
              VMBDebug(">%s", "step");
              ParseCCMD("step");                
              SetFocus(hDbgLine);               
             }
             if (wParam == VK_F9 )
             {
              VMBDebug(">%s", "parse");
              ParseCCMD("parse");                
              SetFocus(hDbgLine);               
             }
             if (wParam == VK_F12 )
             {
              ExitProcess(0);               
             }
             break;
                 
           case WM_CHAR:
             if ( wParam == 13 )
                {
                    ZeroMemory(&inputBuffer, sizeof(inputBuffer));
                    GetWindowText(hDbgLine, inputBuffer, sizeof(inputBuffer));                
                    SetWindowText(hDbgLine, "");                
                    VMBDebug(">%s", inputBuffer);
                    ParseCCMD(inputBuffer);                
                    SetFocus(hDbgLine);         
                }
             break;
          }
          return CallWindowProc(oldWndProc, hWnd, uMsg, wParam, lParam);
}

#endif

#endif
 
