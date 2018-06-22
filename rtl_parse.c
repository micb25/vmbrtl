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
 
#ifndef RTL_PARSE_C
#define RTL_PARSE_C

#include "main.h"

#include "rtl_parse.h"
#include "rtl_error.h"
#include "rtl_vars.h"
#include "rtl_csub.h"
#include "rtl_subs.h"
#include "rtl_ccmd.h"
#include "rtl_dbgwin.h"

extern VMBCSub *CurrentSub;
void VMBCallCustomSubWithReturn(VMBSub *CallSub);
extern int SubLevel;
extern int CL;

#ifdef DEBUGGER
extern HWND CC_SRC;
extern HWND CC_hSrcView;
extern char *SrcCode;
extern unsigned int iSrcSize;
#endif

#ifdef VMBFASTCALL
extern char* FastCallPtr;
#endif

//extern int VMBGetCC(char *szBuffer);

#ifdef DEBUG
extern long MemoryAllocated;
#endif

// -----------------------------------------------------------------------------
// VMBNextCode() - springt zum nächsten Opcode (4 Byte Code + 1 Byte Seperator)
// -----------------------------------------------------------------------------
void VMBNextCode()
{
 RTL_Code += 5;
}

// -----------------------------------------------------------------------------
// VMBPrevCode() - springt zum vorherigen Opcode (4 Byte Code + 1 Byte Seperator)
// -----------------------------------------------------------------------------
void VMBPrevCode()
{
 RTL_Code -= 5;
}

// -----------------------------------------------------------------------------
// VMBSkipBrackets() - überspringt OpCodes bei Befehlen ohne Parameter
// -----------------------------------------------------------------------------
void VMBSkipBrackets()
{
/*
 // E000
 // E001     
*/
 RTL_Code += 10;
}

// -----------------------------------------------------------------------------
// VMBGetCurrentCode(char *) - schreibt den aktuellen Opcode in eine Variable
// -----------------------------------------------------------------------------
/*

  in RTL_ASM.ASM ausgelagert!

void VMBGetCurrentCode(char *szBuffer)
{
 int i;
 for (i = 0; i < 4; i++ )
  *(szBuffer++) = *(RTL_Code + i);
 *(szBuffer) = '\0';

// strncpy(szBuffer, RTL_Code, 4);
// *(szBuffer+4) = '\0';
 
#ifdef CODEWINDOW 
 UpdateCC(szBuffer - 4);
#endif
}*/


// -----------------------------------------------------------------------------
// VMBGetCodeAt(char *, int) - schreibt den Code an einer Stelle in eine Variable
// -----------------------------------------------------------------------------
void VMBGetCodeAt(char *szBuffer, int iPos)
{
 int i;
 for (i = 0; i < 4; i++)
  *(szBuffer++) = *(RTL_Code + iPos + i);
 *(szBuffer) = '\0';
}

// -----------------------------------------------------------------------------
// VMBCallCustomSub() - führt eine benutzerdefinierte Sub aus
// -----------------------------------------------------------------------------
void VMBCallCustomSub(VMBSub* MySub)
{
     void *TempPointer;
     VMBCSub *NewSub, *NCurrentSub;
     int i, iTemp;
     double dTemp;

     if (MySub->isExtern == 0 )
     {
         SubLevel++;   
#if VERBOSE > 0
         VMBDebug(" -> %s(%04i), Level = %i", __func__, MySub->iCode, SubLevel);
#endif     
         if ( CurrentSub != NULL ) 
         {
            NCurrentSub = (VMBCSub*)VMBAlloc(sizeof(VMBCSub)); 
            TempPointer = (void*)CurrentSub;  
            //NCurrentSub = (VMBCSub*)NCurrentSub->SnextCSub;
            NCurrentSub->SprevCSub = (void*)TempPointer;
            NCurrentSub->SnextCSub = NULL;
         } else {
            NCurrentSub = (VMBCSub*)VMBAlloc(sizeof(VMBCSub));
            NCurrentSub->SprevCSub = NULL;              
            NCurrentSub->SnextCSub = NULL;
         }
         memcpy(NCurrentSub->Code, MySub->Code, 5);
         NCurrentSub->iCode = MySub->iCode;
         NCurrentSub->isExtern = MySub->isExtern;
         NCurrentSub->Pos = MySub->Pos;
         NCurrentSub->OldPos = MySub->OldPos;
         NCurrentSub->ResultType = MySub->ResultType;
         NCurrentSub->Parameters = MySub->Parameters;
         if ( MySub->Parameters == 0 )
         {
            VMBSkipBrackets();     
            NCurrentSub->SubParams[0] = NULL;
         } else {
                
            for ( i = 0; i < MySub->Parameters; i++ )    
            {
                if ( NCurrentSub->SubParams[i] == NULL ) 
                {
                   NCurrentSub->SubParams[i] = (VMBVar*)VMBAlloc(sizeof(VMBVar));            
                   NCurrentSub->SubParams[i]->isDeclared = 1;
                   NCurrentSub->SubParams[i]->isLocal = 1;
                   NCurrentSub->SubParams[i]->BufferSize = 0;
                   NCurrentSub->SubParams[i]->VarType = MySub->SubParams[i]->VarType;
                   NCurrentSub->SubParams[i]->VarName = (char*)VMBAlloc(5);
                   sprintf(NCurrentSub->SubParams[i]->VarName, "%04X", 0xFFFE - i);
                }
                  
            }            
                
            for ( i = 0; i < MySub->Parameters; i++ )    
            {
                char *szBuffer = NULL;
//                if ( CurrentSub->SprevCSub != NULL )
//                   CurrentSub = (VMBCSub*)CurrentSub->SprevCSub;
                szBuffer = (char*)VMBParseOneString();                            
//                if ( CurrentSub->SnextCSub != NULL )
//                   CurrentSub = (VMBCSub*)CurrentSub->SnextCSub;
                switch ( MySub->SubParams[i]->VarType )
                {
                       case VARTYPE_STR:
                            VMBSetVarContentEx(NCurrentSub->SubParams[i], szBuffer);
                            break;
                       case VARTYPE_INT:
                            iTemp = atoi(szBuffer);
                            VMBSetVarContentEx(NCurrentSub->SubParams[i], (char*)&iTemp);
                            break;
                       case VARTYPE_BOOL:
                            iTemp = atoi(szBuffer);
                            if ( iTemp != 0 )
                            {
                                 iTemp = 1;
                            } else {
                                 iTemp = 0;
                            }                                 
                            VMBSetVarContentEx(NCurrentSub->SubParams[i], (char*)&iTemp);
                            break;
                       case VARTYPE_FLOAT:
                            dTemp = strtod(szBuffer, NULL);
                            VMBSetVarContentEx(NCurrentSub->SubParams[i], (char*)&dTemp);
                            break;
                       case VARTYPE_VAR:
                       case VARTYPE_FORM:
                       case VARTYPE_BUTTON:
                       case VARTYPE_EDIT:                            
                       case VARTYPE_LABEL:                            
                       case VARTYPE_TIMER:                            
                       case VARTYPE_TEXTBOX:
//                       case VARTYPE_CHECKBOX:                            
//                       case VARTYPE_RADIO:                            
                            if ( NCurrentSub->SubParams[i]->VarName != NULL )
                               NCurrentSub->SubParams[i]->VarName = (char*)VMBFree(NCurrentSub->SubParams[i]->VarName);
                            if ( NCurrentSub->SubParams[i]->VarContent != NULL )
                               NCurrentSub->SubParams[i]->VarContent = (char*)VMBFree(NCurrentSub->SubParams[i]->VarContent);                               
                            memcpy(NCurrentSub->SubParams[i], (VMBVar*)atoi(szBuffer), sizeof(VMBVar));
                            NCurrentSub->SubParams[i]->VarName = (char*)VMBAlloc(5);
                            sprintf(NCurrentSub->SubParams[i]->VarName, "%04X", 0xFFFE - i);
                            break;
                }
                szBuffer = VMBFree(szBuffer);
            }
             VMBNextCode();
         }
         // RTL_SaveStatus();
         NCurrentSub->OldPos = (DWORD)RTL_Code;
         NCurrentSub->LoopLevel = CL;     
         RTL_Code = (char*)NCurrentSub->Pos; 
         
         if ( CurrentSub != NULL ) 
         {
            NewSub = CurrentSub;  
            while ( NewSub )
            {
                  if ( NewSub->SnextCSub != NULL ) 
                  {
                    NewSub = (VMBCSub*)NewSub->SnextCSub;
                  } else {
                    break;       
                  }    
            }
            NewSub->SnextCSub = (char*)NCurrentSub;
            CurrentSub = NCurrentSub;
         } else {
            CurrentSub = (VMBCSub*)NCurrentSub;
         }
         VMBCallCustomSubWithReturn(NULL);
     } else {
       /* externe SUB */ 
       int *Stack = (int*)VMBAlloc(256);
       char *RBuffer;
       double dResult;
       int iResult;
       
       if ( MySub->DllHwnd == NULL )
          MySub->DllHwnd = LoadLibrary(MySub->DllName);
       if ( MySub->DllHwnd == NULL )
          VMBCritical("Die DLL \"%s\" wurde nicht gefunden oder konnte nicht geladen werden! Das Programm wird beendet!", MySub->DllName);
       if ( MySub->DllProc == NULL )
          MySub->DllProc = GetProcAddress(MySub->DllHwnd, MySub->DllFunc);     
       if ( MySub->DllProc == NULL )
          VMBCritical("Die Funktion \"%s\" konnte in der DLL \"%s\" nicht gefunden werden! Das Programm wird beendet!", MySub->DllFunc, MySub->DllName);
       if ( MySub->Parameters > 0 )          
       {
             int i;
             int iTemp;
             char *szBuffer;
             for ( i = 0; i < MySub->Parameters; i++ )
             {
                 szBuffer = (char*)VMBParseOneString();
                 switch ( MySub->SubParams[i]->VarType )
                 {
                        case VARTYPE_STR:
                             iTemp = (int)szBuffer;
                             break;
                        case VARTYPE_INT:
                             iTemp = atoi(szBuffer);
                             szBuffer = VMBFree(szBuffer);
                             break;     
                        case VARTYPE_BOOL:
                             iTemp = atoi(szBuffer);
                             if ( iTemp )
                             {
                               iTemp = TRUE;   
                             } else {
                               iTemp = FALSE;       
                             }
                             szBuffer = VMBFree(szBuffer);
                             break;                                     
                        default:
                             VMBPanic(1337);   
                             break;   
                 }
                 *(Stack + ( i * 4 )) = (int)iTemp;
             }
             VMBNextCode();
       } else {
         VMBSkipBrackets();       
       }
#if VERBOSE > 2
       VMBDebug(" -> call extern sub \"%s\" in \"%s\"", MySub->DllFunc, MySub->DllName);
#endif       
       if ( MySub->ResultType != VARTYPE_FLOAT )
          iResult = VMBExCall((int)MySub->DllProc, MySub->Parameters, (int)Stack);
       else
          dResult = VMBExFCall((int)MySub->DllProc, MySub->Parameters, (int)Stack);
       switch ( MySub->ResultType )
       {
              case VARTYPE_STR:
                   VMBPushRStack((char*)iResult, VMBStrLen((char*)iResult));
                   break;
              case VARTYPE_INT:
                   RBuffer = (char*)VMBAlloc(16);
                   sprintf(RBuffer, "%i", iResult);
                   VMBPushRStack((char*)RBuffer, VMBStrLen((char*)RBuffer));
                   RBuffer = VMBFree(RBuffer);
                   break;    
              case VARTYPE_BOOL:
                   if ( iResult )
                   {
                      VMBPushRStack("1", 1);
                   } else {
                      VMBPushRStack("0", 1);
                   }
                   break;  
              case VARTYPE_FLOAT:
                   RBuffer = (char*)VMBGetFloatFloat(dResult);
                   VMBPushRStack(RBuffer, VMBStrLen(RBuffer));
                   RBuffer = VMBFree(RBuffer);                   
                   break; 
       }
       VMBFree(Stack);
     }
}


extern void *CMD_Pointers[];

// -----------------------------------------------------------------------------
// VMBCallCustomSub() - führt eine benutzerdefinierte Sub aus, inkl Rückgabewert
// -----------------------------------------------------------------------------
void VMBCallCustomSubWithReturn(VMBSub *CallSub)
{
 char *CC = (char*)VMBAlloc(5);   
 VMBCmd *CCMD;
 VMBSub *MySub;
#ifdef VMBFASTCALL 
 int *FastCallSub = NULL;
#endif  

 do {
    VMBProcessMessages();
#ifndef FINAL    
    if ( ( isStepMode == FALSE ) || ( ( isStopped == FALSE ) && ( isStepMode == TRUE) ) ) 
    {
#endif         
        VMBGetCurrentCode(CC);
        RTL_Code += 5;
#ifdef VMBFASTCALL
        if ( FastCallPtr )
        {
             FastCallSub = (int*)((int)FastCallPtr + (int)RTL_Code - (int)RTL_CodeOrig - 5);
             if ( FastCallSub != NULL && *FastCallSub != 0x0000 )
             { 
                void (*VMBRunCMD)() = (void (*)()) (int)*FastCallSub;            
                VMBRunCMD();
                if ( !VMBCodeCMP(CC, "CS01") || !VMBCodeCMP(CC, "CS02") )
                     { 
                        break;     
                     }  
                if ( isRStackUsed )
                   VMBResetRStack();     
                if (isTerminated == TRUE)
                   break; 
                continue;
             }
        }
#endif          
            CCMD = (VMBCmd*)VMBFindCMD(CC);
            if (CCMD == NULL)
                {
                     if (*CC == '1')
                     {
                        MySub = (VMBSub*)VMBGetSubByID(atoi(CC));
                        if ( MySub == NULL )
                        {
                             isTerminated = TRUE; 
                             VMBPanic(43);
                        } else {
                               VMBCallCustomSub(MySub); 
                        }
                     } else {
                         isTerminated = TRUE;   
                         VMBPanic(43);                                    
                     }
                } else {
#if VERBOSE > 0
                     VMBDebug(" -> VMBRunCMD(\"%s\") -> @ 0x%08x", CCMD->CommandName, CCMD->Pointer);
#endif                
                    
                     void (*VMBRunCMD)() = (void (*)()) CCMD->Pointer;   
                     VMBRunCMD();                     
                     if ( !VMBCodeCMP(CCMD->CommandName, "CS01") || !VMBCodeCMP(CCMD->CommandName, "CS02") )
                     { 
                        break;     
                     }  
#ifdef VMBFASTCALL                               
                     if ( FastCallPtr )
                     {
                        int *iTe = (char*)((int)FastCallPtr + (int)RTL_Code - (int)RTL_CodeOrig - 5);
                        *iTe = (int)CCMD->Pointer;
                     }
#endif                         
                }
                if (isRStackUsed == TRUE)
                   VMBResetRStack();
#ifndef FINAL    
    }
#endif    
    if (isTerminated == TRUE)
       break;    
 } while ( 1 );
 VMBFree(CC);
}


// -----------------------------------------------------------------------------
// VMBParseIt(int) - interpretiert Opcodes
// -----------------------------------------------------------------------------
char* VMBParseIt(int ShouldReturn)
{          
 int iTemp1, iTemp2, iTemp3;
 double fTemp1;
 char *sVTemp1, *sVTemp2;
 char CC[5];
 char CCT[5];
 DWORD dTemp1;
 VMBSub *MySub;
#ifdef VMBFASTCALL 
 int *FastCallSub = NULL;
#endif 
 
 resetNeeded = TRUE;

#if VERBOSE > 0     
  VMBDebug(" -> %s();", __func__ );
#endif  
 
 VMBCmd *CCMD; // = (VMBCmd *)VMBAlloc(sizeof(VMBCmd)+1);

 do {
    VMBProcessMessages();
#ifdef DEBUG    
    if ( ( isStepMode == FALSE ) || ( ( isStopped == FALSE ) && ( isStepMode == TRUE) ) ) 
#else
    if ( isStopped == FALSE ) 
#endif
    {
        VMBGetCurrentCode(CC);
        RTL_Code += 5;
#ifdef VMBFASTCALL
        if ( FastCallPtr )
        {
             FastCallSub = (int*)((int)FastCallPtr + (int)RTL_Code - (int)RTL_CodeOrig - 5);
             if ( FastCallSub != NULL && *FastCallSub != 0x0000 )
             { 
                void (*VMBRunCMD)() = (void (*)()) (int)*FastCallSub;            
                VMBRunCMD();
                if ( isRStackUsed )
                   VMBResetRStack();     
                if (isTerminated == TRUE)
                   break; 
                continue;
             }
        }
#endif                 
            CCMD  = (VMBCmd*)VMBFindCMD(CC);
            if ( !CCMD )
                {
                     if (*CC == '1')
                     {
                        MySub = (VMBSub*)VMBGetSubByID(atoi(CC));
                        if ( MySub )
                        {
                             VMBCallCustomSub(MySub);  
                        } else {
                             isTerminated = TRUE;   
#ifdef DEBUG
                             VMBDebug("CODE1: %s\r\n%s", CC, RTL_Code);
#endif                             
                             VMBPanic(44);
                        }
                     } else {
                         isTerminated = TRUE;   
#ifdef DEBUG
                             VMBDebug("CODE2: %s\r\n%s", CC, RTL_Code);
#endif
                         VMBPanic(44);
                     }
                } else {
#if VERBOSE > 0
                     VMBDebug(" -> VMBRunCMD(\"%s\") -> @ 0x%08x", CCMD->CommandName, CCMD->Pointer);
#endif                      
                     void (*VMBRunCMD)() = (void (*)()) CCMD->Pointer;  
#ifdef VMBFASTCALL                               
                     if ( FastCallPtr )
                     {
                        int *iTe = (char*)((int)FastCallPtr + (int)RTL_Code - (int)RTL_CodeOrig - 5);
                        *iTe = (int)CCMD->Pointer;
                     }
#endif              
                     VMBRunCMD();
       
                }
          }
          if ( isRStackUsed )
             VMBResetRStack();
//            if (isStepMode == TRUE)
//                    isStopped = TRUE;
    
    if (isTerminated == TRUE)
       break;    
 } while ( 1 );
  
 return NULL;       
}

// -----------------------------------------------------------------------------
// VMBParse() - Einsprungspunkt aus WinMain()
// -----------------------------------------------------------------------------
void VMBParse()
{     
#if VERBOSE > 0     
 VMBDebug(" -> %s();", __func__ );
#endif  
 
 if (isParsing == 1)
 {
#ifdef DEBUG               
  VMBDebug("already in parsing mode!");              
#endif  
  return;
 }
 
#ifdef DEBUGGER
 char *SrcAddr = RTL_Code;
 char *CopySrc = NULL;
 unsigned int iTemp;
 
 while ( *SrcAddr != 0 )
       SrcAddr++;
       
 if ( *(--SrcAddr) != ':' )
    VMBPanic(583);
    
 *SrcAddr = '\0';
    
 while ( SrcAddr >= RTL_Code )
 {
       if ( *--SrcAddr == ':' )      
       {
          SrcAddr++;  
          break;
       } 
 }
 iTemp = iSrcSize = atoi(SrcAddr) + 1;
 while ( iTemp-- )
 {
       SrcAddr--;
       if ( *SrcAddr == 20 )
          *SrcAddr = '\n';
 }

 SrcCode = (char*)VMBAlloc(iSrcSize+1);
 strncpy(SrcCode, SrcAddr, iSrcSize);
 CopySrc = SrcCode;
 iTemp = 0;
 
 while ( *CopySrc != '\0')
 {
       if ( *CopySrc == '\n' )
       {
          *CopySrc = '\0';
          SendMessage(CC_hSrcView, LB_ADDSTRING, 0, (LPARAM)(LPCTSTR)CopySrc-iTemp);      
          iTemp = -1;
       }
       iTemp++;
       CopySrc++;
 }

 iTemp = SendMessage(CC_hSrcView, LB_GETCOUNT, 0, 0);
 SetScrollRange(CC_hSrcView, SB_VERT, 0, iTemp, 1);

 *SrcAddr = '\0';
 RTL_CodeSize = SrcAddr - RTL_Code;
 SetWindowText(CC_SRC, RTL_Code);  
#endif 
 
 Time1 = GetTickCount();
 
#ifdef DEBUG 
 if (resetNeeded == TRUE)
    VMBRTLReset();
#endif   

#ifdef DEBUGGER
  isStepMode = TRUE;
#ifdef DEBUG  
  VMBDebug("StepMode is ON!");
#endif
#endif 
  
 /* an Parser übergeben */
 isTerminated = FALSE;
 if (RTL_Code != NULL)
 {
#ifdef VMBFASTCALL 
    FastCallPtr = (void*)VMBAlloc(RTL_CodeSize+1);
#endif
              
    isParsing = 1;
    VMBParseIt( FALSE );
#ifdef DEBUG    
    VMBDebug("script executed in %ims", GetTickCount()-Time1);
#endif    
 }
#ifdef DEBUG 
 else 
    VMBDebug("There is no code to parse!");   
#endif    
 isParsing = 0;
}

#endif
  
