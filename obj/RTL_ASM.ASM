;/***************************************************************************
; *   Versatile MB 2009 - C Runtime Library                                 *
; *   Copyright (C) 2002-2008 by Michael Böhme & embyte                     *
; *   http://www.v-mb.de | webmaster@v-mb.de                                *
; *                                                                         *
; *   This program is free software; you can redistribute it and/or modify  *
; *   it under the terms of the GNU General Public License as published by  *
; *   the Free Software Foundation; either version 2 of the License, or     *
; *   (at your option) any later version.                                   *
; *                                                                         *
; *   This program is distributed in the hope that it will be useful,       *
; *   but WITHOUT ANY WARRANTY; without even the implied warranty of        *
; *   MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the         *
; *   GNU General Public License for more details.                          *
; *                                                                         *
; *   You should have received a copy of the GNU General Public License     *
; *   along with this program; if not, write to the                         *
; *   Free Software Foundation, Inc.,                                       *
; *   59 Temple Place - Suite 330, Boston, MA  02111-1307, USA.             *
; ***************************************************************************/

format MS COFF

section '.text' code readable executable

 public _VMBExCall
 public _VMBExFCall
 public _VMBCodeCMP
 public _VMBGetCurrentCode

 extrn _RTL_Code

 _VMBExCall:
	push ebp
	mov ebp, esp
	sub esp, 0x0C

	mov ecx, [ebp+12]
	mov ebx, [ebp+16]

	or ecx, ecx
	jz .noparams

.pushloop:
	mov eax, ecx
	mov edx, 16
	mul edx
	push DWORD [ebx+eax-16]
	loop .pushloop
.noparams:

	mov ebx, [ebp+8]
	call ebx

	mov esp, ebp
	pop ebp
	ret

 _VMBExFCall:
	jmp _VMBExCall


 _VMBCodeCMP:
	push ebp
	
	mov ecx, [esp+08]
	mov edx, [esp+12]

	or ecx, ecx
	jz _codecm
	or edx, edx
	jz _codecm

	mov ecx, [ecx]

 _codecm:
	cmp ecx, [edx]
	jnz _false

	xor eax, eax
	pop ebp
 	ret
		
 _false:
	xor eax, eax
	inc eax	
	pop ebp
 	ret

 _VMBGetCurrentCode:
	push ebp
	mov ecx, [esp+08]
	mov edx, _RTL_Code

	mov edx, [edx]
	mov edx, [edx]
	mov dword [ecx], edx
	mov byte [ecx+4], 0x00

	pop ebp
	ret



section '.data' data readable writeable

