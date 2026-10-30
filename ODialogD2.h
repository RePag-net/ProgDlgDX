/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: ODialogD2.h
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
#include "OElementD2.h"
//-------------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
	namespace DirectX
	{
		//---------------------------------------------------------------------------------------------------------------------------------------
		class __declspec(dllexport) CODialog : public COElement
		{
        friend LRESULT CALLBACK WndProc_Dialog(_In_ HWND hWnd, _In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam);
        friend DWORD WINAPI thWM_Command_Dialog(_In_ void* pvParam);

      private:
        struct STWindowConstraction
        {
          COStringA asName;
          VMBLOCK vbTitel;
          DWORD dwWindowStyle;
          DWORD dwExtendStyle;
          WNDCLASSEX wndClass;
        };
        STWindowConstraction* vstWindowConstraction;
        bool bModal;
        bool bLinearGradient;
        long lReturn;
        DWORD dwThreadID;
        IDWriteTextFormat* ifText;
        D2D1_COLOR_F crfText;
        ID2D1SolidColorBrush* ifTextColor;
        D2D1_COLOR_F crfGradient_1_0;
        D2D1_COLOR_F crfGradient_2_1;
        ID2D1SolidColorBrush* ifBackground;
        ID2D1LinearGradientBrush* ifLinearGradientBrush;
        D2D1_POINT_2F ptfStartPoint;
        D2D1_POINT_2F ptfEndPoint;
        D2D1_GRADIENT_STOP gs2GradientStops[2];
        float fEdgeRadius_X;
        float fEdgeRadius_Y;
        void __vectorcall OnRender(void);
        void __vectorcall InitPaint(void);
        void __vectorcall OnPaint(void);
        void __vectorcall WM_Size(LPARAM lParam);
        void __vectorcall WM_Move_Dialog(void);
        void __vectorcall WM_Command_Dialog(_In_ unsigned int uiMessage, _In_ WPARAM wParam, _In_ LPARAM lParam);
        LRESULT(CALLBACK* pfnWndProc_DLGDialog)(_In_ HWND, _In_ unsigned int, _In_ WPARAM, _In_ LPARAM);

      protected:
        void __vectorcall WindowStyle(_In_ DWORD dwFensterStilA);
        void __vectorcall ExtendStyle(_In_ DWORD dwErweitertStilA);
        void __vectorcall WindowTitel(_In_ const char* pcFensterTitel);
        void __vectorcall Icon(_In_ long lIcon);
        void __vectorcall IconSm(_In_ long lIconSm);
        void __vectorcall Menu(_In_ long lMenu);

      public:
        void __vectorcall CODialogV(_In_z_ const char* pcWindowName, _In_ bool bModalA, _In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM),
                                    _In_ STDeviceResources* pstDeviceResourcesA);
        void __vectorcall CODialogV(_In_ const VMEMORY vmMemory, _In_z_ const char* pcWindowName, _In_ bool bModalA,
                                    _In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM), _In_ STDeviceResources* pstDeviceResourcesA);
        VMEMORY __vectorcall COFreiV(void);
        void(__vectorcall* pfnOnRender)(_In_ CODialog* pDialog);
        LRESULT(CALLBACK* pfnWndProc_Dialog)(_In_ HWND, _In_ unsigned int, _In_ WPARAM, _In_ LPARAM);
        void(__vectorcall* pfnWM_Move)(_In_ CODialog*, LPARAM);
        void(__vectorcall* pfnWM_Command)(_In_ HWND, _In_ unsigned int, _In_ WPARAM, _In_ LPARAM);
        void __vectorcall CreateWindowDialog(_In_ HWND hWndMain, _In_ long lHeightA, _In_ long lWidthA, _In_ long lPos_x, _In_ long lPos_y);
        long __vectorcall SetVisible(_In_ bool bSichtbar, _In_ unsigned char ucAusrichtung, _In_ long lRuckgabeA);
        void __vectorcall Close(void);
        void __vectorcall SetFont(_In_ STFont& stFont);
        void __vectorcall SetTextColor(_In_ unsigned char ucRed, _In_ unsigned char ucGreen, _In_ unsigned char ucBlue, _In_ float fAlpha);
        void __vectorcall SetTextColor(_In_ D2D1_COLOR_F& crfTextA);
        void __vectorcall DrawText(_In_z_ const wchar_t* wcInhalt, _In_ size_t szBytes_Text, _In_ const D2D1_RECT_F& rcfText);
        void __vectorcall SetLinearGradient(_In_ bool bLinearGradientA);
        bool __vectorcall GetLinearGradient(void);
        void __vectorcall LinearGradientStartPoint(_Out_ D2D1_POINT_2F& ptfStartPointA);
        void __vectorcall LinearGradientEndPoint(_Out_ D2D1_POINT_2F& ptfEndPointA);
        void __vectorcall GradientStops(_Out_ D2D1_GRADIENT_STOP GradientStopsA[2]);

		};
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}

