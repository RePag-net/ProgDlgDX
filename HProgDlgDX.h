/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: HProgDlgDX.h
For more information see https://github.com/RePag-net/ProgDlgDX

Permission is hereby granted, free of charge, to any person obtaining a copy
of this software and associated documentation files(the "Software"), to deal
in the Software without restriction, including without limitation the rights
to use, copy, modify, merge, publish, distribute, sublicense, and /or sell
copies of the Software, and to permit persons to whom the Software is
furnished to do so, subject to the following conditions :

The above copyright notice and this permission notice shall be included in all
copies or substantial portions of the Software.

THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT.IN NO EVENT SHALL THE
AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
SOFTWARE.
******************************************************************************/
#pragma once
#include "targetver.h"

#define WIN32_LEAN_AND_MEAN             // Selten verwendete Komponenten aus Windows-Headern ausschließen

#include "Aclapi.h"
#include "Sddl.h"

#include <d3d11_4.h>
#include <d2d1_3.h>
#include <DXGI.h>
#include <dxgi1_6.h>
#include <dwrite_3.h>

#include "Menu.h"
#include "CompSys.h"
using namespace RePag::System;
__declspec(dllimport) void __vectorcall RePag::System::DeleteEvent(HANDLE hEreignis);

#include "OGraphicD2.h"
using namespace RePag::DirectX;

//-------------------------------------------------------------------------------------------------------------------------------------------
typedef struct STTHWndKlassen
{
  bool bAbbruch;
  CRITICAL_SECTION csWndKlasse;
} STTHWndKlassen;
DWORD WINAPI thLoschWndKlassen(void* pvParam);
//-------------------------------------------------------------------------------------------------------------------------------------------
typedef struct STThreadId
{
  DWORD dwThreadId;
  COElement* pElement;
} STThreadId;
//-------------------------------------------------------------------------------------------------------------------------------------------
extern COList* vthlThreadId;
extern COList* vthlDialoge;
extern HINSTANCE hInstance;
extern VMEMORY vmDialog;
extern HANDLE htqTimerQueue;
extern char pcRePag_GraphicGroup[];
//-------------------------------------------------------------------------------------------------------------------------------------------
//void __vectorcall CPUID(SYSTEM_INFO& stSystem_Info);
void __vectorcall LoschWndKlasse(ATOM atWndKlasse);
//-------------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
  namespace DirectX
  {
    //---------------------------------------------------------------------------------------------------------------------------------------
    __declspec(dllexport) VMEMORY __vectorcall VMDialog(void);
    __declspec(dllexport) HINSTANCE __vectorcall Hinstance(void);
    __declspec(dllexport) HACCEL __vectorcall Accelerator(void);
    __declspec(dllexport) HANDLE __vectorcall TimerQueue(void);
    __declspec(dllexport) void __vectorcall Cursorform(const char* pcCursor);
    __declspec(dllexport) void __vectorcall EinfugenKurzTasten(const ACCEL* pacTasten, int iAnzahl);
    __declspec(dllexport) HRESULT __vectorcall CreateDeviceIndependentResources(_Out_ STDeviceResources* pstDeviceResources);
    __declspec(dllexport) HRESULT __vectorcall CreateDeviceResources(_Out_ STDeviceResources* pstDeviceResources);
    //---------------------------------------------------------------------------------------------------------------------------------------
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------
template<class Interface>
inline void SafeRelease(Interface** ppInterfaceToRelease)
{
  if(*ppInterfaceToRelease != NULL){
    (*ppInterfaceToRelease)->Release();

    (*ppInterfaceToRelease) = NULL;
  }
}
//-------------------------------------------------------------------------------------------------------------------------------------------
