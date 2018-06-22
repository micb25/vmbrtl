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

#ifndef RTL_CCMD_C
#define RTL_CCMD_C

#include <windows.h>
#include <mmsystem.h>
#include <shlwapi.h>
#include <math.h>

#include "rtl_for.h"
#include "rtl_if.h"
#include "rtl_csub.h"
#include "rtl_subs.h"
#include "rtl_repeat.h"
#include "rtl_cmd.h"
#include "rtl_mem.h"
#include "rtl_ccmd.h"
#include "rtl_vars.h"
#include "rtl_goto.h"
#include "rtl_loops.h"
#include "main.h"

extern VMBCSub* CurrentSub;
extern int SubLevel;
int POSLevel = 0;
extern int CL;
extern int LabelLoopLevel[LOOPMAX];
extern BOOL CALLBACK VMBInputDialogProc(HWND hwndDlg, UINT uMsg, WPARAM wParam, LPARAM lParam);
extern char *KeyBuffer;

#ifdef VCL
extern HWND LastCreatedWindow;
#endif

#ifdef DEBUGGER
extern HWND CC_hSrcView;
#endif

#ifdef VMBFASTCALL
extern char* FastCallPtr;
#endif

// -----------------------------------------------------------------------------
// VMBParseOneString() - verarbeitet einen Parameter einer VMB Funktion
// -----------------------------------------------------------------------------
char *VMBParseOneString()
{
     char sCode[5], VCLCode[5], MemCode[5];
     char isFormula = FALSE;
     int BrackInts = 0;
     char *Val;
     double dRes;
#ifdef VMBFASTCALL
     int *FastCallSub = NULL;           
#endif       
     VMBVar *VCLCom;
     POSLevel++;
     VMBNextStack();
     VMBClearStack();
     VMBNextCode();
     do {
         VMBGetCurrentCode(sCode);
#if VERBOSE > 1              
         VMBDebug("  -> VMBParseOneString() : CC = %s", sCode);
#endif         
         if (!VMBCodeCMP(sCode, "E001") || !VMBCodeCMP(sCode, "E002") )
         {
            break;
         }
         RTL_Code += 5;
         if ((sCode[0] >= '0') && (sCode[0] <= '9'))
          {
           /* einfacher RTL Befehl */            
#ifdef VMBFASTCALL
         if ( FastCallPtr )
         {
             FastCallSub = (int*)((int)FastCallPtr + (int)RTL_Code - (int)RTL_CodeOrig - 5);
             if ( FastCallSub != NULL && *FastCallSub != 0x0000 )
             { 
                void (*VMBRunCMD)() = (void (*)()) (int)*FastCallSub;            
                VMBRunCMD();
                char *StackBuffer = (char *)VMBGetRStack();
                VMBPushStack(StackBuffer);
                continue;
             }
         }           
#endif           
           VMBCmd *CCMD;
           CCMD = (VMBCmd*)VMBFindCMD(sCode);
           if (CCMD != NULL)
           {
              void (*VMBRunCMD)() = (void (*)())CCMD->Pointer;
#ifdef VMBFASTCALL
              if ( FastCallPtr )
              {
                 int *iTe = (int*)((int)FastCallPtr + (int)RTL_Code - (int)RTL_CodeOrig - 5);
                 *iTe = (int)CCMD->Pointer;
              }
#endif              
              VMBRunCMD();      
              char *StackBuffer = (char *)VMBGetRStack();
              VMBPushStack(StackBuffer);
              continue;
           } else {
              int i = atoi(sCode);           
              VMBSub *CallSub = (VMBSub*)VMBGetSubByID(i);
              if ( CallSub != NULL )
              {
                     VMBCallCustomSub(CallSub);
                     VMBGetCurrentCode(&sCode[0]);
                     char *StackBuffer = (char *)VMBGetRStack();
                     VMBPushStack(StackBuffer);
                     continue;
              } else {
                     VMBPanic(31);
              }
           }
          } else {
            // #################################################################
            // # EA00 - Push String
            // #################################################################
            if (!VMBCodeCMP(sCode, "EA00"))      
            {
               char *TempBuffer = NULL;
               int iE = 0;
               RTL_Code++;
               while (*(RTL_Code++) != '\"')                                     
                iE++;
               TempBuffer = (char *)VMBAlloc(iE + 1);
               strncpy(TempBuffer, (RTL_Code++) - iE - 1, iE);
               VMBPushStackWOFree(TempBuffer, iE + 1);
               TempBuffer = NULL;
//               VMBPushStack(TempBuffer);
//               TempBuffer = VMBFree(TempBuffer);
               continue;
            }
            // #################################################################
            // # EA01 - Push Int
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EA01"))
            {
               char *TempBuffer = NULL;
               int iE = 0;               
               while (*(RTL_Code++) != ':')                                     
                iE++;
               TempBuffer = (char *)VMBAlloc(iE + 1);
               strncpy(TempBuffer, RTL_Code - iE - 1, iE);
//               VMBPushStack(TempBuffer);
               VMBPushStackWOFree(TempBuffer, iE + 1);
//               TempBuffer = VMBFree(TempBuffer);
               TempBuffer = NULL;  
               continue;                               
            }            
            // #################################################################
            // # EA02 - Push Var
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EA02"))
            {
               char VarBuffer[5];// = (char*)VMBAlloc(5);
               char *TempBuffer2;
               int *iTemp;  
               VMBVar *MyVar;

               VMBGetCurrentCode(VarBuffer);
               RTL_Code+=5;
               MyVar = (VMBVar*)VMBFindVar(VarBuffer);
               if ( !MyVar )
                  VMBPanic(539);
               switch (MyVar->VarType) 
               {
                      case VARTYPE_STR:
                           TempBuffer2 = (char *)VMBGetVarContentEx(MyVar);
                           VMBPushStackWOFree(TempBuffer2, VMBStrLen(TempBuffer2)+1);             
                           break;    
                      case VARTYPE_INT:
                           iTemp = (int *)VMBGetVarContentEx(MyVar);
                           TempBuffer2 = (char*)VMBAlloc(30);
                           sprintf(TempBuffer2, "%i", *iTemp);
                           VMBPushStackWOFree(TempBuffer2, 30);
                           iTemp = VMBFree(iTemp);
                           break; 
                      case VARTYPE_BOOL:
                           iTemp = (int *)VMBGetVarContentEx(MyVar);
                           if (*iTemp == 0)
                              VMBPushStack("0");
                           else
                              VMBPushStack("1");
                           iTemp = VMBFree(iTemp);                              
                           break;                            
                      case VARTYPE_FLOAT:
                           iTemp = (int *)VMBGetVarContent(VarBuffer);
                           memcpy(&dRes, iTemp, sizeof(double));
                           Val = (char*)VMBGetFloatFloat(dRes);
                           VMBPushStack(Val);
                           MyVar = (VMBVar*)VMBFindVar(VarBuffer);
                           Val = VMBFree(Val);
                           iTemp = VMBFree(iTemp);
                           break;
                      case 0:
                      default: 
#ifdef DEBUG                           
                           VMBDebug("unknown var '%s' found!", VarBuffer);
#endif                           
                           VMBPanic(30);
                           break;                           
               }
//               VarBuffer = VMBFree(VarBuffer);
               continue; 
            }
            // #################################################################
            // # EA03 - Push Float
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EA03"))
            {
               char *TempBuffer;
               int iE = 0;               
               isFormula = 1;
               while (*(RTL_Code++) != ':')                                     
                iE++;
               TempBuffer = (char *)VMBAlloc(iE + 1);
               strncpy(TempBuffer, RTL_Code - iE - 1, iE);
               VMBPushStack(TempBuffer);
               TempBuffer = VMBFree(TempBuffer);
               continue;                               
            }  
            // #################################################################
            // # EB00 - Push +
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EB00"))
            {
               isFormula = 1;
               VMBPushStack("+");
               continue;                               
            } 
            // #################################################################
            // # EB01 - Push -
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EB01"))
            {
               isFormula = 1;
               VMBPushStack("-");
               continue;                               
            }             
            // #################################################################
            // # EB02 - Push *
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EB02"))
            {
               isFormula = 1;
               VMBPushStack("*");
               continue;                               
            }     
            // #################################################################
            // # EB03 - Push /
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EB03"))
            {
               isFormula = 1;
               VMBPushStack("/");
               continue;                               
            }     
            // #################################################################
            // # EB04 - Push (
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EB04"))
            {
               isFormula = 1;
               BrackInts = 1;
               VMBPushStack("(");
               continue;                               
            } 
            // #################################################################
            // # EB05 - Push )
            // #################################################################                
            if (!VMBCodeCMP(sCode, "EB05"))
            {
               isFormula = 1;
               if ( BrackInts == 1 )
                  {
                    VMBPushStack("+0)");          
                    BrackInts = 0;            
                  } else {
                    VMBPushStack(")");
                  }
               continue;                               
            }       
        
            // #################################################################
            // # G002 - Push VCL Method
            // #################################################################             
            if (!VMBCodeCMP(sCode, "G001"))
            {
                                                         
#ifdef VCL   
                 VMBProcessMessages();
                 VMBGetCurrentCode(&VCLCode[0]);
                 VMBNextCode();
                 VMBGetCurrentCode(&MemCode[0]);
                 VMBNextCode();                   
                 VCLCom = (VMBVar*)VMBFindVar(VCLCode);
                 if (VCLCom)
                 {
                   if ( VCLCom->hWnd != NULL && VCLCom->VarType == VARTYPE_TEXTBOX )
                   {                            
                      char *TempBuffer;
                      VCL_TextBoxMethod(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);                                     
                   } else       
                   if ( VCLCom->hWnd != NULL && VCLCom->VarType == VARTYPE_FILE )
                   {                            
                      char *TempBuffer;
                      VCL_FileMethod(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);                                     
                   }                               
                   VMBSkipBrackets();                                            
                   continue;
                 } 
                 VMBPanic(46);      
#else
                 VMBPanic(32);         
#endif
            }
            // #################################################################
            // # G002 - Push VCL Member
            // #################################################################             
            if (!VMBCodeCMP(sCode, "G002"))
            {
#ifdef VCL                               
                 VMBProcessMessages();
                 VMBGetCurrentCode(&VCLCode[0]);
                 VMBNextCode();
                 VMBGetCurrentCode(&MemCode[0]);
                 VMBNextCode();                 
                 VCLCom = (VMBVar*)VMBFindVar(VCLCode);
                 if (VCLCom)
                 {
                   if ( VCLCom->hWnd != NULL && VCLCom->VarType == VARTYPE_FORM )
                   {
                      char *TempBuffer;
                      VCL_FormGetMember(VCLCom->hWnd, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);
                   } else
                   if ( VCLCom->VarType == VARTYPE_TIMER )
                   {
                      char *TempBuffer;
                      VCL_TimerGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);                        
                   } else
                   if ( VCLCom->VarType == VARTYPE_BUTTON )
                   {
                      char *TempBuffer;
                      VCL_ButtonGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);   
                   } else    
                  if ( VCLCom->VarType == VARTYPE_IMAGE )
                   {
                      char *TempBuffer;
                      VCL_ImageGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);   
                   } else                      
                   if ( VCLCom->VarType == VARTYPE_CHECKBOX )
                   {
                      char *TempBuffer;
                      VCL_CheckBoxGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);   
                   } else                                       
                   if ( VCLCom->VarType == VARTYPE_RADIOBUTTON )
                   {
                      char *TempBuffer;
                      VCL_RadioButtonGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);   
                   } else                                                          
                   if ( VCLCom->VarType == VARTYPE_GROUPBOX )
                   {
                      char *TempBuffer;
                      VCL_GroupBoxGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);   
                   } else                                                          
                   if ( VCLCom->VarType == VARTYPE_LABEL )
                   {
                      char *TempBuffer;
                      VCL_LabelGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);   
                   } else                     
                   if ( VCLCom->VarType == VARTYPE_EDIT )
                   {
                      char *TempBuffer;
                      VCL_EditGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);                                              
                   } else                     
                   if ( VCLCom->VarType == VARTYPE_TEXTBOX )
                   {
                      char *TempBuffer;
                      VCL_TextBoxGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);                                              
                   } else 
                   if ( VCLCom->VarType == VARTYPE_FILE )
                   {
                      char *TempBuffer;
                      VCL_FileGetMember(VCLCom, atoi(MemCode));     
                      TempBuffer = (char*)VMBGetRStack();
                      VMBPushStack(TempBuffer);                                              
                   } else {
                      VMBPushRStack("0", 1);                                 
                   }
                 } else {
                  VMBPushRStack("0", 1);       
                 }
                 continue;
#else
            VMBPanic(32);                 
#endif                 
            }
            // #################################################################
            // # G003 - Push VCL 
            // #################################################################             
            if (!VMBCodeCMP(sCode, "G003"))
            {
#ifdef VCL                             
                 isFormula = 0;
                 VMBGetCurrentCode(&VCLCode[0]);
                 VMBNextCode();
                 VCLCom = (VMBVar*)VMBFindVar(VCLCode);
                 if (VCLCom)
                 {
                      VMBPushStack2("%i", (int)VCLCom);                                 
                 } else {
#ifdef DEBUG
                   VMBDebug("CODE %s", VCLCode);
#endif            
                   VMBPanic(46);       
                 }
                 continue;
#else
            VMBPanic(32);                 
#endif                 
            }            
            
#ifdef DEBUG                      
            VMBDebug("CODE: %s", sCode);        
#endif            
            VMBPanic(33);
          }
               
                  
     } while ( 1 );

#if VERBOSE > 2     
     VMBDebug("   -> Leaving %s(): isFormula = %i", __func__, (int)isFormula);
#endif     

POSLevel--;

     if ( isFormula )
     {
        double Res;  
        char *Result = (char *)VMBGetStack();
        char *dbRes;
        VMBPrevStack(); 
        VMBMathParse(Result, &Res);
        Result = VMBFree(Result);
        dbRes = (char*)VMBGetFloatFloat(Res);
#if VERBOSE > 2     
         VMBDebug("   -> Result = \"%s\"", dbRes);
#endif            
        return (char*)dbRes;     
     } else {
        char *Result;
        Result = (char *)VMBGetStack();
        VMBPrevStack();
#if VERBOSE > 0
         VMBDebug("   -> Result = \"%s\"", Result);
#endif            
        return Result;
     }
}

// #############################################################################
// # 0015/0000 - Beep()
// #############################################################################
void RTL_Beep()
{
     MessageBeep((UINT)MB_OK);
     VMBSkipBrackets();   
}

// #############################################################################
// # 0001 - Term()
// #############################################################################
void RTL_Term()
{
     VMBSkipBrackets();     
     VMBExit(0);
}

// #############################################################################
// # 0002 - ConsoleCreate()
// #############################################################################
void RTL_ConsoleCreate()
{
     VMBSkipBrackets();
     AllocConsole();
}

// #############################################################################
// # 0003 - ConsoleDestroy()
// #############################################################################
void RTL_ConsoleDestroy()
{
     VMBSkipBrackets();
     FreeConsole();
}

// #############################################################################
// # 0004 - ConsoleClear()
// #############################################################################
void RTL_ConsoleClear()
{
     int iTemp1, iTemp2;
     DWORD dTemp1;
     COORD cCOORD;
     CONSOLE_SCREEN_BUFFER_INFO csbi;
     
     VMBSkipBrackets();
     iTemp1 = (INT)GetStdHandle(STD_OUTPUT_HANDLE);
     if ( iTemp1 != (INT)INVALID_HANDLE_VALUE )
     {
        cCOORD.X = 0; cCOORD.Y = 0;
        SetConsoleCursorPosition((HANDLE)iTemp1, cCOORD);                    
        GetConsoleScreenBufferInfo((HANDLE)iTemp1, &csbi);
        iTemp2 = csbi.dwSize.X * csbi.dwSize.Y;
        FillConsoleOutputCharacter((HANDLE)iTemp1, ' ', iTemp2, cCOORD, &dTemp1);
        GetConsoleScreenBufferInfo((HANDLE)iTemp1, &csbi);
        FillConsoleOutputAttribute((HANDLE)iTemp1, csbi.wAttributes, iTemp2, cCOORD, &dTemp1);
        cCOORD.X = 0; cCOORD.Y = 0;
        SetConsoleCursorPosition((HANDLE)iTemp1, cCOORD);                            
     }
}

// #############################################################################
// # 0005 - Pos(Str, SubStr)
// #############################################################################
void RTL_Pos()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();

     if ( VMBStrLen(Param1) == 0 ||VMBStrLen(Param2) == 0 )
     {
          VMBPushRStack("0", 1);     
     } else {
          char *newBuffer = strstr(Param1, Param2);
          if (!newBuffer){
             VMBPushRStack("0", 1);                                  
          } else {
             char Buffer[16];
             sprintf(Buffer, "%i", ( (int)newBuffer - (int)Param1 + 1) );
             VMBPushRStack(Buffer, VMBStrLen(Buffer));             
          }
          Param1 = VMBFree(Param1);
          Param2 = VMBFree(Param2);
     }
     VMBNextCode();
}

// #############################################################################
// # 0006 - ConsoleReadLn()
// #############################################################################
void RTL_ConsoleReadLn()
{
     HANDLE hConsole = GetStdHandle(STD_INPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)    
     {
      char *Param1 = (char*)VMBAlloc(257);
      DWORD iCharsRead;
      if ( ReadConsole(hConsole, Param1, 256, &iCharsRead, NULL) != 0 )
      {
         if (*(Param1+iCharsRead-2) == '\r')                
         {
            *(Param1+iCharsRead-1) = '\0';
            *(Param1+iCharsRead-2) = '\0';
         }
      }
      VMBPushRStack(Param1, VMBStrLen(Param1));
      Param1 = VMBFree(Param1);
     } else {
      VMBPushRStack("", 0); 
     }
     VMBSkipBrackets();
}
// #############################################################################
// # 0007 - Abort()
// #############################################################################
void RTL_Abort()
{
     VMBSkipBrackets();
     VMBExit(1);
}

// #############################################################################
// # 0008 - ConsoleWrite
// #############################################################################
void RTL_ConsoleWrite()
{
     char *Param1;
     Param1 = VMBParseOneString();
     DWORD i = 0;
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
      WriteConsole(hConsole, Param1, VMBStrLen(Param1), &i, 0);     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0009 - ConsoleWriteLn
// #############################################################################
void RTL_ConsoleWriteLn()
{
     char Enter[] = "\r\n";
     DWORD i = 0;
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     RTL_ConsoleWrite();
     if (hConsole != INVALID_HANDLE_VALUE)
      WriteConsole(hConsole, Enter, VMBStrLen(Enter), &i, 0);     
}

// #############################################################################
// # 0010 - ConsoleName
// #############################################################################
void RTL_ConsoleName()
{
     char *Param1 = VMBParseOneString();
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
      SetConsoleTitle(Param1);
     VMBNextCode();     
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0011 - Length
// #############################################################################
void RTL_Length()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(10);
     sprintf(Param2, "%d", VMBStrLen(Param1));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0012 - Trim
// #############################################################################
void RTL_Trim()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBTrim(Param1);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0013 - UCase
// #############################################################################
void RTL_UCase()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBUpperCase(Param1);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0014 - LCase
// #############################################################################
void RTL_LCase()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBLowerCase(Param1);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();     
     Param1 = VMBFree(Param1); 
}

// #############################################################################
// # 0016 - Shell
// #############################################################################
void RTL_Shell()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char *)VMBAlloc(16);
     int r = WinExec(Param1, SW_SHOW);
     sprintf(Param2, "%i", r);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0017 - MCISend
// #############################################################################
void RTL_MCISend()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char *)VMBAlloc(256);     
     mciSendString(Param1, Param2, 256, 0);     
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}


// #############################################################################
// # 0018 - ConsoleGetName
// #############################################################################
void RTL_ConsoleGetName()
{
     char *Param1 = (char *)VMBAlloc(256);

     if (GetConsoleTitle(Param1, 256)) 
     {
            VMBPushRStack(Param1, VMBStrLen(Param1));
     } else {
            VMBPushRStack("", 0);
     }
     Param1 = VMBFree(Param1);
     VMBSkipBrackets();          
}

// #############################################################################
// # 0019 - ConsoleColor
// #############################################################################
void RTL_ConsoleColor()
{
     char *szBuffer = VMBParseOneString();
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
     {
        SetConsoleTextAttribute(hConsole, atoi(szBuffer));
     }
     VMBNextCode();
     szBuffer = VMBFree(szBuffer);
}

// #############################################################################
// # 0020 - ConsoleSetX
// #############################################################################
void RTL_ConsoleSetX()
{
     char *szBuffer;
     szBuffer = VMBParseOneString();
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
     {
        CONSOLE_SCREEN_BUFFER_INFO hCSBI;    
        COORD CursorPos;              
        GetConsoleScreenBufferInfo(hConsole, &hCSBI);
        CursorPos.X = atoi(szBuffer) - 1;
        CursorPos.Y = hCSBI.dwCursorPosition.Y;        
        SetConsoleCursorPosition(hConsole, CursorPos);
     }
     VMBNextCode();
     szBuffer = VMBFree(szBuffer);
}

// #############################################################################
// # 0021 - ConsoleSetY
// #############################################################################
void RTL_ConsoleSetY()
{
     char *szBuffer = VMBParseOneString();
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
     {
        CONSOLE_SCREEN_BUFFER_INFO hCSBI;    
        COORD CursorPos;              
        GetConsoleScreenBufferInfo(hConsole, &hCSBI);
        CursorPos.Y = atoi(szBuffer) - 1;
        CursorPos.X = hCSBI.dwCursorPosition.X;        
        SetConsoleCursorPosition(hConsole, CursorPos);
     }
     VMBNextCode();
     szBuffer = VMBFree(szBuffer);
}

// #############################################################################
// # 0022 - ParamCount()
// #############################################################################
void RTL_ParamCount()
{    
     char TempBuffer[16];
     sprintf((char*)&TempBuffer, "%i", VMBParamCount());
     VMBPushRStack(TempBuffer, VMBStrLen(TempBuffer));          
     VMBSkipBrackets();
}

// #############################################################################
// # 0023 - Sleep
// #############################################################################
void RTL_Sleep()
{
     char *szBuffer = VMBParseOneString();
//#ifndef DEBUG
     Sleep((unsigned)atoi(szBuffer));     
/*#else
     DWORD StartTime = GetTickCount();
     while ( 1 )
     {
           VMBProcessMessages();
           if (GetTickCount() > StartTime + (unsigned)atoi(szBuffer))      
              break;
     }
#endif*/
     VMBNextCode();
     szBuffer = VMBFree(szBuffer);
}

// #############################################################################
// # 0024 - Char
// #############################################################################
void RTL_Char()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(5);
     int i = atoi(Param1);
     if ( i < 0 )
        i = 0;
     sprintf(Param2, "%c", (char) i & 0xFF );
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0025 - Int
// #############################################################################
void RTL_Int()
{
     int iTemp;
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(16);
     iTemp = atoi(Param1);
     sprintf(Param2, "%i", iTemp);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0026 - Str
// #############################################################################
void RTL_Str()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char *)VMBGetFloatStr(Param1);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);
}

// #############################################################################
// # 0027 - LeftStr
// #############################################################################
void RTL_LeftStr()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     int iLength = atoi(Param2);

     char *Buffer = (char*)VMBLeftStr(Param1, iLength);
     VMBPushRStack(Buffer, VMBStrLen(Buffer));

     VMBNextCode();  
     Buffer = VMBFree(Buffer);
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);
}

// #############################################################################
// # 0028 - RightStr
// #############################################################################
void RTL_RightStr()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     int iLength = atoi(Param2);

     char *Buffer = (char*)VMBRightStr(Param1, iLength);
     VMBPushRStack(Buffer, VMBStrLen(Buffer));

     VMBNextCode();  
     Buffer = VMBFree(Buffer);
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);
}

// #############################################################################
// # 0029 - MidStr
// #############################################################################
void RTL_MidStr()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     char *Param3 = VMBParseOneString();
     int iPos = atoi(Param2);
     int iLength = atoi(Param3);

     char *Buffer = (char*)VMBMidStr(Param1, iPos, iLength);
     VMBPushRStack(Buffer, VMBStrLen(Buffer));

     VMBNextCode();  
     Buffer = VMBFree(Buffer);
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);
     Param2 = VMBFree(Param3);     
}

// #############################################################################
// # 0030 - ParamStr
// #############################################################################
void RTL_ParamStr()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char *)VMBParamStr(atoi(Param1));     
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);               
}

// #############################################################################
// # 0031 - CharAt
// #############################################################################
void RTL_CharAt()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     char *Param3 = (char*)VMBAlloc(4);
     int iLength = atoi(Param2);
     char szTemp[] = { '\0', '\0', '\0' };
     szTemp[0] = VMBCharAt(Param1, iLength - 1);
     sprintf(Param3, "%c", szTemp[0]);
     VMBPushRStack(Param3, VMBStrLen(Param3));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);
     Param3 = VMBFree(Param3);     
}

// #############################################################################
// # 0032 - Random
// #############################################################################
void RTL_Random()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(16);
     int i = atoi(Param1);
     sprintf(Param2, "%i", VMBRandom(i));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0033 - FileExists
// #############################################################################
void RTL_FileExists()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(4);
     sprintf(Param2, "%i", VMBFileExists(Param1));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0034 - ASCII
// #############################################################################
void RTL_ASCII()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(5);
     sprintf(Param2, "%i", (int)( *(Param1) & 0xFF ) );
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);     
 
}

// #############################################################################
// # 0035 - CommandLine()
// #############################################################################
void RTL_CommandLine()
{    
     char *szBuffer = GetCommandLine();
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));          
     VMBSkipBrackets();
}

// #############################################################################
// # 0036 - Hanlde()
// #############################################################################
void RTL_Handle()
{    
     char *Param1 = (char *)VMBAlloc(16);
     int i = (int)GetModuleHandle(NULL);
     sprintf(Param1, "%i", i);
     VMBPushRStack(Param1, VMBStrLen(Param1));          
     VMBSkipBrackets();
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0037 - Msgbox
// #############################################################################
void RTL_MsgBox()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     char *Param3 = VMBParseOneString();
     char *Param4 = (char*)VMBAlloc(16);
     int i = atoi(Param3), r;
     
     r = MessageBox(LastCreatedWindow, Param1, Param2, i);
     sprintf(Param4, "%i", r);
     VMBPushRStack(Param4, VMBStrLen(Param4));
          
     VMBNextCode();    
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);   
     Param3 = VMBFree(Param3);          
     Param4 = VMBFree(Param4);               
}

// #############################################################################
// # 0038 - MouseHide
// #############################################################################
void RTL_MouseHide()
{    
     while ( ShowCursor(0) >= 0 )
     {
           
     }
     VMBSkipBrackets();
}

// #############################################################################
// # 0039 - MouseShow
// #############################################################################
void RTL_MouseShow()
{    
     while ( ShowCursor(-1) < 0 )
     {
           
     }
     VMBSkipBrackets();
}

// #############################################################################
// # 0040 - MouseGetX
// #############################################################################
void RTL_MouseGetX()
{    
     POINT CursorPos;
     GetCursorPos(&CursorPos);
     char *Buffer = (char *)VMBAlloc(16);
     
     sprintf(Buffer, "%i", CursorPos.x);
     VMBPushRStack(Buffer, VMBStrLen(Buffer));
     
     Buffer = VMBFree(Buffer);
     VMBSkipBrackets();
}

// #############################################################################
// # 0041 - MouseGetY
// #############################################################################
void RTL_MouseGetY()
{    
     POINT CursorPos;
     GetCursorPos(&CursorPos);
     char *Buffer = (char *)VMBAlloc(16);
     
     sprintf(Buffer, "%i", CursorPos.y);
     VMBPushRStack(Buffer, VMBStrLen(Buffer));
     
     Buffer = VMBFree(Buffer);
     VMBSkipBrackets();
}

// #############################################################################
// # 0042 - MouseSetX
// #############################################################################
void RTL_MouseSetX()
{    
     POINT CursorPos;
     char *Param1 = VMBParseOneString();
     int iPos = atoi(Param1);
     
     GetCursorPos(&CursorPos);     
     SetCursorPos(iPos, CursorPos.y);

     VMBNextCode();     
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0043 - MouseSetY
// #############################################################################
void RTL_MouseSetY()
{    
     POINT CursorPos;
     char *Param1 = VMBParseOneString();
     int iPos = atoi(Param1);
     
     GetCursorPos(&CursorPos);     
     SetCursorPos(CursorPos.x, iPos);

     VMBNextCode();     
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0044 - ClipboardSet
// #############################################################################
void RTL_ClipboardSet()
{    
     HANDLE Clip, hMem;
     char *Param1 = VMBParseOneString();
     if (OpenClipboard(NULL))
     {
        char *GlobalMem = GlobalAlloc(GHND | GMEM_SHARE, VMBStrLen(Param1)+2);
        if (GlobalMem != NULL)
        {
           hMem = GlobalLock(GlobalMem);
           if (hMem != NULL)
           {
              strncpy(hMem, Param1, VMBStrLen(Param1));
              GlobalUnlock(hMem);
              EmptyClipboard();
              SetClipboardData(CF_TEXT, hMem);
           }
        }
        CloseClipboard();             
     }
     VMBNextCode();   
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0045 - ClipboardGet
// #############################################################################
void RTL_ClipboardGet()
{    
     HANDLE Clip;
     if (OpenClipboard(NULL))
     {
      Clip = GetClipboardData(CF_TEXT);
      if ( Clip != NULL)
      {
        HANDLE hMem = GlobalLock(Clip);
        if (hMem != NULL)
        {
          char *newBuffer = (char*)VMBAlloc(VMBStrLen(hMem)+2);
          strncpy(newBuffer, (char*)hMem, VMBStrLen(hMem));
          VMBPushRStack(newBuffer, VMBStrLen(newBuffer));
          newBuffer = (char*)VMBFree(newBuffer);
        } else {
          VMBPushRStack("", 0);     
        }
      } else {
        VMBPushRStack("", 0);     
      }   
      CloseClipboard();   
     }
     VMBSkipBrackets();
}

// #############################################################################
// # 0046 - ClipboardClear
// #############################################################################
void RTL_ClipboardClear()
{    
     if (OpenClipboard(NULL))
     {
      EmptyClipboard();
      CloseClipboard();
     }
     VMBSkipBrackets();
}

// #############################################################################
// # 0047 - Sin
// #############################################################################
void RTL_Sin()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)sin(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0048 - Cos
// #############################################################################
void RTL_Cos()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)cos(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0049 - Tan
// #############################################################################
void RTL_Tan()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)tan(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0050 - isFloat
// #############################################################################
void RTL_isFloat()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     if ( !strcmp(Param1, "") )
     {
          VMBPushRStack("0", 1);          
     } else {
         dTemp = strtod(Param1, &Param2);   
         if ( dTemp != 0 )
            {
                  VMBPushRStack("1", 1);                
            } else {
                  if ( (int)(char)*Param2 == 0x00 )
                  {
                     VMBPushRStack("1", 1);                
                  } else {
                     VMBPushRStack("0", 1);                                     
                  }
            }
     }
     VMBNextCode();  
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0051 - isInt
// #############################################################################
void RTL_isInt()
{
     long dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     if ( !strcmp(Param1, "") )
     {
          VMBPushRStack("0", 1);          
     } else {
         dTemp = strtol(Param1, &Param2, 10);   
         if ( dTemp != 0 )
            {
                  VMBPushRStack("1", 1);                
            } else {
                  if ( (int)(char)*Param2 == 0x00 )
                  {
                     VMBPushRStack("1", 1);                
                  } else {
                     VMBPushRStack("0", 1);                                     
                  }
            }
     }
     VMBNextCode();  
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0052 - Or
// #############################################################################
void RTL_Or()
{    
     int i1, i2;
     char *Param1 = VMBParseOneString();     
     char *Param2 = VMBParseOneString();          
     char *szBuffer = (char*)VMBAlloc(16);
     i1 = atoi(Param1);
     i2 = atoi(Param2);
     sprintf(szBuffer, "%i", i1 | i2);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     VMBNextCode();
     VMBFree(Param1);
     VMBFree(Param2);
     VMBFree(szBuffer);          
}

// #############################################################################
// # 0053 - Not
// #############################################################################
void RTL_Not()
{    
     int i1, i2;
     char *Param1 = VMBParseOneString();     
     char *szBuffer = (char*)VMBAlloc(16);
     i1 = atoi(Param1);
     // TODO: nur 0 und 1?!
     sprintf(szBuffer, "%i", (int)!i1);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     VMBNextCode();
     VMBFree(Param1);
     VMBFree(szBuffer);          
}

// #############################################################################
// # 0054 - And
// #############################################################################
void RTL_And()
{    
     int i1, i2;
     char *Param1 = VMBParseOneString();     
     char *Param2 = VMBParseOneString();          
     char *szBuffer = (char*)VMBAlloc(16);
     i1 = atoi(Param1);
     i2 = atoi(Param2);
     sprintf(szBuffer, "%i", i1 & i2);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     VMBNextCode();
     VMBFree(Param1);
     VMBFree(Param2);
     VMBFree(szBuffer);          
}

// #############################################################################
// # 0055 - Xor
// #############################################################################
void RTL_Xor()
{    
     int i1, i2;
     char *Param1 = VMBParseOneString();     
     char *Param2 = VMBParseOneString();          
     char *szBuffer = (char*)VMBAlloc(16);
     i1 = atoi(Param1);
     i2 = atoi(Param2);
     sprintf(szBuffer, "%i", i1 ^ i2);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     VMBNextCode();
     VMBFree(Param1);
     VMBFree(Param2);
     VMBFree(szBuffer);          
}

// #############################################################################
// # 0056 - Mod
// #############################################################################
void RTL_Mod()
{    
     int i1, i2;
     char *Param1 = VMBParseOneString();     
     char *Param2 = VMBParseOneString();          
     char *szBuffer = (char*)VMBAlloc(16);
     i1 = atoi(Param1);
     i2 = atoi(Param2);
     sprintf(szBuffer, "%i", i1 % i2);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     VMBNextCode();
     VMBFree(Param1);
     VMBFree(Param2);
     VMBFree(szBuffer);          
}


// #############################################################################
// # 0057 - SetPrecision
// #############################################################################
void RTL_SetPrecision()
{
     char *Param1 = VMBParseOneString();
     VMBSetFloatPrecision(atoi(Param1));     
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
}

// #############################################################################
// # 0058 - GetPrecision
// #############################################################################
void RTL_GetPrecision()
{    
     char szBuffer[5];
     sprintf(szBuffer, "%i", VMBGetFloatPrecision());
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));
     VMBSkipBrackets();
}

// #############################################################################
// # 0059 - true
// #############################################################################
void RTL_true()
{    
     char *Buffer = (char *)VMBAlloc(5);     
     sprintf(Buffer, "1");     
     VMBPushRStack(Buffer, VMBStrLen(Buffer));     
     Buffer = VMBFree(Buffer);
     VMBSkipBrackets();
}

// #############################################################################
// # 0060 - false
// #############################################################################
void RTL_false()
{    
     char *Buffer = (char *)VMBAlloc(5);     
     sprintf(Buffer, "0");     
     VMBPushRStack(Buffer, VMBStrLen(Buffer));     
     Buffer = VMBFree(Buffer);
     VMBSkipBrackets();
}

// #############################################################################
// # 0061 - DirExists
// #############################################################################
void RTL_DirExists()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(4);
     sprintf(Param2, "%i", VMBDirExists(Param1));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     
     VMBNextCode();   
     Param1 = VMBFree(Param1);          
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0062 - GetTickCount
// #############################################################################
void RTL_GetTickCount()
{    
     char *szBuffer = (char *)VMBAlloc(16);
     sprintf(szBuffer, "%i", GetTickCount());
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));          
     VMBSkipBrackets();
     szBuffer = VMBFree(szBuffer); 
}

// #############################################################################
// # 0063 - Pi
// #############################################################################
void RTL_Pi()
{
     char *Param2 = (char*)VMBGetFloatFloat((double)M_PI);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBSkipBrackets();  
     Param2 = VMBFree(Param2);     
}


// #############################################################################
// # 0064 - CurrentDir()
// #############################################################################
void RTL_CurrentDir()
{    
     char *szBuffer = (char *)VMBAlloc(MAX_PATH + 1);
     int iResult = GetCurrentDirectory(MAX_PATH, szBuffer);
     if (iResult)
     {
          VMBPushRStack(szBuffer, VMBStrLen(szBuffer));          
     } else {
          VMBPushRStack("", 0);       
     }
     szBuffer = VMBFree(szBuffer);
     VMBSkipBrackets();
}

// #############################################################################
// # 0065 - MkDir
// #############################################################################
void RTL_MkDir()
{
     char *szBuffer = VMBParseOneString();
     if ( VMBMkDir(szBuffer) )
     {
        VMBPushRStack("1", 1);     
     } else {
        VMBPushRStack("0", 1);    
     }
     VMBNextCode();
     szBuffer = VMBFree(szBuffer);
}


// #############################################################################
// # 0066 - ChDir
// #############################################################################
void RTL_ChDir()
{
     char *szBuffer = VMBParseOneString();
     SetCurrentDirectory(szBuffer);
     VMBNextCode();
     szBuffer = VMBFree(szBuffer);
}

// #############################################################################
// # 0067 - RmDir
// #############################################################################
void RTL_RmDir()
{
     char *szBuffer = VMBParseOneString();
     char *DblBuffer = (char*)VMBAlloc(VMBStrLen(szBuffer)+2);
     memcpy(DblBuffer, szBuffer, VMBStrLen(szBuffer));
     
     SHFILEOPSTRUCT FOS;
     memset(&FOS, 0, sizeof(SHFILEOPSTRUCT));
     
     FOS.fFlags = ( FOF_SILENT | FOF_NOCONFIRMATION | FOF_NOERRORUI | FOF_NOCONFIRMMKDIR );
     FOS.wFunc = FO_DELETE;
     FOS.pFrom = DblBuffer;
     
     if ( !SHFileOperation(&FOS) )
     {
        VMBPushRStack("1", 1);    
     } else {
        VMBPushRStack("0", 1);    
     }

     VMBNextCode();
     szBuffer = (char*)VMBFree(szBuffer);
     DblBuffer = (char*)VMBFree(szBuffer);
}

// #############################################################################
// # 0068 - Round
// #############################################################################
void RTL_Round()
{
     double dTemp;
     int iTemp;
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(16);
     iTemp = (int)(strtod(Param1, NULL) + 0.5);
     sprintf(Param2, "%i", iTemp);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0069 - RoundTo
// #############################################################################
void RTL_RoundTo()
{
     double dTemp;
     double dFact = 1;
     int iCount;
     long Temp;
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     char *Param3 = (char*)VMBAlloc(32);     
     iCount = atoi(Param2);
     if (iCount < 0 | iCount > 16)
        iCount = 0;        
     while (iCount--)
     {
           dFact *= 10;      
     }
     dTemp = strtod(Param1, NULL) * dFact + 0.5;   
     dTemp = (int)dTemp;
     dTemp = dTemp / dFact;
     sprintf(Param3, "%f", dTemp);       
     VMBPushRStack(Param3, VMBStrLen(Param3));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);     
}

// #############################################################################
// # 0070 - ConsoleGetX
// #############################################################################
void RTL_ConsoleGetX()
{    
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
     {
        char TempBuffer[8];
        CONSOLE_SCREEN_BUFFER_INFO hCSBI;      
        GetConsoleScreenBufferInfo(hConsole, &hCSBI);
        sprintf(TempBuffer, "%i", hCSBI.dwCursorPosition.X);
        VMBPushRStack(TempBuffer, VMBStrLen(TempBuffer));
     } else {
        VMBPushRStack("0", 1);
     }
     VMBSkipBrackets();
}

// #############################################################################
// # 0071 - ConsoleGetY
// #############################################################################
void RTL_ConsoleGetY()
{    
     HANDLE hConsole = GetStdHandle(STD_OUTPUT_HANDLE);
     if (hConsole != INVALID_HANDLE_VALUE)
     {
        char TempBuffer[8];
        CONSOLE_SCREEN_BUFFER_INFO hCSBI;      
        GetConsoleScreenBufferInfo(hConsole, &hCSBI);
        sprintf(TempBuffer, "%i", hCSBI.dwCursorPosition.Y);
        VMBPushRStack(TempBuffer, VMBStrLen(TempBuffer));
     } else {
        VMBPushRStack("0", 1);
     }
     VMBSkipBrackets();
}

// #############################################################################
// # 0072 - MemWriteByte
// #############################################################################
void RTL_MemWriteByte()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     int iTemp = atoi(Param1);
     char cTemp = atoi(Param2);     
     VMBWriteCharTo(iTemp, cTemp);
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0073 - MemWriteInt
// #############################################################################
void RTL_MemWriteInt()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     int iTemp = atoi(Param1);
     int cTemp = atoi(Param2);     
     VMBWriteIntTo(iTemp, cTemp);
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0074 - MemReadByte
// #############################################################################
void RTL_MemReadByte()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(4);
     int iTemp = atoi(Param1);     
     char cTemp = VMBReadCharAt(iTemp);    
     sprintf(Param2, "%i", (int)cTemp);
     VMBPushRStack(Param2, VMBStrLen(Param2));     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0075 - MemReadInt
// #############################################################################
void RTL_MemReadInt()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(16);
     int iTemp = atoi(Param1);     
     int cTemp = VMBReadIntAt(iTemp);     
     sprintf(Param2, "%i", cTemp);
     VMBPushRStack(Param2, VMBStrLen(Param2));     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0076 - ArcSin
// #############################################################################
void RTL_ArcSin()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)asin(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0077 - ArcCos
// #############################################################################
void RTL_ArcCos()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)acos(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0078 - ArcTan
// #############################################################################
void RTL_ArcTan()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)atan(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0079 - Log
// #############################################################################
void RTL_Log()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)log10(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0080 - Ln
// #############################################################################
void RTL_Ln()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)log(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0081 - Exp
// #############################################################################
void RTL_Exp()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)exp(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}


// #############################################################################
// # 0082 - LogN
// #############################################################################
void RTL_LogN()
{
     double dTemp, dTemp2;
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();
     char *Param3;
     dTemp = strtod(Param1, NULL);
     dTemp2 = strtod(Param2, NULL);
     Param3 = (char*)VMBGetFloatFloat((double)log(dTemp)/log(dTemp2));
     VMBPushRStack(Param3, VMBStrLen(Param3));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);          
}

// #############################################################################
// # 0083 - SQRT
// #############################################################################
void RTL_SQRT()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)sqrt(dTemp));
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0084 - Float
// #############################################################################
void RTL_Float()
{
     double dTemp;
     char *Param1 = VMBParseOneString();
     char *Param2;
     dTemp = strtod(Param1, NULL);
     Param2 = (char*)VMBGetFloatFloat((double)dTemp);
     VMBPushRStack(Param2, VMBStrLen(Param2));
     VMBNextCode();  
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0086 - SetENV
// #############################################################################
void RTL_SetENV()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     if ( SetEnvironmentVariable(Param1, Param2) )
     {
        VMBPushRStack("1", 1);        
     } else {
        VMBPushRStack("0", 1);                    
     }     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0087 - GetENV
// #############################################################################
void RTL_GetENV()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = (char*)VMBAlloc(2048);
     int iTemp;
     GetEnvironmentVariable(Param1, Param2, 2048);
     VMBPushRStack(Param2, VMBStrLen(Param2));           
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0088 - GetAppPath
// #############################################################################
void RTL_GetAppPath()
{
     char *Param1 = (char*)VMBParamStr(0);
     int iTemp = VMBStrLen(Param1) - 1;
     
     if ( *Param1 == '\"' )
     {
        Param1++;    
        iTemp--; 
     }
     
     while ( *(Param1+iTemp) != '\\' && *(Param1+iTemp) != '\0' )
     {
      iTemp--;      
     }
     
     *(Param1+iTemp+1) = '\0';     
     VMBPushRStack(Param1, VMBStrLen(Param1));           
     VMBSkipBrackets();     
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # 0089 - SelectDir
// #############################################################################
void RTL_SelectDir()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Result = (char*)VMBSelectPath(Param1, Param2);
     VMBPushRStack(Result, VMBStrLen(Result));
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Result = VMBFree(Result);
}

// #############################################################################
// # 0089 - SelectColor
// #############################################################################
void RTL_SelectColor()
{
     CHOOSECOLOR *MyColor = (CHOOSECOLOR*)VMBAlloc(sizeof(CHOOSECOLOR)+1);
     char *Param1 = VMBParseOneString();

     MyColor->Flags = CC_FULLOPEN | CC_RGBINIT;
     MyColor->rgbResult = atoi(Param1);
     MyColor->lpCustColors = (COLORREF*)VMBAlloc(sizeof(COLORREF)*16);
     MyColor->lStructSize = sizeof(CHOOSECOLOR);

     int Result = ChooseColor(MyColor);
     if ( Result )
     {
       char *Param2 = (char*)VMBAlloc(16);   
       sprintf(Param2, "%i", MyColor->rgbResult & 0xFFFFFF );
       VMBPushRStack(Param2, VMBStrLen(Param2));
       VMBFree(Param2);
     } else {
       VMBPushRStack("-1", 2);       
     }
         
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     VMBFree(MyColor->lpCustColors);
     MyColor = VMBFree(MyColor);
}

// #############################################################################
// # 0090 - InputDialog
// #############################################################################
void RTL_InputDialog()
{
#ifdef VCL
     HWND Owner = LastCreatedWindow;
     if ( !Owner )
        Owner = GetDesktopWindow();
#else
     HWND Owner = GetDesktopWindow();
#endif

     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Param3 = VMBParseOneString();     

     VMBInputWindow *My = (VMBInputWindow*)VMBAlloc(sizeof(VMBInputWindow));
     My->szCaption = Param1;
     My->szDesc = Param2;           
     My->Value = Param3;           

     char *Res = (char*)DialogBoxParam(RTL_HINSTANCE, MAKEINTRESOURCE(2000), Owner, &VMBInputDialogProc, (LPARAM)My);
     VMBPushRStack(Res, VMBStrLen(Res));               
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);     
     VMBFree(My);
     VMBFree(Res);
}

// #############################################################################
// # 0091 - PasswordDialog
// #############################################################################
void RTL_PasswordDialog()
{
#ifdef VCL
     HWND Owner = LastCreatedWindow;
     if ( !Owner )
        Owner = GetDesktopWindow();
#else
     HWND Owner = GetDesktopWindow();
#endif

     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Param3 = VMBParseOneString();     

     VMBInputWindow *My = (VMBInputWindow*)VMBAlloc(sizeof(VMBInputWindow));
     My->szCaption = Param1;
     My->szDesc = Param2;           
     My->Value = Param3;     
     My->isPassword = 1;      

     char *Res = (char*)DialogBoxParam(RTL_HINSTANCE, MAKEINTRESOURCE(2000), Owner, &VMBInputDialogProc, (LPARAM)My);
     VMBPushRStack(Res, VMBStrLen(Res));               
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);     
     VMBFree(My);
     VMBFree(Res);
}

// #############################################################################
// # 0093 - IsKeyPressed
// #############################################################################
void RTL_IsKeyPressed()
{
     char *Param1 = VMBParseOneString();
     unsigned int iKey = atoi(Param1);
     iKey = iKey & 0xFF;
     char *Buffer = VMBAlloc(256);
     GetKeyboardState(Buffer);        
     if ( *(Buffer+iKey) & 0x80 )
     {
       VMBPushRStack("0", 1);      
     } else {
       if ( *(KeyBuffer+iKey) & 0x80 )   
       {
        VMBPushRStack("1", 1);                    
       } else {
        VMBPushRStack("0", 1);                    
       }
     }
     Param1 = VMBFree(Param1);
     Buffer = VMBFree(Buffer);
     VMBNextCode();
}

// #############################################################################
// # 0094 - IsKeyPushed
// #############################################################################
void RTL_IsKeyPushed()
{
     char *Param1 = VMBParseOneString();
     short Result = GetKeyState(atoi(Param1));
     if ( Result & 0x8000)
     {
      VMBPushRStack("1", 1);     
     } else {
      VMBPushRStack("0", 1);                 
     }
     Param1 = VMBFree(Param1);
     VMBNextCode();
}

// #############################################################################
// # 0095 - UpdateKeys
// #############################################################################
void RTL_UpdateKeys()
{
     GetKeyboardState(KeyBuffer);
     VMBSkipBrackets();
}

HKEY RTL_GetHKEYFromInt(int i)
{
    switch ( i )
    {
           case 1:
                return HKEY_CLASSES_ROOT;
                break;
           case 2:
                return HKEY_CURRENT_USER;
                break;
           case 3:
                return HKEY_LOCAL_MACHINE;
                break;
           case 4:
                return HKEY_USERS;
                break;
           case 5:
                return HKEY_PERFORMANCE_DATA;
                break;
           case 6:
                return HKEY_CURRENT_CONFIG;
                break;
           case 7:
                return HKEY_DYN_DATA;
                break;
           default:
                 return NULL;
                 break;  
    }
}

// #############################################################################
// # 0096 - RegCreateKey
// #############################################################################
void RTL_RegCreateKey()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     HKEY MainKey = RTL_GetHKEYFromInt(atoi(Param1));
     HKEY res;

     if ( RegCreateKey(MainKey, Param2, &res) == ERROR_SUCCESS )
     {
          VMBPushRStack("1", 1);          
     } else {
          VMBPushRStack("0", 1);
     }
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0097 - RegDelKey
// #############################################################################
void RTL_RegDelKey()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     HKEY MainKey = RTL_GetHKEYFromInt(atoi(Param1));
     
     if ( SHDeleteKey(MainKey, Param2) == ERROR_SUCCESS )
     {
          VMBPushRStack("1", 1);          
     } else {
          VMBPushRStack("0", 1);
     }
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
}

// #############################################################################
// # 0098 - RegWriteStr
// #############################################################################
void RTL_RegWriteStr()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Param3 = VMBParseOneString();                     
     char *Param4 = VMBParseOneString();               
     HKEY MainKey = RTL_GetHKEYFromInt(atoi(Param1));
     HKEY SubKey;
     
     if ( RegOpenKey(MainKey, Param2, &SubKey) == ERROR_SUCCESS )         
     {
            if ( RegSetValueEx(SubKey, Param3, 0, REG_SZ, Param4, VMBStrLen(Param4)) == ERROR_SUCCESS ) 
            {
              VMBPushRStack("1", 1);               
            } else {
              VMBPushRStack("0", 1);            
            }
     } else {
            VMBPushRStack("0", 1);            
     }
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);          
     Param4 = VMBFree(Param4);               
}

// #############################################################################
// # 0099 - RegReadStr
// #############################################################################
void RTL_RegReadStr()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Param3 = VMBParseOneString();                                
     HKEY MainKey = RTL_GetHKEYFromInt(atoi(Param1));
     HKEY SubKey;
     
     if ( RegOpenKey(MainKey, Param2, &SubKey) == ERROR_SUCCESS )         
     {
            char *szBuffer = (char*)VMBAlloc(1025);
            DWORD RgSize = 1024;
            if ( RegQueryValueEx(SubKey, Param3, NULL, NULL, szBuffer, &RgSize) == ERROR_SUCCESS )
            {
                 VMBPushRStack(szBuffer, RgSize);
            } else {
              VMBPushRStack("", 0);
            }
            szBuffer = (char*)VMBFree(szBuffer);
     } else {
            VMBPushRStack("", 0);            
     }
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);                    
}

// #############################################################################
// # 0100 - RegWriteInt
// #############################################################################
void RTL_RegWriteInt()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Param3 = VMBParseOneString();                     
     char *Param4 = VMBParseOneString();    
     DWORD P4 = atoi(Param4);           
     HKEY MainKey = RTL_GetHKEYFromInt(atoi(Param1));
     HKEY SubKey;
     
     if ( RegOpenKey(MainKey, Param2, &SubKey) == ERROR_SUCCESS )         
     {
            if ( RegSetValueEx(SubKey, Param3, 0, REG_DWORD, (BYTE*)&P4, sizeof(DWORD)) == ERROR_SUCCESS ) 
            {
              VMBPushRStack("1", 1);               
            } else {
              VMBPushRStack("0", 1);            
            }
     } else {
            VMBPushRStack("0", 1);            
     }
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);          
     Param4 = VMBFree(Param4);               
}

// #############################################################################
// # 0101 - RegReadInt
// #############################################################################
void RTL_RegReadInt()
{
     char *Param1 = VMBParseOneString();
     char *Param2 = VMBParseOneString();     
     char *Param3 = VMBParseOneString();                                
     HKEY MainKey = RTL_GetHKEYFromInt(atoi(Param1));
     HKEY SubKey;
     
     if ( RegOpenKey(MainKey, Param2, &SubKey) == ERROR_SUCCESS )         
     {
            DWORD Res;
            DWORD RgSize = sizeof(DWORD);
            if ( RegQueryValueEx(SubKey, Param3, NULL, NULL, (BYTE*)&Res, &RgSize) == ERROR_SUCCESS )
            {
                 char *TempBuffer = (char*)VMBAlloc(32);
                 sprintf(TempBuffer, "%d", (int)Res);
                 VMBPushRStack(TempBuffer, VMBStrLen(TempBuffer));
                 TempBuffer = (char*)VMBFree(TempBuffer);
            } else {
              VMBPushRStack("0", 1);
            }
     } else {
            VMBPushRStack("0", 1);            
     }
     
     VMBNextCode();     
     Param1 = VMBFree(Param1);
     Param2 = VMBFree(Param2);     
     Param3 = VMBFree(Param3);                    
}

// #############################################################################
// # 0102 - GetScreenWidth
// #############################################################################
void RTL_GetScreenWidth()
{
     char szBuffer[16];
     RECT Desktop;
     memset(&szBuffer, 0, sizeof(szBuffer));

     GetClientRect(GetDesktopWindow(), &Desktop);
     sprintf((char*)&szBuffer, "%d", (int)Desktop.right);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));

     VMBSkipBrackets();          
}

// #############################################################################
// # 0103 - GetScreenHeight
// #############################################################################
void RTL_GetScreenHeight()
{
     char szBuffer[16];
     RECT Desktop;
     memset(&szBuffer, 0, sizeof(szBuffer));

     GetClientRect(GetDesktopWindow(), &Desktop);
     sprintf((char*)&szBuffer, "%d", (int)Desktop.bottom);
     VMBPushRStack(szBuffer, VMBStrLen(szBuffer));

     VMBSkipBrackets();          
}

// #############################################################################
// # C000 - Dec [xyz]
// #############################################################################
void RTL_Dec()
{
     char sVTemp1[5];
     VMBGetCurrentCode(sVTemp1);
     VMBNewVar(sVTemp1);  
     VMBNextCode();
#ifdef DEBUGGER
     VMBNextCode();
     char *TempBuffer = NULL;
     int iE = 0;               
     while (*(RTL_Code++) != ':')                                     
           iE++;
     TempBuffer = (char *)VMBAlloc(iE + 1);
     strncpy(TempBuffer, RTL_Code - iE - 1, iE);
     VMBSetVarDbgName(sVTemp1, TempBuffer);
     TempBuffer = VMBFree(TempBuffer); 
#endif     
}

// #############################################################################
// # C001 - Break
// #############################################################################
void RTL_Break()
{
     int LoopT = VMBGetCurrentLoopType();
     
     VMBPanic(1337);
     
     if (LoopT == LOOPTYPE_FOR) 
     {

     } else
     if (LoopT == LOOPTYPE_REPEAT)
     {
               
     } else {
       VMBPanic(34);     
     }
}

// #############################################################################
// # C002 - intern SetVarType
// #############################################################################
void RTL_SetVarType()
{
     char sVTemp1[5], sVTemp2[5], sAType[5], *sDim;
     int VarType;
     VMBGetCurrentCode(sVTemp1);
     VMBNextCode();
     VMBGetCurrentCode(sVTemp2);
     VMBNextCode();  
     VarType = VMBGetIntHexCode(sVTemp2);     
     VMBSetVarType(sVTemp1, VarType);     
     if ( VarType == VARTYPE_ARRAY ) 
     {
        VMBNextCode();
        VMBGetCurrentCode(sAType);     
        VMBNextCode();
        sDim = (char*)VMBParseOneString();
        VMBSetVarTypeArray(sVTemp1, VMBGetIntHexCode(sAType), atoi(sDim));
        sDim = VMBFree(sDim);
        VMBNextCode();
     }
     
}

// #############################################################################
// # EC00 - [var]++
// #############################################################################
void RTL_VarInc()
{
     char sVTemp1[5];
     int *iTemp1;
     double *fTemp1;  
     VMBVar* MyVar;   
     VMBGetCurrentCode(sVTemp1);     
     VMBNextCode();
     MyVar = (VMBVar*)VMBFindVar(sVTemp1);
     if ( !MyVar )
        VMBPanic(539);
     switch ( MyVar->VarType ) {
              case VARTYPE_INT:
                   iTemp1 = (int*)VMBGetVarContentEx(MyVar);
                   (*iTemp1)++;
                   VMBSetVarContent(MyVar, iTemp1);
                   iTemp1 = VMBFree(iTemp1);
                   break;
              case VARTYPE_FLOAT:
                   fTemp1 = (double*)VMBGetVarContentEx(MyVar);
                   (*fTemp1)++;
                   VMBSetVarContent(MyVar, fTemp1);
                   fTemp1 = VMBFree(iTemp1);
                   break;
     }      
}

// #############################################################################
// # EC01 - [var]--
// #############################################################################
void RTL_VarDec()
{
     char sVTemp1[5];
     int *iTemp1;
     double *fTemp1;   
     VMBVar* MyVar;
     VMBGetCurrentCode(sVTemp1);     
     VMBNextCode();
     MyVar = (VMBVar*)VMBFindVar(sVTemp1);
     if ( !MyVar )
        VMBPanic(539);     
     switch ( MyVar->VarType ) {
              case VARTYPE_INT:
                   iTemp1 = (int*)VMBGetVarContentEx(MyVar);
                   (*iTemp1)--;
                   VMBSetVarContent(MyVar, iTemp1);
                   break;
              case VARTYPE_FLOAT:
                   fTemp1 = (double*)VMBGetVarContentEx(MyVar);
                   (*fTemp1)--;
                   VMBSetVarContent(MyVar, fTemp1);
                   break;
     }          
}

// #############################################################################
// # L000 - intern Label %0000%
// #############################################################################
void RTL_DefLabel()
{
     char cC[5];
     VMBGetCurrentCode(&cC[0]);
     LabelLoopLevel[VMBGetIntHexCode(cC)] = CL;
     VMBNextCode();  
}

// #############################################################################
// # L001 - Goto %Label%
// #############################################################################
void RTL_Goto()
{
    char LabelName[5], SearchString[12];
    int I1 = 0;
    int iPos = 0, LabelFound = 0;
    char cC;
    VMBGoto *MyGoto;
    
    VMBGetCurrentCode(&LabelName[0]);
    MyGoto = (VMBGoto*)VMBGetGoto(LabelName);
    if ( MyGoto )
    {
        RTL_Code = (char*)MyGoto->iCode; 

        // Workaround:
        int LP =  VMBGetCurrentLoopType();
        if ( LP == LOOPTYPE_IF )
        {
           CL = LabelLoopLevel[VMBGetIntHexCode(LabelName)];
        } else
        if ( LP == LOOPTYPE_REPEAT )
        {
           CL = LabelLoopLevel[VMBGetIntHexCode(LabelName)];
        }    
    } else {  
        sprintf((char*)&SearchString, "L000:%s:", (char*)&LabelName);
        RTL_Code = RTL_CodeOrig;
        
        // TODO
        I1 = (int)strstr(RTL_Code, SearchString);          
        if (I1 > 0)
        {
               LabelFound = TRUE;
               while ( VMBisQuote(I1-(int)RTL_CodeOrig) )
               {
                     VMBNextCode();
                     if (VMBisQuote(I1-(int)RTL_CodeOrig))
                        I1 = (int)strstr(RTL_Code, SearchString);      
               }
        }
        
        if (!LabelFound)
        {
            VMBPanic(35);
        } else { 
            RTL_Code = (char*)I1; 
            VMBAddGoto(LabelName, I1);
            // Workaround:
            int LP =  VMBGetCurrentLoopType();
            if ( LP == LOOPTYPE_IF )
            {
               CL = LabelLoopLevel[VMBGetIntHexCode(LabelName)];
            } else
            if ( LP == LOOPTYPE_REPEAT )
            {
               CL = LabelLoopLevel[VMBGetIntHexCode(LabelName)];
            }
        }
    }
}

// #############################################################################
// # FFFF - End Of Source
// #############################################################################
void RTL_EoS()
{
     VMBExit(0);     
}

// #############################################################################
// # CCCC - VarAssign
// #############################################################################
void RTL_VarAssign()
{
     char sVTemp1[5];
     char *ContentBuffer;
     int iTemp;
     double dTemp;
     VMBVar *MyVar;
     VMBGetCurrentCode(sVTemp1);  
     VMBNextCode();  
     ContentBuffer = (char *)VMBParseOneString();
     MyVar = (VMBVar*)VMBFindVar(sVTemp1);
     if ( !MyVar )
        VMBPanic(539);
     switch ( MyVar->VarType )
     {
            case VARTYPE_STR:    
                 VMBSetVarContentSTRWOFRee(MyVar, ContentBuffer);
                 break;       
            case VARTYPE_INT:
                 iTemp = atoi(ContentBuffer);
                 VMBSetVarContent(MyVar, &iTemp);     
                 ContentBuffer = VMBFree(ContentBuffer); 
                 break; 
            case VARTYPE_FLOAT:
                 dTemp = strtod(ContentBuffer, NULL);
                 VMBSetVarContent(MyVar, &dTemp);
                 ContentBuffer = VMBFree(ContentBuffer); 
                 break;  
            case VARTYPE_BOOL:
                 iTemp = atoi(ContentBuffer);
                 if ( iTemp != 0 )
                    iTemp = 1;
                 VMBSetVarContent(MyVar, &iTemp);     
                 ContentBuffer = VMBFree(ContentBuffer); 
                 break;         
     }
     VMBNextCode();
}

// #############################################################################
// # F100 - For Definition
// #############################################################################
void RTL_For()
{
     char sCode[5], sVarCode[5], sType[5];
     char *Param1;
     VMBVar *MyVar;
     int iBegin = 0;
     VMBLoopsInc();
     VMBSetCurrentLoopType(LOOPTYPE_FOR);
     
     VMBGetCurrentCode(sCode);
     VMBNextCode();
     
     VMBGetCurrentCode(sVarCode);
     VMBNextCode();
     Param1 = VMBParseOneString();
     iBegin = atoi(Param1);
     VMBNextCode();     
     
     VMBGetCurrentCode(sType);
     VMBNextCode();     
     
     if (!VMBCodeCMP(sType, "F104"))
     {
        VMBAddFor(VMBGetIntHexCode(sCode), sVarCode, 1, RTL_Code);
     } else {
        VMBAddFor(VMBGetIntHexCode(sCode), sVarCode, 2, RTL_Code);            
     }     

     VMBNextCode();     // F101
     VMBNextCode();     // 0001
     VMBNextCode();     // 0001
     
     char *Param2 = VMBParseOneString();
     VMBNextCode();
     
     MyVar = (VMBVar*)VMBFindVar(sVarCode);
     if ( !MyVar )
        VMBPanic(539);
     VMBSetVarContent(MyVar, &iBegin);
     Param2 = VMBFree(Param2);
     Param1 = VMBFree(Param1);
}

// #############################################################################
// # F101 - NextFor Definition
// #############################################################################
void RTL_NextFor()
{
     char sCode[5], TempLabel[5], *Param1;
     int iTemp, iVal;
     int *Temp2;
     
     VMBFor *CFor;
     VMBGetCurrentCode(&sCode[0]);
     VMBNextCode();
     
     CFor = (VMBFor *)VMBGetFor(VMBGetIntHexCode(sCode));
     if (CFor)
     {
       VMBVar *MyVar = (VMBVar*)VMBFindVar(CFor->VarCode);
       Temp2 = (int *)VMBGetVarContent(CFor->VarCode);
       VMBNextCode();
       Param1 = VMBParseOneString();
       VMBNextCode();
       iVal = atoi(Param1);
       Param1 = VMBFree(Param1);
       if (*Temp2 < iVal && CFor->ForType == 1)
       {
          (*Temp2)++;
          VMBSetVarContent(MyVar, (char*)Temp2);
       } else 
       if (*Temp2 > iVal && CFor->ForType == 2)
       {
          (*Temp2)--;
          VMBSetVarContent(MyVar, (char*)Temp2);                 
       } else {
          int isQuote = 0, iBegin = 0, LabelFound = 0, iPos = 0;
          char cC;
          
          RTL_Code = RTL_CodeOrig;
          while ( iPos < RTL_CodeSize )
             {      
                 cC = *(RTL_Code + iPos);
                 if ( cC == '"' )
                     isQuote = !isQuote;
                 if ( cC == 'F' && !isQuote && iPos + 4 < RTL_CodeSize)
                 {         
                           VMBGetCodeAt(&TempLabel, iPos);    
                           if (!VMBCodeCMP(TempLabel, "F103") && iPos + 4 < RTL_CodeSize )
                           {
                              iPos += 5;
                              VMBGetCodeAt(&TempLabel, iPos);
                              iBegin = VMBGetIntHexCode(TempLabel);
                              if ( iBegin == CFor->ForCode )
                              {
                                 LabelFound = 1;
                                 break;                       
                              }
                           }
                 }
                 iPos++;
             }     
          if (!LabelFound)
          {
             VMBExit(1);                
          } else {
             RTL_Code = RTL_CodeOrig + iPos + 5;     
             VMBDelLastFor();
             VMBLoopsDec();
          }
       }
       Temp2 = VMBFree(Temp2);
     } else {
        VMBPanic(36);
     }     
}

// #############################################################################
// # F102 - Next
// #############################################################################
void RTL_Next()
{
     RTL_EndFor();
}

// #############################################################################
// # F103 - EndFor Definition
// #############################################################################
void RTL_EndFor()
{
     char sCode[5], TempLabel[5], cC;
     char *Param1;
     int isQuote = 0, iBegin = 0, LabelFound = 0, iPos = 0;
     
     VMBGetCurrentCode(sCode);

     VMBFor *CFor = (VMBFor *)VMBGetFor(VMBGetIntHexCode(sCode));   
     if ( !CFor ) 
        VMBPanic(540);
     RTL_Code = (char*)CFor->Code_Begin;         
     return;

/*
     VMBNextCode();     
     RTL_Code = RTL_CodeOrig;
     while ( iPos < RTL_CodeSize )
     {      
         cC = *(RTL_Code + iPos);
         if ( cC == '"' )
             isQuote = !isQuote;
         if ( cC == 'F' && !isQuote && iPos + 4 < RTL_CodeSize)
         {         
                   VMBGetCodeAt(&TempLabel, iPos);    
                   if (!VMBCodeCMP(TempLabel, "F101") && iPos + 4 < RTL_CodeSize )
                   {
                      iPos += 5;
                      VMBGetCodeAt(&TempLabel, iPos);
                      if (!VMBCodeCMP(TempLabel, sCode))
                      {
                         LabelFound = 1;
                         break;                       
                      }
                   }
         }
         iPos++;
     }     
     if (!LabelFound)
     {
         VMBExit(1);
     } else {         
         RTL_Code = RTL_CodeOrig + iPos - 5; 
     }*/
}

#ifdef VCL     
// #############################################################################
// # G001 - Component Method
// #############################################################################
void RTL_VCLMethod()
{
     char sVCLName[5], sVCLMethod[5];
     VMBVar *cVCL;
     VMBGetCurrentCode(sVCLName);  
     VMBNextCode();
     VMBGetCurrentCode(sVCLMethod);
     VMBNextCode();
     int iVCLMethod = atoi(sVCLMethod);
     
     cVCL = (VMBVar *)VMBFindVar(sVCLName);
     if ( cVCL != NULL )
     {
         if ( cVCL->VarType == VARTYPE_FORM )
         {
            VCL_FormMethod(cVCL->hWnd, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_TIMER )
         {
            VCL_TimerMethod(cVCL, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_BUTTON )
         {
            VCL_ButtonMethod(cVCL, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_IMAGE )
         {
            VCL_ImageMethod(cVCL, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_CHECKBOX )
         {
            VCL_CheckBoxMethod(cVCL, iVCLMethod);     
         } else   
         if ( cVCL->VarType == VARTYPE_RADIOBUTTON )
         {
            VCL_RadioButtonMethod(cVCL, iVCLMethod);     
         } else         
         if ( cVCL->VarType == VARTYPE_GROUPBOX )
         {
            VCL_GroupBoxMethod(cVCL, iVCLMethod);     
         } else                  
         if ( cVCL->VarType == VARTYPE_LABEL )
         {
            VCL_LabelMethod(cVCL, iVCLMethod);     
         } else    
         if ( cVCL->VarType == VARTYPE_EDIT )
         {
            VCL_EditMethod(cVCL, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_TEXTBOX )
         {
            VCL_TextBoxMethod(cVCL, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_FILE )
         {
            VCL_FileMethod(cVCL, iVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_ARRAY )
         {
            VMBPrevCode();
            char *sVarName = (char*)VMBAlloc(9);
            char *sIndex = VMBParseOneString();            
            int iIndex = atoi(sIndex);
            int oIndex = cVCL->BufferSize;
            sprintf(sVarName, "%s%04i", cVCL->VarName, iIndex);
            VMBNextCode();
            VMBGetCurrentCode(sVCLMethod);
            iVCLMethod = atoi(sVCLMethod);
            VMBNextCode();
            
            cVCL = (VMBVar *)VMBFindVar(sVarName);
            if ( cVCL != NULL )
            {     
                 if ( cVCL->VarType == VARTYPE_FORM )
                 {
                    VCL_FormMethod(cVCL->hWnd, iVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_TIMER )
                 {
                    VCL_TimerMethod(cVCL, iVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_BUTTON )
                 {
                    VCL_ButtonMethod(cVCL, iVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_IMAGE )
                 {
                    VCL_ImageMethod(cVCL, iVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_CHECKBOX )
                 {
                    VCL_CheckBoxMethod(cVCL, iVCLMethod);     
                 } else                 
                 if ( cVCL->VarType == VARTYPE_RADIOBUTTON )
                 {
                    VCL_RadioButtonMethod(cVCL, iVCLMethod);     
                 } else                 
                 if ( cVCL->VarType == VARTYPE_GROUPBOX )
                 {
                    VCL_GroupBoxMethod(cVCL, iVCLMethod);     
                 } else                 
                 if ( cVCL->VarType == VARTYPE_LABEL )
                 {
                    VCL_LabelMethod(cVCL, iVCLMethod);     
                 } else    
                 if ( cVCL->VarType == VARTYPE_EDIT )
                 {
                    VCL_EditMethod(cVCL, iVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_TEXTBOX )
                 {
                    VCL_TextBoxMethod(cVCL, iVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_FILE )
                 {
                    VCL_FileMethod(cVCL, iVCLMethod);     
                 }       
            } else {
              if ( iIndex < 1 || iIndex > oIndex )              
              {
                 VMBCritical("Index(%i) liegt außerhalb des gültigen Bereichs(1-%i)!", iIndex, oIndex);     
              } else {
                VMBPanic(42);      
              }              
            }
                        
            sVarName = (char*)VMBFree(sVarName);            
            sIndex = (char*)VMBFree(sIndex);
         }          
     }     
     // ToDo - nicht jede Methode ist parameterlos!
     VMBProcessMessages();
     VMBSkipBrackets();     
}

// #############################################################################
// # G002 - Component Member
// #############################################################################
void RTL_VCLMember()
{
     char *sVCLName = VMBAlloc(6);
     char *sVCLMethod = VMBAlloc(6);
     VMBVar *cVCL;
     VMBGetCurrentCode(sVCLName);  
     VMBNextCode();
     VMBGetCurrentCode(sVCLMethod);
     VMBNextCode();
     int isVCLMethod = atoi(sVCLMethod);
     
     cVCL = (VMBVar *)VMBFindVar(sVCLName);
     if ( cVCL != NULL )
     {
         if ( cVCL->VarType == VARTYPE_FORM )
         {
            VCL_FormMember(cVCL->hWnd, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_TIMER )
         {
            VCL_TimerMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_BUTTON )
         {
            VCL_ButtonMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_IMAGE )
         {
            VCL_ImageMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_CHECKBOX )
         {
            VCL_CheckBoxMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_RADIOBUTTON )
         {
            VCL_RadioButtonMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_GROUPBOX )
         {
            VCL_GroupBoxMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_LABEL )
         {
            VCL_LabelMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_EDIT )
         {
            VCL_EditMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_TEXTBOX )
         {
            VCL_TextBoxMember(cVCL, isVCLMethod);     
         } else
         if ( cVCL->VarType == VARTYPE_FILE )
         {            
            VCL_FileMember(cVCL, isVCLMethod);     
         } else 
         if ( cVCL->VarType == VARTYPE_ARRAY )
         {            
            VMBPrevCode();
            char *sVarName = (char*)VMBAlloc(9);
            char *sIndex = VMBParseOneString();            
            int iIndex = atoi(sIndex);
            int oIndex = cVCL->BufferSize;
            sprintf(sVarName, "%s%04i", cVCL->VarName, iIndex);
            VMBNextCode();
            VMBGetCurrentCode(sVCLMethod);
            isVCLMethod = atoi(sVCLMethod);
            VMBNextCode();  
            
            cVCL = (VMBVar *)VMBFindVar(sVarName);
            if ( cVCL != NULL )
            {
                 if ( cVCL->VarType == VARTYPE_FORM )
                 {
                    VCL_FormMember(cVCL->hWnd, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_TIMER )
                 {
                    VCL_TimerMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_BUTTON )
                 {
                    VCL_ButtonMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_IMAGE )
                 {
                    VCL_ImageMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_CHECKBOX )
                 {
                    VCL_CheckBoxMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_RADIOBUTTON )
                 {
                    VCL_RadioButtonMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_GROUPBOX )
                 {
                    VCL_GroupBoxMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_LABEL )
                 {
                    VCL_LabelMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_EDIT )
                 {
                    VCL_EditMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_TEXTBOX )
                 {
                    VCL_TextBoxMember(cVCL, isVCLMethod);     
                 } else
                 if ( cVCL->VarType == VARTYPE_FILE )
                 {            
                    VCL_FileMember(cVCL, isVCLMethod);     
                 }
            } else {
              if ( iIndex < 1 || iIndex > oIndex )              
              {
                 VMBCritical("Index(%i) liegt außerhalb des gültigen Bereichs(1-%i)!", iIndex, oIndex);     
              } else {
                VMBPanic(42);      
              }              
            }
                        
            sVarName = (char*)VMBFree(sVarName);            
            sIndex = (char*)VMBFree(sIndex);                             
         }
     } else {
#ifdef DEBUG
         VMBDebug("CODE %s", sVCLName);
#endif            
         VMBPanic(46);       
     } 
     VMBProcessMessages();
     sVCLName = (char*)VMBFree(sVCLName);
     sVCLMethod = (char*)VMBFree(sVCLMethod);     
}
#endif  

// #############################################################################
// # CI00 - If
// #############################################################################
void RTL_If()
{
     char IfType[5], IfCode[5], Operator[5];
     char iParamFound = 0;     
     char *Param1, *Param2, *Result;     
     char isRepeat = 0;
     double dRes1, dRes2;
     unsigned int iStrLen = 0, I1, I2, I3, I4;     
     Result = (char*)VMBAlloc(1024);     
     VMBIF *NewIF;
     
     RTL_Code -= 5;
     VMBGetCurrentCode(&IfType[0]);
     if (!VMBCodeCMP(IfType, "CI00"))
     {     
         VMBNextCode();  
         VMBAddLoop(LOOPTYPE_IF);         
         VMBGetCurrentCode(&IfCode[0]);     
         VMBNextCode();
         NewIF = (VMBIF*)VMBAddIf(IfCode, RTL_Code-RTL_CodeOrig-10);     
         NewIF->wasExecuted = 0;
         VMBNextCode();  
     } else 
     if (!VMBCodeCMP(IfType, "CI03")){
          VMBNextCode();  
          VMBGetCurrentCode(&IfCode[0]);     
          VMBNextCode();
          NewIF = (VMBIF*)VMBGetIf(IfCode);
          if ( 1 == NewIF->wasExecuted )
          {
               sprintf(Result, "CI02:%s:", NewIF->IfCode);        
               I3 = (int)strstr(RTL_Code, Result);  
               if (I3 > 0)
               {
                   while ( VMBisQuote(I3-(int)RTL_CodeOrig) )
                   {
                         VMBNextCode();
                         if (VMBisQuote(I3-(int)RTL_CodeOrig))
                            I3 = (int)strstr(RTL_Code, Result);      
                   }
               }      
               if (I3 == 0)         
                  VMBPanic(38);
               RTL_Code = (char*)I3;
               VMBFree(Result);
               return;
          }
          VMBNextCode();          
     } else
     if (!VMBCodeCMP(IfType, "CR01")){
          VMBNextCode();  
          VMBGetCurrentCode(&IfCode[0]);     
          VMBNextCode();
          VMBNextCode();          
     } else {
          VMBPanic(37);
     }
     while ( !iParamFound )
     {
           Param1 = VMBParseOneString();            
           VMBNextCode();
           VMBGetCurrentCode(&Operator[0]);
           VMBNextCode();
           Param2 = VMBParseOneString();
           VMBNextCode();
#if VERBOSE > 2           
           VMBDebug("IF: Param1: %s\r\nParam2: %s\r\nOperator: %s", Param1, Param2, Operator);
#endif
           
           if (!VMBCodeCMP(Operator, "CI20"))
           {
              if (!strcmp(Param1, Param2))
              {
                 *(Result+(iStrLen++)) = '1';
              } else {
                 *(Result+(iStrLen++)) = '0';                     
              }
           } else
           if (!VMBCodeCMP(Operator, "CI21"))
           {
              if (strcmp(Param1, Param2) != 0)
              {
                 *(Result+(iStrLen++)) = '1';
              } else {
                 *(Result+(iStrLen++)) = '0';                     
              }
           } else  
           if (!VMBCodeCMP(Operator, "CI22"))
           {
              dRes1 = strtod(Param1, NULL);                   
              dRes2 = strtod(Param2, NULL);                   
              if ( dRes1 < dRes2 )
              {
                 *(Result+(iStrLen++)) = '1';
              } else {
                 *(Result+(iStrLen++)) = '0';                     
              }   
           } else  
           if (!VMBCodeCMP(Operator, "CI23"))
           {
              dRes1 = strtod(Param1, NULL);                   
              dRes2 = strtod(Param2, NULL);                   
              if ( dRes1 > dRes2 )
              {
                 *(Result+(iStrLen++)) = '1';
              } else {
                 *(Result+(iStrLen++)) = '0';                     
              }   
           } else  
           if (!VMBCodeCMP(Operator, "CI24"))
           {
              dRes1 = strtod(Param1, NULL);                   
              dRes2 = strtod(Param2, NULL);                   
              if ( dRes1 <= dRes2 )
              {
                 *(Result+(iStrLen++)) = '1';
              } else {
                 *(Result+(iStrLen++)) = '0';                     
              }   
           } else  
           if (!VMBCodeCMP(Operator, "CI25"))
           {
              dRes1 = strtod(Param1, NULL);                   
              dRes2 = strtod(Param2, NULL);                   
              if ( dRes1 >= dRes2 )
              {
                 *(Result+(iStrLen++)) = '1';
              } else {
                 *(Result+(iStrLen++)) = '0';                     
              }   
           }   
           
           Param1 = VMBFree(Param1);
           Param2 = VMBFree(Param2);        
           
           VMBGetCurrentCode(&Operator[0]);
           VMBNextCode();
           if (!VMBCodeCMP(Operator, "CI11"))
              iParamFound = 1;
           if (!VMBCodeCMP(Operator, "CI12"))
           {
              VMBGetCurrentCode(&Operator[0]);
              VMBNextCode();
              if (!VMBCodeCMP(Operator, "CI13"))
                *(Result+(iStrLen++)) = '&';
              else
              if (!VMBCodeCMP(Operator, "CI14"))
                *(Result+(iStrLen++)) = '|';                
              else
              if (!VMBCodeCMP(Operator, "CI15"))
                *(Result+(iStrLen++)) = '^';                
              else
              if (!VMBCodeCMP(Operator, "CI16"))
                *(Result+(iStrLen++)) = '!';                
              else
                *(Result+(iStrLen++)) = '§';
           }
     }
     if (VMBIfisTrue(Result))
     {  
       Result = (char *)VMBFree(Result);                      
       if (VMBGetCurrentLoopType() == LOOPTYPE_IF)       
       {
              NewIF->wasExecuted = 1;              
              return;
       } else
       if (VMBGetCurrentLoopType() == LOOPTYPE_REPEAT)
       {
              VMBDelLastRep();
              VMBLoopsDec();
              return;
       }
     } else {   
       Result = (char *)VMBFree(Result); 
       if (VMBGetCurrentLoopType() == LOOPTYPE_IF)       
       {           
           char SearchString1[16], SearchString2[16], SearchString3[16];
           
           sprintf((char*)&SearchString1, "CI01:%s:", NewIF->IfCode);
           sprintf((char*)&SearchString2, "CI02:%s:", NewIF->IfCode);
           sprintf((char*)&SearchString3, "CI03:%s:", NewIF->IfCode);    
           
           NewIF->wasExecuted = 0;
           
           I1 = (int)strstr(RTL_Code, SearchString1);          
           I2 = (int)strstr(RTL_Code, SearchString2);          
           I3 = (int)strstr(RTL_Code, SearchString3);  
           I4 = (int)RTL_Code;
           
           if (I1 > 0)
           {
               while ( VMBisQuote(I1-(int)RTL_CodeOrig) )
               {
                     VMBNextCode();
                     if (VMBisQuote(I1-(int)RTL_CodeOrig))
                        I1 = (int)strstr(RTL_Code, SearchString1);      
               }
               RTL_Code = (char *)I4;
           }
    
           if (I2 > 0)
           {
               while ( VMBisQuote(I2-(int)RTL_CodeOrig) )
               {
                     VMBNextCode();
                     if (VMBisQuote(I2-(int)RTL_CodeOrig))
                        I2 = (int)strstr(RTL_Code, SearchString2);      
               }
               RTL_Code = (char *)I4;
           }
           
           if (I3 > 0)
           {
               while ( VMBisQuote(I3-(int)RTL_CodeOrig) )
               {
                     VMBNextCode();
                     if (VMBisQuote(I3-(int)RTL_CodeOrig))
                        I3 = (int)strstr(RTL_Code, SearchString3);      
               }
               RTL_Code = (char *)I4;
           }
    
           if (I3 &&  ( I3 < I2 || I2 == 0) )
           {
                  RTL_Code = (char*)I3;
           } else
           if (I2 && ( I2 < I1 || I1 == 0 ) )
           {
                  RTL_Code = (char*)I2;       
           } else {
             RTL_Code = (char*)I1;
             }
           if (RTL_Code == NULL)
           {
              VMBPanic(38);            
           }
       } else
       if (VMBGetCurrentLoopType() == LOOPTYPE_REPEAT)       
       {
           VMBREPEAT *Rep = (VMBREPEAT*)VMBGetRep(IfCode);
           RTL_Code = (char*)Rep->iPos;
           return;                                   
       }
     }
}

// #############################################################################
// # CI01 - If
// #############################################################################
void RTL_Else()
{
     char IfCode[5];
     VMBGetCurrentCode(&IfCode[0]);
     VMBIF *CurrentIF = (VMBIF*)VMBGetIf(IfCode);
     if ( CurrentIF == NULL )
     {
       VMBPanic(39);
     } else {
       if ( CurrentIF->wasExecuted )
       {
          char SearchString[16];
          sprintf((char*)&SearchString, "CI02:%s:", CurrentIF->IfCode);
          RTL_Code = strstr(RTL_Code, SearchString);       
          while ( VMBisQuote(RTL_Code - RTL_CodeOrig) )
          {
                VMBNextCode(38);
                RTL_Code = strstr(RTL_Code, SearchString);       
                if ( RTL_Code == NULL )
                {
                 VMBPanic(57);
                }
          }
       } else {
         CurrentIF->wasExecuted = 1;
         VMBNextCode();      
       }
     }
}

// #############################################################################
// # CI02 - If
// #############################################################################
void RTL_IfEnd()
{
     char IfCode[5];
     VMBGetCurrentCode(&IfCode[0]);
     VMBDelIf(IfCode);
     VMBLoopsDec();
     VMBNextCode();     
}

// #############################################################################
// # CI03 - ElseIf
// #############################################################################
void RTL_ElseIf()
{
     RTL_If();
}

// #############################################################################
// # CR00 - Repeat
// #############################################################################
void RTL_Repeat()
{   
     char RepCode[5];
     VMBAddLoop(LOOPTYPE_REPEAT);
     VMBGetCurrentCode(&RepCode[0]);
     VMBNextCode();
     VMBAddRep(RepCode, (int)RTL_Code);
}

// #############################################################################
// # CR01 - Until
// #############################################################################
void RTL_Until()
{
     RTL_If();
}

// #############################################################################
// # CS00 - SubDef
// #############################################################################
void RTL_SubDef()
{
      int i;
      char SubCode[5], SCode[5];
      char *SearchBuffer = NULL;
      VMBSub *NewSub = NULL;
      
      VMBGetCurrentCode(&SCode[0]);
      if ( !VMBisSubDefined(SCode) )
      {
         NewSub = (VMBSub*)VMBGetNewSub();
         strncpy(NewSub->Code, (char*)&SCode, 4);
         NewSub->iCode = VMBGetIntHexCode(SCode) + 1023;
         NewSub->isDefined = TRUE;
         NewSub->isExtern = FALSE;

         VMBNextCode();
         VMBGetCurrentCode(&SubCode[0]);
         NewSub->ResultType = VMBGetIntHexCode(SubCode);
         VMBNextCode();
         VMBGetCurrentCode(&SubCode[0]);
         NewSub->Parameters = VMBGetIntHexCode(SubCode);
         VMBNextCode();
         if ( NewSub->Parameters > 0 )
         {
            for ( i = 0; i < NewSub->Parameters; i++ )
            {
                VMBGetCurrentCode(&SubCode[0]);     
                NewSub->SubParams[i] = (VMBVar*)VMBAlloc(sizeof(VMBVar));
                NewSub->SubParams[i]->isDeclared = 1;
                NewSub->SubParams[i]->isLocal = 1;                
                NewSub->SubParams[i]->VarName = (char*)VMBAlloc(5);                
                sprintf(NewSub->SubParams[i]->VarName, "%04X", 0xFFFF - i - 1);                
                NewSub->SubParams[i]->VarType = VMBGetIntHexCode(SubCode); 

                // TODO: nur ein Workaround!
//                NewSub->SubParams[i]->VarContent = VMBAlloc(16);
//                NewSub->SubParams[i]->BufferSize = 16;
                
                VMBNextCode();    
            }
         }
         NewSub->Pos = (DWORD)RTL_Code;
      }
      SearchBuffer = (char*)VMBAlloc(16);
      sprintf(SearchBuffer, "CS01:%s:", SCode);
      i = (int)strstr(RTL_Code, SearchBuffer);
      if (i > 0)
           {
               while ( VMBisQuote(i-(int)RTL_CodeOrig) )
               {
                     VMBNextCode();
                     if (VMBisQuote(i-(int)RTL_CodeOrig))
                        i = (int)strstr(RTL_Code, SearchBuffer);      
               }
           }
      VMBFree(SearchBuffer);
      if ( i == 0 )
      {
           VMBPanic(40);
      } else {
           RTL_Code = (char*)i + 10;
      }
}

// #############################################################################
// # CS01 - EndSub
// #############################################################################
void RTL_EndSub()
{
      int i;
      void *OldCSub;
      
      VMBDelLocalVars();
      VMBClearLocalIfs();

      RTL_Code = (char*)CurrentSub->OldPos;
      for (i = 0; i < CurrentSub->Parameters; i++ )
      {
          if (CurrentSub->SubParams[i] != NULL)
          {
                  if ( CurrentSub->SubParams[i]->VarType < VARTYPE_ARRAY )
                  {
                      CurrentSub->SubParams[i]->VarContent = VMBFree(CurrentSub->SubParams[i]->VarContent);
                  } 
                  CurrentSub->SubParams[i]->VarName = (char*)VMBFree(CurrentSub->SubParams[i]->VarName);
                  CurrentSub->SubParams[i] = VMBFree(CurrentSub->SubParams[i]);
          }
      }
      
      if ( !CurrentSub->Result && CurrentSub->ResultType > 0 )
      {
          if ( CurrentSub->ResultType == VARTYPE_BOOL )
          {
             VMBPushRStack("0", 1);        
          } else
          if ( CurrentSub->ResultType == VARTYPE_INT )
          {
             VMBPushRStack("0", 1);        
          } else          
          if ( CurrentSub->ResultType == VARTYPE_FLOAT )
          {
             VMBPushRStack("0", 1);        
          } else          
          if ( CurrentSub->ResultType == VARTYPE_STR )
          {
             VMBPushRStack("", 0);        
          } 
      }
      
      OldCSub = (void*)CurrentSub;
      if ( CurrentSub != NULL )
         CL = CurrentSub->LoopLevel;
      CurrentSub = (VMBCSub*)CurrentSub->SprevCSub;
      if ( CurrentSub != NULL) 
         CurrentSub->SnextCSub = NULL;
      if ( OldCSub != NULL )
         VMBFree(OldCSub);         
      SubLevel--;
}


// #############################################################################
// # CS02 - Return
// #############################################################################
void RTL_Return()
{
      int iTemp;
      double dTemp;
      if (CurrentSub->ResultType > 0 )
      {
          char *Param1 = VMBParseOneString();
/*          switch ( CurrentSub->ResultType )
          {
                 case VARTYPE_STR:
                      CurrentSub->Result = (char*)VMBAlloc(VMBStrLen(Param1)+1);
                      strncpy(CurrentSub->Result, Param1, VMBStrLen(Param1));
                      break;       
                 case VARTYPE_INT:
                      iTemp = atoi(Param1);
                      CurrentSub->Result = (char*)VMBAlloc(sizeof(int));
                      memcpy(CurrentSub->Result, &iTemp, sizeof(int));
                      break;
                 case VARTYPE_BOOL:
                      iTemp = atoi(Param1);
                      if (iTemp != 0)
                         iTemp = 1;
                      CurrentSub->Result = (char*)VMBAlloc(sizeof(int));
                      memcpy(CurrentSub->Result, &iTemp, sizeof(int));
                      break;
                 case VARTYPE_FLOAT:
                      dTemp = strtod(Param1, NULL);
                      CurrentSub->Result = (char*)VMBAlloc(sizeof(double));
                      memcpy(CurrentSub->Result, &iTemp, sizeof(double));
                      break;                  
          }*/
          VMBPushRStack(Param1, VMBStrLen(Param1));
          Param1 = (char*)VMBFree(Param1);
          CurrentSub->Result = 1;
      }
      RTL_EndSub();
}

// #############################################################################
// # EX00 - Extern Sub
// #############################################################################
void RTL_ExternSub()
{
     char sCode[5];
     int iE = 0, i;
     char *TempBuffer = NULL;
     VMBSub *NewSub;
     
     VMBGetCurrentCode(&sCode[0]);
     if ( !VMBisSubDefined(sCode) )
     {     
            NewSub = (VMBSub*)VMBGetNewSub();
            NewSub->isExtern = 1;
            NewSub->isDefined = 1;
            strncpy(NewSub->Code, (char*)&sCode, 5);
            NewSub->iCode = VMBGetIntHexCode(sCode) + 1023;
            VMBNextCode();           
            RTL_Code++;
            while (*(RTL_Code++) != '\"')                                     
                  iE++;
            TempBuffer = (char *)VMBAlloc(iE + 1);
            strncpy(TempBuffer, (RTL_Code++) - iE - 1, iE);
            NewSub->DllName = TempBuffer;          
            RTL_Code++;            
            iE = 0;
            while (*(RTL_Code++) != '\"')                                     
                  iE++;
            TempBuffer = (char *)VMBAlloc(iE + 1);
            strncpy(TempBuffer, (RTL_Code++) - iE - 1, iE);                      
            NewSub->DllFunc = TempBuffer;          
            VMBGetCurrentCode(&sCode[0]);
            NewSub->ResultType = VMBGetIntHexCode(sCode);
            VMBNextCode();
            VMBGetCurrentCode(&sCode[0]);
            NewSub->Parameters = VMBGetIntHexCode(sCode);
            VMBNextCode();
            if ( NewSub->Parameters > 0 )
            {
                for ( i = 0; i < NewSub->Parameters; i++ )
                {
                    VMBGetCurrentCode(&sCode[0]);     
                    NewSub->SubParams[i] = (VMBVar*)VMBAlloc(sizeof(VMBVar));
                    NewSub->SubParams[i]->isDeclared = 1;
                    NewSub->SubParams[i]->isLocal = 1;                
                    NewSub->SubParams[i]->VarName = (char*)VMBAlloc(5);                
                    sprintf(NewSub->SubParams[i]->VarName, "%04X", 0xFFFF - i - 1);                
                    NewSub->SubParams[i]->VarType = VMBGetIntHexCode(sCode); 
                    // TODO: nur ein Workaround!
                    NewSub->SubParams[i]->VarContent = VMBAlloc(16);
                    NewSub->SubParams[i]->BufferSize = 16;                    
                    VMBNextCode();    
                }        
            }
     }
}

#ifdef DEBUGGER

extern unsigned int CurrentSRCLine;

void RTL_DbgSetLine()
{
   char *TempBuffer = NULL;
   int iE = 0;               
   while (*(RTL_Code++) != ':')                                     
    iE++;
   TempBuffer = (char *)VMBAlloc(iE + 1);
   strncpy(TempBuffer, RTL_Code - iE - 1, iE);
   CurrentSRCLine = atoi(TempBuffer) - 1;
   SendMessage(CC_hSrcView, LB_SETCURSEL, (WPARAM) CurrentSRCLine, 0 );
   TempBuffer = VMBFree(TempBuffer);    
   if (isStepMode == TRUE)
    isStopped = TRUE;

}

#endif
 
#endif
