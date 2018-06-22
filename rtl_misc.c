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
 
#ifndef RTL_MISC_C
#define RTL_MISC_C

#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

#include <stdio.h>

#include <shlobj.h>
#include <windows.h>

extern char isWin2k;
extern HWND hDbgWnd;

extern HWND LastCreatedWindow;

HRESULT CoInit = 0;

// -----------------------------------------------------------------------------
//  VMBRand(int, int) - gibt eine Zufallszahl zwischen min und max zurück
// -----------------------------------------------------------------------------
unsigned int VMBRand(unsigned int min, unsigned int max)
{
         return ( rand() % max ) + min;
}

// -----------------------------------------------------------------------------
//  VMBFileExists(char *) - überprüft, ob eine Datei existiert
// -----------------------------------------------------------------------------
int VMBFileExists(char *filename)
{
  WIN32_FIND_DATA FindData;
  HANDLE FFF = FindFirstFile(filename, &FindData);
  if ( FFF != INVALID_HANDLE_VALUE )
  {
    FindClose(FFF);
    return FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY == 0 ? 1 : 0;
  }
  return 0;
}

// -----------------------------------------------------------------------------
//  VMBDirExists(char *) - überprüft, ob ein Verzeichnis existiert
// -----------------------------------------------------------------------------
int VMBDirExists(char *filename)
{
  WIN32_FIND_DATA FindData;
  HANDLE FFF = FindFirstFile(filename, &FindData);
  if ( FFF != INVALID_HANDLE_VALUE )
  {
    FindClose(FFF);
    return FindData.dwFileAttributes & FILE_ATTRIBUTE_DIRECTORY > 0 ? 1 : 0; 
  }
  return 0;
}

// -----------------------------------------------------------------------------
//  VMBMkDir(char *) - erzeugt ein Verzeichnis und ggf. die Unterverzeichnisse
// -----------------------------------------------------------------------------
int VMBMkDir(char *path)
{
    if (!path)    
       return 0;
    char *Buffer = path;
    char *fullPath = (char *)VMBAlloc(VMBStrLen(path)+3);
    Buffer = strtok(Buffer, "\\");
    if (Buffer != NULL )
        sprintf(fullPath, "%s", Buffer);
    while ( Buffer != NULL )
    {
          if (!VMBDirExists(fullPath))
          {
             mkdir(fullPath);                          
          }
          Buffer = strtok(0, "\\");
          if ( Buffer )
             sprintf(fullPath, "%s\\%s", fullPath, Buffer);          
    }
    fullPath = (char*)VMBFree(fullPath);
    return 1;
}

// -----------------------------------------------------------------------------
//  VMBInt64Test() - Test eines 64bit Wertes
// -----------------------------------------------------------------------------
#ifdef DEBUG
void VMBInt64Test()
{
     __int64 a;
     GetDiskFreeSpaceEx("G:", NULL, (PULARGE_INTEGER)&a, NULL);     
     VMBDebug("Frei: %I64d", a);
}
#endif

// -----------------------------------------------------------------------------
//  isMinWin2k() - überprüft, ob die RTL mindestens unter Win2k läuft
// -----------------------------------------------------------------------------
int isMinWin2k()
{
   OSVERSIONINFO win;
   win.dwOSVersionInfoSize = sizeof(OSVERSIONINFO);
   GetVersionEx(&win);
   return win.dwMajorVersion < 5 ? 0 : 1;
}

// -----------------------------------------------------------------------------
//  VMBSetLayeredWindowAttributes(hwnd, colorref, byte, dword) - AlphaBlending / Transparenz (benötigt Win2k)
// -----------------------------------------------------------------------------
void VMBSetLayeredWindowAttributes(HWND hwnd, COLORREF crKey, BYTE bAlpha, DWORD dwFlags)
{
 if (isWin2k)
 {
        HINSTANCE user32 = LoadLibrary("user32.dll");
        if ( user32 )
        {
          FARPROC *SLWA = (FARPROC*)GetProcAddress(user32, "SetLayeredWindowAttributes");
          if ( SLWA )
          {
            void (*WinCMD)(HWND hwnd2, COLORREF crKey2, BYTE bAlpha2, DWORD dwFlags2) = (void (*)(HWND hwnd2, COLORREF crKey2, BYTE bAlpha2, DWORD dwFlags2))GetProcAddress(user32, "SetLayeredWindowAttributes");       
            WinCMD(hwnd,crKey, bAlpha, dwFlags);
          }
          FreeLibrary(user32);
        }  
     }         
}

// -----------------------------------------------------------------------------
//  VMBParamCount() - gibt die Anzahl der Parameter zurück
// -----------------------------------------------------------------------------
unsigned int VMBParamCount()
{
    char *CMDLine = GetCommandLine();
    char isQuote = 0;
    unsigned int iRes = 0;
    if ( VMBStrLen(CMDLine) == 0 )    
       return 0;
    while ( *CMDLine )
    {
          if ( *CMDLine == '\"' )
             {
               isQuote = !isQuote;           
             }
          if ( *CMDLine == ' ' && !isQuote && *(CMDLine+1) != '\0' )
             {
                 iRes++;       
             }          
          CMDLine++;
    }
    return iRes;
}

// -----------------------------------------------------------------------------
//  VMBParamStr(int) - gibt einen bestimmten Parameter zurück
// -----------------------------------------------------------------------------
char *VMBParamStr(int iIndex)
{
     if ( iIndex > VMBParamCount())     
     {
      return NULL;     
     } else {
      char *CMDLine = GetCommandLine();
      int iStart = 0, iEnd = 0, iRes = 0;
      char isQuote = 0;
      iStart = (int)CMDLine;
      while ( *CMDLine )
        {
              if ( *CMDLine == '\"' )
                 {
                   isQuote = !isQuote;           
                 }
              if ( *CMDLine == ' ' && !isQuote )
              {
                 iEnd = (int)CMDLine;
                 if ( iIndex == iRes )     
                 {
                       char *szBuffer = (char *)VMBAlloc(iEnd-iStart+2);
                       strncpy(szBuffer, (char *)iStart, iEnd-iStart);                      
                       return szBuffer;
                 } else {
                   iRes++;
                   iStart = (int)CMDLine + 1;       
                   
                 }
              }
              CMDLine++;
        }
      return NULL;
     }
}

// -----------------------------------------------------------------------------
//  VMBisQuote(int) - überprüft, ob es sich um einen String handelt
// -----------------------------------------------------------------------------
char VMBisQuote(int Pos)
{
     unsigned int i = 0;
     char isQuote = 0;
     if (RTL_Code == NULL)
        return 0;
     while ( i < Pos)
     {
           if (*(RTL_CodeOrig+(i++)) == '\"')      
              isQuote = !isQuote;
     }     
     return isQuote;
}

// -----------------------------------------------------------------------------
//  VMBLoadCodeFromExe() - lädt den VMB Code aus der Exe Datei
// -----------------------------------------------------------------------------
void VMBLoadCodeFromExe()
{
     DWORD iCodeSize;
     unsigned int i = 0;
#if VERBOSE > 1
     VMBDebug(" -> %s();", __func__);  
#endif
     HRSRC VMBSource = FindResource(NULL, "#1", RT_RCDATA);
     if (VMBSource == NULL)
     {
        VMBPanic(10);
     } else {
        HGLOBAL MySource = LoadResource(NULL, VMBSource);
        if (MySource == NULL)
        {
           VMBPanic(11);
        } else {
           RTL_CodeSize = SizeofResource(NULL, VMBSource);
           if (RTL_Code == NULL)
           {
              RTL_Code = (char*)VMBAlloc(RTL_CodeSize + 1);
           } else {
              VMBFree(RTL_Code);       
              RTL_Code = (char*)VMBAlloc(RTL_CodeSize + 1);  
           }
           memcpy(RTL_Code, MySource, RTL_CodeSize);
           RTL_CodeOrig = RTL_Code;
           while (i < RTL_CodeSize)
            *RTL_Code = (*RTL_Code++ - ++i) ^ 0xA5;
           RTL_Code = RTL_CodeOrig;
           if ( *(RTL_Code + 4 ) != ':' )
            VMBPanic(12);
        }
     } 
}

#ifdef VCL
// -----------------------------------------------------------------------------
//  VMBCreateFont(char*, int, int) - erzeugt einen Font
// -----------------------------------------------------------------------------
HFONT VMBCreateFont(char *FontName, int iFontSize, int iFlags)
{
      DWORD Style1, Style2 = FALSE, Style3 = FALSE, Style4 = FALSE;
      if ( iFlags & 1 ) // bold
      {
         Style1 = FW_BOLD;
      } else {
         Style1 = FW_LIGHT;             
      }
      if ( iFlags & 2 ) // italic
      {
         Style2 = TRUE;
      }
      if ( iFlags & 4 ) // underline
      {
         Style3 = TRUE;
      }
      if ( iFlags & 8 ) // strikeout
      {
         Style4 = TRUE;
      }      
#ifdef DEBUG      
      return CreateFont( -MulDiv( iFontSize, GetDeviceCaps( GetDC(hDbgWnd), LOGPIXELSY), 72), 0, 0, 0, Style1, Style2, Style3, Style4, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, FontName);
#else
      return CreateFont( -MulDiv( iFontSize, GetDeviceCaps( GetDC(GetDesktopWindow()), LOGPIXELSY), 72), 0, 0, 0, Style1, Style2, Style3, Style4, DEFAULT_CHARSET, OUT_DEFAULT_PRECIS, CLIP_DEFAULT_PRECIS, DEFAULT_QUALITY, FIXED_PITCH | FF_DONTCARE, FontName);
#endif      
}

#endif

int CALLBACK VMBBrowseCallbackProc(HWND hwnd, UINT uMsg, LPARAM lParam, LPARAM lpData)
{
    switch ( uMsg ) 
    {
           case BFFM_INITIALIZED:
                SendMessage(hwnd, BFFM_SETSELECTION, TRUE, (LPARAM)lpData);
                break;
                
    }
    return 1;
}

char *VMBSelectPath(char *Title, char* DefaultPath)
{
     HANDLE Shell32;
     FARPROC Proc1, Proc2;
     if ( CoInit != S_OK )
     {
          CoInit = CoInitialize(NULL);
     }
     if ( CoInit != S_OK )
        return NULL;     
     Shell32 = LoadLibraryA("shell32.dll");   
     if ( Shell32 )
     {
          Proc1 = GetProcAddress(Shell32, "SHBrowseForFolder");
          Proc2 = GetProcAddress(Shell32, "SHGetPathFromIDList");
          
          if ( Proc1 == NULL || Proc2 == NULL )
          {
             FreeLibrary(Shell32);          
             return "";
          }
          BROWSEINFO *Path = (BROWSEINFO*)CoTaskMemAlloc(sizeof(BROWSEINFO));                            
          if ( Path != NULL )
          {
              ZeroMemory(Path, sizeof(BROWSEINFO)); 
              Path->lpszTitle = Title;
              Path->lpfn = &VMBBrowseCallbackProc;
              Path->lParam = (LPARAM)DefaultPath;
#ifdef VCL
              Path->hwndOwner = LastCreatedWindow;
#else
              Path->hwndOwner = NULL;
#endif
              Path->ulFlags =  BIF_RETURNONLYFSDIRS | BIF_DONTGOBELOWDOMAIN;
              
              int (*SHBrowseForFolder)(LPBROWSEINFO lpbi2) = (int (*)(LPBROWSEINFO lpbi2))Proc1;       
              int Result = SHBrowseForFolder(Path);
              CoTaskMemFree(Path);
              FreeLibrary(Shell32);          
              if ( Result != 0 )
              {
                   char *PathSelected = (char*)VMBAlloc(2048);
                   int (*SHGetPathFromIDList)(int pidl,LPTSTR pszPath) = (int (*)(int pidl2,LPTSTR pszPath2))Proc2;  
                   if ( SHGetPathFromIDList(Result, PathSelected) == 0 )
                      return "";    
                   else
                      return PathSelected;
              } else {
                return "";       
              }
          } else {
              FreeLibrary(Shell32);          
          }
     } else {
          return "";  
     }
}



BOOL CALLBACK VMBInputDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
     RECT MyWin;
     char *Param;
     switch (uMsg) {
            case WM_INITDIALOG:
                 GetWindowRect(hwndDlg, &MyWin);
                 if ( lParam != 0 )
                 {
                    VMBInputWindow *MyWindow = (VMBInputWindow*)lParam;  
                    SetWindowText(hwndDlg, MyWindow->szCaption);     
                    SetWindowText(GetDlgItem(hwndDlg, 2001), MyWindow->szDesc);
                    SetWindowText(GetDlgItem(hwndDlg, 2002), MyWindow->Value);
                    if (MyWindow->isPassword)
                       SendMessage(GetDlgItem(hwndDlg, 2002), EM_SETPASSWORDCHAR, (WPARAM)(UINT)'*', 0);
                 }
                 SetWindowPos(hwndDlg, HWND_TOP, (GetSystemMetrics(SM_CXSCREEN) - MyWin.right+MyWin.left ) / 2 , (GetSystemMetrics(SM_CYSCREEN) - MyWin.bottom+MyWin.top ) / 2, 0, 0, SWP_NOSIZE | SWP_NOZORDER );
                 SetFocus(GetDlgItem(hwndDlg, 2002));
                 break;       
            case WM_COMMAND:
                 if ( LOWORD(wParam) == IDOK )
                 {  
                  int iLength = SendMessage(GetDlgItem(hwndDlg, 2002), WM_GETTEXTLENGTH, 0, 0);    
                  Param = (char*)VMBAlloc(iLength + 3);
                  GetDlgItemText(hwndDlg, 2002, Param, iLength + 2);
                  EndDialog(hwndDlg, (int)Param);
                 }
                 break;
     }
     return 0;
}

#endif
