;******************************************************************************
;MIT License

;Copyright(c) 2025 René Pagel

;Filename: ProgDlgDX_x64.asm
;For more information see https://github.com/RePag-net/ProgDlgDX

;Permission is hereby granted, free of charge, to any person obtaining a copy
;of this software and associated documentation files(the "Software"), to deal
;in the Software without restriction, including without limitation the rights
;to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
;copies of the Software, and to permit persons to whom the Software is
;furnished to do so, subject to the following conditions :

;The above copyright notice and this permission notice shall be included in all
;copies or substantial portions of the Software.

;THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
;IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
;FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
;AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
;LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
;OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
;SOFTWARE.
;******************************************************************************
INCLUDE listing.inc

INCLUDE ..\..\Include\CompSys_x64.inc
INCLUDE ..\..\Include\ADT_x64.inc
INCLUDELIB OLDNAMES

EXTRN __imp_GetObjectA:PROC

PUBLIC dwEAX_CPUID_EAX_1
PUBLIC dwEBX_CPUID_EAX_1
PUBLIC dwECX_CPUID_EAX_1
PUBLIC dwEDX_CPUID_EAX_1

PUBLIC _SSE3
PUBLIC _AES
PUBLIC _AVX
PUBLIC _RDRAND
PUBLIC _MMX
PUBLIC _SSE
PUBLIC _SSE2

.DATA?
qwEineSpeicherSeite QWORD ?
qwDoppelSpeicherSeite QWORD ?
dbIntel DB ?

dwEAX_CPUID_EAX_1 DWORD ?
dwEBX_CPUID_EAX_1 DWORD ?
dwECX_CPUID_EAX_1 DWORD ?
dwEDX_CPUID_EAX_1 DWORD ?

dwEBX_CPUID_EAX_7 DWORD ?
dwECX_CPUID_EAX_7 DWORD ?

.DATA
;---ECX----
_SSE3 equ 0
_AES equ 25
_AVX equ 28
_RDRAND equ 30
;---EDX----
_MMX equ 23
_SSE equ 25
_SSE2 equ 26
;---EBX---
_AVX2 equ 5
_RDSEED equ 18
_SHA equ 29

CS_Graphic SEGMENT EXECUTE
;----------------------------------------------------------------------------
_Text SEGMENT
rsp_Bytes = 48
s_stSystemInfo = 32
?CPUID@@YQXAEAU_SYSTEM_INFO@@@Z PROC PUBLIC
		push rbx
    sub rsp, rsp_Bytes

    mov qword ptr s_stSystemInfo[rsp], rcx
		mov dword ptr [dbIntel], 00h
		xor rax, rax
		cpuid
		cmp ebx, 756e6547h
		jne short Speicher
		cmp ecx, 6c65746eh
		jne short Speicher
		cmp edx, 49656e69h
		jne short Speicher
		mov dword ptr [dbIntel], 01h

	Speicher:
		xor rax, rax
    mov rcx, qword ptr s_stSystemInfo[rsp]
    mov eax, dword ptr [rcx + 4]
		mov qword ptr [qwEineSpeicherSeite], rax
		imul rax, 02h
		mov qword ptr [qwDoppelSpeicherSeite], rax

		mov rax, 01h
		cpuid
		mov dword ptr [dwEAX_CPUID_EAX_1], eax
		mov dword ptr [dwEBX_CPUID_EAX_1], ebx
		mov dword ptr [dwECX_CPUID_EAX_1], ecx
		mov dword ptr [dwEDX_CPUID_EAX_1], edx

    mov rax, 07h
    xor rcx, rcx
		cpuid
		mov dword ptr [dwEBX_CPUID_EAX_7], ebx
		mov dword ptr [dwECX_CPUID_EAX_7], ecx

    add rsp, rsp_Bytes
		pop	rbx
		ret
?CPUID@@YQXAEAU_SYSTEM_INFO@@@Z ENDP
_Text ENDS
;---------------------------------------------------------------------------
_Text SEGMENT
s_push = 24 
s_Bytes = 104

COElement_hbmBild = 32
COElement_stHintergrundeffektfarbe = 42
COElement_stHintergrundfarbe = 88
COElement_rcEffektrand_oben = 96
COElement_rcEffektrand_unten = 104

rcZeichnen_left = 0
rcZeichnen_top = 4
rcZeichnen_right = 8
rcZeichnen_bottom = 12

STDIBSection_dsBm_bmBits = 24 + s_ShadowRegister
STDIBSection_dsBmih_biWidth = 36 + s_ShadowRegister
STDIBSection_dsBmih_biHeight = 40 + s_ShadowRegister
STDIBSection_dsBmih_biBitCount = 46 + s_ShadowRegister

so104_stDIBSection = 0 + s_ShadowRegister
?Hintergrund@COElement@DirectX@RePag@@AEAQXAEAUtagRECT@@@Z PROC PUBLIC
    push rbp
    push rbx
    push rdi
    sub rsp, s_ShadowRegister + s_Bytes

    mov rbp, rcx
    mov rdi, rdx

    lea r8, qword ptr so104_stDIBSection[rsp]
    mov rdx, 104
    mov rcx, qword ptr COElement_hbmBild[rbp]
    call qword ptr __imp_GetObjectA

		mov ecx, dword ptr COElement_stHintergrundfarbe[rbp]
		mov rax, rcx
		shl rax, 28h
		shld rcx, rax, 18h
		shld rcx, rax, 10h
		mov r8, rcx
		shr rax, 28h
		shrd rcx, rax, 10h
		vmovq xmm1, rcx
		vpslldq xmm1, xmm1, 08h
		vmovq xmm0, r8
		vpaddq xmm0, xmm0, xmm1

  DIBSection:
		mov edx, dword ptr STDIBSection_dsBmih_biHeight[rsp]
    mov rbx, rdx
		sub edx, dword ptr rcZeichnen_top[rdi]
		movq xmm1, rdx
		sub ebx, dword ptr rcZeichnen_bottom[rdi]
		mov r9d, dword ptr rcZeichnen_right[rdi]
		mov r11d, dword ptr rcZeichnen_left[rdi]
		sub r9, r11
    imul r9, 03h
		imul r11, 03h
		add r11, qword ptr STDIBSection_dsBm_bmBits[rsp]

		movzx r10, word ptr STDIBSection_dsBmih_biBitCount[rsp]
		imul r10d, dword ptr STDIBSection_dsBmih_biWidth[rsp]
		add r10, 1Fh 
		and r10, 0FFFFFFE0h 
		sar r10, 03h

	ZeilenAnfang:
		mov rcx, r10
		imul rcx, rbx 
		mov rax, r11
		add rax, rcx

		mov rdx, r9

	Register_16:
		cmp rdx, 10h
		jl Register_8
		movdqu xmmword ptr [rax], xmm0
		add rax, 0fh
		sub rdx, 0fh
		jmp Register_16

	Register_8:
		cmp rdx, 08h
		jl Register_4
		mov qword ptr [rax], r8
		add rax, 06h
		sub rdx, 06h
		jmp Register_8

	Register_4:
		cmp rdx, 04h
		jl Register_2
		mov dword ptr [rax], r8d
		add rax, 03h
		sub rdx, 03h
		jmp Register_4

	Register_2:
		cmp rdx, 03h
		jl ZeilenEnde
		mov ecx, r8d
		mov word ptr [rax], cx
		add rax, 02h
		shr ecx, 08h
		mov byte ptr [rax], ch

	ZeilenEnde:
		movq rcx, xmm1
		inc rbx
		cmp rbx, rcx
		jl ZeilenAnfang

	Ende:
    add rsp, s_ShadowRegister + s_Bytes
    pop rdi
    pop rbx
    pop rbp
    ret
?Hintergrund@COElement@DirectX@RePag@@AEAQXAEAUtagRECT@@@Z ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
s_push = 24 
s_Bytes = 104
c_rcEffektrand_links = 64 
c_rcEffektrand_oben = 68
c_rcEffektrand_rechts = 72
c_rcEffektrand_unten = 0
c_stHintergrundeffektfarbe = 26

COElement_hbmBild = 32
COElement_stHintergrundeffektfarbe = 42
COElement_rcEffektrand_oben = 96
COElement_rcEffektrand_unten = 104

STDIBSection_dsBm_bmBits = 24 + s_ShadowRegister
STDIBSection_dsBmih_biWidth = 36 + s_ShadowRegister
STDIBSection_dsBmih_biHeight = 40 + s_ShadowRegister
STDIBSection_dsBmih_biBitCount = 46 + s_ShadowRegister

so104_stDIBSection = 0 + s_ShadowRegister
?Effekt_Rand_Horizontal@COElement@DirectX@RePag@@AEAQXXZ PROC
    push rbp
    push rbx
    push rdi
    sub rsp, s_ShadowRegister + s_Bytes

    mov rbp, rcx
    mov rdi, rdx

    lea r8, so104_stDIBSection[rsp]
    mov rdx, 104
    mov rcx, qword ptr COElement_hbmBild[rbp]
    call qword ptr __imp_GetObjectA
    
		mov ecx, dword ptr COElement_stHintergrundeffektfarbe[rbp]
		mov rax, rcx
		shl rax, 28h
		shld rcx, rax, 18h
		shld rcx, rax, 10h
		mov r8, rcx
		shr rax, 28h
		shrd rcx, rax, 10h
		vmovq xmm1, rcx
		vpslldq xmm1, xmm1, 08h
		vmovq xmm0, r8
		vpaddq xmm0, xmm0, xmm1

    mov edx, dword ptr STDIBSection_dsBmih_biWidth[rsp]
		imul rdx, 03h
		movd mm2, rdx
		mov rax, qword ptr STDIBSection_dsBm_bmBits[rsp]		
		movd mm4, rax
		movzx rcx, word ptr STDIBSection_dsBmih_biBitCount[rsp] 
		imul ecx, dword ptr STDIBSection_dsBmih_biWidth[rsp] 
		add rcx, 1Fh 
		and rcx, 0FFFFFFE0h 
		sar rcx, 03h
		movq mm3, rcx

    mov edx, dword ptr COElement_rcEffektrand_unten[rbp]
		test rdx, rdx
		je RandOben
		sub rdx, 1

	RandOben:
		movd mm7, rdx
		mov ebx, dword ptr STDIBSection_dsBmih_biHeight[rsp]
		sub rbx, 1
		mov rcx, rbx
    sub ecx, dword ptr COElement_rcEffektrand_oben[rbp]
		movd mm6, rcx
		cmp rbx, rcx
		je ZeilenEnde
		cmp rcx, rdx
		jge ZeilenAnfang
		movq mm6, mm7

	ZeilenAnfang:
		movd rcx, mm3
		imul rcx, rbx 
		movd rax, mm4
		add rax, rcx
		movd rdx, mm2

	Register_16:
		cmp rdx, 10h
		jl Register_8
		movups xmmword ptr [rax], xmm0
		add rax, 0fh
		sub rdx, 0fh
		jmp Register_16

	Register_8:
		cmp rdx, 08h
		jl Register_4
		mov qword ptr [rax], r8
		add rax, 06h
		sub rdx, 06h
		jmp Register_8

	Register_4:
		cmp rdx, 04h
		jl Register_2
		mov dword ptr [rax], r8d
		add rax, 03h
		sub rdx, 03h
		jmp Register_4

	Register_2:
		cmp rdx, 03h
		jl ZeilenEnde
		mov rcx, r8
		mov word ptr [rax], cx
		add rax, 02h
		shr rcx, 08h
		mov byte ptr [rax], ch

	ZeilenEnde:
		movd rcx, mm6
		dec rbx
		cmp rbx, rcx
		jge ZeilenAnfang
		cmp rbx, 0h
		jl Ende
		movd rbx, mm7
		cmp rbx, 0h
		je Ende

		mov rbx, 0h
		movd mm6, rbx
    movd rbx, mm7
		jmp ZeilenAnfang

	Ende:
		emms
    add rsp, s_ShadowRegister + s_Bytes
    pop rdi
    pop rbx
    pop rbp
    ret
?Effekt_Rand_Horizontal@COElement@DirectX@RePag@@AEAQXXZ ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
s_push = 24 
s_Bytes = 104

COElement_stHintergrundeffektfarbe = 42
COElement_hbmBild = 32
COElement_rcEffektrand_links = 92
COElement_rcEffektrand_rechts = 100

STDIBSection_dsBm_bmBits = 24 + s_ShadowRegister
STDIBSection_dsBmih_biWidth = 36 + s_ShadowRegister
STDIBSection_dsBmih_biHeight = 40 + s_ShadowRegister
STDIBSection_dsBmih_biBitCount = 46 + s_ShadowRegister

so104_stDIBSection = 0 + s_ShadowRegister
?Effekt_Rand_Vertical@COElement@DirectX@RePag@@AEAQXXZ PROC
    push rbp
    push rbx
    push rdi
    sub rsp, s_ShadowRegister + s_Bytes

    mov rbp, rcx
    mov rdi, rdx

    lea r8, so104_stDIBSection[rsp]
    mov rdx, 104
    mov rcx, qword ptr COElement_hbmBild[rbp]
    call qword ptr __imp_GetObjectA

    mov ecx, dword ptr COElement_stHintergrundeffektfarbe[rbp]
		mov rax, rcx
		shl rax, 28h
		shld rcx, rax, 18h
		shld rcx, rax, 10h
		mov r8, rcx
		shr rax, 28h
		shrd rcx, rax, 10h
		vmovq xmm1, rcx
		vpslldq xmm1, xmm1, 08h
		vmovq xmm0, r8
		vpaddq xmm0, xmm0, xmm1

		mov edx, dword ptr STDIBSection_dsBmih_biHeight[rsp]
		movd mm1, rdx
		mov ecx, dword ptr STDIBSection_dsBmih_biWidth[rsp]
		mov edx, dword ptr COElement_rcEffektrand_links[rbp]
		sub rcx, rdx
		imul rdx, 03h
		movd mm5, rdx
		mov edx, dword ptr COElement_rcEffektrand_rechts[rbp]
		movd mm6, rdx
		sub rcx, rdx
		imul rcx, 03h
		movd mm2, rcx
		movzx rcx, word ptr STDIBSection_dsBmih_biBitCount[rsp] 
		imul ecx, dword ptr STDIBSection_dsBmih_biWidth[rsp] 
		add rcx, 1Fh 
		and rcx, 0FFFFFFE0h 
		sar rcx, 03h
		movd mm3, rcx
		mov rax, qword ptr STDIBSection_dsBm_bmBits[rsp]
		movd mm4, rax
		add rax, rcx
		xor rbx, rbx

	ZeilenAnfang:
		movd rcx, mm3
		imul rcx, rbx 
		movd rax, mm4
		add rax, rcx

		movd rdx, mm5
		cmp rdx, 0h
		je Rechts

	Register_16:
		cmp rdx, 10h
		jl Register_8
		movups xmmword ptr [rax], xmm0
		add rax, 0fh
		sub rdx, 0fh
		jmp Register_16

	Register_8:
		cmp rdx, 08h
		jl Register_4
		mov qword ptr [rax], r8
		add rax, 06h
		sub rdx, 06h
		jmp Register_8

	Register_4:
		cmp rdx, 04h
		jl Register_2
		mov dword ptr [rax], r8d
		add rax, 03h
		sub rdx, 03h
		jmp Register_4

	Register_2:
		cmp rdx, 03h
		jl ZeilenEnde

	Rechts:
		mov rcx, r8
		mov word ptr [rax], cx
		add rax, 02h
		shr rcx, 08h
		mov byte ptr [rax], ch

		movd rcx, mm2
		add rax, rcx
		inc rax

		movd rdx, mm6
		imul rdx, 03h

	Register_16A:
		cmp rdx, 10h
		jl Register_8A
		movups xmmword ptr [rax], xmm0
		add rax, 0fh
		sub rdx, 0fh
		jmp Register_16A

	Register_8A:
		cmp rdx, 08h
		jl Register_4A
		mov qword ptr [rax], r8
		add rax, 06h
		sub rdx, 06h
		jmp Register_8A

	Register_4A:
		cmp rdx, 04h
		jl Register_2A
		mov dword ptr [rax], r8d
		add rax, 03h
		sub rdx, 03h
		jmp Register_4A

	Register_2A:
		cmp rdx, 03h
		jl ZeilenEnde
		mov rcx, r8
		mov word ptr [rax], cx
		add rax, 02h
		shr rcx, 08h
		mov byte ptr [rax], ch

	ZeilenEnde:
		movd rcx, mm1
		inc rbx
		cmp rbx, rcx
		jl ZeilenAnfang

	Ende:
		emms
    add rsp, s_ShadowRegister + s_Bytes
    pop rdi
    pop rbx
    pop rbp
    ret
?Effekt_Rand_Vertical@COElement@DirectX@RePag@@AEAQXXZ ENDP
_Text ENDS
;----------------------------------------------------------------------------
_Text SEGMENT
s_push = 24 
s_Bytes = 104

COElement_stHintergrundeffektfarbe = 42
COElement_hbmBild = 32
COElement_rcEffektrand_oben = 96
COElement_rcEffektrand_unten = 104

STDIBSection_dsBm_bmBits = 24 + s_ShadowRegister
STDIBSection_dsBmih_biWidth = 36 + s_ShadowRegister
STDIBSection_dsBmih_biHeight = 40 + s_ShadowRegister
STDIBSection_dsBmih_biBitCount = 46 + s_ShadowRegister

so104_stDIBSection = 0 + s_ShadowRegister
?Effekt_Beleuchtung_Horizontal@COElement@DirectX@RePag@@AEAQXXZ PROC
    push rbp
    push rbx
    push rdi
    sub rsp, s_ShadowRegister + s_Bytes

    mov rbp, rcx
    mov rdi, rdx

    lea r8, so104_stDIBSection[rsp]
    mov rdx, 104
    mov rcx, qword ptr COElement_hbmBild[rbp]
    call qword ptr __imp_GetObjectA

		mov ecx, dword ptr COElement_stHintergrundeffektfarbe[rbp]
		mov ebx, dword ptr COElement_rcEffektrand_oben[rbp]
		test rbx, rbx
		je Schritt
      
		mov ebx, ecx
		shl ecx, 08h
		shr ebx, 08h
		mov cl, bh
		mov dx, bx
		mov ebx, ecx
		shl ebx, 08h
		mov bl, dl

		movd mm0, ebx
		movd mm1, ecx
		punpckldq mm0, mm1
		movq mm1, mm0
		psrlq mm1, 10h
		punpckldq mm1, mm0

		movq2dq xmm0, mm0
		movq2dq xmm1, mm1
		movlhps xmm0, xmm1

	Schritt:
		mov rcx, 0303030303030303h
		movq mm5, rcx

    mov edx, dword ptr STDIBSection_dsBmih_biWidth[rsp]
		imul rdx, 03h
		movd mm2, rdx
		mov rax, qword ptr STDIBSection_dsBm_bmBits[rsp]
		movd mm4, rax
		movzx rcx, word ptr STDIBSection_dsBmih_biBitCount[rsp] 
		imul ecx, dword ptr STDIBSection_dsBmih_biWidth[rsp] 
		add rcx, 1Fh 
		and rcx, 0FFFFFFE0h 
		sar rcx, 03h
		movd mm3, rcx

    mov edx, dword ptr COElement_rcEffektrand_unten[rbp]
		test rdx, rdx
		je RandOben
		sub rdx, 1

	RandOben:
		movd mm7, rdx
		mov ebx, dword ptr STDIBSection_dsBmih_biHeight[rsp]
		sub rbx, 1
		mov rcx, rbx
    sub ecx, dword ptr COElement_rcEffektrand_oben[rbp]
		movd mm6, rcx
		cmp rbx, rcx
		je ZeilenEnde
		cmp rcx, rdx
		jge ZeilenAnfang
		movq mm6, mm7

	ZeilenAnfang:
		movd rcx, mm3
		imul rcx, rbx 
		movd rax, mm4
		add rax, rcx
		movd rdx, mm2

		movd rcx, mm7
		cmp rbx, rcx
		jl Unten
		je Register_16
		psubb mm0, mm5
		psubb mm1, mm5
		movq2dq xmm0, mm0
		movq2dq xmm1, mm1
		movlhps xmm0, xmm1
		jmp Register_16

	Unten:
		paddb mm0, mm5
		paddb mm1, mm5
		movq2dq xmm0, mm0
		movq2dq xmm1, mm1
		movlhps xmm0, xmm1

	Register_16:
		cmp rdx, 10h
		jl Register_8
		movups xmmword ptr [rax], xmm0
		add rax, 0fh
		sub rdx, 0fh
		jmp Register_16

	Register_8:
		cmp rdx, 08h
		jl Register_4
		movq mmword ptr [rax], mm0
		add rax, 06h
		sub rdx, 06h
		jmp Register_8

	Register_4:
		cmp rdx, 04h
		jl Register_2
		movd dword ptr [rax], mm0
		add rax, 03h
		sub rdx, 03h
		jmp Register_4

	Register_2:
		cmp rdx, 03h
		jl ZeilenEnde
		movd ecx, mm0
		mov word ptr [rax], cx
		add rax, 02h
		shr rcx, 08h
		mov byte ptr [rax], ch

	ZeilenEnde:
		movd rcx, mm6
		dec rbx
		cmp rbx, rcx
		jge ZeilenAnfang
		test rbx, rbx
		jl Ende
		movd rbx, mm7
		test rbx, rbx
		je Ende

		mov rcx, qword ptr COElement_stHintergrundeffektfarbe[rbp]
		mov ebx, dword ptr COElement_rcEffektrand_unten[rbp]
		imul rbx, 030303h
		sub rcx, rbx
      
		mov rbx, rcx
		shl rcx, 08h
		shr rbx, 08h
		mov cl, bh
		mov dx, bx
		mov rbx, rcx
		shl rbx, 08h
		mov bl, dl

		movd mm0, rbx
		movd mm1, rcx
		punpckldq mm0, mm1
		movq mm1, mm0
		psrlq mm1, 10h
		punpckldq mm1, mm0

		movq2dq xmm0, mm0
		movq2dq xmm1, mm1
		movlhps xmm0, xmm1

		mov rbx, 0h
		movd mm6, rbx
    movd rbx, mm7
		jmp ZeilenAnfang

	Ende:
		emms
    add rsp, s_ShadowRegister + s_Bytes
    pop rdi
    pop rbx
    pop rbp
    ret
?Effekt_Beleuchtung_Horizontal@COElement@DirectX@RePag@@AEAQXXZ ENDP
_Text ENDS
;----------------------------------------------------------------------------
CS_Graphic ENDS
END
