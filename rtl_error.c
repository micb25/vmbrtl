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
 
#ifndef RTL_ERROR_C
#define RTL_ERROR_C

#include "rtl_error.h"
#include "rtl_dbgwin.h"
#include "main.h"

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBDebug(char *, ...) - RTL Debug Ausgabe
// -----------------------------------------------------------------------------
void VMBDebug(const char *DebugMsg, ...)
{
 char *szBuffer = (char *)malloc(2048);
 va_list va_args;

 va_start(va_args, DebugMsg);     
 vsprintf(szBuffer, DebugMsg, va_args);
 va_end(va_args); 
 if (hDbgWnd != NULL)
 {
    int iLength = SendMessage(hDbgEdit, WM_GETTEXTLENGTH, 0, 0);
    int iSize = iLength + strlen(szBuffer) + 3;
    char *DbgBuffer = (char *)malloc(iSize);
    SendMessage(hDbgEdit, WM_GETTEXT, (WPARAM)iSize, (LPARAM)DbgBuffer);
    sprintf(DbgBuffer, "%s\r\n%s", DbgBuffer, szBuffer);
    SendMessage(hDbgEdit, WM_SETTEXT, 0, (LPARAM)(LPCTSTR)DbgBuffer);
    SendMessage(hDbgEdit, EM_LINESCROLL, 0, 0xFFFF );
    free(DbgBuffer);
 }
 free(szBuffer);
}
#endif

// -----------------------------------------------------------------------------
// VMBPanic(int) - Laufzeitfehler
// -----------------------------------------------------------------------------
void VMBPanic(int ErrorCode)
{
     VMBCritical("Es ist ein interner Laufzeitfehler aufgetreten!\nDas Programm wird nun beendet!\n\nFehler #%i\r\n\nKlicken Sie auf \"OK\", um das Programm zu beenden.", ErrorCode);     
}

// -----------------------------------------------------------------------------
// VMBCritical(char *, ...) - RTL Fehler mit anschließendem Shutdown
// -----------------------------------------------------------------------------
void VMBCritical(const char *ErrorMsg, ...)
{
 char *szBuffer = (char *)malloc(1024);
 va_list va_args;
 
 va_start(va_args, ErrorMsg);     
 vsprintf(szBuffer, ErrorMsg, va_args);
 va_end(va_args); 
 
#ifdef DEBUG  
 DestroyWindow(hDbgLine);
 DestroyWindow(hOk);
 
 SetClassLong(hDbgWnd, GCL_HBRBACKGROUND, (LONG) CreateSolidBrush(RGB(0xFF, 0x00, 0x00)));
 RedrawWindow(hDbgWnd, NULL, 0, RDW_ERASE | RDW_ALLCHILDREN | RDW_FRAME | RDW_INVALIDATE );
 
 SetTimer(hDbgWnd, ERRORTIMERID, 750, NULL);
 

 if (hDbgWnd != NULL)
 {
    VMBDebug("\r\n---\r\nCRITICAL ERROR:\r\n%s\r\n---", szBuffer);            
 }
 
#ifdef CREATELOGFILESONCRASH
       VMBSubDump();
       VMBIfDump();
       VMBVarDump();
#endif
 
#endif
 
 MessageBox(0, szBuffer, "Versatile MB Laufzeit-Fehler", MB_ICONERROR); 
 free(szBuffer);
 VMBExit(1);
}

#ifdef VCL
void VCLError(int code)
{
 VMBCritical(VCLERROR, code);
}
#endif

#endif
