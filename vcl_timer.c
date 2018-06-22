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

#ifdef VCL

#ifndef VCL_TIMER_C
#define VCL_TIMER_C

#include <windows.h>
 
#include "main.h"
#include "rtl_vars.h"
#include "rtl_mem.h"

char VMBTimerWnd[] = "VMBTimerWND\0";
LRESULT CALLBACK VCL_TimerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam);
int iVCLTimerCount = 0;

HWND TimerProcWnd = NULL;

// -----------------------------------------------------------------------------
//  VCL_Timer_Init() - initialisiert die VCL_Timer
// -----------------------------------------------------------------------------
void VCL_Timer_Init()
{
 WNDCLASS wc;
 
#if VERBOSE > 1
 VMBDebug(" -> %s();", __func__);
#endif
 ZeroMemory(&wc, sizeof(WNDCLASS));
#ifndef OPTIMIZESIZE
 wc.hIcon = NULL;
 wc.lpszMenuName = NULL;
 wc.style = 0;
#endif
 wc.hInstance = RTL_HINSTANCE;
 wc.hCursor = LoadCursor(NULL, IDC_ARROW);
 wc.hbrBackground = (HBRUSH)(COLOR_BTNFACE+1);
 wc.lpfnWndProc = (WNDPROC)VCL_TimerProc;
 wc.lpszClassName = VMBTimerWnd;
     
 if (RegisterClass(&wc) == 0)
    VMBPanic(50);
     
 TimerProcWnd = CreateWindow(VMBTimerWnd, VMBTimerWnd, WS_CAPTION | WS_BORDER | WS_SYSMENU, CW_USEDEFAULT, CW_USEDEFAULT, 50, 50, NULL, NULL, RTL_HINSTANCE, NULL); 

 if ( !TimerProcWnd ) 
 {
    VMBPanic(51);
 }    
}

// -----------------------------------------------------------------------------
//  VCL_TimerMember(VAR, int) - ändert einen Member 
// -----------------------------------------------------------------------------
void VCL_TimerMember(VMBVar *VCLCom, unsigned int Method)
{
     char *szBuffer1 = NULL;
     int iBuffer = 0, iX = 0, iY = 0, iW = 0, iH = 0;
     VCLTimerProperties *Properties;
#if VERBOSE > 2
    VMBDebug(" -> %s(%04i)", __func__, Method);
#endif     
     Properties = (VCLTimerProperties *)VCLCom->Properties;
     szBuffer1 = (char *)VMBParseOneString();
 
     switch(Method) 
        {
                       case 1:
                            Properties->Interval = atoi(szBuffer1);
                            break;
                       case 2:
                            if ( atoi(szBuffer1) )
                            {
                             Properties->Event_Elapsed = 1;     
                            } else {
                             Properties->Event_Elapsed = 0;                                        
                            }                            
                            break;                            
                       default:
#ifdef DEBUG                               
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VMBPanic(52);
                            break;               
        }
     VMBNextCode();   
     szBuffer1 = (char *)VMBFree(szBuffer1);
}


// -----------------------------------------------------------------------------
//  VCL_TimerGetMember(VAR, uint) - liest einen Member
// -----------------------------------------------------------------------------
void VCL_TimerGetMember(VMBVar *VCLCom, unsigned int Method)
{
     char TempBuffer[16];
     int iBuffer = 0;
#if VERBOSE > 2
    VMBDebug(" -> %s(%04i)", __func__, Method);
#endif     
    VCLTimerProperties *Prop = (VCLTimerProperties *)VCLCom->Properties;
     switch(Method) 
        {
                       case 1:
                            sprintf((char*)&TempBuffer, "%i", Prop->Interval);
                            KillTimer(TimerProcWnd, Prop->TimerID);
                            SetTimer(TimerProcWnd, Prop->TimerID, Prop->Interval, NULL);                            
                            break;
                       case 2:
                            if ( Prop->Event_Elapsed )
                            {
                               sprintf((char*)&TempBuffer, "1");
                            } else {
                               sprintf((char*)&TempBuffer, "0");                                   
                            }
                            break;  
                                                                                                                                                                                                                                                                                                                                                                    
                       default: 
#ifdef DEBUG                                
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VMBPanic(53);
                            break;  
     }
     VMBPushRStack(TempBuffer, VMBStrLen(TempBuffer));
}

// -----------------------------------------------------------------------------
//  VCL_TimerMethod(VAR, int) - ruft eine Methode auf
// -----------------------------------------------------------------------------
void VCL_TimerMethod(VMBVar *VCLCom, unsigned int Method)
{
#if VERBOSE > 2
    VMBDebug(" -> %s(%08x, %04i)", __func__, Method);
#endif     
     VCLTimerProperties *Prop = (VCLTimerProperties*)VCLCom->Properties;
     switch(Method) 
        {
                       case 1:
                            SetTimer(TimerProcWnd, Prop->TimerID, Prop->Interval, NULL);                            
                            break;
                       case 2:
                            KillTimer(TimerProcWnd, Prop->TimerID);
                            break; 
                       case 3:
                            Prop->Event_Elapsed = 0;
                            break;                          
                       default:
#ifdef DEBUG                               
                            VMBDebug("unkown method \"%i\"!", Method);
#endif                            
                            VMBPanic(54);
                            break;               
        }
}


// -----------------------------------------------------------------------------
// VCL_TimerProc(int, int, int, int) - Callback für Timer
// -----------------------------------------------------------------------------
LRESULT CALLBACK VCL_TimerProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{    
     char TempBuffer[16];
     switch(uMsg) {
        case WM_TIMER:
#ifdef SHOWWINDOWMESSAGES
             VMBDebug("Timer: WM_TIMER received!");
#endif                            
             sprintf((char*)&TempBuffer, "%04X", wParam);
             VMBVar *MyTimer = (VMBVar*)VMBFindVar(TempBuffer);
             if ( MyTimer )
             {
               VCLTimerProperties *Prop = (VCLTimerProperties*)MyTimer->Properties;    
               Prop->Event_Elapsed = 1;
             } else {
               VMBPanic(55);       
             }
             break;
     }
     return DefWindowProc(hwnd, uMsg, wParam, lParam);        
}
 

#endif

#endif
