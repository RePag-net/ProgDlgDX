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

	vstWindowConstraction = (STWindowConstraction*)VMBlock(sizeof(STWindowConstraction));
	vstWindowConstraction->dwWindowStyle = WS_CHILDWINDOW | WS_VISIBLE | WS_CLIPSIBLINGS;
	vstWindowConstraction->dwExtendStyle = WS_EX_LEFT | WS_EX_NOPARENTNOTIFY;
	vstWindowConstraction->uiIDElement = uiElementA;
	vstWindowConstraction->asWindowName = pcWindowName;
	BYTE ucBytes_Name = (BYTE)StrLength(pcClassName);
	vstWindowConstraction->vbClassName = VMBlock(ucBytes_Name + 1); vstWindowConstraction->vbClassName[ucBytes_Name] = 0;
	MemCopy(vstWindowConstraction->vbClassName, pcClassName, ucBytes_Name);

	heRender = CreateEvent(nullptr, false, true, nullptr);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::COGraphic::COFreiV(void)
{
	CloseHandle(heRender);
	return ((COElement*)this)->COFreiV();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::WindowStyle(_In_ DWORD dwWindowStyleA)
{
	if(vstWindowConstraction) vstWindowConstraction->dwWindowStyle = dwWindowStyleA;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::ExtendStyle(_In_ DWORD dwExtendStyleA)
{
	if(vstWindowConstraction) vstWindowConstraction->dwExtendStyle = dwExtendStyleA | WS_EX_NOPARENTNOTIFY;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::CreateWindowGraphic(_In_ HWND hWndMain, _In_ long lHeightA, _In_ long lWidthA, _In_ long lPos_x, _In_ long lPos_y)
{
	if(vstWindowConstraction){
		while(FindWindowEx(hWndMain, nullptr, nullptr, vstWindowConstraction->asWindowName.c_Str())) vstWindowConstraction->asWindowName += "A";

		lHeight = lHeightA; lWidth = lWidthA; ptPosition.x = lPos_x; ptPosition.y = lPos_y;

		hWndElement = CreateWindowEx(vstWindowConstraction->dwExtendStyle, vstWindowConstraction->vbClassName, vstWindowConstraction->asWindowName.c_Str(),
																 vstWindowConstraction->dwWindowStyle, ptPosition.x, ptPosition.y, lWidth, lHeight, hWndMain,
																 reinterpret_cast<HMENU>(static_cast<LONG_PTR>(vstWindowConstraction->uiIDElement)), hInstance, this);

		if(hWndElement){
			SetWindowLongPtr(hWndElement, GWLP_USERDATA, (LONG_PTR)this);
			RECT rcClient; GetClientRect(hWndElement, &rcClient);	lWidth = rcClient.right; lHeight = rcClient.bottom;
			VMFrei(vstWindowConstraction->vbClassName); vstWindowConstraction->asWindowName.~COStringA(); VMFrei(vstWindowConstraction); vstWindowConstraction = nullptr;
		}
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------
unsigned int __vectorcall RePag::DirectX::COGraphic::IDElement(void)
{
	return GetWindowLongPtr(hWndElement, GWLP_ID);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::SetVisible(_In_ bool bVisible)
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