/***************************************************************************
 *   Versatile MB 2009 - C Runtime Library                                 *
 *   Copyright (C) 2002-2010 by Michael B�hme & embyte                     *
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

#ifndef RTL_CMD_H
#define RTL_CMD_H

#include <windows.h>

#define VMBRandom(max) rand() % max

// 0000 / 0000
void RTL_Beep();
// 0001
void RTL_Term();
// 0002
void RTL_ConsoleCreate();
// 0003
void RTL_ConsoleDestroy();
// 0004
void RTL_ConsoleClear();
// 0005
void RTL_Pos();
// 0006
void RTL_ConsoleReadLn();
// 0007
void RTL_Abort();
// 0008
void RTL_ConsoleWrite();
// 0009
void RTL_ConsoleWriteLn();
// 0010
void RTL_ConsoleName();
// 0011
void RTL_Length();
// 0012
void RTL_Trim();
// 0013
void RTL_UCase();
// 0014
void RTL_LCase();
// 0016
void RTL_Shell();
// 0017
void RTL_MCISend();
// 0018
void RTL_ConsoleGetName();
// 0019
void RTL_ConsoleColor();
// 0020
void RTL_ConsoleSetX();
// 0021
void RTL_ConsoleSetY();
// 0022
void RTL_ParamCount();
// 0023
void RTL_Sleep();
// 0024
void RTL_Char();
// 0025
void RTL_Int();
// 0026
void RTL_Str();
// 0027
void RTL_LeftStr();
// 0028
void RTL_RightStr();
// 0029
void RTL_MidStr();
// 0030
void RTL_ParamStr();
// 0031
void RTL_CharAt();
// 0032
void RTL_Random();
// 0033
void RTL_FileExists();
// 0034
void RTL_ASCII();
// 0035
void RTL_CommandLine();
// 0036
void RTL_Handle();
// 0037
void RTL_MsgBox();
// 0038
void RTL_MouseHide();
// 0039
void RTL_MouseShow();
// 0040
void RTL_MouseGetX();
// 0041
void RTL_MouseGetY();
// 0042
void RTL_MouseSetX();
// 0043
void RTL_MouseSetY();
// 0044
void RTL_ClipboardSet();
// 0045
void RTL_ClipboardGet();
// 0046
void RTL_ClipboardClear();
// 0047
void RTL_Sin();
// 0048
void RTL_Cos();
// 0049
void RTL_Tan();
// 0050
void RTL_isFloat();
// 0051
void RTL_isInt();
// 0052
void RTL_Or();
// 0053
void RTL_Not();
// 0054
void RTL_And();
// 0055
void RTL_Xor();
// 0056
void RTL_Mod();
// 0057
void RTL_SetPrecision();
// 0058
void RTL_GetPrecision();
// 0059
void RTL_true();
// 0060
void RTL_false();
// 0061
void RTL_DirExists();
// 0062
void RTL_GetTickCount();
// 0063
void RTL_Pi();
// 0064
void RTL_CurrentDir();
// 0065
void RTL_MkDir();
// 0066
void RTL_ChDir();
// 0067
void RTL_RmDir();
// 0068
void RTL_Round();
// 0069
void RTL_RoundTo();
// 0070
void RTL_ConsoleGetX();
// 0071
void RTL_ConsoleGetY();
// 0072
void RTL_MemWriteByte();
// 0073
void RTL_MemWriteInt();
// 0074
void RTL_MemReadByte();
// 0075
void RTL_MemReadInt();
// 0076
void RTL_ArcSin();
// 0077
void RTL_ArcCos();
// 0078
void RTL_ArcTan();
// 0079
void RTL_Log();
// 0080
void RTL_Ln();
// 0081
void RTL_Exp();
// 0082
void RTL_LogN();
// 0083
void RTL_SQRT();
// 0084
void RTL_Float();
// 0086
void RTL_SetENV();
// 0087
void RTL_GetENV();
// 0088
void RTL_GetAppPath();
// 0089
void RTL_SelectDir();
// 0090
void RTL_SelectColor();
// 0091
void RTL_InputDialog();
// 0092
void RTL_PasswordDialog();
// 0093
void RTL_IsKeyPressed();
// 0094
void RTL_IsKeyPushed();
// 0095
void RTL_UpdateKeys();
// 0096
void RTL_RegCreateKey();
// 0097
void RTL_RegDelKey();
// 0098
void RTL_RegWriteStr();
// 0099
void RTL_RegReadStr();
// 0100
void RTL_RegWriteInt();
// 0101
void RTL_RegReadInt();
// 0102
void RTL_GetScreenWidth();
// 0103
void RTL_GetScreenHeight();
// C000
void RTL_Dec();
// C001
void RTL_Break();
// C002
void RTL_SetVarType();
// EC00
void RTL_VarInc();
// EC01
void RTL_VarDec();
// L000
void RTL_DefLabel();
// L001
void RTL_Goto();

// FFFF
void RTL_EoS();
// CCCC
void RTL_VarAssign();
// F100
void RTL_For();
// F101
void RTL_NextFor();
// F102
void RTL_Next();
// F103
void RTL_EndFor();
// G001
void RTL_VCLMethod();
// G002
void RTL_VCLMember();
// CI00
void RTL_If();
// CI01
void RTL_Else();
// CI02
void RTL_IfEnd();
// CI03
void  RTL_ElseIf();
// CR00
void RTL_Repeat();
// CR01
void RTL_Until();
// CS00
void RTL_SubDef();
// CS01
void RTL_EndSub();
// CS02
void RTL_Return();
// EX00
void RTL_ExternSub();

#ifdef DEBUGGER
// C004
void RTL_DbgSetLine();
#endif

#endif