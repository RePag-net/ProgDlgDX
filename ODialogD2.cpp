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
constexpr BYTE DLG_LEFT = 1;
constexpr BYTE DLG_RIGHT = 2;
constexpr BYTE DLG_TOP = 4;
constexpr BYTE DLG_BOTTOM = 8;
constexpr BYTE DLG_CENTERVERTICAL = 16;
constexpr BYTE DLG_CENTERHORIZONTAL = 32;
constexpr BYTE DLG_MINIMAL = 64;
constexpr BYTE DLG_MAXIMAL = 128;
constexpr BYTE IDE_DLG_VISIBLE = 0xFFFF;
constexpr BYTE IDE_DLG_WM_QUIT = 0xFFFE;
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
			_WM_Command->pDialog->ThreadSafe_Begin();
			_WM_Command->pDialog->pfnWM_Command(_WM_Command->hWndDlg, _WM_Command->uiMessage, _WM_Command->wParam, _WM_Command->lParam);
			_WM_Command->pDialog->ThreadSafe_End();

			void* pvThreadId = vthlThreadId->ThIteratorToBegin_Lock(); void* pvDelete = nullptr;
			while(pvThreadId){
				if(((STThreadId*)vthlThreadId->Element(pvThreadId))->dwThreadId == _WM_Command->dwThreadId){ vthlThreadId->DeleteElement(pvThreadId, pvDelete, true); break; }
				vthlThreadId->NextElement(pvThreadId, pvDelete);
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
														pDialog->ThreadSafe_Begin();
														pDialog->WM_Size_Element(hWnd, lParam);
														if(pDialog->pfnWM_Size) pDialog->pfnWM_Size(pDialog, wParam, lParam);
														pDialog->ThreadSafe_End();
													}
													else return DefWindowProc(hWnd, uiMessage, wParam, lParam);
													return NULL;
				case WM_MOVE		:	pDialog = (CODialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(pDialog){
														pDialog->ThreadSafe_Begin();
														pDialog->WM_Move_Dialog();
														if(pDialog->pfnWM_Move) pDialog->pfnWM_Move(pDialog, lParam);
														pDialog->ThreadSafe_End();
													}
													else return DefWindowProc(hWnd, uiMessage, wParam, lParam);
													return NULL;
				case WM_COMMAND:	pDialog = (CODialog*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
													if(pDialog->pfnWM_Command){ pDialog->WM_Command_Dialog(uiMessage, wParam, lParam); return NULL; }
													else break;
				case WM_CLOSE		:	DestroyWindow(hWnd);
													return NULL;
				case WM_DESTROY: if(hWnd == hWndErstes) PostQuitMessage(0);
			}
			return DefWindowProc(hWnd, uiMessage, wParam, lParam);
		}
		//-----------------------------------------------------------------------------------------------------------------------------------------
	}
}
void __vectorcall RePag::DirectX::CODialog::CODialogV(_In_ const VMEMORY vmMemory, _In_z_ const char* pcWindowName, _In_ bool bModalA,
																									_In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM),
																									_In_ STDeviceResources* pstDeviceResourcesA)
{
	COElementV(vmMemory, pstDeviceResourcesA);

	vthlDialoge->ThToEnd(this);

	vstWindowConstraction = (STWindowConstraction*)VMBlock(sizeof(STWindowConstraction));
	vstWindowConstraction->dwWindowStyle = WS_POPUP | WS_CAPTION | WS_SYSMENU | WS_VISIBLE | WS_CLIPCHILDREN;
	vstWindowConstraction->dwExtendStyle = WS_EX_TOOLWINDOW;
	vstWindowConstraction->asName = pcWindowName;

	vstWindowConstraction->wndClass.cbSize = sizeof(WNDCLASSEX);
	vstWindowConstraction->wndClass.style = CS_OWNDC;
	vstWindowConstraction->wndClass.cbClsExtra = 0;
	vstWindowConstraction->wndClass.cbWndExtra = 16;
	vstWindowConstraction->wndClass.hInstance = hInstance;
	vstWindowConstraction->wndClass.lpfnWndProc = WndProc_Dialog;
	vstWindowConstraction->wndClass.hCursor = LoadCursor(nullptr, IDC_ARROW);
	vstWindowConstraction->wndClass.hbrBackground = nullptr;
	vstWindowConstraction->wndClass.lpszMenuName = nullptr;
	vstWindowConstraction->wndClass.hIcon = nullptr;
	vstWindowConstraction->wndClass.hIconSm = nullptr;

	pfnWndProc_DLGDialog = WndProc_DLG;
	bModal = bModalA;

	pfnWM_Move = nullptr;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::CODialogV(_In_z_ const char* pcWindowName, _In_ bool bModalA,
																									_In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM),
																									_In_ STDeviceResources* pstDeviceResourcesA)
{
	CODialogV(vmDialog, pcWindowName, bModalA, WndProc_DLG, pstDeviceResourcesA);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::CODialog::COFreiV(void)
{
	if(htEffect_Timer) DeleteTimerQueueTimer(htqTimerQueue, htEffect_Timer, INVALID_HANDLE_VALUE);
	DeleteWndClass(GetClassWord(hWndElement, GCW_ATOM));

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
void __vectorcall RePag::DirectX::CODialog::WM_Move_Dialog(void)
{
	if(hWndElement){ RECT rcWindow; GetWindowRect(hWndElement, &rcWindow); ptPosition.x = rcWindow.left; ptPosition.y = rcWindow.top; }
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::WM_Command_Dialog(_In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	STTHWM_CommandDlg* stthWM_Command = (STTHWM_CommandDlg*)VMBlock(sizeof(STTHWM_CommandDlg));
	stthWM_Command->hWndDlg = hWndElement;	stthWM_Command->pDialog = this; stthWM_Command->uiMessage = uiMessage; stthWM_Command->wParam = wParam;	stthWM_Command->lParam = lParam;
	stthWM_Command->hThread = CreateThread(nullptr, 0, thWM_Command_Dialog, stthWM_Command, CREATE_SUSPENDED, &stthWM_Command->dwThreadId);

	STThreadId* stThreadId = (STThreadId*)VMBlock(vmDialog, sizeof(STThreadId));
	stThreadId->pElement = this; stThreadId->dwThreadId = stthWM_Command->dwThreadId; vthlThreadId->ThToEnd(stThreadId);

	if(stthWM_Command->hThread){ SetThreadPriority(stthWM_Command->hThread, THREAD_PRIORITY_NORMAL); ResumeThread(stthWM_Command->hThread); }
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::WindowStyle(_In_ DWORD dwWindowStyleA)
{
	if(vstWindowConstraction) vstWindowConstraction->dwWindowStyle = dwWindowStyleA;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::ExtendStyle(_In_ DWORD dwExtendStyleA)
{
	if(vstWindowConstraction) vstWindowConstraction->dwExtendStyle = dwExtendStyleA;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::WindowTitel(_In_ const char* pcWindowTitel)
{
	if(vstWindowConstraction){
		BYTE ucBytes_Titel = (BYTE)StrLength((char*)pcWindowTitel);
		vstWindowConstraction->vbTitel = VMBlock(ucBytes_Titel + 1); vstWindowConstraction->vbTitel[ucBytes_Titel] = 0;
		MemCopy(vstWindowConstraction->vbTitel, pcWindowTitel, ucBytes_Titel);
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::CreateWindowDialog(_In_ HWND hWndMain, _In_ long lHeightA, _In_ long lWidthA, _In_ long lPos_x, _In_ long lPos_y)
{
	if(vstWindowConstraction){
		vstWindowConstraction->wndClass.lpszClassName = vstWindowConstraction->asName.c_Str();
		while(!RegisterClassEx(&vstWindowConstraction->wndClass)){
			while(FindWindow(vstWindowConstraction->wndClass.lpszClassName, nullptr)){
				vstWindowConstraction->asName += "A";
				vstWindowConstraction->wndClass.lpszClassName = vstWindowConstraction->asName.c_Str();
			}
		}
		lHeight = lHeightA; lWidth = lWidthA; ptPosition.x = lPos_x; ptPosition.y = lPos_y;

		hWndElement = CreateWindowEx(vstWindowConstraction->dwExtendStyle, vstWindowConstraction->asName.c_Str(), vstWindowConstraction->asName.c_Str(),
																 vstWindowConstraction->dwWindowStyle, ptPosition.x, ptPosition.y, lWidth, lHeight, hWndMain, nullptr, hInstance, this);

		if(!hWndErstes && !hWndMain) hWndErstes = hWndElement;
		if(hWndElement){
			SetWindowText(hWndElement, vstWindowConstraction->vbTitel); SetWindowLongPtr(hWndElement, GWLP_USERDATA, (LONG_PTR)this);
			RECT rcClient; GetClientRect(hWndElement, &rcClient); lWidth = rcClient.right; lHeight = rcClient.bottom;
			VMFrei(vstWindowConstraction->vbTitel); vstWindowConstraction->asName.~COStringA(); VMFrei(vstWindowConstraction); vstWindowConstraction = nullptr;
		}
	}
}
//---------------------------------------------------------------------------------------------------------------------------------------
long __vectorcall RePag::DirectX::CODialog::SetVisible(_In_ bool bVisible, _In_ unsigned char ucAlignment, _In_ long lReturnA)
{
	if(bVisible){
		int iShow = SW_SHOWNORMAL;
		if(ucAlignment == DLG_MAXIMAL) iShow = SW_SHOWMAXIMIZED;
		else if(ucAlignment == DLG_MINIMAL) iShow = SW_MINIMIZE;
		else if(ucAlignment){
			RECT rcRect; POINT ptDialog; long lTemp;
			GetWindowRect(GetParent(hWndElement), &rcRect);
			ptDialog.x = rcRect.left; ptDialog.y = rcRect.top;
			if(ucAlignment & DLG_LEFT) ptDialog.x = rcRect.left;
			if(ucAlignment & DLG_RIGHT) ptDialog.x = rcRect.right - Width(lTemp);
			if(ucAlignment & DLG_TOP) ptDialog.y = rcRect.top;
			if(ucAlignment & DLG_BOTTOM) ptDialog.y = rcRect.bottom - Height(lTemp);
			if(ucAlignment & DLG_CENTERVERTICAL) ptDialog.x = rcRect.left + (rcRect.right - rcRect.left) / 2 - Width(lTemp) / 2;
			if(ucAlignment & DLG_CENTERHORIZONTAL) ptDialog.y = rcRect.top + (rcRect.bottom - rcRect.top) / 2 - Height(lTemp) / 2;
			NewWindowPosition(ptDialog);
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
			PostMessage(hWndElement, WM_COMMAND, IDE_DLG_VISIBLE, NULL);
			while(GetMessage(&msg, nullptr, NULL, NULL)){ if(!TranslateAccelerator(msg.hwnd, hAccelerator, &msg)){ TranslateMessage(&msg); DispatchMessage(&msg); } }
		}
		else{
			void* pvThreadId = vthlThreadId->ThIteratorToBegin();  COElement* pElement = nullptr;
			while(pvThreadId){
				if(((STThreadId*)vthlThreadId->Element(pvThreadId))->dwThreadId == dwThreadID){ pElement = ((STThreadId*)vthlThreadId->Element(pvThreadId))->pElement; break; }
				vthlThreadId->NextElement(pvThreadId);
			}
			vthlThreadId->ThIteratorEnd();
			if(pElement) ((CODialog*)pElement)->ThreadSafe_End();
			PostMessage(hWndElement, WM_COMMAND, IDE_DLG_VISIBLE, NULL);
			while(GetMessage(&msg, nullptr, NULL, NULL)){
				if(msg.message == WM_COMMAND) WM_Command_Dialog(WM_COMMAND, msg.wParam, msg.lParam);
				else{ if(!TranslateAccelerator(msg.hwnd, hAccelerator, &msg)){ TranslateMessage(&msg); DispatchMessage(&msg); } }
			}
			if(pElement) ((CODialog*)pElement)->ThreadSafe_Begin();
		}
		if(bModal){
			void* pvDialoge = vthlDialoge->ThIteratorToBegin();
			while(pvDialoge){ EnableWindow(((CODialog*)vthlDialoge->Element(pvDialoge))->HWND_Element(), true); vthlDialoge->NextElement(pvDialoge); }
			vthlDialoge->ThIteratorEnd();
		}
		ShowWindow(hWndElement, SW_HIDE);
	}
	else{ lReturn = lReturnA; PostThreadMessage(dwThreadID, WM_QUIT, NULL, NULL); }
	return lReturn;
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::Close(void)
{
	PostMessage(hWndElement, WM_CLOSE, NULL, NULL);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::Icon(_In_ long lIcon)
{
	if(hWndElement) SetClassLongPtr(hWndElement, GCLP_HICON, lIcon);
	else vstWindowConstraction->wndClass.hIcon = LoadIcon(hInstance, MAKEINTRESOURCE(lIcon));
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::IconSm(_In_ long lIconSm)
{
	if(hWndElement) SetClassLongPtr(hWndElement, GCLP_HICONSM, lIconSm);
	else vstWindowConstraction->wndClass.hIconSm = LoadIcon(hInstance, MAKEINTRESOURCE(lIconSm));
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::CODialog::Menu(_In_ long lMenu)
{
	if(hWndElement) SetClassLongPtr(hWndElement, GCLP_MENUNAME, lMenu);
	else vstWindowConstraction->wndClass.lpszMenuName = MAKEINTRESOURCE(lMenu);
}
//---------------------------------------------------------------------------------------------------------------------------------------
