.686P
.XMM
INCLUDE listing.inc
.MODEL FLAT
INCLUDELIB LIBCMTD
INCLUDELIB OLDNAMES

EXTRN	__imp__GetObjectA@12:PROC

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

.DATA ?
dwEineSpeicherSeite DWORD ?
dwDoppelSpeicherSeite DWORD ?
dbIntel DB ?

dwEAX_CPUID_EAX_1 DWORD ?
dwEBX_CPUID_EAX_1 DWORD ?
dwECX_CPUID_EAX_1 DWORD ?
dwEDX_CPUID_EAX_1 DWORD ?

dwEBX_CPUID_EAX_7 DWORD ?
dwECX_CPUID_EAX_7 DWORD ?

.DATA
; --- ECX----
_SSE3 equ 0
_AES equ 25
_AVX equ 28
_RDRAND equ 30
; --- EDX----
_MMX equ 23
_SSE equ 25
_SSE2 equ 26
; --- EBX-- -
_AVX2 equ 5
_RDSEED equ 18
_SHA equ 29

CS_Prozessor SEGMENT PARA PRIVATE FLAT EXECUTE
; ----------------------------------------------------------------------------
a_stSystemInfo_dwPageSize = 4
? CPUID@@YQXAAU_SYSTEM_INFO@@@Z PROC PUBLIC
push ebx
push esi
mov esi, ecx

mov dword ptr[dbIntel], 00h
xor eax, eax
cpuid
cmp ebx, 756e6547h
jne Speicher
cmp ecx, 6c65746eh
jne Speicher
cmp edx, 49656e69h
jne Speicher
mov dword ptr[dbIntel], 01h

Speicher :
mov ecx, dword ptr a_stSystemInfo_dwPageSize[esi]
mov dword ptr[dwEineSpeicherSeite], ecx
imul ecx, 02h
mov dword ptr[dwDoppelSpeicherSeite], ecx

mov eax, 01h
cpuid
mov dword ptr[dwEAX_CPUID_EAX_1], eax
mov dword ptr[dwEBX_CPUID_EAX_1], ebx
mov dword ptr[dwECX_CPUID_EAX_1], ecx
mov dword ptr[dwEDX_CPUID_EAX_1], edx

mov eax, 07h
xor ecx, ecx
cpuid
mov dword ptr[dwEBX_CPUID_EAX_7], ebx
mov dword ptr[dwECX_CPUID_EAX_7], ecx

pop	ebx
pop esi
ret 0
? CPUID@@YQXAAU_SYSTEM_INFO@@@Z ENDP
; ----------------------------------------------------------------------------
CS_Prozessor ENDS