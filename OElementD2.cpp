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
	stBackColor.r = stBackColor.b = stBackColor.g = stBackColor.a = 1.0f;
	ucHintergrundeffekt = HGE_HINTERGRUND;
	stBackColorEffect.r = stBackColorEffect.b = stBackColorEffect.g = stBackColorEffect.a = 0.0f;
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

	//ifTextFormat3 = nullptr;
	//ifTextLayout3 = nullptr;

	//ifpLightSlateGrayBrush = nullptr;
	//ifpCornflowerBlueBrush = nullptr;
	//ifpBlackBrush = nullptr;
	//ifpLinearGradientBrush = nullptr;
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

	//SafeRelease(&ifpLightSlateGrayBrush);
	//SafeRelease(&ifpCornflowerBlueBrush);
	//SafeRelease(&ifpBlackBrush);

	//SafeRelease(&ifpLinearGradientBrush);

	//SafeRelease(&ifTextFormat3);
	//SafeRelease(&ifTextLayout3);

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
//void __vectorcall RePag::DirectX::COElement::WM_Paint(void)
//{
//	ThreadSicher_Anfang();
//	ifDXGISwapChain4->Present1(0, NULL, &dxgiPresent);
//	ThreadSicher_Ende();
//	ValidateRect(hWndElement, NULL);
//}
////-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::WM_Create_Element(HWND hWnd)
{
	hWndElement = hWnd;

	CreateWindowSizeDependentResources();

	RECT rcClient; GetClientRect(hWndElement, &rcClient);
	rclDirty.top = 0; rclDirty.left = 0; rclDirty.right = rcClient.right; rclDirty.bottom = rcClient.bottom;
	dxgiPresent.DirtyRectsCount = 1; dxgiPresent.pDirtyRects = &rclDirty;
	dxgiPresent.pScrollOffset = nullptr; dxgiPresent.pScrollRect = nullptr;

	ifD2D1Context6->CreateSolidColorBrush(stBackColor, &ifBackColor);	ifBackColor->SetColor(stBackColor);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::WM_Size_Element(HWND hWnd, LPARAM lParam)
{
	ThreadSicher_Anfang();
	//RECT rcFenster; GetWindowRect(hWnd, &rcFenster); lHohe = rcFenster.bottom - rcFenster.top; lBreite = rcFenster.right - rcFenster.left;
	//if(bHintergrund){
	//	BITMAPINFOHEADER bmih;
	//	bmih.biSize = sizeof(BITMAPINFOHEADER);
	//	bmih.biPlanes = 1; bmih.biBitCount = 24; bmih.biCompression = BI_RGB;

	//	rcFenster.top = rcFenster.left = 0;
	//	rcFenster.right = bmih.biWidth = LOWORD(lParam);
	//	rcFenster.bottom = bmih.biHeight = HIWORD(lParam);

	//	if(hbmBild){
	//		if(ucHintergrundeffekt > HGE_HINTERGRUND){
	//			DIBSECTION stDIBSection; GetObject(hbmBild, sizeof(DIBSECTION), &stDIBSection);
	//			RECT rcClient; rcClient.bottom = bmih.biHeight; rcClient.right = bmih.biWidth;
	//			if(stDIBSection.dsBmih.biHeight != bmih.biHeight){
	//				rcClient.left = 0;
	//				if(stDIBSection.dsBmih.biHeight > bmih.biHeight) rcClient.top = bmih.biHeight - rcEffektrand.bottom;
	//				else rcClient.top = stDIBSection.dsBmih.biHeight - rcEffektrand.bottom;
	//				InvalidateRect(hWndElement, &rcClient, false);
	//			}

	//			if(stDIBSection.dsBmih.biWidth != bmih.biWidth){
	//				rcClient.top = 0;
	//				if(stDIBSection.dsBmih.biWidth > bmih.biWidth) rcClient.left = bmih.biWidth - rcEffektrand.right;
	//				else rcClient.left = stDIBSection.dsBmih.biWidth - rcEffektrand.right;
	//				InvalidateRect(hWndElement, &rcClient, false);
	//			}
	//		}
	//		DeleteObject(hbmBild);
	//	}
	//	hbmBild = CreateDIBSection(0, (BITMAPINFO*)&bmih, 0, nullptr, nullptr, NULL);
	//	Hintergrund_Effekt(&rcFenster);
	//}
	static UINT uiHeight = 0, uiWidth = 0;
	if(uiHeight != HIWORD(lParam) || uiWidth != LOWORD(lParam)){
		uiHeight = HIWORD(lParam); uiWidth = LOWORD(lParam);
		CreateWindowSizeDependentResources();
	}
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
//void __vectorcall RePag::DirectX::COElement::WM_Paint_Hintergrund(PAINTSTRUCT& stPaint)
//{
//	if(hbmBild){
//		if(!bHintergrund) bHintergrund = true;
//		else{
//			HDC hdcZeichnen = CreateCompatibleDC(stPaint.hdc);
//			SelectObject(hdcZeichnen, hbmBild);
//			BitBlt(stPaint.hdc, stPaint.rcPaint.left, stPaint.rcPaint.top, stPaint.rcPaint.right, stPaint.rcPaint.bottom, hdcZeichnen, stPaint.rcPaint.left, stPaint.rcPaint.top, SRCCOPY);
//			DeleteDC(hdcZeichnen);
//		}
//	}
//}
////-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Hintergrund_Effekt(RECT* prcZeichnen)
{
	if(ucHintergrundeffekt > HGE_HINTERGRUND){
		if(ucHintergrundeffekt & HGE_BELEUCHTUNG_H) Effekt_Beleuchtung_Horizontal();
		if(ucHintergrundeffekt & HGE_BELEUCHTUNG_V) Effekt_Beleuchtung_Vertical();
		if(ucHintergrundeffekt & HGE_RAND_H) Effekt_Rand_Horizontal();
		if(ucHintergrundeffekt & HGE_RAND_V) Effekt_Rand_Vertical();
		if(prcZeichnen){
			RECT rcClient; GetClientRect(hWndElement, &rcClient);
			if(prcZeichnen->top < rcEffektrand.top) prcZeichnen->top = rcEffektrand.top + 1;
			if(prcZeichnen->left < rcEffektrand.left) prcZeichnen->left = rcEffektrand.left;
			if(prcZeichnen->bottom > rcClient.bottom - rcEffektrand.bottom) prcZeichnen->bottom = rcClient.bottom - rcEffektrand.bottom;
			if(prcZeichnen->right > rcClient.right - rcEffektrand.right) prcZeichnen->right = rcClient.right - rcEffektrand.right;
		}
	}
	if(ucHintergrundeffekt & HGE_HINTERGRUND){
		if(prcZeichnen) Hintergrund(*prcZeichnen);
		else{
			RECT* prcClient = (RECT*)VMBlock(16); GetClientRect(hWndElement, prcClient);
			prcClient->top += rcEffektrand.top; prcClient->left += rcEffektrand.left;
			prcClient->bottom -= rcEffektrand.bottom; prcClient->right -= rcEffektrand.right;
			Hintergrund(*prcClient); VMFrei(prcClient);
		}
	}
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
long& __vectorcall RePag::DirectX::COElement::Breite(long& lBreiteA)
{
	ThreadSicher_Anfang();
	lBreiteA = lBreite;
	ThreadSicher_Ende();
	return lBreiteA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Hohe(long& lHoheA)
{
	ThreadSicher_Anfang();
	lHoheA = lHohe;
	ThreadSicher_Ende();
	return lHoheA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Pos_X(long& lPos_x)
{
	ThreadSicher_Anfang();
	lPos_x = ptPosition.x;
	ThreadSicher_Ende();
	return lPos_x;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Pos_Y(long& lPos_y)
{
	ThreadSicher_Anfang();
	lPos_y = ptPosition.y;
	ThreadSicher_Ende();
	return lPos_y;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
POINT& __vectorcall RePag::DirectX::COElement::Position(POINT& ptPositionA)
{
	ThreadSicher_Anfang();
	ptPositionA = ptPosition;
	ThreadSicher_Ende();
	return ptPositionA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
RECT& __vectorcall RePag::DirectX::COElement::Fenster(RECT& rcFenster)
{
	ThreadSicher_Anfang();
	rcFenster.left = ptPosition.x;
	rcFenster.top = ptPosition.y;
	rcFenster.right = ptPosition.x + lBreite;
	rcFenster.bottom = ptPosition.y + lHohe;
	ThreadSicher_Ende();
	return rcFenster;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterPosition(long lPos_x, long lPos_y)
{
	ThreadSicher_Anfang();
	ptPosition.x = lPos_x; ptPosition.y = lPos_y;

	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterPosition(POINT& ptPositionA)
{
	ThreadSicher_Anfang();
	ptPosition = ptPositionA;

	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterGrosse(long lHoheA, long lBreiteA)
{
	ThreadSicher_Anfang();
	lHohe = lHoheA; lBreite = lBreiteA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterHohe(long lHoheA)
{
	ThreadSicher_Anfang();
	lHohe = lHoheA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeueFensterBreite(long lBreiteA)
{
	ThreadSicher_Anfang();
	lBreite = lBreiteA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NeuesFenster(long lHoheA, long lBreiteA, long lPos_x, long lPos_y)
{
	ThreadSicher_Anfang();
	lHohe = lHoheA; lBreite = lBreiteA; ptPosition.x = lPos_x; ptPosition.y = lPos_y;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::AndernFensterGrosse(long lHoheA, long lBreiteA)
{
	ThreadSicher_Anfang();
	lHohe += lHoheA; lBreite += lBreiteA;

	InvalidateRect(hWndElement, nullptr, false);
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::AndernFensterPosition(long lPos_x, long lPos_y)
{
	ThreadSicher_Anfang();
	ptPosition.x += lPos_x; ptPosition.y += lPos_y;

	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lBreite, lHohe, IsWindowVisible(hWndElement));
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetzHintergrund(bool bMitHintergrund)
{
	if(!hWndElement) bHintergrund = bMitHintergrund;
	else{
		ThreadSicher_Anfang();
		if(bHintergrund && !bMitHintergrund){ bHintergrund = bMitHintergrund; DeleteObject(hbmBild); hbmBild = nullptr; }
		else if(!bHintergrund && bMitHintergrund){
			bHintergrund = bMitHintergrund; RECT rcClient; GetClientRect(hWndElement, &rcClient);
			WM_Size_Element(hWndElement, MAKELPARAM(rcClient.right, rcClient.bottom));
			UpdateFenster(nullptr, true, true);
		}
		ThreadSicher_Ende();
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
void __vectorcall RePag::DirectX::COElement::UpdateFenster(RECT* prcFenster, bool bUpdateHintergrund, bool bUpdateHintergrundeffekt)
{
	if(hWndElement){
		ThreadSicher_Anfang();
		if(bUpdateHintergrundeffekt && hbmBild) Hintergrund_Effekt(prcFenster);
		if(bUpdateHintergrund && hbmBild) bHintergrund = true;
		InvalidateRect(hWndElement, prcFenster, false); UpdateWindow(hWndElement);
		ThreadSicher_Ende();
	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::BackroundColor(_In_ unsigned char ucRot, _In_ unsigned char ucGrun, _In_ unsigned char ucBlau, _In_ unsigned char ucAlpha)
{
	ThreadSicher_Anfang();
	stBackColor.r = static_cast<float>(ucRot) / 255.0f; 
	stBackColor.g = static_cast<float>(ucGrun) / 255.0f;
	stBackColor.b = static_cast<float>(ucBlau) / 255.0f;
	stBackColor.a = static_cast<float>(ucAlpha) / 255.0f;
	if(ifBackColor) ifBackColor->SetColor(stBackColor);
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Hintergrundeffekt(unsigned char ucEffekt)
{
	ThreadSicher_Anfang();
	ucHintergrundeffekt = ucEffekt;
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Hintergrundeffektfarbe(unsigned char ucRot, unsigned char ucGrun, unsigned char ucBlau, unsigned char ucAlpha)
{
	ThreadSicher_Anfang();
	stBackColorEffect.r = static_cast<float>(ucRot) / 255.0f;
	stBackColorEffect.g = static_cast<float>(ucGrun) / 255.0f;
	stBackColorEffect.b = static_cast<float>(ucBlau) / 255.0f;
	stBackColorEffect.a = static_cast<float>(ucAlpha) / 255.0f;
	if(ifBackColor) ifBackColor->SetColor(stBackColor);
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Hintergrundeffektrand(long lOben, long lUnten, long lLinks, long lRechts)
{
	ThreadSicher_Anfang();
	(lOben < 0 ? rcEffektrand.top = 0 : rcEffektrand.top = lOben);
	(lUnten < 0 ? rcEffektrand.bottom = 0 : rcEffektrand.bottom = lUnten);
	(lLinks < 0 ? rcEffektrand.left = 0 : rcEffektrand.left = lLinks);
	(lRechts < 0 ? rcEffektrand.right = 0 : rcEffektrand.right = lRechts);
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Effekt_Timer(bool bStarten, unsigned long ulInterval_msek, void CALLBACK fnEffekt_Timer(COElement*, bool bTimerOrWaitFired))
{
	ThreadSicher_Anfang();
	if(bStarten && !htEffekt_Timer) CreateTimerQueueTimer(&htEffekt_Timer, htqTimerQueue, (WAITORTIMERCALLBACK)fnEffekt_Timer, this, 0, ulInterval_msek, WT_EXECUTEINPERSISTENTTHREAD);
	else if(!bStarten && htEffekt_Timer){ DeleteTimerQueueTimer(htqTimerQueue, htEffekt_Timer, INVALID_HANDLE_VALUE); htEffekt_Timer = nullptr; }
	ThreadSicher_Ende();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::Effekt_Beleuchtung_Vertical(void)
{
//	DIBSECTION stDIBSection; GetObject(hbmBild, sizeof(DIBSECTION), &stDIBSection);
//
//	COLORREF crFarbe, crHintergrundfarbe; long lx, ly, lz, lPixel, lRand = ((stDIBSection.dsBmih.biWidth * stDIBSection.dsBmih.biBitCount + 31) & ~31) >> 3;
//
//	for(ly = 0; ly < stDIBSection.dsBmih.biHeight; ly++){
//		crHintergrundfarbe = RGBA(stHintergrundeffektfarbe.ucRot + rcEffektrand.left / 3,
//														stHintergrundeffektfarbe.ucGrun + rcEffektrand.left / 3,
//														stHintergrundeffektfarbe.ucBlau + rcEffektrand.left / 3);
//
//		for(lx = 0; lx < rcEffektrand.left * 3; lx += 3){
//			crFarbe = crHintergrundfarbe - RGB(lx, lx, lx);
//			lPixel = lx + ly * lRand;
//			((BYTE*)stDIBSection.dsBm.bmBits)[lPixel] = GetBValue(crFarbe);
//			((BYTE*)stDIBSection.dsBm.bmBits)[++lPixel] = GetGValue(crFarbe);
//			((BYTE*)stDIBSection.dsBm.bmBits)[++lPixel] = GetRValue(crFarbe);
//		}
//
//		if(rcEffektrand.left) crHintergrundfarbe = crFarbe;
//		else crHintergrundfarbe = RGBA(stHintergrundeffektfarbe.ucRot - rcEffektrand.right * 3,
//																	stHintergrundeffektfarbe.ucGrun - rcEffektrand.right * 3,
//																	stHintergrundeffektfarbe.ucBlau - rcEffektrand.right * 3);
//
//		lz = 0;
//		for(lx = (stDIBSection.dsBmih.biWidth - rcEffektrand.right) * 3; lx < stDIBSection.dsBmih.biWidth * 3; lx += 3){
//			crFarbe = crHintergrundfarbe + RGB(lz, lz, lz); lz += 3;
//			lPixel = lx + ly * lRand;
//			((BYTE*)stDIBSection.dsBm.bmBits)[lPixel] = GetBValue(crFarbe);
//			((BYTE*)stDIBSection.dsBm.bmBits)[++lPixel] = GetGValue(crFarbe);
//			((BYTE*)stDIBSection.dsBm.bmBits)[++lPixel] = GetRValue(crFarbe);
//		}
//	}
}
//-----------------------------------------------------------------------------------------------------------------------------------------
inline void __vectorcall RePag::DirectX::COElement::ThreadSicher_Anfang(void)
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
inline void __vectorcall RePag::DirectX::COElement::ThreadSicher_Ende(void)
{
	WaitForSingleObjectEx(stThread.heElement[0], INFINITE, false);
	if(!--stThread.ucFunktion && stThread.ucWarten)	SetEvent(stThread.heElement[1]);
	SetEvent(stThread.heElement[0]);
}
//-----------------------------------------------------------------------------------------------------------------------------------------