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
			friend LRESULT CALLBACK WndProc_Dialog(HWND hWnd, unsigned int uiMessage, WPARAM wParam, LPARAM lParam);
			friend DWORD WINAPI thWM_Command_Dialog(void* pvParam);

		private:
			struct STFensterBau
			{
				COStringA asName;
				VMBLOCK vbTitel;
				DWORD dwFensterStil;
				DWORD dwErweitertStil;
				WNDCLASSEX wndKlasse;
			};
			STFensterBau* vstFensterBau;
			long lRuckgabe;
			bool bModal;
			DWORD dwThreadID;
			void __vectorcall WM_Move_Dialog(void);
			void __vectorcall WM_Paint_Dialog(void);
			void __vectorcall WM_Command_Dialog(unsigned int uiMessage, WPARAM wParam, LPARAM lParam);
			LRESULT(CALLBACK* pfnWndProc_DLGDialog)(HWND, unsigned int, WPARAM, LPARAM);

		protected:
			void __vectorcall WM_Create(void);
			void __vectorcall FensterStil(DWORD dwFensterStilA);
			void __vectorcall ErweitertStil(DWORD dwErweitertStilA);
			void __vectorcall FensterTitel(const char* pcFensterTitel);
			void __vectorcall Icon(long lIcon);
			void __vectorcall IconSm(long lIconSm);
			void __vectorcall Menu(long lMenu);

		public:
			void __vectorcall CODialogV(_In_z_ const char* pcFensterName, _In_ bool bModalA, _In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM),
																	_In_ STDeviceResources* pstDeviceResourcesA);
			void __vectorcall CODialogV(_In_ const VMEMORY vmSpeicher, _In_z_ const char* pcFensterName, _In_ bool bModalA,
																	_In_ LRESULT CALLBACK WndProc_DLG(HWND, unsigned int, WPARAM, LPARAM), _In_ STDeviceResources* pstDeviceResourcesA);
			VMEMORY __vectorcall COFreiV(void);
			LRESULT(CALLBACK* pfnWndProc_Dialog)(HWND, unsigned int, WPARAM, LPARAM);
			void(__vectorcall* pfnWM_Paint)(CODialog*, PAINTSTRUCT&);
			void(__vectorcall* pfnWM_Move)(CODialog*, LPARAM);
			void(__vectorcall* pfnWM_Command)(HWND, unsigned int, WPARAM, LPARAM);
			void __vectorcall ErstellFenster(HWND hWndHaupt, long lHoheA, long lBreiteA, long lPos_x, long lPos_y);
			long __vectorcall SetzSichtbar(bool bSichtbar, unsigned char ucAusrichtung, long lRuckgabeA);
			void __vectorcall Schliessen(void);
			LOGFONT lfSchrift;
		};
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}

