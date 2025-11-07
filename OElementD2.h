/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: OElementD2.h
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
//-------------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
	namespace DirectX
	{
		//---------------------------------------------------------------------------------------------------------------------------------------
		typedef struct STDeviceResources
		{
			ID2D1Factory7* ifd2d1Factory7;
			ID3D11Device5* ifD311Device5;
			ID2D1Device6* ifD2D1Device6;
			IDXGIFactory7* ifDXGIFactory7;
			IDWriteFactory7* ifdwriteFactory7;
			FLOAT dpiX;
			FLOAT dpiY;
		} STDeviceResources;
		//---------------------------------------------------------------------------------------------------------------------------------------
		class __declspec(dllexport) COElement
		{
		private:
			struct STThread
			{
				HANDLE heElement[2];
				DWORD dwMain;
				DWORD dwActiv;
				unsigned char ucFunction;
				unsigned char ucWait;
			};
			STThread stThread;

		protected:
			VMEMORY vmMemory;
			HWND hWndElement;
			HANDLE htEffect_Timer;
			long lHeight;
			long lWidth;
			POINT ptPosition;
			D2D1_COLOR_F crfBackground;
			ID2D1SolidColorBrush* ifBackColor;
			RECT rclDirty;
			DXGI_PRESENT_PARAMETERS dxgiPresent;
			void __vectorcall WM_Create_Element(_In_ HWND hWnd);
			void __vectorcall WM_Size_Element(_In_ HWND hWnd, _In_  LPARAM lParam);
			inline void __vectorcall ThreadSafe_Begin(void);
			inline void __vectorcall ThreadSafe_End(void);
			void __vectorcall COElementV(_In_ const VMEMORY vmMemoryA, _In_ STDeviceResources* pstDeviceResourcesA);

			STDeviceResources* pstDeviceResources;
			ID2D1DeviceContext6* ifD2D1Context6;
			IDXGISwapChain4* ifDXGISwapChain4;
			ID2D1Bitmap1* ifd2dBitmap1;
			IDXGISurface2* ifDXGIBackBuffer;
			void __vectorcall CreateWindowSizeDependentResources(void);

		public:
			VMEMORY __vectorcall COFreiV(void);
			void(__vectorcall* pfnWM_Size)(_In_ COElement*, _In_ WPARAM, _In_ LPARAM);
			HWND __vectorcall HWND_Element(void);
			HWND __vectorcall HWND_Main(void);
			long& __vectorcall Width(_Out_ long& lWidthA);
			long& __vectorcall Height(_Out_ long& lHeightA);
			long& __vectorcall Pos_X(_Out_ long& lPos_x);
			long& __vectorcall Pos_Y(_Out_ long& lPos_y);
			POINT& __vectorcall Position(_Out_ POINT& ptPositionA);
			RECT& __vectorcall Window(_Out_ RECT& rcWindow);
			void __vectorcall NewWindowPosition(_In_ long lPos_x, _In_ long lPos_y);
			void __vectorcall NewWindowPosition(_In_ POINT& ptPositionA);
			void __vectorcall NewWindow(long lHeightA, long lWidthA, long lPos_x, long lPos_y);
			void __vectorcall NewWindowSize(_In_ long lHeightA, _In_ long lWidthA);
			void __vectorcall NewWindowHeight(_In_ long lHeightA);
			void __vectorcall NewWindowWidth(_In_ long lWidthA);
			void __vectorcall ChangeWindowSize(_In_ long lHeightA, _In_ long lWidthA);
			void __vectorcall ChangeWindowPosition(_In_ long lPos_x, _In_ long lPos_y);
			void __vectorcall SetBackgroundColor(_In_ unsigned char ucRot, _In_ unsigned char ucGrun, _In_ unsigned char ucBlau, _In_ unsigned char ucAlpha);
			void __vectorcall SetBackgroundColor(_In_ D2D1_COLOR_F& crfBackgroundA);
			void __vectorcall GetBackgroundColor(_Out_ D2D1_COLOR_F& crfBackgroundA);
			void __vectorcall Effect_Timer(_In_ bool bStart, _In_ unsigned long ulInterval_msek, _In_ void CALLBACK fnEffect_Timer(COElement*, bool bTimerOrWaitFired));

		};
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}

