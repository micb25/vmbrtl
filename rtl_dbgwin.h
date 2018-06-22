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

#ifndef RTL_DBGWIN_H
#define RTL_DBGWIN_H

#ifdef DEBUG

#include <windows.h>

#define ID_OK 1000

LRESULT CALLBACK RTL_DebugWndProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);	 
LRESULT CALLBACK RTL_InputWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);

WNDCLASS wc;
HWND hDbgWnd;
HWND hDbgEdit;
HWND hOk;
HWND hDbgLine;

WNDPROC oldWndProc;

char inputBuffer[512];

#endif
 
#endif
