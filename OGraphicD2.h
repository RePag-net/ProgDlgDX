/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: OGraphicD2.h
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
		class __declspec(dllexport) COGraphic : public COElement
		{
		private:
			struct STFensterBau
			{
				VMBLOCK vbKlassenName;
				COStringA asFensterName;
				DWORD dwFensterStil;
				DWORD dwErweitertStil;
				unsigned int uiIDElement;
			};
			STFensterBau* vstFensterBau;

		protected:
		void __vectorcall COGraphicV(_In_ const VMEMORY vmMemory, _In_ const char* pcClassName, _In_ const char* pcWindowName, _In_ unsigned int uiElementA,
																 _In_ STDeviceResources* pstDeviceResourcesA);
		HANDLE heRender;

		public:
			VMEMORY __vectorcall COFreiV(void);
			void __vectorcall FensterStil(DWORD dwFensterStilA);
			void __vectorcall ErweitertStil(DWORD dwErweitertStilA);
			void __vectorcall ErstellFenster(HWND hWndHaupt, long lHeightA, long lWidthA, long lPos_x, long lPos_y);
			unsigned int __vectorcall IDElement(void);
			void __vectorcall SetzSichtbar(bool bSichtbar);
			bool __vectorcall Sichtbar(void);
		};
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}

