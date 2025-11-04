/******************************************************************************
MIT License

Copyright(c) 2025 René Pagel

Filename: OGraphicD2.cpp
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
#include "OGraphicD2.h"
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::COGraphicV(_In_ const VMEMORY vmMemory, _In_ const char* pcClassName, _In_ const char* pcWindowName,
																										_In_ unsigned int uiElementA,	_In_ STDeviceResources* pstDeviceResourcesA)
{
	COElementV(vmMemory, pstDeviceResourcesA);

	vstFensterBau = (STFensterBau*)VMBlock(sizeof(STFensterBau));
	vstFensterBau->dwFensterStil = WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS;
	vstFensterBau->dwErweitertStil = WS_EX_LEFT | WS_EX_NOPARENTNOTIFY;
	vstFensterBau->uiIDElement = uiElementA;
	vstFensterBau->asFensterName = pcWindowName;
	BYTE ucBytes_Name = (BYTE)StrLength(pcClassName);
	vstFensterBau->vbKlassenName = VMBlock(ucBytes_Name + 1); vstFensterBau->vbKlassenName[ucBytes_Name] = 0;
	MemCopy(vstFensterBau->vbKlassenName, pcClassName, ucBytes_Name);

	heRender = CreateEvent(nullptr, false, true, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::COGraphic::COFreiV(void)
{
	CloseHandle(heRender);
	return ((COElement*)this)->COFreiV();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::FensterStil(DWORD dwFensterStilA)
{
	if(vstFensterBau) vstFensterBau->dwFensterStil = dwFensterStilA;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::ErweitertStil(DWORD dwErweitertStilA)
{
	if(vstFensterBau) vstFensterBau->dwErweitertStil = dwErweitertStilA | WS_EX_NOPARENTNOTIFY;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::CreateWindowGraphic(HWND hWndMain, long lHeightA, long lWidthA, long lPos_x, long lPos_y)
{
	if(vstFensterBau){
		while(FindWindowEx(hWndMain, nullptr, nullptr, vstFensterBau->asFensterName.c_Str())) vstFensterBau->asFensterName += "A";

		lHeight = lHeightA; lWidth = lWidthA; ptPosition.x = lPos_x; ptPosition.y = lPos_y;

		hWndElement = CreateWindowEx(vstFensterBau->dwErweitertStil, vstFensterBau->vbKlassenName, vstFensterBau->asFensterName.c_Str(), vstFensterBau->dwFensterStil,
																 ptPosition.x, ptPosition.y, lWidth, lHeight, hWndMain, reinterpret_cast<HMENU>(static_cast<LONG_PTR>(vstFensterBau->uiIDElement)), hInstance, this);

		if(hWndElement){
			SetWindowLongPtr(hWndElement, GWLP_USERDATA, (LONG_PTR)this);
			RECT rcClient; GetClientRect(hWndElement, &rcClient);	lWidth = rcClient.right; lHeight = rcClient.bottom;
			VMFrei(vstFensterBau->vbKlassenName); vstFensterBau->asFensterName.~COStringA(); VMFrei(vstFensterBau); vstFensterBau = nullptr;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------
unsigned int __vectorcall RePag::DirectX::COGraphic::IDElement(void)
{
	return GetWindowLongPtr(hWndElement, GWLP_ID);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::SetVisible(bool bVisible)
{
	if(bVisible) ShowWindow(hWndElement, SW_SHOWNORMAL);
	else{ if(hWndElement == GetFocus()) SetFocus(GetParent(hWndElement)); ShowWindow(hWndElement, SW_HIDE); }
}
//-------------------------------------------------------------------------------------------------------------------------------------------
bool __vectorcall RePag::DirectX::COGraphic::Visible(void)
{
	return IsWindowVisible(hWndElement);
}
//-------------------------------------------------------------------------------------------------------------------------------------------