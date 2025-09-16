/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: ProgDlgDX.cpp
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
#include "HProgDlgDX.h"
#include "OGraphicGroupD2.h"
//-------------------------------------------------------------------------------------------------------------------------------------------
#pragma comment(lib, "D3D11")
#pragma comment(lib, "D2d1")
#pragma comment(lib, "Dwrite")
//-------------------------------------------------------------------------------------------------------------------------------------------
HINSTANCE hInstance;
VMEMORY vmDialog;
HACCEL hAccelerator;
HANDLE htqTimerQueue;
COList* vthlWndKlassen;
HANDLE heWndKlassen;
HANDLE hthWndKlassen;
STTHWndKlassen thstWndKlassen;
COList* vthlDialoge;
COList* vthlThreadId;
char pcRePag_GraphicGroup[] = "RePag_GrafikGruppe";
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePagPrgDX_Start(void)
{
  SYSTEM_INFO stSystem_Info; GetSystemInfo(&stSystem_Info);
  CPUID(stSystem_Info);

  ULONG ulMinimumWorkingSetSize, ulMaximumWorkingSetSize;
  HANDLE hProcess = OpenProcess(PROCESS_ALL_ACCESS, false, GetCurrentProcessId());
  ulMinimumWorkingSetSize = 12288000; ulMaximumWorkingSetSize = 20480000;
  SetProcessWorkingSetSize(hProcess, ulMinimumWorkingSetSize, ulMaximumWorkingSetSize);

  vmDialog = InitVirtualMem(true, "Dialoge");

  WNDCLASSEX wndKlasse;
  wndKlasse.cbSize = sizeof(WNDCLASSEX);
  wndKlasse.style = CS_OWNDC;
  wndKlasse.cbClsExtra = 0;
  wndKlasse.cbWndExtra = 16;
  wndKlasse.hInstance = NULL;
  wndKlasse.hIcon = NULL;
  wndKlasse.hCursor = LoadCursor(NULL, IDC_ARROW);
  wndKlasse.hbrBackground = NULL;
  wndKlasse.lpszMenuName = NULL;
  wndKlasse.hIconSm = NULL;
  wndKlasse.lpszClassName = pcRePag_GraphicGroup;
  wndKlasse.lpfnWndProc = WndProc_GraphicGroup;
  RegisterClassEx(&wndKlasse);

  hInstance = wndKlasse.hInstance;

  ACCEL accel7[7];
  accel7[0].fVirt = FVIRTKEY | FCONTROL;
  accel7[0].key = 0x58;
  accel7[0].cmd = IDM_AUSSCHNEIDEN;

  accel7[1].fVirt = FVIRTKEY | FCONTROL;
  accel7[1].key = 0x43;
  accel7[1].cmd = IDM_KOPIEREN;

  accel7[2].fVirt = FVIRTKEY | FCONTROL;
  accel7[2].key = 0x56;
  accel7[2].cmd = IDM_EINFUGEN;

  accel7[3].fVirt = FVIRTKEY | FCONTROL;
  accel7[3].key = 0x4f;
  accel7[3].cmd = IDM_DATEI_OFFNEN;

  accel7[4].fVirt = FVIRTKEY | FCONTROL;
  accel7[4].key = 0x53;
  accel7[4].cmd = IDM_DATEI_SPEICHERN;

  accel7[5].fVirt = FVIRTKEY | FCONTROL | FSHIFT;
  accel7[5].key = VK_DELETE;
  accel7[5].cmd = IDM_DATEI_ENTFERNEN;

  accel7[6].fVirt = FVIRTKEY | FCONTROL;
  accel7[6].key = 0x4c;
  accel7[6].cmd = IDM_DATEI_LADEN;

  hAccelerator = CreateAcceleratorTable(accel7, 7);
  htqTimerQueue = CreateTimerQueue();

  vthlDialoge = COListV(vmDialog, true);
  vthlThreadId = COListV(vmDialog, true);
  vthlWndKlassen = COListV(true);
  InitializeCriticalSectionAndSpinCount(&thstWndKlassen.csWndKlasse, 1000);
  thstWndKlassen.bAbbruch = false;
  heWndKlassen = CreateEvent(NULL, false, false, NULL);
  hthWndKlassen = CreateThread(NULL, 0, thLoschWndKlassen, &thstWndKlassen, CREATE_SUSPENDED, NULL);
  if(hthWndKlassen){ SetThreadPriority(hthWndKlassen, THREAD_PRIORITY_IDLE); ResumeThread(hthWndKlassen); }

  CoInitialize(NULL);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePagPrgDX_Ende(void)
{
  CoUninitialize();

  DestroyAcceleratorTable(hAccelerator);
  DeleteTimerQueueEx(htqTimerQueue, INVALID_HANDLE_VALUE);
  UnregisterClass(pcRePag_GraphicGroup, hInstance);

  EnterCriticalSection(&thstWndKlassen.csWndKlasse);
  thstWndKlassen.bAbbruch = true;
  LeaveCriticalSection(&thstWndKlassen.csWndKlasse);

  SetEvent(heWndKlassen);
  Sleep(500);
  CloseHandle(hthWndKlassen);

  VMFreiV(vthlDialoge); VMFreiV(vthlThreadId);
  FreeVirtualMem(vmDialog);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::VMDialog(void)
{
  return vmDialog;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HINSTANCE __vectorcall RePag::DirectX::Hinstance(void)
{
  return hInstance;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HACCEL __vectorcall RePag::DirectX::Accelerator(void)
{
  return hAccelerator;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HANDLE __vectorcall RePag::DirectX::TimerQueue(void)
{
  return htqTimerQueue;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::Cursorform(const char* pcCursor)
{
  (pcCursor == NULL ? SetCursor(NULL) : SetCursor(LoadCursor(hInstance, pcCursor)));
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::EinfugenKurzTasten(const ACCEL* pacTasten, int iAnzahl)
{
  int iKurzTasten = CopyAcceleratorTable(hAccelerator, NULL, 0);
  ACCEL* vacTasten = (ACCEL*)malloc(sizeof(ACCEL) * (iKurzTasten + iAnzahl));
  CopyAcceleratorTable(hAccelerator, vacTasten, iKurzTasten);
  DestroyAcceleratorTable(hAccelerator);
  MemCopy(&vacTasten[iKurzTasten], pacTasten, sizeof(ACCEL) * iAnzahl);
  hAccelerator = CreateAcceleratorTable(vacTasten, iKurzTasten + iAnzahl);
  free(vacTasten);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall LoschWndKlasse(ATOM atWndKlasse)
{
  ATOM* patWndKlasse = (ATOM*)VMBlock(2); MemCopy(patWndKlasse, &atWndKlasse, 2);
  vthlWndKlassen->ThToEnd(patWndKlasse);
  SetEvent(heWndKlassen);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
DWORD WINAPI thLoschWndKlassen(void* pvParam)
{
  void* pvIterator;
  TryEnterCriticalSection(&thstWndKlassen.csWndKlasse);
  while(!((STTHWndKlassen*)pvParam)->bAbbruch){
    LeaveCriticalSection(&thstWndKlassen.csWndKlasse);
    pvIterator = vthlWndKlassen->ThIteratorToBegin_Lock();
    while(pvIterator){
      if(UnregisterClass(MAKEINTATOM(*((ATOM*)vthlWndKlassen->Element(pvIterator))), hInstance)) vthlWndKlassen->DeleteFirstElement(pvIterator, true);
    }
    vthlWndKlassen->ThIteratorEnd();
    WaitForSingleObject(heWndKlassen, INFINITE);
    TryEnterCriticalSection(&thstWndKlassen.csWndKlasse);
  }

  LeaveCriticalSection(&thstWndKlassen.csWndKlasse);
  DeleteCriticalSection(&thstWndKlassen.csWndKlasse);
  CloseHandle(heWndKlassen);
  vthlWndKlassen->ThDeleteList(false);
  VMFreiV(vthlWndKlassen);
  return NULL;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::CreateDeviceIndependentResources(_Out_ STDeviceResources* pstDeviceResources)
{
  HRESULT hr = S_OK;
  D2D1_FACTORY_OPTIONS options;
  ZeroMemory(&options, sizeof(D2D1_FACTORY_OPTIONS));

#if defined(_DEBUG)
  options.debugLevel = D2D1_DEBUG_LEVEL_INFORMATION;
#endif

  if(FAILED(hr = D2D1CreateFactory(D2D1_FACTORY_TYPE_SINGLE_THREADED, __uuidof(ID2D1Factory7), &options, (void**)&pstDeviceResources->ifd2d1Factory7))) goto Ende;

  if(FAILED(hr = DWriteCreateFactory(DWRITE_FACTORY_TYPE_SHARED, __uuidof(IDWriteFactory7), reinterpret_cast<IUnknown**>(&pstDeviceResources->ifdwriteFactory7)))) goto Ende;

  return hr;

Ende:
  SafeRelease(&pstDeviceResources->ifd2d1Factory7); SafeRelease(&pstDeviceResources->ifdwriteFactory7);
  return hr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HRESULT __vectorcall RePag::DirectX::CreateDeviceResources(_Out_ STDeviceResources* pstDeviceResources)
{
  HRESULT hr = S_OK;
  UINT creationFlags = D3D11_CREATE_DEVICE_BGRA_SUPPORT;

  D3D_FEATURE_LEVEL featureLevels[] = {D3D_FEATURE_LEVEL_11_1, D3D_FEATURE_LEVEL_11_0, D3D_FEATURE_LEVEL_10_1, D3D_FEATURE_LEVEL_10_0,
                                        D3D_FEATURE_LEVEL_9_3,  D3D_FEATURE_LEVEL_9_2,  D3D_FEATURE_LEVEL_9_1};

  ID3D11Device* ifD3D11Device = nullptr; ID3D11DeviceContext* ifD3D11Context = nullptr; D3D_FEATURE_LEVEL d3dFeatureLevel;
  if(FAILED(hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_HARDWARE, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels),
                                   D3D11_SDK_VERSION, &ifD3D11Device, &d3dFeatureLevel, &ifD3D11Context))){

    if(FAILED(hr = D3D11CreateDevice(nullptr, D3D_DRIVER_TYPE_WARP, 0, creationFlags, featureLevels, ARRAYSIZE(featureLevels),
                                     D3D11_SDK_VERSION, &ifD3D11Device, &d3dFeatureLevel, &ifD3D11Context))) goto Error;
  }
  ifD3D11Context->Release(); ifD3D11Context = nullptr;

  if(FAILED(hr = ifD3D11Device->QueryInterface(IID_PPV_ARGS(&pstDeviceResources->ifD311Device5)))) goto Error;
  ifD3D11Device->Release(); ifD3D11Device = nullptr;


#pragma warning(disable : 4996)
  pstDeviceResources->ifd2d1Factory7->GetDesktopDpi(&pstDeviceResources->dpiX, &pstDeviceResources->dpiY);

  IDXGIDevice4* ifDXGIDevice4;
  if(FAILED(hr = pstDeviceResources->ifD311Device5->QueryInterface(IID_PPV_ARGS(&ifDXGIDevice4)))) goto Error;

  if(FAILED(hr = ifDXGIDevice4->SetMaximumFrameLatency(1))) goto Error;

  IDXGIAdapter* ifDXGIAdapter;
  if(FAILED(hr = ifDXGIDevice4->GetAdapter(&ifDXGIAdapter))) goto Error;

  IDXGIAdapter4* ifDXGIAdapter4;
  if(FAILED(hr = ifDXGIAdapter->QueryInterface(IID_PPV_ARGS(&ifDXGIAdapter4)))) goto Error;
  ifDXGIAdapter->Release(); ifDXGIAdapter = nullptr;

  if(FAILED(hr = ifDXGIAdapter4->GetParent(IID_PPV_ARGS(&pstDeviceResources->ifDXGIFactory7)))) goto Error;
  ifDXGIAdapter4->Release(); ifDXGIAdapter4 = nullptr;

  if(FAILED(hr = pstDeviceResources->ifd2d1Factory7->CreateDevice(ifDXGIDevice4, &pstDeviceResources->ifD2D1Device6))) goto Error;
  ifDXGIDevice4->Release(); ifDXGIDevice4 = nullptr;

  return hr;

Error:
  SafeRelease(&ifD3D11Device); SafeRelease(&ifD3D11Context); 
  SafeRelease(&pstDeviceResources->ifD311Device5); SafeRelease(&pstDeviceResources->ifd2d1Factory7);
  SafeRelease(&pstDeviceResources->ifDXGIFactory7); SafeRelease(&ifDXGIAdapter); SafeRelease(&ifDXGIAdapter4);
  SafeRelease(&pstDeviceResources->ifD2D1Device6); SafeRelease(&ifDXGIDevice4);
  return hr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------