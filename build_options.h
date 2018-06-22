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
 
#define VERSIONSTR "Versatile MB C-Runtime Library\r\ncompiled: " __DATE__ " " __TIME__ "\r\n"
#define BUILDDATE "build date: " __DATE__ " " __TIME__ 

#define LOOPMAX 1024
#define FLOAT_PRECISION 6

// erzeugt das Debugfenster
#define DEBUG 

#define CREATELOGFILESONCRASH

// verringert die Größe der Datei
#define OPTIMIZESIZE

// erzeugt ein Debug Kompilat
//#define DEBUGGER

// zeigt den kompilierten VMB Code an
//#define CODEWINDOW

// lädt Code aus der Exe und startet ihn sofort
#define FINAL

// Flackern in Form Komponente verhindern
#define FORM_DOUBLEBUFFERED

// visuelle Komponenten
#define VCL

// schnelle interne Calls
#define VMBFASTCALL

// zeigt Messages an, die die Komponenten erhalten
//#define SHOWWINDOWMESSAGES

// 0 = keine Meldungen
// 1 = wenig Meldungen
// 2 = viele Meldungen
// 3 = noch viel mehr Meldungen
#ifdef DEBUG
#define VERBOSE 0
#endif

#define VCLERROR "Komponentenfehler! - Fehler #%i"

#if BUILDING_DLL
# define DLLIMPORT __declspec (dllexport)
#else /* Not BUILDING_DLL */
# define DLLIMPORT __declspec (dllimport)
#endif /* Not BUILDING_DLL */
