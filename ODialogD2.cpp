/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: ODialogD2.cpp
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
#include "ODialogD2.h"
//-----------------------------------------------------------------------------------------------------------------------------------------
#define _Dialog ((CODialog*)((LPCREATESTRUCT)lParam)->lpCreateParams)
#define DLG_LINKS 1
#define DLG_RECHTS 2
#define DLG_OBEN 4
#define DLG_UNTEN 8
#define DLG_MITTEVERTICAL 16
#define DLG_MITTEHORIZONTAL 32
#define DLG_MINIMAL 64
#define DLG_MAXIMAL 128
#define IDE_DLG_SICHTBAR 0xFFFF
#define IDE_DLG_WM_QUIT 0xFFFE
//-----------------------------------------------------------------------------------------------------------------------------------------
#define _WM_Command ((STTHWM_CommandDlg*)pvParam)
//-----------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
	namespace DirectX
	{
		HWND hWndErstes = nullptr;
		//-----------------------------------------------------------------------------------------------------------------------------------------
		typedef struct STTHWM_CommandDlg
		{
			HANDLE hThread;
			DWORD dwThreadId;
			HWND hWndDlg;
			unsigned int uiMessage;
			WPARAM wParam;
			LPARAM lParam;
			CODialog* pDialog;
		} STTHWM_CommandDlg;
		//-----------------------------------------------------------------------------------------------------------------------------------------
		DWORD WINAPI thWM_Command_Dialog(void* pvParam)
		{
			_WM_Command->pDialog->ThreadSicher_Anfang();
			_WM_Command->pDialog->pfnWM_Command(_WM_Command->hWndDlg, _WM_Command->uiMessage, _WM_Command->wParam, _WM_Command->lParam);
			_WM_Command->pDialog->ThreadSicher_Ende();

			void* pvThreadId = vthlThreadId->ThIteratorToBegin_Lock(); void* pvLoschen = nullptr;
			while(pvThreadId){
				if(((STThreadId*)vthlThreadId->Element(pvThreadId))->dwThreadId == _WM_Command->dwThreadId){ vthlThreadId->DeleteElement(pvThreadId, pvLoschen, true); break; }
				vthlThreadId->NextElement(pvThreadId, pvLoschen);
			}
			vthlThreadId->ThIteratorEnd();
			DeleteEvent(_WM_Command->hThread);
			VMFrei(pvParam);
			return NULL;
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------
		LRESULT CALLBACK WndProc_Dialog(HWND hWnd, unsigned int uiMessage, WPARAM wParam, LPARAM lParam)
		{
			CODialog* pDialog;
			switch(uiMessage){
				case WM_CREATE	:	_Dialog->ptPosition.x = ((LPCREATESTRUCT)lParam)->x; _Dialog->ptPosition.y = ((LPCREATESTRUCT)lParam)->y;
													_Dialog->pfnWndProc_Dialog = (WNDPROC)SetWindowLongPtr(hWnd, GWLP_WNDPROC, (LONG_PTR)_Dialog->pfnWndProc_DLGDialog);
													_Dialog->WM_Create_Element(hWnd);
													_Dialog->WM_Create();
													SendMessage(hWnd, WM_CREATE, wParam, lParam);
													return NULL;
				case WM_SIZE		:	pDialog = (CODialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(pDialog){
														pDialog->ThreadSicher_Anfang();
														pDialog->WM_Size_Element(hWnd, lParam);
														if(pDialog->pfnWM_Size) pDialog->pfnWM_Size(pDialog, wParam, lParam);
														pDialog->ThreadSicher_Ende();
													}
													else return DefWindowProc(hWnd, uiMessage, wParam, lParam);
													return NULL;
				//case WM_MOVE		:	pDialog = (CODialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				//									if(pDialog){
				//										pDialog->ThreadSicher_Anfang();
				//										pDialog->WM_Move_Dialog();
				//										if(pDialog->pfnWM_Move) pDialog->pfnWM_Move(pDialog, lParam);
				//										pDialog->ThreadSicher_Ende();
				//									}
				//									else return DefWindowProc(hWnd, uiMessage, wParam, lParam);
				//									return NULL;
				case WM_COMMAND:	pDialog = (CODialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(pDialog->pfnWM_Command){ pDialog->WM_Command_Dialog(uiMessage, wParam, lParam); return NULL; }
													else break;

				//case WM_PAINT		:	pDialog = (CODialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
				//									if(pDialog)	pDialog->WM_Paint_Dialog();
				//									return NULL;
				case WM_CLOSE		:	DestroyWindow(hWnd);
													return NULL;
				case WM_DESTROY: if(hWnd == hWndErstes) PostQuitMessage(0);
			}
			return DefWindowProc(hWnd, uiMessage, wParam, lParam);
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------
	}
}
void __vectorcall RePag::DirectX::CODialog::CODialogV(_In_ const VMEMORY vmSpeicher, _In_z_ const char* pcFensterName, _In_ bool bModalA,
																									_In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM),
																									_In_ STDeviceResources* pstDeviceResourcesA)
{
	COElementV(vmSpeicher, pstDeviceResourcesA);

	vthlDialoge->ThToEnd(this);

	vstFensterBau = (STFensterBau*)VMBlock(sizeof(STFensterBau));
	vstFensterBau->dwFensterStil = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN;
	vstFensterBau->dwErweitertStil = WS_EX_TOOLWINDOW;
	vstFensterBau->asName = pcFensterName;

	vstFensterBau->wndKlasse.cbSize = sizeof(WNDCLASSEX);
	vstFensterBau->wndKlasse.style = CS_OWNDC;
	vstFensterBau->wndKlasse.cbClsExtra = 0;
	vstFensterBau->wndKlasse.cbWndExtra = 16;
	vstFensterBau->wndKlasse.hInstance = hInstance;
	vstFensterBau->wndKlasse.lpfnWndProc = WndProc_Dialog;
	vstFensterBau->wndKlasse.hCursor = LoadCursor(nullptr, IDC_ARROW);
	vstFensterBau->wndKlasse.hbrBackground = nullptr;
	vstFensterBau->wndKlasse.lpszMenuName = nullptr;
	vstFensterBau->wndKlasse.hIcon = nullptr;
	vstFensterBau->wndKlasse.hIconSm = nullptr;

	pfnWndProc_DLGDialog = WndProc_DLG;
	bModal = bModalA;

	lfSchrift.lfHeight = -13;
	lfSchrift.lfWidth = 0;
	lfSchrift.lfFaceName[0] = '\0';
	lfSchrift.lfEscapement = GM_COMPATIBLE;
	lfSchrift.lfOrientation = GM_COMPATIBLE;
	lfSchrift.lfItalic = FALSE;
	lfSchrift.lfUnderline = FALSE;
	lfSchrift.lfStrikeOut = FALSE;
	lfSchrift.lfWeight = FW_DONTCARE;
	lfSchrift.lfCharSet = DEFAULT_CHARSET;
	lfSchrift.lfOutPrecision = OUT_DEVICE_PRECIS;
	lfSchrift.lfClipPrecision = CLIP_DEFAULT_PRECIS;
	lfSchrift.lfQuality = DEFAULT_QUALITY;
	lfSchrift.lfPitchAndFamily = DEFAULT_PITCH | FF_DONTCARE;

	pfnWM_Paint = nullptr;
	pfnWM_Move = nullptr;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::CODialogV(_In_z_ const char* pcFensterName, _In_ bool bModalA,
																									_In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM),
																									_In_ STDeviceResources* pstDeviceResourcesA)
{
	CODialogV(vmDialog, pcFensterName, bModalA, WndProc_DLG, pstDeviceResourcesA);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::CODialog::COFreiV(void)
{
	if(htEffekt_Timer) DeleteTimerQueueTimer(htqTimerQueue, htEffekt_Timer, INVALID_HANDLE_VALUE);
	LoschWndKlasse(GetClassWord(hWndElement, GCW_ATOM));

	void* pvDialoge = vthlDialoge->ThIteratorToBegin_Lock(); void* pvLoschen = nullptr;
	while(pvDialoge){
		if(vthlDialoge->Element(pvDialoge) == this){ vthlDialoge->DeleteElement(pvDialoge, pvLoschen, false); break; }
		vthlDialoge->NextElement(pvDialoge, pvLoschen);
	}
	vthlDialoge->ThIteratorEnd();

 return ((COElement*)this)->COFreiV();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::WM_Create(void)
{
	ifD2D1Context6->BeginDraw();
	ifD2D1Context6->Clear(crfBackground);
	ifD2D1Context6->EndDraw();
	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
//void __vectorcall RePag::DirectX::CODialog::WM_Move_Dialog(void)
//{
//	if(hWndElement){ RECT rcFenster; GetWindowRect(hWndElement, &rcFenster); ptPosition.x = rcFenster.left; ptPosition.y = rcFenster.top; }
//}
////-----------------------------------------------------------------------------------------------------------------------------------------
//void __vectorcall RePag::DirectX::CODialog::WM_Paint_Dialog(void)
//{
//	ThreadSicher_Anfang();
//	PAINTSTRUCT stPaint;
//	BeginPaint(hWndElement, &stPaint);
//	if(pfnWM_Paint) pfnWM_Paint(this, stPaint);
//
//	ifD2D1Context6->BeginDraw();
//	ifD2D1Context6->Clear(stBackColor);
//	ifD2D1Context6->EndDraw();
//	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
//
//	EndPaint(hWndElement, &stPaint);
//	ThreadSicher_Ende();
//}
////-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::WM_Command_Dialog(unsigned int uiMessage, WPARAM wParam, LPARAM lParam)
{
	STTHWM_CommandDlg* stthWM_Command = (STTHWM_CommandDlg*)VMBlock(sizeof(STTHWM_CommandDlg));
	stthWM_Command->hWndDlg = hWndElement;	stthWM_Command->pDialog = this; stthWM_Command->uiMessage = uiMessage; stthWM_Command->wParam = wParam;	stthWM_Command->lParam = lParam;
	stthWM_Command->hThread = CreateThread(nullptr, 0, thWM_Command_Dialog, stthWM_Command, CREATE_SUSPENDED, &stthWM_Command->dwThreadId);

	STThreadId* stThreadId = (STThreadId*)VMBlock(vmDialog, sizeof(STThreadId));
	stThreadId->pElement = this; stThreadId->dwThreadId = stthWM_Command->dwThreadId; vthlThreadId->ThToEnd(stThreadId);

	if(stthWM_Command->hThread){ SetThreadPriority(stthWM_Command->hThread, THREAD_PRIORITY_NORMAL); ResumeThread(stthWM_Command->hThread); }
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::FensterStil(DWORD dwFensterStilA)
{
	if(vstFensterBau) vstFensterBau->dwFensterStil = dwFensterStilA;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::ErweitertStil(DWORD dwErweitertStilA)
{
	if(vstFensterBau) vstFensterBau->dwErweitertStil = dwErweitertStilA;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::FensterTitel(const char* pcFensterTitel)
{
	if(vstFensterBau){
		BYTE ucBytes_Titel = (BYTE)StrLength((char*)pcFensterTitel);
		vstFensterBau->vbTitel = VMBlock(ucBytes_Titel + 1); vstFensterBau->vbTitel[ucBytes_Titel] = 0;
		MemCopy(vstFensterBau->vbTitel, pcFensterTitel, ucBytes_Titel);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::ErstellFenster(HWND hWndHaupt, long lHeightA, long lWidthA, long lPos_x, long lPos_y)
{
	if(vstFensterBau){
		vstFensterBau->wndKlasse.lpszClassName = vstFensterBau->asName.c_Str();
		while(!RegisterClassEx(&vstFensterBau->wndKlasse)){
			while(FindWindow(vstFensterBau->wndKlasse.lpszClassName, nullptr)){
				vstFensterBau->asName += "A";
				vstFensterBau->wndKlasse.lpszClassName = vstFensterBau->asName.c_Str();
			}
		}
		lHeight = lHeightA; lWidth = lWidthA; ptPosition.x = lPos_x; ptPosition.y = lPos_y;

		hWndElement = CreateWindowEx(vstFensterBau->dwErweitertStil, vstFensterBau->asName.c_Str(), vstFensterBau->asName.c_Str(), vstFensterBau->dwFensterStil,
																 ptPosition.x, ptPosition.y, lWidth, lHeight, hWndHaupt, nullptr, hInstance, this);

		if(!hWndErstes && !hWndHaupt) hWndErstes = hWndElement;
		if(hWndElement){
			SetWindowText(hWndElement, vstFensterBau->vbTitel); SetWindowLongPtr(hWndElement, GWLP_USERDATA, (LONG_PTR)this);
			RECT rcClient; GetClientRect(hWndElement, &rcClient); lWidth = rcClient.right; lHeight = rcClient.bottom;
			VMFrei(vstFensterBau->vbTitel); vstFensterBau->asName.~COStringA(); VMFrei(vstFensterBau); vstFensterBau = nullptr;
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------
long __vectorcall RePag::DirectX::CODialog::SetzSichtbar(bool bSichtbar, unsigned char ucAusrichtung, long lRuckgabeA)
{
	if(bSichtbar){
		int iShow = SW_SHOWNORMAL;
		if(ucAusrichtung == DLG_MAXIMAL) iShow = SW_SHOWMAXIMIZED;
		else if(ucAusrichtung == DLG_MINIMAL) iShow = SW_MINIMIZE;
		else if(ucAusrichtung){
			RECT rcRect; POINT ptDialog; long lTemp;
			GetWindowRect(GetParent(hWndElement), &rcRect);
			ptDialog.x = rcRect.left; ptDialog.y = rcRect.top;
			if(ucAusrichtung & DLG_LINKS) ptDialog.x = rcRect.left;
			if(ucAusrichtung & DLG_RECHTS) ptDialog.x = rcRect.right - Width(lTemp);
			if(ucAusrichtung & DLG_OBEN) ptDialog.y = rcRect.top;
			if(ucAusrichtung & DLG_UNTEN) ptDialog.y = rcRect.bottom - Height(lTemp);
			if(ucAusrichtung & DLG_MITTEVERTICAL) ptDialog.x = rcRect.left + (rcRect.right - rcRect.left) / 2 - Width(lTemp) / 2;
			if(ucAusrichtung & DLG_MITTEHORIZONTAL) ptDialog.y = rcRect.top + (rcRect.bottom - rcRect.top) / 2 - Height(lTemp) / 2;
			NeueFensterPosition(ptDialog);
		}

		if(bModal){
			void* pvDialoge = vthlDialoge->ThIteratorToBegin();
			while(pvDialoge){
				EnableWindow(((CODialog*)vthlDialoge->Element(pvDialoge))->HWND_Element(), false);
				ValidateRect(((CODialog*)vthlDialoge->Element(pvDialoge))->HWND_Element(), nullptr);
				vthlDialoge->NextElement(pvDialoge);
			}
			vthlDialoge->ThIteratorEnd();
			EnableWindow(hWndElement, true);
		}
		ShowWindow(hWndElement, iShow);

		MSG msg; HACCEL hAccelerator = Accelerator(); dwThreadID = GetCurrentThreadId();
		if(hWndErstes == hWndElement){
			PostMessage(hWndElement, WM_COMMAND, IDE_DLG_SICHTBAR, NULL);
			while(GetMessage(&msg, nullptr, NULL, NULL)){ if(!TranslateAccelerator(msg.hwnd, hAccelerator, &msg)){ TranslateMessage(&msg); DispatchMessage(&msg); } }
		}
		else{
			void* pvThreadId = vthlThreadId->ThIteratorToBegin();  COElement* pElement = nullptr;
			while(pvThreadId){
				if(((STThreadId*)vthlThreadId->Element(pvThreadId))->dwThreadId == dwThreadID){ pElement = ((STThreadId*)vthlThreadId->Element(pvThreadId))->pElement; break; }
				vthlThreadId->NextElement(pvThreadId);
			}
			vthlThreadId->ThIteratorEnd();
			if(pElement) ((CODialog*)pElement)->ThreadSicher_Ende();
			PostMessage(hWndElement, WM_COMMAND, IDE_DLG_SICHTBAR, NULL);
			while(GetMessage(&msg, nullptr, NULL, NULL)){
				if(msg.message == WM_COMMAND) WM_Command_Dialog(WM_COMMAND, msg.wParam, msg.lParam);
				else{ if(!TranslateAccelerator(msg.hwnd, hAccelerator, &msg)){ TranslateMessage(&msg); DispatchMessage(&msg); } }
			}
			if(pElement) ((CODialog*)pElement)->ThreadSicher_Anfang();
		}
		if(bModal){
			void* pvDialoge = vthlDialoge->ThIteratorToBegin();
			while(pvDialoge){ EnableWindow(((CODialog*)vthlDialoge->Element(pvDialoge))->HWND_Element(), true); vthlDialoge->NextElement(pvDialoge); }
			vthlDialoge->ThIteratorEnd();
		}
		ShowWindow(hWndElement, SW_HIDE);
	}
	else{ lRuckgabe = lRuckgabeA; PostThreadMessage(dwThreadID, WM_QUIT, NULL, NULL); }
	return lRuckgabe;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::Schliessen(void)
{
	PostMessage(hWndElement, WM_CLOSE, NULL, NULL);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::Icon(long lIcon)
{
	if(hWndElement) SetClassLongPtr(hWndElement, GCLP_HICON, lIcon);
	else vstFensterBau->wndKlasse.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(lIcon));
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::IconSm(long lIconSm)
{
	if(hWndElement) SetClassLongPtr(hWndElement, GCLP_HICONSM, lIconSm);
	else vstFensterBau->wndKlasse.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(lIconSm));
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::Menu(long lMenu)
{
	if(hWndElement) SetClassLongPtr(hWndElement, GCLP_MENUNAME, lMenu);
	else vstFensterBau->wndKlasse.lpszMenuName = MAKEINTRESOURCE(lMenu);
}
//---------------------------------------------------------------------------------------------------------------------------------------
