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
//------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::COElementV(_In_ const VMEMORY vmMemoryA, _In_ STDeviceResources* pstDeviceResourcesA)
{
	vmMemory = vmMemoryA;
	pstDeviceResources = pstDeviceResourcesA;
	hWndElement = nullptr;
	crfBackground = D2D1::ColorF(1.0f, 1.0f, 1.0f, 0.0f);

	stThread.ucFunction = stThread.dwActive = stThread.ucWait = 0; stThread.dwMain = stThread.dwActive = GetCurrentThreadId();
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
	CloseHandle(stThread.heElement[0]); CloseHandle(stThread.heElement[1]);

	SafeRelease(&ifD2D1Context6);
	SafeRelease(&ifDXGIBackBuffer);
	SafeRelease(&ifDXGISwapChain4);
	SafeRelease(&ifd2dBitmap1);

	return vmMemory;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::CreateWindowSizeDependentResources(void)
{
	DXGI_FORMAT dxgi_Format = DXGI_FORMAT_B8G8R8A8_UNORM;

	if(ifDXGISwapChain4 != nullptr){

		ifD2D1Context6->SetTarget(nullptr);
		SafeRelease(&ifd2dBitmap1);	SafeRelease(&ifDXGIBackBuffer);

		ifDXGISwapChain4->ResizeBuffers(0, 0, 0, dxgi_Format, NULL);
    rclDirty.top = 0; rclDirty.left = 0; rclDirty.right = lWidth; rclDirty.bottom = lHeight;
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
		swapChainDesc.Scaling = DXGI_SCALING_NONE; // DXGI_SCALING_STRETCH;

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
void __vectorcall RePag::DirectX::COElement::WM_Create_Element(_In_ HWND hWnd)
{
	hWndElement = hWnd;

	CreateWindowSizeDependentResources();

	RECT rcClient; GetClientRect(hWndElement, &rcClient);
	rclDirty.top = 0; rclDirty.left = 0; rclDirty.right = rcClient.right; rclDirty.bottom = rcClient.bottom;
	dxgiPresent.DirtyRectsCount = 1; dxgiPresent.pDirtyRects = &rclDirty;
	dxgiPresent.pScrollOffset = nullptr; dxgiPresent.pScrollRect = nullptr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
HWND __vectorcall RePag::DirectX::COElement::HWND_Element(void)
{
	return hWndElement;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
HWND __vectorcall RePag::DirectX::COElement::HWND_Main(void)
{
	return GetParent(hWndElement);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Width(_Out_ long& lWidthA)
{
	ThreadSafe_Begin();
	lWidthA = lWidth;
	ThreadSafe_End();
	return lWidthA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Height(_Out_ long& lHeightA)
{
	ThreadSafe_Begin();
	lHeightA = lHeight;
	ThreadSafe_End();
	return lHeightA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Pos_X(_Out_ long& lPos_x)
{
	ThreadSafe_Begin();
	lPos_x = ptPosition.x;
	ThreadSafe_End();
	return lPos_x;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
long& __vectorcall RePag::DirectX::COElement::Pos_Y(_Out_ long& lPos_y)
{
	ThreadSafe_Begin();
	lPos_y = ptPosition.y;
	ThreadSafe_End();
	return lPos_y;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
POINT& __vectorcall RePag::DirectX::COElement::Position(_Out_ POINT& ptPositionA)
{
	ThreadSafe_Begin();
	ptPositionA = ptPosition;
	ThreadSafe_End();
	return ptPositionA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
RECT& __vectorcall RePag::DirectX::COElement::Window(_Out_ RECT& rcWindow)
{
	ThreadSafe_Begin();
	rcWindow.left = ptPosition.x;
	rcWindow.top = ptPosition.y;
	rcWindow.right = ptPosition.x + lWidth;
	rcWindow.bottom = ptPosition.y + lHeight;
	ThreadSafe_End();
	return rcWindow;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NewWindowPosition(_In_ long lPos_x, _In_ long lPos_y)
{
	ThreadSafe_Begin();
	ptPosition.x = lPos_x; ptPosition.y = lPos_y;
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NewWindowPosition(_In_ POINT& ptPositionA)
{
	ThreadSafe_Begin();
	ptPosition = ptPositionA;
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NewWindowSize(_In_ long lHeightA, _In_ long lWidthA)
{
	ThreadSafe_Begin();
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidthA, lHeightA, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NewWindowHeight(_In_ long lHeightA)
{
	ThreadSafe_Begin();
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeightA, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NewWindowWidth(_In_ long lWidthA)
{
	ThreadSafe_Begin();
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidthA, lHeight, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::NewWindow(_In_ long lHeightA, _In_ long lWidthA, _In_ long lPos_x, _In_ long lPos_y)
{
	ThreadSafe_Begin();
	ptPosition.x = lPos_x; ptPosition.y = lPos_y;
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidthA, lHeightA, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::ChangeWindowSize(_In_ long lHeightA, _In_ long lWidthA)
{
	ThreadSafe_Begin();
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidthA, lHeightA, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::ChangeWindowPosition(_In_ long lPos_x, _In_ long lPos_y)
{
	ThreadSafe_Begin();
	ptPosition.x += lPos_x; ptPosition.y += lPos_y;
	MoveWindow(hWndElement, ptPosition.x, ptPosition.y, lWidth, lHeight, false);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetBackgroundColor(_In_ unsigned char ucRed, _In_ unsigned char ucGreen, _In_ unsigned char ucBlue, _In_ float fAlpha)
{
	ThreadSafe_Begin();
	crfBackground =	D2D1::ColorF(RGB(ucBlue, ucGreen, ucRed), fAlpha);
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COElement::SetBackgroundColor(_In_ D2D1_COLOR_F& crfBackgroundA)
{
	ThreadSafe_Begin();
	crfBackground = crfBackgroundA;
	ThreadSafe_End();
}
//-----------------------------------------------------------------------------------------------------------------------------------------
D2D1_COLOR_F& __vectorcall RePag::DirectX::COElement::GetBackgroundColor(_Out_ D2D1_COLOR_F& crfBackgroundA)
{
	ThreadSafe_Begin();
	crfBackgroundA = crfBackground;
	ThreadSafe_End();
	return crfBackgroundA;
}
//-----------------------------------------------------------------------------------------------------------------------------------------
inline void __vectorcall RePag::DirectX::COElement::ThreadSafe_Begin(void)
{
	WaitForSingleObjectEx(stThread.heElement[0], INFINITE, false);
	DWORD dwThreadID = GetCurrentThreadId();
	if(!stThread.ucFunction) stThread.dwActive = dwThreadID;
	else if(stThread.dwActive != dwThreadID){
		if(stThread.dwMain != dwThreadID || stThread.dwMain == stThread.dwActive){
			stThread.ucWait++;
			SetEvent(stThread.heElement[0]);
			WaitForMultipleObjectsEx(2, stThread.heElement, true, INFINITE, false);
			stThread.dwActive = dwThreadID;
			stThread.ucWait--;
		}
	}
	stThread.ucFunction++; SetEvent(stThread.heElement[0]);
}
//-----------------------------------------------------------------------------------------------------------------------------------------
inline void __vectorcall RePag::DirectX::COElement::ThreadSafe_End(void)
{
	WaitForSingleObjectEx(stThread.heElement[0], INFINITE, false);
	if(!--stThread.ucFunction && stThread.ucWait)	SetEvent(stThread.heElement[1]);
	SetEvent(stThread.heElement[0]);
}
//-----------------------------------------------------------------------------------------------------------------------------------------