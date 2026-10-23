/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: OGraphicGroupD2.cpp
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

#define _WM_Command ((STTHWM_CommandGG*)pvParam)
//-------------------------------------------------------------------------------------------------------------------------------------------
COGraphicGroup* __vectorcall RePag::DirectX::COGraphicGroupV(_In_ const char* pcWindowName, _In_ unsigned int uiIDElement,
																												 _In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicGroup* vGraphicGroup = (COGraphicGroup*)VMBlock(vmDialog, sizeof(COGraphicGroup));
	vGraphicGroup->COGraphicGroupV((BYTE*)vmDialog, pcWindowName, uiIDElement, pstDeviceResourcesA);
	return vGraphicGroup;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
COGraphicGroup* __vectorcall RePag::DirectX::COGraphicGroupV(_In_ const VMEMORY vmMemory, _In_ const char* pcWindowName, _In_ unsigned int uiIDElement,
																												 _In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicGroup* vGraphicGroup = (COGraphicGroup*)VMBlock(vmMemory, sizeof(COGraphicGroup));
	vGraphicGroup->COGraphicGroupV((BYTE*)vmMemory, pcWindowName, uiIDElement, pstDeviceResourcesA);
	return vGraphicGroup;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
	namespace DirectX
	{
		//---------------------------------------------------------------------------------------------------------------------------------------
		typedef struct STTHWM_CommandGG
		{
			HANDLE hThread;
			DWORD dwThreadId;
			HWND hWndGG;
			unsigned int uiMessage;
			WPARAM wParam;
			LPARAM lParam;
			COGraphicGroup* pGraphicGroup;
		} STTHWM_CommandGG;
		//---------------------------------------------------------------------------------------------------------------------------------------
		DWORD WINAPI thWM_Command_GraphicGroup(_In_ void* pvParam)
		{
			_WM_Command->pGraphicGroup->ThreadSafe_Begin();
			_WM_Command->pGraphicGroup->pfnWM_Command(_WM_Command->hWndGG, _WM_Command->uiMessage, _WM_Command->wParam, _WM_Command->lParam);
			_WM_Command->pGraphicGroup->ThreadSafe_End();

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
		//---------------------------------------------------------------------------------------------------------------------------------------
		LRESULT CALLBACK WndProc_GraphicGroup(_In_ HWND hWnd, _In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
		{
			COGraphicGroup* pGraphicGroup;
			switch(uiMessage){
				case WM_CREATE		: ((COGraphicGroup*)((LPCREATESTRUCT)lParam)->lpCreateParams)->WM_Create_Element(hWnd);
														((COGraphicGroup*)((LPCREATESTRUCT)lParam)->lpCreateParams)->WM_Create();
				case WM_SIZE			: pGraphicGroup = (COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
														if(pGraphicGroup) pGraphicGroup->WM_Size(lParam);
														return NULL;
				case WM_COMMAND		:	pGraphicGroup = (COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
														if(pGraphicGroup->pfnWM_Command){ pGraphicGroup->WM_Command_GraphicGroup(uiMessage, wParam, lParam); return NULL; }
														else PostMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);
														break;
				case WM_NCDESTROY	:	VMFreiV((COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA));
														return NULL;
			}
			return DefWindowProc(hWnd, uiMessage, wParam, lParam);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::COGraphicGroupV(_In_ const VMEMORY vmMemory, _In_ const char* pcWindowName, _In_ unsigned int uiDElementA,
																																	_In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicV(vmMemory, pcRePag_GraphicGroup, pcWindowName, uiDElementA, pstDeviceResourcesA);
	crfBackground = D2D1::ColorF(RGB(90, 90, 90), 0.0f);
	crfText = D2D1::ColorF(RGB(240, 240, 240), 1.0f);
	pstDeviceResources->ifdwriteFactory7->CreateTextFormat(L"Arial", NULL, DWRITE_FONT_WEIGHT_NORMAL, DWRITE_FONT_STYLE_NORMAL, DWRITE_FONT_STRETCH_NORMAL,
																												 13.0f, L"de-DE", &ifText);

	pfnWM_Command = nullptr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::COGraphicGroup::COFreiV(void)
{
	SafeRelease(&ifText); SafeRelease(&ifTextColor);
	return ((COElement*)this)->COFreiV();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::OnRender(void)
{
	ifD2D1Context6->BeginDraw();
	ifD2D1Context6->Clear(crfBackground);
	if(pfnOnRender) pfnOnRender(this);
	ifD2D1Context6->EndDraw();
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::OnPaint(void)
{
	ThreadSafe_Begin();
	OnRender();
	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
	ThreadSafe_End();
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::WM_Create(void)
{
	ifD2D1Context6->CreateSolidColorBrush(crfText, &ifTextColor);

	if(crfBackground.a == 0.0f) crfBackground.a = 1.0f;

	OnRender();
	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::WM_Size(LPARAM lParam)
{
	ThreadSafe_Begin();
	if(lHeight != HIWORD(lParam) || lWidth != LOWORD(lParam)){
		lHeight = HIWORD(lParam); lWidth = LOWORD(lParam);
		CreateWindowSizeDependentResources();
		OnRender();
		ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
	}
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::SetFont(STFont& stFont)
{
	ThreadSafe_Begin();
	SafeRelease(&ifText);
	pstDeviceResources->ifdwriteFactory7->CreateTextFormat(stFont.fontFamilyName, stFont.fontCollection, stFont.fontWeight, stFont.fontSytle,
																												 stFont.fontStretch, stFont.fontSize, stFont.localeName, &ifText);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::SetTextColor(_In_ unsigned char ucRed, _In_ unsigned char ucGreen, _In_ unsigned char ucBlue, _In_ float fAlpha)
{
	ThreadSafe_Begin();
	crfText = D2D1::ColorF(RGB(ucBlue, ucGreen, ucRed), fAlpha);
	if(ifTextColor) ifTextColor->SetColor(crfText);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::SetTextColor(_In_ D2D1_COLOR_F& crfTextA)
{
	ThreadSafe_Begin();
	crfText = crfTextA;
	if(ifTextColor) ifTextColor->SetColor(crfText);
	ThreadSafe_End();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::DrawText(_In_z_ const wchar_t* wcInhalt, _In_ size_t szBytes_Text, _In_ const D2D1_RECT_F& rcfText)
{
	ifD2D1Context6->DrawText(wcInhalt, (UINT32)szBytes_Text, ifText, rcfText, ifTextColor);
}
//---------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::WM_Command_GraphicGroup(_In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam)
{
	STTHWM_CommandGG* stthWM_Command = (STTHWM_CommandGG*)VMBlock(sizeof(STTHWM_CommandGG));
	stthWM_Command->hWndGG = hWndElement;	stthWM_Command->pGraphicGroup = this; stthWM_Command->uiMessage = uiMessage; stthWM_Command->wParam = wParam;	stthWM_Command->lParam = lParam;
	stthWM_Command->hThread = CreateThread(nullptr, 0, thWM_Command_GraphicGroup, stthWM_Command, CREATE_SUSPENDED, &stthWM_Command->dwThreadId);

	STThreadId* stThreadId = (STThreadId*)VMBlock(vmDialog, sizeof(STThreadId));
	stThreadId->pElement = this; stThreadId->dwThreadId = stthWM_Command->dwThreadId; vthlThreadId->ThToEnd(stThreadId);

	if(stthWM_Command->hThread){ SetThreadPriority(stthWM_Command->hThread, THREAD_PRIORITY_NORMAL); ResumeThread(stthWM_Command->hThread); }
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::CreteWindowInGroup(COGraphic* pGrafik, long lHeightA, long lWidthA, long lPos_x, long lPos_y)
{
	pGrafik->CreateWindowGraphic(this, hWndElement, lHeightA, lWidthA, lPos_x, lPos_y);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::SetVisible(bool bVisible)
{
	if(bVisible) ShowWindow(hWndElement, SW_SHOW);
	else ShowWindow(hWndElement, SW_HIDE);
}