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

#include <stdio.h>
#include <stdlib.h>

#include "rtl_ccmd.h"
#include "rtl_cmd.h"
#include "rtl_mem.h"

VMBCmd *FirstCMD;

// -----------------------------------------------------------------------------
// VMBAddCMD(char *, void* Pointer) - fügt einen RTL Befehl hinzu
// -----------------------------------------------------------------------------
void VMBAddCMD(char *CMDName, void* CMDPointer)
{
#if VERBOSE > 2
 VMBDebug("  -> %s(%s, %08X);", __func__, CMDName, CMDPointer);
#endif  
 VMBCmd *CMD = FirstCMD;
 if ( CMD && CMDName )
 {
    VMBCmd *MyCMD = NULL;
    while ( CMD->nextCMD != NULL )
    {
          CMD = (VMBCmd*)CMD->nextCMD;
    }    
    if ( !FirstCMD->CommandName )
    {
        MyCMD = FirstCMD;         
    } else {
        MyCMD = (VMBCmd*)VMBAlloc(sizeof(VMBCmd));
        CMD->nextCMD = (void*)MyCMD;
    }
    MyCMD->CommandName = VMBAlloc(5);
    strncpy(MyCMD->CommandName, CMDName, 4);
    MyCMD->Pointer = CMDPointer;
    if ( *CMDName == '0' )
    {
     MyCMD->OpCode = VMBGetIntHexCode(CMDName);    
    }   
 }
}

// -----------------------------------------------------------------------------
// VMBAddCMDI(int, void* Pointer) - fügt einen RTL Befehl mittels Code hinzu
// -----------------------------------------------------------------------------
void *VMBAddCMDI(int Code, void* Pointer)
{
     if ( Pointer ) 
     {
         char *TempBuffer = (char*)VMBAlloc(5);
         sprintf(TempBuffer, "%04i", Code);
         VMBAddCMD(TempBuffer, Pointer);     
         VMBFree(TempBuffer);
     }
}

// -----------------------------------------------------------------------------
// VMBFindCMD(char *) - sucht den genannten RTL Befehl
// -----------------------------------------------------------------------------
VMBCmd *VMBFindCMD(char *CMDName)
{
#if VERBOSE > 2
 VMBDebug(" -> %s(%s);", __func__ , CMDName);
#endif         
  
 VMBCmd *CCMD = FirstCMD;

 if (*CMDName != '0' )
 { 
     do {
        if (!VMBCodeCMP(CMDName, CCMD->CommandName))    
        {
           return CCMD;
        } else {
           if ( CCMD->nextCMD == NULL )      
            break;
           CCMD = (VMBCmd*)CCMD->nextCMD;
        }
     } while ( CCMD->nextCMD != NULL ); 
     
     if (!VMBCodeCMP(CMDName, CCMD->CommandName))
     { 
       return CCMD;
     } else {
       return NULL;
     }
 } else {
     int Op = VMBGetIntHexCode(CMDName);
     do {
        if ( Op == CCMD->OpCode )    
        {
           return CCMD;
        } else {
           if ( CCMD->nextCMD == NULL )      
            break;
           CCMD = (VMBCmd*)CCMD->nextCMD;
        }
     } while ( CCMD->nextCMD != NULL ); 
     
     if ( Op != CCMD->OpCode)
     { 
       return NULL;
     } else {
       return CCMD;
     }              
 }
}

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBCMDDump() - listet alle verfügbaren RTL Befehle auf
// -----------------------------------------------------------------------------
void VMBCMDDump()
{
 char DumpFileName[] = "cmds.txt\0";    
 FILE *DumpFile;
 
#if VERBOSE > 2
 VMBDebug(" -> %s();", __func__ );
#endif   
 
 DumpFile = fopen(DumpFileName, "w");
 if (!DumpFile)
 {
    VMBDebug("VMBCMDDump() failed!");
    return;              
 } 
    
 int i = 0;
 VMBCmd *CMD = FirstCMD;
 while ( 1 ) 
 {
     fprintf(DumpFile, "VMBCmd(%d) = {\r\n  CommandName = \"%s\"\r\n  Pointer = 0x%08X\r\n" /*"  prevCMD = 0x%08X\r\n*/"  nextCMD = 0x%08X\r\n}\r\n", i++, CMD->CommandName, CMD->Pointer ,/* CMD->prevCMD,*/ CMD->nextCMD);
     if ( CMD->nextCMD != NULL )
     {
       CMD = (VMBCmd *)CMD->nextCMD;
     } else {
       break;       
     }
 } 
 fclose(DumpFile); 
 VMBDebug("command list wrote to %s.", DumpFileName);
}
#endif

                              /*   0000,      0001,               0002,                0003,              0004,     0005 */
void *CMD_Pointers[] = {&RTL_Beep, &RTL_Term, &RTL_ConsoleCreate, &RTL_ConsoleDestroy, &RTL_ConsoleClear, &RTL_Pos, 
                        /*            0006,       0007,              0008,                0009,             0010,  */
                        &RTL_ConsoleReadLn, &RTL_Abort, &RTL_ConsoleWrite, &RTL_ConsoleWriteLn, &RTL_ConsoleName, 
                        /*     0011,      0012,       0013,       0014, 0015,       0016,         0017,  */ 
                        &RTL_Length, &RTL_Trim, &RTL_UCase, &RTL_LCase, &RTL_Beep, &RTL_Shell, &RTL_MCISend, 
                        /*             0018,              0019,             0020,             0021,            0022, */ 
                        &RTL_ConsoleGetName, &RTL_ConsoleColor, &RTL_ConsoleSetX, &RTL_ConsoleSetY, &RTL_ParamCount, 
                        /*    0023,      0024,     0025,     0026,         0027,          0028,        0029, */ 
                        &RTL_Sleep, &RTL_Char, &RTL_Int, &RTL_Str, &RTL_LeftStr, &RTL_RightStr, &RTL_MidStr, 
                        /*       0030,        0031,        0032,            0033,       0034,             0035, */ 
                        &RTL_ParamStr, &RTL_CharAt, &RTL_Random, &RTL_FileExists, &RTL_ASCII, &RTL_CommandLine, 
                        /*     0036,        0037,           0038,           0039,           0040,           0041, */ 
                        &RTL_Handle, &RTL_MsgBox, &RTL_MouseHide, &RTL_MouseShow, &RTL_MouseGetX, &RTL_MouseGetY, 
                        /*        0042,           0043,              0044,              0045,                0046, */
                        &RTL_MouseSetX, &RTL_MouseSetY, &RTL_ClipboardSet, &RTL_ClipboardGet, &RTL_ClipboardClear, 
                        /*  0047,     0048,     0049, 0050, 0051,    0052,     0053,     0054,     0055,     0056, */ 
                        &RTL_Sin, &RTL_Cos, &RTL_Tan, &RTL_isFloat, &RTL_isInt, &RTL_Or, &RTL_Not, &RTL_And, &RTL_Xor, &RTL_Mod, 
                        /*           0057,              0058,      0059,       0060,           0061,              0062, */ 
                        &RTL_SetPrecision, &RTL_GetPrecision, &RTL_true, &RTL_false, &RTL_DirExists, &RTL_GetTickCount, 
                        /* 0063,            0064,       0065,       0066,       0067,       0068,         0069, */
                        &RTL_Pi, &RTL_CurrentDir, &RTL_MkDir, &RTL_ChDir, &RTL_RmDir, &RTL_Round, &RTL_RoundTo, 
                        /*          0070,             0071,              0072,             0073,             0074, */
                        &RTL_ConsoleGetX, &RTL_ConsoleGetY, &RTL_MemWriteByte, &RTL_MemWriteInt, &RTL_MemReadByte, 
                        /*         0075,        0076,        0077,        0078,     0079,    0080,     0081,      0082, */                        
                        &RTL_MemReadInt, &RTL_ArcSin, &RTL_ArcCos, &RTL_ArcTan, &RTL_Log, &RTL_Ln, &RTL_Exp, &RTL_LogN, 
                        /*   0083,       0084, 0085,        0086,        0087,            0088,           0089, */                        
                        &RTL_SQRT, &RTL_Float, NULL, &RTL_SetENV, &RTL_GetENV, &RTL_GetAppPath, &RTL_SelectDir, 
                        /*          0090,             0091,                0092,              0093,             0094, */                        
                        &RTL_SelectColor, &RTL_InputDialog, &RTL_PasswordDialog, &RTL_IsKeyPressed, &RTL_IsKeyPushed, 
                        /*         0095               0096,           0097,             0098,            0099,             0100, */                        
                        &RTL_UpdateKeys, &RTL_RegCreateKey, &RTL_RegDelKey, &RTL_RegWriteStr, &RTL_RegReadStr, &RTL_RegWriteInt, 
                        /*         0101,                0102,                 0103 */ 
                        &RTL_RegReadInt, &RTL_GetScreenWidth, &RTL_GetScreenHeight};

// -----------------------------------------------------------------------------
// VMBInitCommands() - initialisiert die RTL Befehle
// -----------------------------------------------------------------------------
void VMBInitCommands()
{
 int c;    
     
#if VERBOSE > 0     
 VMBDebug(" -> %s();", __func__ );
#endif  
 FirstCMD = (VMBCmd *)VMBAlloc(sizeof(VMBCmd));

 VMBAddCMD("CCCC", &RTL_VarAssign);  
 VMBAddCMD("F100", &RTL_For);
 VMBAddCMD("F101", &RTL_NextFor); 
 VMBAddCMD("F102", &RTL_Next);  
 VMBAddCMD("F103", &RTL_EndFor);
#ifdef VCL
 VMBAddCMD("G001", &RTL_VCLMethod);   
 VMBAddCMD("G002", &RTL_VCLMember);
#endif
 VMBAddCMD("EC00", &RTL_VarInc);   
 VMBAddCMD("EC01", &RTL_VarDec);
 VMBAddCMD("L000", &RTL_DefLabel);
 VMBAddCMD("L001", &RTL_Goto); 
 VMBAddCMD("CI00", &RTL_If);
 VMBAddCMD("CI01", &RTL_Else);
 VMBAddCMD("CI02", &RTL_IfEnd);
 VMBAddCMD("CI03", &RTL_ElseIf);

 VMBAddCMD("CR00", &RTL_Repeat);
 VMBAddCMD("CR01", &RTL_Until);

 for ( c = 0; c < sizeof(CMD_Pointers); c++ )
 {
     VMBAddCMDI(c, CMD_Pointers[c]);    
 }

/*
 VMBAddCMDI(0, &RTL_Beep);
 VMBAddCMDI(1, &RTL_Term);
 VMBAddCMDI(2, &RTL_ConsoleCreate);
 VMBAddCMDI(3, &RTL_ConsoleDestroy); 
 VMBAddCMDI(4, &RTL_ConsoleClear); 
 VMBAddCMDI(5, &RTL_Pos);  
 VMBAddCMDI(6, &RTL_ConsoleReadLn);
 VMBAddCMDI(7, &RTL_Abort);
 VMBAddCMDI(8, &RTL_ConsoleWrite);
 VMBAddCMDI(9, &RTL_ConsoleWriteLn); 
 VMBAddCMDI(10, &RTL_ConsoleName);  
 VMBAddCMDI(11, &RTL_Length);  
 VMBAddCMDI(12, &RTL_Trim);    
 VMBAddCMDI(13, &RTL_UCase);   
 VMBAddCMDI(14, &RTL_LCase);    
 VMBAddCMDI(16, &RTL_Shell);
 VMBAddCMDI(17, &RTL_MCISend); 
 VMBAddCMDI(18, &RTL_ConsoleGetName);
 VMBAddCMDI(19, &RTL_ConsoleColor);  
 VMBAddCMDI(20, &RTL_ConsoleSetX);   
 VMBAddCMDI(21, &RTL_ConsoleSetY); 
 VMBAddCMDI(22, &RTL_ParamCount);   
 VMBAddCMDI(23, &RTL_Sleep);     
 VMBAddCMDI(24, &RTL_Char);    
 VMBAddCMDI(25, &RTL_Int);
 VMBAddCMDI(26, &RTL_Str);    
 VMBAddCMDI(27, &RTL_LeftStr);      
 VMBAddCMDI(28, &RTL_RightStr); 
 VMBAddCMDI(29, &RTL_MidStr);
 VMBAddCMDI(30, &RTL_ParamStr);     
 VMBAddCMDI(31, &RTL_CharAt);
 VMBAddCMDI(32, &RTL_Random);
 VMBAddCMDI(33, &RTL_FileExists);
 VMBAddCMDI(34, &RTL_ASCII);     
 VMBAddCMDI(35, &RTL_CommandLine);   
 VMBAddCMDI(36, &RTL_Handle);        
 VMBAddCMDI(37, &RTL_MsgBox);       
 VMBAddCMDI(38, &RTL_MouseHide);       
 VMBAddCMDI(39, &RTL_MouseShow);         
 VMBAddCMDI(40, &RTL_MouseGetX);    
 VMBAddCMDI(41, &RTL_MouseGetY);
 VMBAddCMDI(42, &RTL_MouseSetX);    
 VMBAddCMDI(43, &RTL_MouseSetY); 
 VMBAddCMDI(44, &RTL_ClipboardSet); 
 VMBAddCMDI(45, &RTL_ClipboardGet); 
 VMBAddCMDI(46, &RTL_ClipboardClear);   
 VMBAddCMDI(47, &RTL_Sin);
 VMBAddCMDI(48, &RTL_Cos);
 VMBAddCMDI(49, &RTL_Tan);
 
 VMBAddCMDI(52, &RTL_Or);  
 VMBAddCMDI(53, &RTL_Not);   
 VMBAddCMDI(54, &RTL_And);   
 VMBAddCMDI(55, &RTL_Xor);  
 VMBAddCMDI(56, &RTL_Mod);    
 VMBAddCMDI(57, &RTL_SetPrecision);  
 VMBAddCMDI(58, &RTL_GetPrecision);  
 VMBAddCMDI(59, &RTL_true);      
 VMBAddCMDI(60, &RTL_false);      
 VMBAddCMDI(61, &RTL_DirExists);   
 VMBAddCMDI(62, &RTL_GetTickCount);
 VMBAddCMDI(63, &RTL_Pi);
 VMBAddCMDI(64, &RTL_CurrentDir);      
 VMBAddCMDI(65, &RTL_MkDir);     
 VMBAddCMDI(66, &RTL_ChDir);     

 VMBAddCMDI(68, &RTL_Round);  
 VMBAddCMDI(69, &RTL_RoundTo);          
 VMBAddCMDI(70, &RTL_ConsoleGetX);     
 VMBAddCMDI(71, &RTL_ConsoleGetY); 
 VMBAddCMDI(72, &RTL_MemWriteByte);
 VMBAddCMDI(73, &RTL_MemWriteInt);
 VMBAddCMDI(74, &RTL_MemReadByte);
 VMBAddCMDI(75, &RTL_MemReadInt); 
 VMBAddCMDI(76, &RTL_ArcSin);
 VMBAddCMDI(77, &RTL_ArcCos);
 VMBAddCMDI(78, &RTL_ArcTan); 
 VMBAddCMDI(79, &RTL_Log);
 VMBAddCMDI(80, &RTL_Ln);
 VMBAddCMDI(81, &RTL_Exp);
 VMBAddCMDI(82, &RTL_LogN);
 VMBAddCMDI(83, &RTL_SQRT);   
 VMBAddCMDI(84, &RTL_Float);

 VMBAddCMDI(86, &RTL_SetENV);
 VMBAddCMDI(87, &RTL_GetENV);
 VMBAddCMDI(88, &RTL_GetAppPath);
 VMBAddCMDI(89, &RTL_SelectDir);
 VMBAddCMDI(90, &RTL_SelectColor);
 VMBAddCMDI(91, &RTL_InputDialog);
 VMBAddCMDI(92, &RTL_PasswordDialog);  
 VMBAddCMDI(93, &RTL_IsKeyPressed);
 VMBAddCMDI(94, &RTL_IsKeyPushed);
 VMBAddCMDI(95, &RTL_UpdateKeys);
*/


 VMBAddCMD("C000", &RTL_Dec);   
 VMBAddCMD("C001", &RTL_Break);    
 VMBAddCMD("C002", &RTL_SetVarType);  
#ifdef DEBUGGER
 VMBAddCMD("C004", &RTL_DbgSetLine);
#endif 
 
 VMBAddCMD("CS00", &RTL_SubDef);  
 VMBAddCMD("CS01", &RTL_EndSub); 
 VMBAddCMD("CS02", &RTL_Return);
 VMBAddCMD("EX00", &RTL_ExternSub);
 
 VMBAddCMD("FFFF", &RTL_EoS);
}

#endif
 
