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
				DWORD dwHaupt;
				DWORD dwAktiv;
				unsigned char ucFunktion;
				unsigned char ucWarten;
			};
			STThread stThread;
			HBITMAP hbmBild;
			bool bHintergrund;
			unsigned char ucHintergrundeffekt;
			D2D1_COLOR_F crfBackgroundEffect;

		protected:
			VMEMORY vmMemory;
			HWND hWndElement;
			HANDLE htEffekt_Timer;
			long lHeight;
			long lWidth;
			POINT ptPosition;
			D2D1_COLOR_F crfBackground;
			ID2D1SolidColorBrush* ifBackColor;
			RECT rclDirty;
			DXGI_PRESENT_PARAMETERS dxgiPresent;
			RECT rcEffektrand;
			//unsigned char ucRahmenbreite;
			void __vectorcall WM_Create_Element(HWND hWnd);
			void __vectorcall WM_Size_Element(HWND hWnd, LPARAM lParam);
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
			void(__vectorcall* pfnWM_Size)(COElement*, WPARAM, LPARAM);
			HWND __vectorcall HWND_Element(void);
			HWND __vectorcall HWND_Main(void);
			long& __vectorcall Width(long& lWidthA);
			long& __vectorcall Height(long& lHeightA);
			long& __vectorcall Pos_X(long& lPos_x);
			long& __vectorcall Pos_Y(long& lPos_y);
			POINT& __vectorcall Position(POINT& ptPositionA);
			RECT& __vectorcall Fenster(RECT& rcFenster);
			void __vectorcall NeueFensterPosition(long lPos_x, long lPos_y);
			void __vectorcall NeueFensterPosition(POINT& ptPositionA);
			void __vectorcall NeuesFenster(long lHeightA, long lWidthA, long lPos_x, long lPos_y);
			void __vectorcall NeueFensterGrosse(long lHeightA, long lWidthA);
			void __vectorcall NeueFensterHohe(long lHeightA);
			void __vectorcall NeueFensterBreite(long lWidthA);
			void __vectorcall AndernFensterGrosse(long lHeightA, long lWidthA);
			void __vectorcall AndernFensterPosition(long lPos_x, long lPos_y);
			void __vectorcall SetBackgroundColor(_In_ unsigned char ucRot, _In_ unsigned char ucGrun, _In_ unsigned char ucBlau, _In_ unsigned char ucAlpha);
			void __vectorcall SetBackgroundColor(_In_ D2D1_COLOR_F& crfBackgroundA);
			void __vectorcall GetBackgroundColor(_In_ D2D1_COLOR_F& crfBackgroundA);
			void __vectorcall Hintergrundeffekt(unsigned char ucEffekt);
			void __vectorcall Effekt_Timer(bool bStarten, unsigned long ulInterval_msek, void CALLBACK fnEffekt_Timer(COElement*, bool bTimerOrWaitFired));
			//void __vectorcall UpdateFenster(RECT* prcFenster, bool bUpdateHintergrund, bool bUpdateHintergrundeffekt);
			void __vectorcall SetzHintergrund(bool bMitHintergrund);
			void __vectorcall SetzVerfugbar(bool bVerfugbar);
			bool __vectorcall Verfugbar(void);
			void __vectorcall Loschen(void);

		};
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}

