/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: OElementD2.cpp
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
#include "OElementD2.h"
//-----------------------------------------------------------------------------------------------------------------------------------------
#define _rcZeichnen_left eax
#define _rcZeichnen_top eax + 4
#define _rcZeichnen_right eax + 8
#define _rcZeichnen_bottom eax + 12
#define _stDIBSection_dsBmih_biBitCount edx + 38
#define _stDIBSection_dsBmih_biWidth edx + 28
#define _stDIBSection_dsBmih_biHeight edx + 32
#define _stDIBSection_dsBm_bmBits edx + 20
#define _rcEffektrand_links eax + rcEffektrand + 0 
#define _rcEffektrand_oben eax + rcEffektrand + 4
#define _rcEffektrand_rechts eax + rcEffektrand + 8
#define _rcEffektrand_unten eax + rcEffektrand + 12
#define _stHintergrundeffektfarbe eax + stHintergrundeffektfarbe

#define HGE_KEINHINTERGRUND 0
#define HGE_HINTERGRUND 1
#define HGE_BELEUCHTUNG_H 2
#define HGE_BELEUCHTUNG_V 4
#define HGE_RAND_H 8
#define HGE_RAND_V 16
//------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::COElementV(_In_ const VMEMORY vmSpeicherA, _In_ STDeviceResources* pstDeviceResourcesA)
{
	vmSpeicher = vmSpeicherA;
	pstDeviceResources = pstDeviceResourcesA;
	bHintergrund = true;
	hWndElement = nullptr;
	hbmBild = nullptr;
	htEffekt_Timer = nullptr;
	crfBackground = D2D1::ColorF(1.0f, 1.0f, 1.0f, 1.0f);
	ucHintergrundeffekt = HGE_HINTERGRUND;
	crfBackgroundEffect = D2D1::ColorF(0.0f, 0.0f, 0.0f, 0.0f);
	rcEffektrand.left = rcEffektrand.right = rcEffektrand.top = rcEffektrand.bottom = 0;
	ucRahmenbreite = 0;

	stThread.ucFunktion = stThread.dwAktiv = stThread.ucWarten = 0; stThread.dwHaupt = stThread.dwAktiv = GetCurrentThreadId();
	stThread.heElement[0] = CreateEvent(nullptr, false, true, nullptr); stThread.heElement[1] = CreateEvent(nullptr, false, false, nullptr);

	pfnWM_Size = nullptr;

	ifD2D1Context6 = nullptr;
	ifDXGIBackBuffer = nullptr;
	ifDXGISwapChain4 = nullptr;
	ifd2dBitmap1 = nullptr;

	dxgiPresent = {0};
}
//-----------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::COElement::COFreiV(void)
{
	if(hbmBild) DeleteObject(hbmBild);
	CloseHandle(stThread.heElement[0]); CloseHandle(stThread.heElement[1]);

	SafeRelease(&ifD2D1Context6);
	SafeRelease(&ifDXGIBackBuffer);
	SafeRelease(&ifDXGISwapChain4);
	SafeRelease(&ifd2dBitmap1);
	SafeRelease(&ifBackColor);

	return vmSpeicher;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::CreateWindowSizeDependentResources(void)
{
	DXGI_FORMAT dxgi_Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	if(ifDXGISwapChain4 != nullptr){

		ifD2D1Context6->SetTarget(nullptr);
		SafeRelease(&ifd2dBitmap1); ifd2dBitmap1 = nullptr;
		SafeRelease(&ifDXGIBackBuffer); ifDXGIBackBuffer = nullptr;

		ifDXGISwapChain4->ResizeBuffers(0, 0, 0, dxgi_Format, NULL);
	}
	else{
		pstDeviceResources->ifD2D1Device6->CreateDeviceContext(D2D1_DEVICE_CONTEXT_OPTIONS_NONE, &ifD2D1Context6);
		
		DXGI_SWAP_CHAIN_DESC1 swapChainDesc = {0};
		swapChainDesc.Width = 0;
		swapChainDesc.Height = 0;
		swapChainDesc.Format = dxgi_Format;
		swapChainDesc.Stereo = false;
		swapChainDesc.SampleDesc.Count = 1;
		swapChainDesc.SampleDesc.Quality = 0;
		swapChainDesc.BufferUsage = DXGI_USAGE_RENDER_TARGET_OUTPUT;
		swapChainDesc.BufferCount = 2;
		swapChainDesc.SwapEffect = DXGI_SWAP_EFFECT_FLIP_SEQUENTIAL;
		swapChainDesc.Flags = NULL;
		swapChainDesc.AlphaMode = DXGI_ALPHA_MODE_UNSPECIFIED; // DXGI_ALPHA_MODE_IGNORE;
		swapChainDesc.Scaling = DXGI_SCALING_STRETCH; // DXGI_SCALING_NONE;

		IDXGISwapChain1* ifDXGISwapChain1;
		pstDeviceResources->ifDXGIFactory7->CreateSwapChainForHwnd(pstDeviceResources->ifD311Device5, hWndElement, &swapChainDesc,
																															 nullptr, nullptr, &ifDXGISwapChain1);

		ifDXGISwapChain1->QueryInterface(IID_PPV_ARGS(&ifDXGISwapChain4));
		ifDXGISwapChain1->Release(); ifDXGISwapChain1 = nullptr;
	}

		D2D1_BITMAP_PROPERTIES1 bitmapProperties =
			D2D1::BitmapProperties1(D2D1_BITMAP_OPTIONS_TARGET | D2D1_BITMAP_OPTIONS_CANNOT_DRAW,
															D2D1::PixelFormat(dxgi_Format, D2D1_ALPHA_MODE_PREMULTIPLIED), pstDeviceResources->dpiX, pstDeviceResources->dpiY);

	ifDXGISwapChain4->GetBuffer(0, IID_PPV_ARGS(&ifDXGIBackBuffer));

	ifD2D1Context6->CreateBitmapFromDxgiSurface(ifDXGIBackBuffer, &bitmapProperties, &ifd2dBitmap1);

	ifD2D1Context6->SetTarget(ifd2dBitmap1);
	ifD2D1Context6->SetTransform(D2D1::Matrix3x2F::Identity());
	ifD2D1Context6->SetTextAntialiasMode(D2D1_TEXT_ANTIALIAS_MODE_DEFAULT);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::WM_Create_Element(HWND hWnd)
{
	hWndElement = hWnd;

	CreateWindowSizeDependentResources();

	RECT rcClient; GetClientRect(hWndElement, &rcClient);
	rclDirty.top = 0; rclDirty.left = 0; rclDirty.right = rcClient.right; rclDirty.bottom = rcClient.bottom;
	dxgiPresent.DirtyRectsCount = 1; dxgiPresent.pDirtyRects = &rclDirty;
	dxgiPresent.pScrollOffset = nullptr; dxgiPresent.pScrollRect = nullptr;

	ifD2D1Context6->CreateSolidColorBrush(crfBackground, &ifBackColor);	ifBackColor->SetColor(crfBackground);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::WM_Size_Element(HWND hWnd, LPARAM lParam)
{
	ThreadSafe_Begin();
	static UINT uiHeight = 0, uiWidth = 0;
	if(uiHeight != HIWORD(lParam) || uiWidth != LOWORD(lParam)){
		uiHeight = HIWORD(lParam); uiWidth = LOWORD(lParam);
		CreateWindowSizeDependentResources();
	}
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
HWND __vectorcall RePag::DirectX::COElement::HWND_Element(void)
{
	return hWndElement;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
HWND __vectorcall RePag::DirectX::COElement::HWND_Haupt(void)
{
	return GetParent(hWndElement);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Width(long& lWidthA)
{
	ThreadSafe_Begin();
	lWidthA = lWidth;
	ThreadSafe_End();
	return lWidthA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Height(long& lHeightA)
{
	ThreadSafe_Begin();
	lHeightA = lHeight;
	ThreadSafe_End();
	return lHeightA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Pos_X(long& lPos_x)
{
	ThreadSafe_Begin();
	lPos_x = ptPosition.x;
	ThreadSafe_End();
	return lPos_x;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Pos_Y(long& lPos_y)
{
	ThreadSafe_Begin();
	lPos_y = ptPosition.y;
	ThreadSafe_End();
	return lPos_y;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
POINT& __vectorcall RePag::DirectX::COElement::Position(POINT& ptPositionA)
{
	ThreadSafe_Begin();
	ptPositionA = ptPosition;
	ThreadSafe_End();
	return ptPositionA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
RECT& __vectorcall RePag::DirectX::COElement::Fenster(RECT& rcFenster)
{
	ThreadSafe_Begin();
	rcFenster.left = ptPosition.x;
	rcFenster.top = ptPosition.y;
	rcFenster.right = ptPosition.x + lWidth;
	rcFenster.bottom = ptPosition.y + lHeight;
	ThreadSafe_End();
	return rcFenster;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterPosition(long lPos_x, long lPos_y)
{
	ThreadSafe_Begin();
	ptPosition.x = lPos_x; ptPosition.y = lPos_y;

	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterPosition(POINT& ptPositionA)
{
	ThreadSafe_Begin();
	ptPosition = ptPositionA;

	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterGrosse(long lHeightA, long lWidthA)
{
	ThreadSafe_Begin();
	lHeight = lHeightA; lWidth = lWidthA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterHohe(long lHeightA)
{
	ThreadSafe_Begin();
	lHeight = lHeightA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterBreite(long lWidthA)
{
	ThreadSafe_Begin();
	lWidth = lWidthA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeuesFenster(long lHeightA, long lWidthA, long lPos_x, long lPos_y)
{
	ThreadSafe_Begin();
	lHeight = lHeightA; lWidth = lWidthA; ptPosition.x = lPos_x; ptPosition.y = lPos_y;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::AndernFensterGrosse(long lHeightA, long lWidthA)
{
	ThreadSafe_Begin();
	lHeight += lHeightA; lWidth += lWidthA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::AndernFensterPosition(long lPos_x, long lPos_y)
{
	ThreadSafe_Begin();
	ptPosition.x += lPos_x; ptPosition.y += lPos_y;

	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, IsWindowVisible(hWndElement));
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetzHintergrund(bool bMitHintergrund)
{
	if(!hWndElement) bHintergrund = bMitHintergrund;
	else{
		ThreadSafe_Begin();
		if(bHintergrund && !bMitHintergrund){ bHintergrund = bMitHintergrund; DeleteObject(hbmBild); hbmBild = nullptr; }
		else if(!bHintergrund && bMitHintergrund){
			bHintergrund = bMitHintergrund; RECT rcClient; GetClientRect(hWndElement, &rcClient);
			WM_Size_Element(hWndElement, MAKELPARAM(rcClient.right, rcClient.bottom));
			//UpdateFenster(nullptr, true, true);
		}
		ThreadSafe_End();
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetzVerfugbar(bool bVerfugbar)
{
	EnableWindow(hWndElement, bVerfugbar);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
bool __vectorcall RePag::DirectX::COElement::Verfugbar(void)
{
	return IsWindowEnabled(hWndElement);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Loschen(void)
{
	DestroyWindow(hWndElement);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
//void __vectorcall RePag::DirectX::COElement::UpdateFenster(RECT* prcFenster, bool bUpdateHintergrund, bool bUpdateHintergrundeffekt)
//{
//	if(hWndElement){
//		ThreadSafe_Begin();
//		if(bUpdateHintergrundeffekt && hbmBild) Hintergrund_Effekt(prcFenster);
//		if(bUpdateHintergrund && hbmBild) bHintergrund = true;
//		InvalidateRect(hWndElement, prcFenster, false); UpdateWindow(hWndElement);
//		ThreadSafe_End();
//	}
//}
////-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetBackgroundColor(_In_ unsigned char ucRot, _In_ unsigned char ucGrun, _In_ unsigned char ucBlau, _In_ unsigned char ucAlpha)
{
	ThreadSafe_Begin();
	crfBackground =	D2D1::ColorF(RGB(ucBlau, ucGrun, ucRot), ucAlpha);
	if(ifBackColor) ifBackColor->SetColor(crfBackground);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetBackgroundColor(_In_ D2D1_COLOR_F& crfBackgroundA)
{
	ThreadSafe_Begin();
	crfBackground = crfBackgroundA;
	if(ifBackColor) ifBackColor->SetColor(crfBackground);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::GetBackgroundColor(_In_ D2D1_COLOR_F& crfBackgroundA)
{
	ThreadSafe_Begin();
	crfBackgroundA = crfBackground;
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Hintergrundeffekt(unsigned char ucEffekt)
{
	ThreadSafe_Begin();
	ucHintergrundeffekt = ucEffekt;
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Effekt_Timer(bool bStarten, unsigned long ulInterval_msek, void CALLBACK fnEffekt_Timer(COElement*, bool bTimerOrWaitFired))
{
	ThreadSafe_Begin();
	if(bStarten && !htEffekt_Timer) CreateTimerQueueTimer(&htEffekt_Timer, htqTimerQueue, (WAITORTIMERCALLBACK)fnEffekt_Timer, this, 0, ulInterval_msek, WT_EXECUTEINPERSISTENTTHREAD);
	else if(!bStarten && htEffekt_Timer){ DeleteTimerQueueTimer(htqTimerQueue, htEffekt_Timer, INVALID_HANDLE_VALUE); htEffekt_Timer = nullptr; }
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
inline void __vectorcall RePag::DirectX::COElement::ThreadSafe_Begin(void)
{
	WaitForSingleObjectEx(stThread.heElement[0], INFINITE, false);
	DWORD dwThreadID = GetCurrentThreadId();
	if(!stThread.ucFunktion) stThread.dwAktiv = dwThreadID;
	else if(stThread.dwAktiv != dwThreadID){
		if(stThread.dwHaupt != dwThreadID || stThread.dwHaupt == stThread.dwAktiv){
			stThread.ucWarten++;
			SetEvent(stThread.heElement[0]);
			WaitForMultipleObjectsEx(2, stThread.heElement, true, INFINITE, false);
			stThread.dwAktiv = dwThreadID;
			stThread.ucWarten--;
		}
	}
	stThread.ucFunktion++; SetEvent(stThread.heElement[0]);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
inline void __vectorcall RePag::DirectX::COElement::ThreadSafe_End(void)
{
	WaitForSingleObjectEx(stThread.heElement[0], INFINITE, false);
	if(!--stThread.ucFunktion && stThread.ucWarten)	SetEvent(stThread.heElement[1]);
	SetEvent(stThread.heElement[0]);
}
//-----------------------------------------------------------------------------------------------------------------------------------------