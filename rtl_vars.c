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
 
#include "rtl_vars.h"
#include "rtl_subs.h"
#include "rtl_mem.h"

#include "main.h"
#include <stdio.h>
#include <stdlib.h>

#ifdef VCL
extern WNDCLASS VCL_FORMCLASS;
extern const char VCL_FORMCLASSNAME;
extern int iVCLFormsCount;
extern int iVCLTimerCount;
#endif

int VarCount = 0;

void VMBSetVarContent(VMBVar *CVar, char *Value);
#ifdef DEBUG
extern HWND hDbgWnd;
#endif
extern VMBSub *CurrentSub;
extern int SubLevel;

// -----------------------------------------------------------------------------
// VMBFindvar(char *) - sucht eine Variable und gibt deren Pointer zurück
// -----------------------------------------------------------------------------
VMBVar *VMBFindVar(char *VarName)
{
#if VERBOSE > 1
 VMBDebug("  ->%s(%s), Level = %i, CSUB = %08X", __func__, VarName, SubLevel, CurrentSub );       
#endif 
 if ( *VarName == 'F' && CurrentSub != NULL )      
 {
    if ( *(VarName+1) == 'F')  
    {
        int i;
        if ( CurrentSub->Parameters > 0 )
        {
            for ( i = 0; i < CurrentSub->Parameters; i++ )  
            {
                if ( CurrentSub->SubParams[i] != NULL )
                {
                   if (!VMBCodeCMP(VarName, CurrentSub->SubParams[i]->VarName))     
                   {
                      return CurrentSub->SubParams[i];
                   }
                }
            }
                     
            return NULL;
        } else {
            return NULL;       
        }
    } else {
      if ( CurrentSub->LocalVars != NULL )      
      {
         VMBVar *SearchVar = CurrentSub->LocalVars;  
         do {
             if ( SearchVar->isDeclared )
             {
                  if (!VMBCodeCMP(SearchVar->VarName, VarName))
                     return SearchVar;
             }           
             if ( SearchVar->nextVar == NULL )
                break;
             SearchVar = (VMBVar*)SearchVar->nextVar;
         } while ( 1 );
      } else {
        return NULL;       
      }
    }
 } 
 if ( FirstVar->isDeclared == FALSE )
  return NULL;
 VMBVar *CVar = FirstVar;
 do {
     if (!VMBCodeCMP(VarName, CVar->VarName))
     {
       return CVar;                     
     } else {
       if ( CVar->nextVar == NULL )
          break;
       CVar = (VMBVar *)CVar->nextVar;
     }
 } while ( CVar->nextVar != NULL );
 if (VMBCodeCMP(CVar->VarName, VarName) != 0)
   return NULL;       
 return CVar;
}

#ifdef VCL
// -----------------------------------------------------------------------------
// VMBFindVarByHWND(char *) - sucht eine Komponente nach Handle
// -----------------------------------------------------------------------------
VMBVar *VMBFindVarByHWND(HWND hwnd)
{
 if ( FirstVar->isDeclared == FALSE )
  return NULL;
 VMBVar *CVar = FirstVar;
 do {
     if ( hwnd == CVar->hWnd )
     {
       return CVar;                     
     } else {
       if ( CVar->nextVar == NULL )
          break;
       CVar = (VMBVar *)CVar->nextVar;
     }
 } while ( CVar->nextVar != NULL );
 if ( hwnd != CVar->hWnd )
    CVar = NULL;
 return CVar;
}
#endif

// -----------------------------------------------------------------------------
// VMBSetVarType(char *, int) - setzt den Vartype einer VMBVar
// -----------------------------------------------------------------------------
void VMBSetVarType(char *VarName, int VarType)
{
 VMBVar *CVar = VMBFindVar(VarName);
#if VERBOSE > 0
 VMBDebug("%s(%s, %i)", __func__, VarName, VarType);
#endif 
 if ( CVar != (VMBVar *)NULL )
  if ( CVar->isDeclared == TRUE )
  {
   CVar->VarType = VarType;
   if ( VarType == VARTYPE_INT )
   {
           int iTemp = 0;
           VMBSetVarContent(CVar, (char*)&iTemp);
   }
   if ( VarType == VARTYPE_ARRAY )
   {
           CVar->isArray = 1;  
           CVar->Properties = NULL;       
   }
#ifdef VCL
   if (VarType == VARTYPE_FORM)
   {
     if (CVar->hWnd == NULL)            
     {
      CVar->WindowClass = (WNDCLASS*)VMBAlloc(sizeof(WNDCLASS));
      memcpy(CVar->WindowClass, &VCL_FORMCLASS, sizeof(WNDCLASS));
      WNDCLASS *MyNewClass = CVar->WindowClass;
      MyNewClass->lpszClassName = (char *)VMBAlloc(32);
      sprintf((char *)MyNewClass->lpszClassName, "%s%i", &VCL_FORMCLASSNAME, iVCLFormsCount);
      if ( !RegisterClass(MyNewClass) )
      {
         VMBPanic();
         return;     
      }
      CVar->hWnd = (HWND)VCL_FormCreate(MyNewClass->lpszClassName, "Form");               

      VCLFormProperties *Properties = (VCLFormProperties *)VMBAlloc(sizeof(VCLFormProperties)+1);
      CVar->Properties = (char *)Properties;
      Properties->BkColor = GetSysColor(COLOR_BTNFACE);
      Properties->Background = CreateSolidBrush(Properties->BkColor);
      Properties->AlphaValue = 255;
      Properties->canClose = 1;
      Properties->Icons = 7;
      Properties->hCursor = LoadCursor(NULL, MAKEINTRESOURCE(IDC_ARROW));           
      Properties->hDC = GetDC(CVar->hWnd);
     }
   }
  else
  if ( VarType == VARTYPE_TIMER )
  {
      if ( CVar->hWnd == NULL )
      {
          VCLTimerProperties *Prop = (VCLTimerProperties *)VMBAlloc(sizeof(VCLTimerProperties)+1);           
          CVar->hWnd = (HWND)0xDEADBEEF;
          CVar->Properties = (char*)Prop;
          Prop->Enabled = 0;
          Prop->Interval = 1000;
          Prop->Event_Elapsed = 0;
          Prop->TimerID = VMBGetIntHex(VarName);
          iVCLTimerCount++;
      }
  }
  else
  if ( VarType == VARTYPE_BUTTON )
  {
          CVar->hWnd = (HWND)VCL_ButtonCreate("Button");
          VCLButtonProperties *Prop = (VCLButtonProperties *)VMBAlloc(sizeof(VCLButtonProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->FontName = (char*)VMBAlloc(256);
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif          
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_ButtonUpdateFont(Prop, CVar->hWnd);          
  } 
  if ( VarType == VARTYPE_CHECKBOX )
  {
          CVar->hWnd = (HWND)VCL_CheckBoxCreate("CheckBox");
          VCLCheckBoxProperties *Prop = (VCLCheckBoxProperties *)VMBAlloc(sizeof(VCLCheckBoxProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->FontName = (char*)VMBAlloc(256);
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif          
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          Prop->BkColor = GetSysColor(COLOR_BTNFACE);
          Prop->Color = GetTextColor(GetDC(CVar->hWnd));
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_CheckBoxUpdateFont(Prop, CVar->hWnd);    
  }
  else
  if ( VarType == VARTYPE_RADIOBUTTON )
  {
          CVar->hWnd = (HWND)VCL_RadioButtonCreate("RadioButton");
          VCLRadioButtonProperties *Prop = (VCLRadioButtonProperties *)VMBAlloc(sizeof(VCLRadioButtonProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->FontName = (char*)VMBAlloc(256);
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif          
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          Prop->BkColor = GetSysColor(COLOR_BTNFACE);
          Prop->Color = GetTextColor(GetDC(CVar->hWnd));
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_RadioButtonUpdateFont(Prop, CVar->hWnd);    
  }
  else  
  if ( VarType == VARTYPE_GROUPBOX )
  {
          CVar->hWnd = (HWND)VCL_GroupBoxCreate("GroupBox");
          VCLGroupBoxProperties *Prop = (VCLGroupBoxProperties *)VMBAlloc(sizeof(VCLGroupBoxProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->FontName = (char*)VMBAlloc(256);
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif          
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          Prop->BkColor = GetSysColor(COLOR_BTNFACE);
          Prop->Color = GetTextColor(GetDC(CVar->hWnd));
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_GroupBoxUpdateFont(Prop, CVar->hWnd);    
  }
  else   
  if ( VarType == VARTYPE_LABEL )
  {
          CVar->hWnd = (HWND)VCL_LabelCreate("Label");
          VCLLabelProperties *Prop = (VCLLabelProperties *)VMBAlloc(sizeof(VCLLabelProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->BkColor = GetSysColor(COLOR_BTNFACE);
          Prop->Color = GetTextColor(GetDC(CVar->hWnd));
          Prop->FontName = (char*)VMBAlloc(256);
          Prop->AutoSize = 1;
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif 
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_LabelUpdateFont(Prop, CVar->hWnd);
  }   
  else
  if ( VarType == VARTYPE_EDIT )
  {
          CVar->hWnd = (HWND)VCL_EditCreate("Edit");
          VCLEditProperties *Prop = (VCLEditProperties *)VMBAlloc(sizeof(VCLEditProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->BkColor = GetBkColor(GetDC(CVar->hWnd));
          Prop->Background = CreateSolidBrush(Prop->BkColor);
          Prop->Color = GetTextColor(GetDC(CVar->hWnd));
          Prop->FontName = (char*)VMBAlloc(256);
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif 
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_EditUpdateFont(Prop, CVar->hWnd);
  } 
  else
  if ( VarType == VARTYPE_TEXTBOX )
  {
          CVar->hWnd = (HWND)VCL_TextBoxCreate("TextBox");
          VCLTextBoxProperties *Prop = (VCLTextBoxProperties *)VMBAlloc(sizeof(VCLTextBoxProperties)+1);           
          CVar->Properties = (char*)Prop;
          Prop->BkColor = GetBkColor(GetDC(CVar->hWnd));
          Prop->Background = CreateSolidBrush(Prop->BkColor);
          Prop->Color = GetTextColor(GetDC(CVar->hWnd));
          Prop->FontName = (char*)VMBAlloc(256);
          GetTextFace(GetDC(CVar->hWnd), 255, Prop->FontName);
          TEXTMETRIC TM;
          GetTextMetrics(GetDC(CVar->hWnd), &TM);          
#ifdef DEBUG
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(hDbgWnd), LOGPIXELSY));
#else
          Prop->FontSize = MulDiv(TM.tmHeight, 72, GetDeviceCaps(GetDC(GetDesktopWindow()), LOGPIXELSY));
#endif 
          Prop->Font = (HFONT)SendMessage(CVar->hWnd, WM_GETFONT, 0, 0);
          Prop->FontSize = 8;
          strcpy(Prop->FontName, "MS Sans Serif");
          VCL_TextBoxUpdateFont(Prop, CVar->hWnd);
  } else
  if ( VarType == VARTYPE_IMAGE )
  {
          CVar->hWnd = (HWND)VCL_ImageCreate("");
          VCLImageProperties *Prop = (VCLImageProperties *)VMBAlloc(sizeof(VCLImageProperties)+1);                           
          CVar->Properties = (char*)Prop;                
          Prop->Transparent = 1;      
          VCL_ImageUpdate(CVar);      
  }   
  if ( VarType == VARTYPE_FILE )
  {
          CVar->hWnd = (HWND)0xC0FFEE;
          VCLFileProperties *Prop = (VCLFileProperties *)VMBAlloc(sizeof(VCLFileProperties)+1);           
          CVar->Properties = (char*)Prop;
  }   
#endif
 }
#ifdef DEBUGGER   
   DEBUGGER_UpdateVarContent(CVar);
#endif    
}

// -----------------------------------------------------------------------------
// VMBGetVarType(char *, int) - setzt den Vartype einer VMBVar
// -----------------------------------------------------------------------------
int VMBGetVarType(char *VarName)
{
 VMBVar *CVar = VMBFindVar(VarName);
 if ( CVar != (VMBVar *)NULL )
  if ( CVar->isDeclared == TRUE )
   return CVar->VarType;
 return 0;
}

// -----------------------------------------------------------------------------
// VMBNewVar(char *) - erzeugt eine neue VMB Variable
// -----------------------------------------------------------------------------
void VMBNewVar(char *VarName)
{
 // Variable bereits deklariert?!
 if ( (VMBVar *)VMBFindVar(VarName) != (VMBVar *)NULL )
    return;     
    
 if ( *VarName == 'F' && *(VarName+1) == 'E' && CurrentSub != NULL )
 {
    if ( CurrentSub->LocalVars == NULL )
    {
       CurrentSub->LocalVars = (VMBVar*)VMBAlloc(sizeof(VMBVar));       
       CurrentSub->LocalVars->isLocal = 1;
       CurrentSub->LocalVars->isDeclared = 1;
       CurrentSub->LocalVars->VarName = (char*)VMBAlloc(VMBStrLen(VarName)+1);
       strncpy(CurrentSub->LocalVars->VarName, VarName, VMBStrLen(VarName));
    } else {
       VMBVar *LastVar = CurrentSub->LocalVars;
       while ( LastVar->nextVar != NULL )
       {
             LastVar = (VMBVar*)LastVar->nextVar;
       }   
       VMBVar *NewVar = (VMBVar*)VMBAlloc(sizeof(VMBVar));
       LastVar->nextVar = (void*)NewVar;
#ifndef OPTIMIZESIZE
       NewVar->prevVar = (void*)LastVar;
       NewVar->isArray = 0;
#endif
       NewVar->isLocal = 1;
       NewVar->isDeclared = 1;
       NewVar->VarName = (char*)VMBAlloc(VMBStrLen(VarName)+1);
       strncpy((char*)NewVar->VarName, (char*)VarName, VMBStrLen(VarName));                     
    }
    
    
         
 }

 VMBVar *newVMBVar = (VMBVar *)VMBAlloc(sizeof(VMBVar));
 
 newVMBVar->nextVar = NULL;
 newVMBVar->isDeclared = FALSE;

 if ( FirstVar->nextVar == NULL )
 {
  if ( FirstVar->isDeclared == FALSE )     
   {   
       // erste Variable initialisieren
       FirstVar->isDeclared = TRUE;
       FirstVar->VarName    = (char *)VMBAlloc(strlen(VarName) + 1);
       strcpy(FirstVar->VarName, VarName);
   } else {
       // zweite Variable initialisieren
       FirstVar->nextVar     = (char *)newVMBVar;       
       newVMBVar->isDeclared = TRUE;
       newVMBVar->VarName    = (char *)VMBAlloc(strlen(VarName) + 1);
       strcpy(newVMBVar->VarName, VarName);
#ifndef OPTIMIZESIZE
       newVMBVar->VarContent = NULL;
       newVMBVar->BufferSize = 0;
       newVMBVar->prevVar    = (char *)FirstVar;
       newVMBVar->nextVar    = NULL;  
       newVMBVar->VarType    = 0;
       newVMBVar->isArray    = 0;
#ifdef VCL
       newVMBVar->hWnd       = NULL;
       newVMBVar->Properties = NULL;
       newVMBVar->WindowClass = NULL;       
#endif       
#endif
   }
 } else {
  VMBVar *CVar = FirstVar;
  do {
      CVar = (VMBVar *)CVar->nextVar;
  } while ( CVar->nextVar != NULL );
  CVar->nextVar = (char *)newVMBVar;
  newVMBVar->isDeclared = TRUE;
  newVMBVar->VarName    = (char *)VMBAlloc(strlen(VarName) + 1);
  strcpy(newVMBVar->VarName, VarName);
#ifndef OPTIMIZESIZE  
  newVMBVar->VarContent = NULL;
  newVMBVar->BufferSize = 0;
  newVMBVar->isArray    = 0;
#ifdef VCL
  newVMBVar->hWnd       = NULL;
  newVMBVar->Properties = NULL;    
  newVMBVar->WindowClass = NULL;
#endif  
  newVMBVar->prevVar    = (char *)CVar;
  newVMBVar->nextVar    = NULL; 
  newVMBVar->VarType    = 0; 
#endif
 }
 
 VarCount++;
}

// -----------------------------------------------------------------------------
// VMBSetVarContent(char *, char *) - setzt einen V
// -----------------------------------------------------------------------------
void VMBSetVarContent(VMBVar *CVar, char *Value)
{
#if VERBOSE > 0
 VMBDebug(" -> %s(%s, %s);", __func__, CVar->VarName, Value);
#endif
 int iTemp, iSize;
 
 if (CVar != NULL)
 {
   switch ( CVar->VarType ) {
        case VARTYPE_INT:
          if ( CVar->BufferSize != sizeof(int) )
          {
           int iSize = sizeof(int);
           CVar->VarContent = (char *)VMBAlloc(iSize);
           CVar->BufferSize = iSize;
          }
          memcpy(CVar->VarContent, Value, sizeof(int));
          break;
        case VARTYPE_STR:
          iSize = VMBStrLen(Value) + 1;   
          if ( CVar->BufferSize < iSize )
          {
           if ( CVar->VarContent )    
              VMBFree(CVar->VarContent);
           CVar->VarContent = (char *)VMBAlloc(iSize);
           CVar->BufferSize = iSize;
          }
          if (Value != NULL && *Value != '\0')
                    memcpy(CVar->VarContent, Value, iSize);
          else
                    *(CVar->VarContent) = '\0';
          break;
        case VARTYPE_FLOAT:
          if ( CVar->BufferSize != sizeof(double) )
          {
               int iSize = sizeof(double);
               CVar->VarContent = (char *)VMBAlloc(iSize);
               CVar->BufferSize = iSize;
          }
          memcpy(CVar->VarContent, Value, CVar->BufferSize);
          break;
        case VARTYPE_BOOL:          
          if ( CVar->BufferSize != sizeof(int) )
          {
           int iSize = sizeof(int);
           CVar->VarContent = (char *)VMBAlloc(iSize);
           CVar->BufferSize = iSize;
          }
          memcpy(CVar->VarContent, Value, sizeof(int));
          break;
        default:     
         VMBPanic(45);
   }
#ifdef DEBUGGER   
   if ( *(CVar->VarName) != 'F' )
      DEBUGGER_UpdateVarContent(CVar);
#endif   
 } else {                 
   VMBPanic(46);
 }
}

// -----------------------------------------------------------------------------
// VMBSetVarContentSTRWOFRee(VMBVar *, char *) 
// -----------------------------------------------------------------------------
void VMBSetVarContentSTRWOFRee(VMBVar *CVar, char *Value)
{
#if VERBOSE > 0
 VMBDebug(" -> %s(%s);", __func__, CVar->VarName);
#endif
 int iTemp, iSize;
 
 if (CVar != NULL)
 {
   if ( CVar->VarType == VARTYPE_STR )
   {
          iSize = VMBStrLen(Value);   
          if ( CVar->VarContent )    
             VMBFree(CVar->VarContent);
          CVar->VarContent = (char *)Value;
          CVar->BufferSize = iSize + 1;
          if ( Value == NULL)
             *(CVar->VarContent) = '\0';
    }
#ifdef DEBUGGER   
   if ( *(CVar->VarName) != 'F' )
      DEBUGGER_UpdateVarContent(CVar);
#endif   
 } else {                 
   VMBPanic(46);
 }
}

// -----------------------------------------------------------------------------
// VMBSetVarContet(char *, char *) - setzt einen V
// -----------------------------------------------------------------------------
void VMBSetVarContentEx(VMBVar *CVar, char *Value)
{
#if VERBOSE > 0
 VMBDebug(" -> %s(%s);", __func__, CVar->VarName);
#endif
 int iTemp;
 int iSize;
 
 if (CVar != NULL)
 {
   switch ( CVar->VarType ) {
        case VARTYPE_INT:
          if ( CVar->BufferSize != sizeof(int) )
          {
           int iSize = sizeof(int);
           if ( CVar->VarContent != NULL )
              CVar->VarContent = (char*)VMBFree(CVar->VarContent);     
           CVar->VarContent = (char *)VMBAlloc(iSize);
           CVar->BufferSize = iSize;
          }
          memcpy(CVar->VarContent, Value, sizeof(int));
          break;
        case VARTYPE_STR:
          iSize = VMBStrLen(Value) + 1;
          if ( CVar->BufferSize < iSize + 1)
          {
           if ( CVar->VarContent != NULL )
              CVar->VarContent = (char*)VMBFree(CVar->VarContent);     
           CVar->VarContent = (char *)VMBAlloc(iSize);
           CVar->BufferSize = iSize;
          }
          if (Value != NULL && *Value != '\0')
                    memcpy(CVar->VarContent, Value, iSize);
          else
                    *(CVar->VarContent) = '\0';
          break;
        case VARTYPE_FLOAT:
          if ( CVar->BufferSize != sizeof(double) )
          {
               int iSize = sizeof(double);
               CVar->VarContent = (char *)VMBAlloc(iSize);
               CVar->BufferSize = iSize;
          }
          memcpy(CVar->VarContent, Value, CVar->BufferSize);
          break;
        case VARTYPE_BOOL:
          if ( CVar->BufferSize != sizeof(int) )
          {
           int iSize = sizeof(int);
           CVar->VarContent = (char *)VMBAlloc(iSize);
           CVar->BufferSize = iSize;
          }
          memcpy(CVar->VarContent, Value, sizeof(int));
          break;
        default:     
         VMBPanic(45);
         break;
   }
#ifdef DEBUGGER   
   DEBUGGER_UpdateVarContent(CVar);
#endif    
 } else {                  
   VMBPanic(46);
 }
}

// -----------------------------------------------------------------------------
// VMBGetVarContent(char *) - liest den Inhalt einer Variable
// -----------------------------------------------------------------------------
char* VMBGetVarContent(char *VarName)
{    
#if VERBOSE > 0
     VMBDebug(" -> %s(%s);", __func__, VarName);
#endif     
     VMBVar *CC = VMBFindVar(VarName);
     char *resultBuffer;
     if ( !CC )
     {                   
        VMBPanic(46);
     }
     if ( CC->isDeclared == FALSE || CC->VarType == 0 )
     {
        VMBPanic(47);
     }     
     switch ( CC->VarType ) {

            case VARTYPE_STR:
                 if ( CC->VarContent != NULL )
                 {
                   resultBuffer = (char *)VMBAlloc(CC->BufferSize+1);
                   memcpy(resultBuffer, CC->VarContent, VMBStrLen(CC->VarContent)+1);
                 } else {
                   resultBuffer = (char *)VMBAlloc(2);     
                 }
                 return resultBuffer;
                 break;
            case VARTYPE_INT:
            case VARTYPE_FLOAT:
            case VARTYPE_BOOL:
                 resultBuffer = (char *)VMBAlloc(CC->BufferSize);
                 memcpy(resultBuffer, CC->VarContent, CC->BufferSize);  
                 return resultBuffer;
     }     
     return NULL;
}

// -----------------------------------------------------------------------------
// VMBGetVarContent(char *) - liest den Inhalt einer Variable
// -----------------------------------------------------------------------------
char* VMBGetVarContentEx(VMBVar* CC)
{    
#if VERBOSE > 0
 VMBDebug(" -> %s(%s);", __func__, CC->VarName);
#endif     
     char *resultBuffer;
     if ( !CC )
     {                   
        VMBPanic(46);
     }
     switch ( CC->VarType ) {

            case VARTYPE_STR:
                 if ( CC->VarContent != NULL )
                 {
                    resultBuffer = (char *)VMBAlloc(CC->BufferSize+1);
                    memcpy(resultBuffer, CC->VarContent, VMBStrLen(CC->VarContent)+1);
                 } else {
                   resultBuffer = (char *)VMBAlloc(2);     
                 }
                 return resultBuffer;
            case VARTYPE_INT:
            case VARTYPE_FLOAT:
            case VARTYPE_BOOL:
                 resultBuffer = (char *)VMBAlloc(CC->BufferSize);
                 memcpy(resultBuffer, CC->VarContent, CC->BufferSize);  
                 return resultBuffer;
     }     
     return NULL;
}

#ifdef DEBUG
// -----------------------------------------------------------------------------
// VMBVarDump() - zeigt das Variablensystem in einer Debug-MessageBox
// -----------------------------------------------------------------------------
void VMBVarDump()
{
 int i = 0, iTemp1 = 0;
 double fTemp1 = 0.0;
 char *szBuffer = (char*)VMBAlloc(4096);
 VMBVar *CVar = FirstVar;
 FILE *DumpFile;
 char DumpFileName[] = "vars.txt\0";
 
 DumpFile = fopen(DumpFileName, "w");
 if (!DumpFile)
 {
  VMBDebug("VMBVarDump() failed.");
  return;              
 } 
 
 while ( 1 ) 
 {
 fprintf(DumpFile, "VMBVar(%d) @ 0x%08X {\r\n  VarName = \"%s\"\r\n  VarContent = \"", i, (int)(VMBVar *)CVar, CVar->VarName );
 switch (CVar->VarType) {
  case VARTYPE_INT:
       memcpy(&iTemp1, CVar->VarContent, CVar->BufferSize);
       fprintf(DumpFile, "%d", iTemp1);
       break;  
  case VARTYPE_STR:
       fprintf(DumpFile, "%s", CVar->VarContent);
       break;     
  case VARTYPE_FLOAT:
       memcpy(&fTemp1, CVar->VarContent, CVar->BufferSize);
       fprintf(DumpFile, "%f", fTemp1);
       break;
  case VARTYPE_BOOL:
       memcpy(&iTemp1, CVar->VarContent, CVar->BufferSize);
       if (!iTemp1)       
       {
        fprintf(DumpFile, "FALSE");
       } else {
        fprintf(DumpFile, "TRUE");
        }
       break;
  default:
       fprintf(DumpFile, "%08X", CVar->VarContent);   
       break;   
 }
#ifndef OPTIMIZESIZE 
 fprintf(DumpFile, "\"\r\n  VarType = %d\r\n  isDeclared = %c\r\n  nextVar = 0x%08X\r\n  prevVar = 0x%08X\r\n}\r\n", CVar->VarType, CVar->isDeclared + 48, (int)CVar->nextVar, (int)CVar->prevVar);
#else
 fprintf(DumpFile, "\"\r\n  VarType = %d\r\n  isDeclared = %c\r\n  nextVar = 0x%08X\r\n}\r\n", CVar->VarType, CVar->isDeclared + 48, (int)CVar->nextVar); 
#endif 
 if ( CVar->nextVar == NULL )
  break;
 CVar = (VMBVar *)CVar->nextVar;
 i++;
 }
 fclose(DumpFile);
 VMBDebug("var list wrote to %s.", DumpFileName);
}
#endif

// -----------------------------------------------------------------------------
// VMBInitVars() - initialisiert die erste Variable
// -----------------------------------------------------------------------------
void VMBInitVars()
{
#if VERBOSE > 0     
 VMBDebug(" -> %s();", __func__ );
#endif  
 /* erster Variable Speicher zuordnen und sie initialisieren */
 FirstVar = (VMBVar *)VMBAlloc(sizeof(VMBVar));
}

// -----------------------------------------------------------------------------
// VMBResetVars() - löscht alle Variablen
// -----------------------------------------------------------------------------
void VMBResetVars()
{
 VMBVar *TempVar;  
#if VERBOSE > 0  
 VMBDebug(" -> VMBResetVars();");
#endif
 if (FirstVar)
 {
              /* letzte Variable suchen */
              TempVar = FirstVar;              
              while ( TempVar->nextVar != NULL )
                TempVar = (VMBVar *)TempVar->nextVar;
              /* Speicher der Variablen freigeben */
              while ( 1 )
              {
               if (TempVar->VarName)
                  VMBFree(TempVar->VarName);
               if (TempVar->VarContent)
                  VMBFree(TempVar->VarContent);
#ifdef VCL
               if ( TempVar->hWnd )
                  DestroyWindow( TempVar->hWnd );                     

               if (TempVar->Properties)
                  VMBFree(TempVar->Properties);                  
               if ( TempVar->WindowClass )
                  {
// TODO: Unregister                    UnregisterClass((WNDCLASS*)TempVar->WindowClass, RTL_HINSTANCE);                       
                    TempVar->WindowClass = (WNDCLASS*)VMBFree(TempVar->WindowClass);
                  }
#endif                                    
               if (TempVar->prevVar != NULL)
               {
                TempVar = (VMBVar *)TempVar->prevVar;
                VMBFree(TempVar->nextVar);
               } else {
                break;
               }               
              }
              VMBFree(FirstVar);
 }
#ifdef DEBUGGER
 DEBUGGER_VarListReset();
#endif
 /* FirstVar wieder initialisieren */ 
 VMBInitVars(); 
}

#ifdef VCL
// -----------------------------------------------------------------------------
// VMBResetVCLs() - löscht alle Komponenten
// -----------------------------------------------------------------------------
void VMBResetVCLs()
{
 VMBVar *TempVar;  
#if VERBOSE > 1  
 VMBDebug(" -> %s();", __func__);
#endif
 if (FirstVar)
 {
              TempVar = FirstVar;              
              while ( TempVar->nextVar != NULL )
                TempVar = (VMBVar *)TempVar->nextVar;
              while ( 1 )
              {
               if ( TempVar->hWnd )
                  DestroyWindow( TempVar->hWnd );                     
               if (TempVar->prevVar != NULL)
               {
                TempVar = (VMBVar *)TempVar->prevVar;
               } else {
                break;
               }               
              }
 }
}
#endif

// -----------------------------------------------------------------------------
// VMBDelLocalVars() - löscht lokale Variablen
// -----------------------------------------------------------------------------
void VMBDelLocalVars()
{
 VMBVar *TempVar;  
#if VERBOSE > 0  
 VMBDebug(" -> %s();", __func__);
#endif
 if ( CurrentSub != NULL)
 {
      TempVar = CurrentSub->LocalVars;
      if ( TempVar != NULL )
      {
         do {
                 if ( TempVar->VarName != NULL ) 
                    TempVar->VarName = (char*)VMBFree(TempVar->VarName);
                 if ( TempVar->VarContent != NULL ) 
                    TempVar->VarName = (char*)VMBFree(TempVar->VarName);                 
                 if ( TempVar->nextVar == NULL )
                    break;
                 TempVar = (VMBVar*)TempVar->nextVar;
                 if ( TempVar->prevVar != NULL )
                    VMBFree(TempVar->prevVar);
         } while ( TempVar->nextVar != NULL );
         VMBFree(TempVar);
         CurrentSub->LocalVars = NULL;
      }
 }
}

// -----------------------------------------------------------------------------
// VMBSetVarTypeArray(char *, int) - setzt den Arraytype eines Arrays
// -----------------------------------------------------------------------------
void VMBSetVarTypeArray(char *VarName, int ArrayType, unsigned int Dim)
{
 unsigned int x;   
 char AVarName[9];
 VMBVar *CVar = VMBFindVar(VarName);
 if ( CVar != (VMBVar *)NULL )
  if ( CVar->isDeclared == TRUE && CVar->Properties == NULL )
  {
       CVar->BufferSize = Dim;
       for ( x = 1; x < Dim+1; x++ )
       {
                  sprintf((char*)&AVarName, "%s%04i", VarName, x);         
                  VMBNewVar(AVarName);
                  VMBSetVarType(AVarName, ArrayType);
       }
  } 
}

#ifdef DEBUGGER

void VMBSetVarDbgName(char *VarName, char *DbgName)
{
     VMBVar *MyVar = VMBFindVar(VarName);          
     if ( MyVar != NULL )
     {
        MyVar->DbgName = (char*)VMBAlloc(VMBStrLen(DbgName)+1);     
        strncpy(MyVar->DbgName, DbgName, VMBStrLen(DbgName));
           if ( *VarName != 'F' )
           {
              DEBUGGER_AddVar(MyVar);
              if ( MyVar->VarType == VARTYPE_INT )
                 DEBUGGER_UpdateVarContent(MyVar);
           }
     }
}

#endif
