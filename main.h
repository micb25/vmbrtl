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
 
#ifndef MAIN_H
#define MAIN_H

#include "build_options.h"

#include <windows.h>
#include <string.h>
#include <time.h>
#include <stdio.h>
#include <stdlib.h>
#include "build_options.h"

#define SRCFILE "test.vmbc"

#define ERRORTIMERID 1337

void VMBInit(HINSTANCE hThisInstance);
void VMBExit(DWORD ExitCode);
LRESULT CALLBACK WindowProcedure (HWND, UINT, WPARAM, LPARAM);

UINT RTL_CodeSize;
char *RTL_CMDLINE;
FILE *RTL_CodeFile;
char *RTL_Code;
char *RTL_CodeOrig;
HINSTANCE RTL_HINSTANCE;
char isStopped, isStepMode, resetNeeded, isRStackUsed;
static DWORD Time1;
char isWin2k;

#endif
 
