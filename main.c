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

#ifndef MAIN_C
#define MAIN_C
 
#include "main.h"
#include "rtl_mem.h"
//#include "rtl_parse.h"
#include "rtl_for.h"

extern HWND hDbgWnd;
char *KeyBuffer = NULL;

extern char isParsing;
extern char isTerminated;

#ifdef DEBUGGER
char *SrcCode = NULL;
unsigned int iSrcSize = 0;
#endif

#ifdef VMBFASTCALL
char *FastCallPtr = NULL;
#endif

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBLoadSource(char *) - Lädt VMBC Code aus einer Datei
// -----------------------------------------------------------------------------
void VMBLoadSource(char *FileName)
{
 RTL_CodeFile = fopen(FileName, "rb");
 if (RTL_CodeFile == NULL)
 {
  VMBCritical("Runtime-Fehler #01\r\nCode kann nicht gelesen werden!");    
  return;
 }
 fseek(RTL_CodeFile, 0, SEEK_END);
 RTL_CodeSize = ftell(RTL_CodeFile);
 fseek(RTL_CodeFile, 0, SEEK_SET);
 if (RTL_Code == NULL)
 {
  RTL_Code = VMBAlloc(RTL_CodeSize + 1);
 } else {
  VMBFree(RTL_Code);       
  RTL_Code = VMBAlloc(RTL_CodeSize + 1);  
 }
 fread(RTL_Code, 1, RTL_CodeSize, RTL_CodeFile);
 fclose(RTL_CodeFile);
 
 /* Code entschlüsseln */
 RTL_CodeOrig = RTL_Code; 
 unsigned int i = 0;
 while (i < RTL_CodeSize)
  *RTL_Code = (*RTL_Code++ - ++i) ^ 0xA5;
 RTL_Code = RTL_CodeOrig;

 VMBDebug("CodeSize: %d", RTL_CodeSize);

 /* Code Check */
#ifdef DEBUG 
 if ( *(RTL_Code + 4 ) != ':' )
    VMBDebug("this code doesn't appear like VMBC code.");
 VMBDebug("Code(%d Bytes) successfully loaded", RTL_CodeSize); 
#endif
}

#endif

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBRTLReset() - reinitialisiert die RTL
// -----------------------------------------------------------------------------
void VMBRTLReset()
{
     RTL_Code = RTL_CodeOrig;     
     VMBResetStack();
     VMBResetRStack();
     VMBResetVars();    
     VMBInitLoopTypes();
     VMBForClear(); 
     VMBIfClear();
     VMBRepClear();
     resetNeeded = FALSE;
}
#endif

// -----------------------------------------------------------------------------
// VMBInit(int) - initialisiert die RTL
// -----------------------------------------------------------------------------
void VMBInit(HINSTANCE hThisInstance)
{
 char *TempBuffer;
 char *Buffer;
#ifdef DEBUG
 char *ExeName;
 FILE *Exe;
#endif 
 int iFileInfoSize, iZero;

 /* Instance Handle kopieren */
 RTL_HINSTANCE = hThisInstance;
 
 /* ist das aktuelle System mind. Win2k? */
 isWin2k = isMinWin2k();
 
 /* Keyboard Buffer für Eingaben */
 KeyBuffer = VMBAlloc(256);
 
 /* Assigns */
 RTL_Code = NULL;
 RTL_CodeOrig = NULL;
 RTL_CodeSize = 0;
 isStopped = FALSE;
 isStepMode = FALSE;
 resetNeeded = FALSE;
 isRStackUsed = FALSE;
 
#ifdef DEBUG 
 /* Debug Fenster erzeugen */     
 VMBInitDebugWnd();
 /* Version ausgeben */
 VMBDebug(VERSIONSTR);
 VMBDebug("build options:");
 VMBDebug("  MAX_PATH = %i", MAX_PATH);
 VMBDebug("  verbose level = %i", VERBOSE);
 VMBDebug("  max. loop level = %i", LOOPMAX); 
#ifdef DEBUGGER
 VMBDebug("  Debugger included"); 
#endif
#ifdef VCL
 VMBDebug("  VCL included");
#endif 
#ifdef FINAL
 VMBDebug("  loading code from exe");
#endif
#ifdef SHOWWINDOWMESSAGES
 VMBDebug("  showing window messages");
#endif
#ifdef FORM_DOUBLEBUFFERED
 VMBDebug("  double buffered windows");
#endif
#ifdef VMBFASTCALL
 VMBDebug("  FASTCALL");
#endif
 VMBDebug("  gcc Version %i.%i.%i", __GNUC__, __GNUC_MINOR__, __GNUC_PATCHLEVEL__);
 
 ExeName = (char*)VMBAlloc(256);
 GetModuleFileName(NULL, ExeName, 256);
 Exe = fopen(ExeName, "r");
 if ( Exe )
 {
    fseek(Exe, 0, SEEK_END);  
    VMBDebug("  Size of RTL: %d Bytes", ftell(Exe));     
    fclose(Exe);
 }
 ExeName = (char*)VMBFree(ExeName);  
 
 VMBDebug("");
#endif
       
 /* Command Line kopieren */
 RTL_CMDLINE = VMBAlloc(strlen(GetCommandLine()) + 1);         
 strcpy(RTL_CMDLINE, GetCommandLine()); 
#ifdef DEBUG 
 VMBDebug("CMDLINE: %s", RTL_CMDLINE);
#endif
 /* Zufallsgenerator initialisieren */
 srand(time(NULL));
 
}

// -----------------------------------------------------------------------------
// VMBExit(int) - beendet die RTL
// -----------------------------------------------------------------------------
void VMBExit(DWORD ExitCode)
{
#if VERBOSE > 0
 VMBDebug(" -> %s(%i);", __func__ , ExitCode);
#endif       
#ifdef VCL
 VMBResetVCLs();
#endif 
 isTerminated = TRUE;
// ExitProcess(ExitCode);
#ifndef DEBUG
 ExitProcess(ExitCode);
#endif
#ifdef FINAL
 if ( hDbgWnd == NULL)
  ExitProcess(ExitCode);
 else
 if ( !IsWindowVisible(hDbgWnd) )
  ExitProcess(ExitCode);
#endif
 // MessageBox(0, "CRTL wird beendet!", "CRTL Debug", MB_ICONINFORMATION);
 // ExitProcess(ExitCode);     
}

// -----------------------------------------------------------------------------
// VMBProcessMessages() - überprüft ob Messages vorhanden sind
// -----------------------------------------------------------------------------
void VMBProcessMessages()
{
    MSG msg;
    if (PeekMessage(&msg, 0, 0, 0, PM_REMOVE))
    {  
       if ( IsDialogMessage(GetParent(msg.hwnd), &msg) == FALSE )
         {
              TranslateMessage(&msg);
              DispatchMessage(&msg);
         }
    }
}

extern HWND LastCreatedWindow;

// -----------------------------------------------------------------------------
// VMBMainLoop() - Hauptschleife für Messages
// -----------------------------------------------------------------------------
void VMBMainLoop()
{
 while ( 1 )
 {
  MSG msg;
  if (!GetMessage(&msg, 0, 0, 0))
   break;
  if ( IsDialogMessage(GetParent(msg.hwnd), &msg) == FALSE )
  {
     TranslateMessage(&msg);
     DispatchMessage(&msg);
  }
 }
}

#ifndef BUILDING_DLL
// -----------------------------------------------------------------------------
// WinMain(int, int, char *, int) - Program Entry Point
// -----------------------------------------------------------------------------
int WINAPI WinMain (HINSTANCE hThisInstance, HINSTANCE hPrevInstance, LPSTR lpszArgument, int nFunsterStil)
{
    /* RTL initialisieren */    
    VMBInit(hThisInstance);
    
    /* Befehlssystem initialisieren */
    VMBInitCommands(); 
    
    /* Nachkommastellen für Kommazahlen */
    VMBSetFloatPrecision(FLOAT_PRECISION);
    
    /* Stack initialisieren */
    VMBInitStack();   
    
    /* Return Stack initialisieren */ 
    VMBInitRStack();
    
    /* LoopTypes initialisieren */
    VMBInitLoopTypes();
    
    /* Ifs initialisieren */
    VMB_InitIfs();
    
    /* Reps initialisieren */
    VMB_InitReps();
    
    /* Subs initialisieren */
    VMB_InitSubs();
    
    /* Call Subs initialisieren */
    VMB_InitCSubs();
     
    /* Variablensystem initialisieren */
    VMBInitVars(); 
    
    /* FPAR initialisieren */
    FPAR_get_tokens();
    
#ifdef VCL
    /* Komponenten initialisieren */
    VCL_FORM_Init();
    VCL_Timer_Init();
    
    OleInitialize(NULL);
#endif    

#ifdef DEBUGGER
    DEBUGGER_Init();
#ifdef DEBUG    
    SetForegroundWindow(hDbgWnd);
#endif    
#endif

#ifdef FINAL
#ifdef DEBUG
    VMBDebug("\r\nloading code...");
#endif    
    VMBLoadCodeFromExe();
#ifdef DEBUG    
    VMBDebug("parsing code...");
#endif   

#endif

#ifdef FINAL 
    VMBParse();
#endif

    /* Übergabe an Debug Fenster */
    VMBMainLoop();
     
    /* RTL beenden */
    VMBExit(0);
}
#endif

#ifdef BUILDING_DLL
BOOL APIENTRY DllMain (HINSTANCE hInst     /* Library instance handle. */ ,
                       DWORD reason        /* Reason this function is being called. */ ,
                       LPVOID reserved     /* Not used. */ )
{
    switch (reason)
    {
      case DLL_PROCESS_ATTACH:
        break;

      case DLL_PROCESS_DETACH:
        break;

      case DLL_THREAD_ATTACH:
        break;

      case DLL_THREAD_DETACH:
        break;
    }

    /* Returns TRUE on success, FALSE on failure */
    return TRUE;
}

DLLIMPORT void VMBRuntimeMain(char *src, int iSize)
{
    MessageBox (0, "ToDo!", "VMBRuntime", MB_ICONINFORMATION);
}

#endif

#endif
