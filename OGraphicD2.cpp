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
#include "OGraphicGroupD2.h"
#include "ODialogD2.h"
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
	bGraphicGroup = false;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
VMEMORY __vectorcall RePag::DirectX::COGraphic::COFreiV(void)
{
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
void __vectorcall RePag::DirectX::COGraphic::CreateWindowGraphic(_In_ HWND hWndMain, _In_ long lHeightA, _In_ long lWidthA,
																																 _In_ long lPos_x, _In_ long lPos_y)
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
			PostMessage(hWndElement, WM_INITPAINT, NULL, NULL);
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
void __vectorcall RePag::DirectX::COGraphic::Background(void)
{
	ID2D1GradientStopCollection* ifGradientStops = nullptr;
	D2D1_POINT_2F ptfStartPoint = D2D1::Point2F(0.0f, 0.0f); D2D1_POINT_2F ptfEndPoint = D2D1::Point2F(0.0f, 0.0f);
	if(bGraphicGroup){
		COGraphicGroup* pGraphicGroup = (COGraphicGroup*)GetWindowLongPtr(GetParent(hWndElement), GWLP_USERDATA);
		if(!pGraphicGroup->GetLinearGradient()) ifD2D1Context6->Clear(pGraphicGroup->GetBackgroundColor(crfBackground));
		else{
			D2D1_GRADIENT_STOP gs2GradientStops[2];
			pGraphicGroup->GradientStops(gs2GradientStops);
			if(FAILED(ifD2D1Context6->CreateGradientStopCollection(gs2GradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &ifGradientStops))) goto Error;
			pGraphicGroup->LinearGradientStartPoint(ptfStartPoint);
			ptfStartPoint.x -= (float)ptPosition.x; ptfStartPoint.y -= (float)ptPosition.y;
			pGraphicGroup->LinearGradientEndPoint(ptfEndPoint);
			ptfEndPoint.x -= (float)ptPosition.x; ptfEndPoint.y -= (float)ptPosition.y;
		}
	}
	else if(!Background_Dialog(ifGradientStops, ptfStartPoint, ptfEndPoint)) goto Error;
	Background_Rectangle(ifGradientStops, ptfStartPoint, ptfEndPoint);
Error:
	SafeRelease(&ifGradientStops);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
bool __vectorcall RePag::DirectX::COGraphic::Background_Dialog(ID2D1GradientStopCollection*& ifGradientStops,
																															 D2D1_POINT_2F& ptfStartPoint, D2D1_POINT_2F& ptfEndPoint)
{
	CODialog* pDialog = (CODialog*)GetWindowLongPtr(GetParent(hWndElement), GWLP_USERDATA);
	if(!pDialog->GetLinearGradient()) ifD2D1Context6->Clear(pDialog->GetBackgroundColor(crfBackground));
	else{
		D2D1_GRADIENT_STOP gs2GradientStops[2];
		pDialog->GradientStops(gs2GradientStops);
		if(FAILED(ifD2D1Context6->CreateGradientStopCollection(gs2GradientStops, 2, D2D1_GAMMA_2_2, D2D1_EXTEND_MODE_CLAMP, &ifGradientStops))) return false;
		pDialog->LinearGradientStartPoint(ptfStartPoint);
		ptfStartPoint.x -= (float)ptPosition.x; ptfStartPoint.y -= (float)ptPosition.y;
		pDialog->LinearGradientEndPoint(ptfEndPoint);
		ptfEndPoint.x -= (float)ptPosition.x; ptfEndPoint.y -= (float)ptPosition.y;
	}
	return true;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphic::Background_Rectangle(ID2D1GradientStopCollection*& ifGradientStops,
																																	D2D1_POINT_2F& ptfStartPoint, D2D1_POINT_2F& ptfEndPoint)
{
	ID2D1LinearGradientBrush* ifMainGradientBrush = nullptr;
	if(SUCCEEDED(ifD2D1Context6->CreateLinearGradientBrush(
		D2D1::LinearGradientBrushProperties(ptfStartPoint, ptfEndPoint),
		D2D1::BrushProperties(1.0f, D2D1::IdentityMatrix()), ifGradientStops, &ifMainGradientBrush)))
		ifD2D1Context6->FillRectangle(&D2D1::RectF(0.0f, 0.0f, (float)lWidth, (float)lHeight), ifMainGradientBrush);
	SafeRelease(&ifMainGradientBrush);
}