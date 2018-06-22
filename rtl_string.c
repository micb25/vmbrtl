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
 
#ifndef RTL_STRING_C
#define RTL_STRING_C

#include <windows.h>
#include "rtl_mem.h"

// -----------------------------------------------------------------------------
// VMBStrLen(char *) - gibt die Stringlänge zurück
// -----------------------------------------------------------------------------
int VMBStrLen(char *Buffer)
{
    if ( Buffer == NULL )
         return 0;
    else
         return strlen(Buffer);
    
}

// -----------------------------------------------------------------------------
// VMBUpperCase(char *) - konvertiert einen String in Großbuchstaben
// -----------------------------------------------------------------------------
void *VMBUpperCase(char *Buffer)
{
 char *oldBuffer = Buffer; 
 if (Buffer == NULL)     
  return NULL;
 while ( *(Buffer) != '\0' )
 {
  *Buffer = toupper(*Buffer);
  Buffer++;
 }
 return oldBuffer;
}

// -----------------------------------------------------------------------------
// VMBLowerCase(char *) - konvertiert einen String in Kleinbuchstaben
// -----------------------------------------------------------------------------
void *VMBLowerCase(char *Buffer)
{
 char *oldBuffer = Buffer; 
 if (Buffer == NULL)     
  return NULL;
 while ( *(Buffer) != '\0' )
 {
  *Buffer = tolower(*Buffer);
  Buffer++;
 }
 return oldBuffer; 
}

// -----------------------------------------------------------------------------
// VMBLeftStr(char *, int) - LeftStr fuer VMB
// -----------------------------------------------------------------------------
char *VMBLeftStr(char *Buffer, unsigned int iLength)
{
 char *newBuffer; 
 unsigned int iStrLen;

 if (Buffer == NULL || iLength == 0)     
  return NULL;
 
 iStrLen = strlen(Buffer);

 if ( iStrLen <= iLength )
 {
  newBuffer = (char *)VMBAlloc(iStrLen+1);     
  strncpy(newBuffer, Buffer, iStrLen);
 } else {
  newBuffer = (char *)VMBAlloc(iLength+1);     
  strncpy(newBuffer, Buffer, iLength);
 }
 return newBuffer;
}

// -----------------------------------------------------------------------------
// VMBRightStr(char *, int) - RightStr fuer VMB
// -----------------------------------------------------------------------------
char *VMBRightStr(char *Buffer, unsigned int iLength)
{
 char *newBuffer; 
 unsigned int iStrLen, iCounter = 0;

 if (Buffer == NULL || iLength == 0)     
  return NULL;
 
 iStrLen = strlen(Buffer);

 if ( iStrLen <= iLength )
 {
  newBuffer = (char *)VMBAlloc(iStrLen+1);     
  strncpy(newBuffer, Buffer, iStrLen);
 } else {
  newBuffer = (char *)VMBAlloc(iLength+1);     
  strncpy(newBuffer, Buffer-iLength+iStrLen, iLength);
 }
 return newBuffer;
}

// -----------------------------------------------------------------------------
// VMBTrim(char *) - entfernt ' ' und '\t' am Ende und Anfang eines Strings
// -----------------------------------------------------------------------------
char *VMBTrim(char *Buffer)
{
 char *newBuffer; 
 int iPos = 0, iLength = VMBStrLen(Buffer), i;
 
 if (Buffer == NULL || iLength == 0)     
  return NULL;
 
 while ( ( *(Buffer+iPos) == ' ' || *(Buffer+iPos) == '\t' ) && (iPos < iLength) )
 {
       iPos++;
 }

 if (iPos == iLength)
    return NULL;

 newBuffer = (char *)VMBAlloc(iLength-iPos+1);     
 strncpy(newBuffer, Buffer + iPos, iLength-iPos);
 while ( (*(newBuffer+strlen(newBuffer)-1) == ' ' || *(newBuffer+strlen(newBuffer)-1) == '\t') && *(newBuffer+strlen(newBuffer)-1) != '\0')
 {
  *(newBuffer+strlen(newBuffer)-1) = '\0';      
 }
 return newBuffer;
}

// -----------------------------------------------------------------------------
// VMBGetIntHex(char *) - wandelt einen HexCode in einen Integer um
// -----------------------------------------------------------------------------
unsigned int VMBGetIntHex(char *hexcode)
{
         char *newBuffer;
         char szSys[] = "0123456789ABCDEF";
         unsigned int iResult = 0, factor = 1; 
         
         int i, iLength = VMBStrLen(hexcode);
            
         for ( i = iLength - 1; i >= 0; i-- )
         {
             newBuffer = strchr(szSys, *(hexcode + i));
             if (!newBuffer)
                return 0;
             iResult += factor * ((int)newBuffer - (int)&szSys);
             factor *= 16;
         }
         return iResult;
}

// -----------------------------------------------------------------------------
// VMBGetIntHex(char *) - wandelt einen HexCode in einen Integer um
// -----------------------------------------------------------------------------
unsigned int VMBGetIntHexCode(char *hexcode)
{
         unsigned int iResult = 0, factor = 1; 
         
         int i;
         for ( i = 3; i >= 0; i-- )
         {
             //newBuffer = strchr(szSys, *(hexcode + i));
             if ( *(hexcode + i) < 58 )
             {
                  iResult += factor * (*(hexcode + i) - 48);
             } else {
                  iResult += factor * (*(hexcode + i) - 65 + 10);  
             }
//             if (!newBuffer)
//                return 0;
//             iResult += factor * ((int)newBuffer - (int)&szSys);
             factor *= 16;
         }
         return iResult;
}

// -----------------------------------------------------------------------------
// VMBCharAt(char *, int) - gibt das Zeichen an einer bestimmten Stelle zurück
// -----------------------------------------------------------------------------
char VMBCharAt(char *str, int iPos)
{
     if (str)     
     {
        int iLength = VMBStrLen(str);
        if (iPos > iLength)
            return 0;
        else
            return *(str + iPos);
     } else {
        return 0;       
     }
}

char VMBIfisTrue(char *Str)
{
     char cChar;
     int iStrLen = VMBStrLen(Str);   
     if (Str == NULL)
        return 0;
     if (iStrLen == 1)
     {
        if (*Str == '0')                   
           return 0;
        else
           return 1;
     }
     else {
        char *TempBuffer = (char*)VMBAlloc(iStrLen+5);
        char *OldBuf = (char*)TempBuffer;
        while ( VMBStrLen(Str) > 1 )
        {
              if (*(Str+1) == '&')      
              {
                 if ( *Str == '1' && *(Str+2) == '1' ) 
                     cChar = '1';
                 else
                     cChar = '0';
              } else
              if (*(Str+1) == '|')      
              {
                 if ( *Str == '1' || *(Str+2) == '1' ) 
                     cChar = '1';
                 else
                     cChar = '0';
              } else
              if (*(Str+1) == '!')      
              {
                 if ( !( *Str == '1' || *(Str+2) == '1' )  ) 
                     cChar = '1';
                 else
                     cChar = '0';
              } else
              if (*(Str+1) == '^')      
              {
                 if ( ( *Str == '1' || *(Str+2) == '1' ) && ( *Str != *(Str+2)) ) 
                     cChar = '1';
                 else
                     cChar = '0';
              } else {                     
                 if ( !(  *Str == '1' && *(Str+2) == '1' ) )    
                     cChar = '1';
                 else
                     cChar = '0';                 
              } 
              iStrLen = VMBStrLen(Str) - 3;
              if ( iStrLen )
              {
                  char *Right = VMBRightStr(Str, iStrLen);
                  sprintf(TempBuffer, "%c%s", cChar, Right);
                  Right = (char*)VMBFree(Right);
              } else {
                  *TempBuffer = cChar;
                  *(TempBuffer+1) = '\0';
              }                            
              Str = TempBuffer;              
        }
        if (*Str == '1')
        {
            OldBuf = (char*)VMBFree(OldBuf);  
            return 1;       
        } else {    
            OldBuf = (char*)VMBFree(OldBuf);  
            return 0;
        }
     }
}

// -----------------------------------------------------------------------------
// VMBMidStr(char *, int, int) - MidStr fuer VMB
// -----------------------------------------------------------------------------
char *VMBMidStr(char *Buffer, unsigned int Pos, unsigned int Length)
{
 char *newBuffer; 
 unsigned int iStrLen;

 if (Buffer == NULL || Length == 0)     
  return ""; 
 iStrLen = VMBStrLen(Buffer);
 if ( Length > iStrLen )
  Length = iStrLen;
 if ( iStrLen < Pos )
  return "";
 
 Pos--;
 
 while ( *Buffer != '\0' && Pos )
 {
       Pos--;   
       Buffer++;  
 }
 newBuffer = (char *)VMBAlloc(Length+1);     
 strncpy(newBuffer, Buffer, Length);
 return newBuffer;
}

#endif
 
