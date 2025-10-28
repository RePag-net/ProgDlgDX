/******************************************************************************
MIT License

Copyright(c) 2025 Ren� Pagel

Filename: OGraphicGroupD2.h
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
#include "OGraphicD2.h"
//-------------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
	namespace DirectX
	{
		//---------------------------------------------------------------------------------------------------------------------------------------
		class __declspec(dllexport) COGraphicGroup : public COGraphic
		{
			friend LRESULT CALLBACK WndProc_GraphicGroup(HWND hWnd, unsigned int uiMessage, WPARAM wParam, LPARAM lParam);
			friend DWORD WINAPI thWM_Command_GraphicGroup(void* pvParam);

		private:
			void __vectorcall WM_Paint(void);
			void __vectorcall WM_Command_GrafikGruppe(unsigned int uiMessage, WPARAM wParam, LPARAM lParam);

		protected:

		public:
			void __vectorcall COGraphicGroupV(_In_ const VMEMORY vmSpeicher, _In_ const char* pcFensterName, _In_ unsigned int uiDElementA,
																				_In_ STDeviceResources* pstDeviceResourcesA);
			void __vectorcall ErstellFensterInRahmen(COGraphic* pGraphic);
			void __vectorcall ErstellFensterInGruppe(COGraphic* pGraphic, long lHolHeightAheA, long lWidthA, long lPos_x, long lPos_y);
			void(__vectorcall* pfnWM_Paint)(COGraphicGroup*, PAINTSTRUCT&);
			void(__vectorcall* pfnWM_Command)(HWND, unsigned int, WPARAM, LPARAM);
			void __vectorcall SetzSichtbar(bool bSichtbar);
		};
		//---------------------------------------------------------------------------------------------------------------------------------------
		__declspec(dllexport) COGraphicGroup* __vectorcall COGrafikGruppeV(_In_ const char* pcFensterName, _In_ unsigned int uiIDElement, _In_ STDeviceResources* pstDeviceResourcesA);
		__declspec(dllexport) COGraphicGroup* __vectorcall COGrafikGruppeV(_In_ const VMEMORY vmSpeicher, _In_ const char* pcFensterName, _In_ unsigned int uiIDElement,
																																			 _In_ STDeviceResources* pstDeviceResourcesA);
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}
