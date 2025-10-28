/******************************************************************************
MIT License

Copyright(c) 2025 Ren� Pagel

Filename: OGraphicGroupD2.cpp
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
#include "OGraphicGroupD2.h"

#define _WM_Command ((STTHWM_CommandGG*)pvParam)
//-------------------------------------------------------------------------------------------------------------------------------------------
COGraphicGroup* __vectorcall RePag::DirectX::COGrafikGruppeV(_In_ const char* pcFensterName, _In_ unsigned int uiIDElement,
																												 _In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicGroup* vGrafikGruppe = (COGraphicGroup*)VMBlock(vmDialog, sizeof(COGraphicGroup));
	vGrafikGruppe->COGraphicGroupV((BYTE*)vmDialog, pcFensterName, uiIDElement, pstDeviceResourcesA);
	return vGrafikGruppe;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
COGraphicGroup* __vectorcall RePag::DirectX::COGrafikGruppeV(_In_ const VMEMORY vmSpeicher, _In_ const char* pcFensterName, _In_ unsigned int uiIDElement,
																												 _In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicGroup* vGrafikGruppe = (COGraphicGroup*)VMBlock(vmSpeicher, sizeof(COGraphicGroup));
	vGrafikGruppe->COGraphicGroupV((BYTE*)vmSpeicher, pcFensterName, uiIDElement, pstDeviceResourcesA);
	return vGrafikGruppe;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
namespace RePag
{
	namespace DirectX
	{
		//---------------------------------------------------------------------------------------------------------------------------------------
		typedef struct STTHWM_CommandGG
		{
			HANDLE hThread;
			DWORD dwThreadId;
			HWND hWndGG;
			unsigned int uiMessage;
			WPARAM wParam;
			LPARAM lParam;
			COGraphicGroup* pGraphicGroup;
		} STTHWM_CommandGG;
		//---------------------------------------------------------------------------------------------------------------------------------------
		DWORD WINAPI thWM_Command_GraphicGroup(void* pvParam)
		{
			_WM_Command->pGraphicGroup->ThreadSicher_Anfang();
			_WM_Command->pGraphicGroup->pfnWM_Command(_WM_Command->hWndGG, _WM_Command->uiMessage, _WM_Command->wParam, _WM_Command->lParam);
			_WM_Command->pGraphicGroup->ThreadSicher_Ende();

			void* pvThreadId = vthlThreadId->ThIteratorToBegin_Lock(); void* pvLoschen = nullptr;
			while(pvThreadId){
				if(((STThreadId*)vthlThreadId->Element(pvThreadId))->dwThreadId == _WM_Command->dwThreadId){ vthlThreadId->DeleteElement(pvThreadId, pvLoschen, true); break; }
				vthlThreadId->NextElement(pvThreadId, pvLoschen);
			}
			vthlThreadId->ThIteratorEnd();
			DeleteEvent(_WM_Command->hThread);
			VMFrei(pvParam);
			return NULL;
		}
		//---------------------------------------------------------------------------------------------------------------------------------------
		LRESULT CALLBACK WndProc_GraphicGroup(HWND hWnd, unsigned int uiMessage, WPARAM wParam, LPARAM lParam)
		{
			COGraphicGroup* pGrafikGruppe;
			switch(uiMessage){
				case WM_SIZE: pGrafikGruppe = (COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
					if(pGrafikGruppe) pGrafikGruppe->WM_Size_Element(hWnd, lParam);
					return NULL;
				case WM_COMMAND:	pGrafikGruppe = (COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
					if(pGrafikGruppe->pfnWM_Command){ pGrafikGruppe->WM_Command_GrafikGruppe(uiMessage, wParam, lParam); return NULL; }
					else PostMessage(GetParent(hWnd), WM_COMMAND, wParam, lParam);
					break;
				case WM_PAINT:	pGrafikGruppe = (COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
					pGrafikGruppe->WM_Paint();
					return NULL;
				case WM_NCDESTROY:	pGrafikGruppe = (COGraphicGroup*)GetWindowLongPtr(hWnd, GWLP_USERDATA);
					if(pGrafikGruppe->htEffekt_Timer)	DeleteTimerQueueTimer(htqTimerQueue, pGrafikGruppe->htEffekt_Timer, INVALID_HANDLE_VALUE);
					VMFreiV(pGrafikGruppe);
					return NULL;
			}
			return DefWindowProc(hWnd, uiMessage, wParam, lParam);
		}
		//---------------------------------------------------------------------------------------------------------------------------------------
	}
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::COGraphicGroupV(const VMEMORY vmSpeicher, const char* pcFensterName, unsigned int uiDElementA, 
																															_In_ STDeviceResources* pstDeviceResourcesA)
{
	COGraphicV(vmSpeicher, pcRePag_GraphicGroup, pcFensterName, uiDElementA, pstDeviceResourcesA);
	SetzHintergrund(false);

	pfnWM_Command = nullptr;
	pfnWM_Paint = nullptr;
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::WM_Paint(void)
{
	ThreadSicher_Anfang();
	PAINTSTRUCT stPaint;
	BeginPaint(hWndElement, &stPaint);
	//WM_Paint_Hintergrund(stPaint);
	if(pfnWM_Paint) pfnWM_Paint(this, stPaint);
	EndPaint(hWndElement, &stPaint);
	ThreadSicher_Ende();
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::WM_Command_GrafikGruppe(unsigned int uiMessage, WPARAM wParam, LPARAM lParam)
{
	STTHWM_CommandGG* stthWM_Command = (STTHWM_CommandGG*)VMBlock(sizeof(STTHWM_CommandGG));
	stthWM_Command->hWndGG = hWndElement;	stthWM_Command->pGraphicGroup = this; stthWM_Command->uiMessage = uiMessage; stthWM_Command->wParam = wParam;	stthWM_Command->lParam = lParam;
	stthWM_Command->hThread = CreateThread(nullptr, 0, thWM_Command_GraphicGroup, stthWM_Command, CREATE_SUSPENDED, &stthWM_Command->dwThreadId);

	STThreadId* stThreadId = (STThreadId*)VMBlock(vmDialog, sizeof(STThreadId));
	stThreadId->pElement = this; stThreadId->dwThreadId = stthWM_Command->dwThreadId; vthlThreadId->ThToEnd(stThreadId);

	if(stthWM_Command->hThread){ SetThreadPriority(stthWM_Command->hThread, THREAD_PRIORITY_NORMAL); ResumeThread(stthWM_Command->hThread); }
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::ErstellFensterInRahmen(COGraphic* pGrafik)
{
	pGrafik->ErstellFenster(hWndElement, lHeight - rcEffektrand.top - rcEffektrand.bottom, lWidth - rcEffektrand.left - rcEffektrand.right, rcEffektrand.left, rcEffektrand.top);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::ErstellFensterInGruppe(COGraphic* pGrafik, long lHeightA, long lWidthA, long lPos_x, long lPos_y)
{
	pGrafik->ErstellFenster(hWndElement, lHeightA, lWidthA, lPos_x, lPos_y);
}
//-------------------------------------------------------------------------------------------------------------------------------------------
void __vectorcall RePag::DirectX::COGraphicGroup::SetzSichtbar(bool bSichtbar)
{
	if(bSichtbar) ShowWindow(hWndElement, SW_SHOW);
	else ShowWindow(hWndElement, SW_HIDE);
}