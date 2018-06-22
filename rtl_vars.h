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

#define _WIN32_IE 0x500

#include <windows.h>
#include <commctrl.h>
#include <ole2.h>

#include <stdio.h>

#ifndef RTL_VARS_H
#define RTL_VARS_H

#define VARTYPE_INT          1
#define VARTYPE_STR          2
#define VARTYPE_FLOAT        3
#define VARTYPE_BOOL         4
#define VARTYPE_ARRAY        8
#define VARTYPE_VAR          9
#define VARTYPE_FORM        10
#define VARTYPE_BUTTON      11
#define VARTYPE_LABEL       12
#define VARTYPE_EDIT        13
#define VARTYPE_CHECKBOX    14
#define VARTYPE_RADIOBUTTON 15
#define VARTYPE_TIMER       16
#define VARTYPE_FILE        17
#define VARTYPE_TEXTBOX     18
#define VARTYPE_GROUPBOX    19
#define VARTYPE_IMAGE       20

typedef struct {
       char *VarName;
       char *VarContent;
       char VarType;
       unsigned int BufferSize;
       char isDeclared;
       char isLocal;
       char isArray;
       char *Properties;

#ifdef VCL       
       HWND hWnd;
       WNDCLASS *WindowClass;       
#endif       

#ifdef DEBUGGER
       unsigned int iItem;
       char *DbgName;
#endif
      
       char *nextVar;
       char *prevVar;
} VMBVar;

VMBVar *FirstVar;

typedef struct {
        char *szCaption;
        char *szDesc;
        char *Value;
        char isPassword;
} VMBInputWindow;
 
#ifdef VCL 
 
typedef struct {
       int BkColor;
       char Icons;
       char Cursor;
       char Style;
       HCURSOR hCursor;
       HBRUSH Background;
       HDC hDC;
       
       char isAlphaBlending;
       char isTransparent;
       char AlphaValue;
       int TransparentColor;
       char canClose;
       
       /* Events */
       char Event_Clicked;
       char Event_RClicked;
       char Event_Closed;   
       char Event_Moved;
       char Event_Painted;
       
       int Event_ClickedX;  
       int Event_ClickedY;       
       int Event_RClickedX;       
       int Event_RClickedY;       
} VCLFormProperties;

typedef struct {
       char FontStyle;
       int FontSize;
       char *FontName;       
       HFONT Font;
             
       /* Events */
       char Event_Clicked;
       char Event_RClicked;
       
       int Event_ClickedX;  
       int Event_ClickedY;       
       int Event_RClickedX;       
       int Event_RClickedY;       
} VCLButtonProperties;

typedef struct {
       char FontStyle;
       int FontSize;
       char *FontName;       
       HFONT Font;
       
       int BkColor;
       int Color;
             
       /* Events */
       char Event_Clicked;
} VCLCheckBoxProperties;

typedef struct {
       char FontStyle;
       int FontSize;
       char *FontName;       
       HFONT Font;
       
       int BkColor;
       int Color;
             
       /* Events */
       char Event_Clicked;
} VCLRadioButtonProperties;

typedef struct {
       char FontStyle;
       int FontSize;
       char *FontName;       
       HFONT Font;
       
       int BkColor;
       int Color;
             
       /* Events */
//       char Event_Clicked;
} VCLGroupBoxProperties;

typedef struct {
       int BkColor; 
       int Color;
       char Align;
       char FontStyle;
       char AutoSize;
       char Transparent;
       int FontSize;
       char *FontName;       
       HFONT Font;
             
       /* Events */
       char Event_Clicked;
       char Event_DblClicked;
} VCLLabelProperties;

typedef struct {
       int BkColor; 
       int Color;
       char FontStyle;
       int FontSize;
       char *FontName;       
       HFONT Font;
       HBRUSH Background;
             
       /* Events */
       char Event_Clicked;
       char Event_RClicked;
       char Event_Changed;
       
       int Event_ClickedX;  
       int Event_ClickedY;       
       int Event_RClickedX;       
       int Event_RClickedY;       
} VCLEditProperties;

typedef struct {
       int BkColor;       
       char Tiled;
       char Offset;
       char Transparent;
       int XOffset;
       int YOffset;
       char *File;       
       
       void *Picture;
       
       /* Events */
       char Event_Clicked;
       char Event_RClicked;
       
       int Event_ClickedX;  
       int Event_ClickedY;       
       int Event_RClickedX;       
       int Event_RClickedY;       
} VCLImageProperties;

typedef struct {
        UINT TimerID;
        UINT Interval;
        
        char Enabled;
        char Event_Elapsed; 
} VCLTimerProperties;

typedef struct {
       int BkColor; 
       int Color;
       char FontStyle;
       int FontSize;
       char *FontName;       
       HFONT Font;
       HBRUSH Background;
             
       /* Events */
       char Event_Clicked;
       char Event_RClicked;
       char Event_Changed;
       
       int Event_ClickedX;  
       int Event_ClickedY;       
       int Event_RClickedX;       
       int Event_RClickedY;       
} VCLTextBoxProperties;

typedef struct {
        char Opened;
        char *FileName;
        FILE *File;
} VCLFileProperties;



#endif

#endif
