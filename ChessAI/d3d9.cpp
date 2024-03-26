#include "pch.h"
#include "d3d9.h"

#include <gdiplus.h> 
#pragma comment(lib, "gdiplus.lib")


#include<dwmapi.h>
#pragma comment(lib, "dwmapi.lib")

HWND hwnd;

IDirect3DDevice9* g_device;
ID3DXLine* g_line;

LPDIRECT3DTEXTURE9 g_pTex = NULL;    //纹理  
LPD3DXSPRITE g_pSprite = NULL;    //点精灵  


std::vector<hollowRect> hollowRects;


void render() {
	if (NULL == g_device)
	{
		return;
	}

	//g_device->Clear(0, NULL, D3DCLEAR_TARGET, D3DCOLOR_ARGB(0, 0, 255, 200), 1.0f, 0);
	if (SUCCEEDED(g_device->BeginScene()))
	{
		//if (SUCCEEDED(g_pSprite->Begin(D3DXSPRITE_ALPHABLEND)))
		//{

		//	HWND gameHwnd = (HWND)0x00172636;
		//	RECT rect;
	
		//	
		//	GetWindowRect(gameHwnd, &rect);

		//	g_pSprite->Draw(g_pTex, &rect, NULL, NULL, 0xffffffff);
		//	g_pSprite->End();
		//}

		//绘制空心矩形
		for (int i = 0; i < hollowRects.size(); i++)
		{
			float left = (float)hollowRects[i].left;
			float top = (float)hollowRects[i].top;
			float width = (float)hollowRects[i].width;
			float height = (float)hollowRects[i].height;
			float weight = hollowRects[i].weight;
			D3DCOLOR color = hollowRects[i].color;
			D3DXVECTOR2 Vertex[5] = { {left,top},{left+width,top},{left+width,top+height},{left,top+height},{left,top} };
			g_line->SetWidth(weight);
			g_line->Draw(Vertex, 5, color);
		}
		g_device->EndScene();
	}
	g_device->Present(NULL, NULL, NULL, NULL);
}

LRESULT __stdcall Wndproc(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
{
	switch (msg)
	{
	case WM_CREATE:

		break;
	case WM_DESTROY:
		//CleanUp();
		PostQuitMessage(0);
		return 0;
	case WM_PAINT:
		render();
		ValidateRect(hwnd, NULL);
		return 0;
	default:
		break;
	}

	return DefWindowProc(hwnd, msg, wParam, lParam);
}



boolean d3d9::initD3d()
{

	WNDCLASS wc;
	wc.style = CS_VREDRAW | CS_HREDRAW;
	wc.lpfnWndProc = (WNDPROC)Wndproc;
	wc.cbClsExtra = 0;
	wc.cbWndExtra = 0;
	wc.hInstance = NULL;
	wc.hIcon = LoadIcon(0,IDI_APPLICATION);
	wc.hCursor = LoadCursor(0, IDC_ARROW);
	wc.hbrBackground = /*(HBRUSH)GetStockObject(WHITE_BRUSH)*/NULL;
	wc.lpszClassName = L"ChessAI";
	wc.lpszMenuName = NULL;
	RegisterClass(&wc);

	HWND gameHwnd = ((HWND)0x00172636);
	RECT rect;
	GetWindowRect(gameHwnd, &rect);

	//hwnd = CreateWindowEx(WS_EX_LAYERED, L"ChessAI", L"2b", WS_OVERLAPPEDWINDOW, CW_USEDEFAULT, CW_USEDEFAULT, rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, NULL, NULL);
	
	hwnd = CreateWindowA("ChessAI","绘制窗口", /*WS_OVERLAPPEDWINDOW*/WS_POPUP | WS_SYSMENU | WS_VISIBLE,0,0,rect.right - rect.left, rect.bottom - rect.top, NULL, NULL, NULL, NULL); //AfxGetMainWnd()->m_hWnd
	::SetWindowLong(hwnd, GWL_EXSTYLE, GetWindowLong(hwnd, GWL_EXSTYLE) | WS_EX_LAYERED);
	SetLayeredWindowAttributes(hwnd, 0, (255 * 70) / 150, LWA_ALPHA);

	//MoveWindow(hwnd, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top,FALSE);


	//DWM_BLURBEHIND db{};
	//db.dwFlags = DWM_BB_ENABLE | DWM_BB_BLURREGION;
	//db.hRgnBlur = CreateRectRgn(0, 0, -1, -1);
	//db.fEnable = TRUE;
	//DwmEnableBlurBehindWindow(hwnd, &db);

	//HWND hStatic = CreateWindowEx(0, _T("STATIC"), _T(""), WS_VISIBLE | WS_CHILD | SS_BITMAP,
	//	0, 0, rect.right - rect.left, rect.bottom - rect.top, hwnd, NULL, GetModuleHandle(NULL), NULL);
	//SendMessageA(hStatic, STM_SETIMAGE,IMAGE_BITMAP,(LPARAM)Utils::WindowCapture_Front(gameHwnd));



	IDirect3D9* d3d9 = Direct3DCreate9(D3D_SDK_VERSION);
	D3DCAPS9 caps;
	d3d9->GetDeviceCaps(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, &caps);

	D3DPRESENT_PARAMETERS d3dpp;
	ZeroMemory(&d3dpp, sizeof(d3dpp));
	d3dpp.Windowed = TRUE;
	d3dpp.SwapEffect = D3DSWAPEFFECT_DISCARD;
	d3dpp.BackBufferFormat = D3DFMT_UNKNOWN;
	d3dpp.BackBufferWidth = rect.right - rect.left;
	d3dpp.BackBufferHeight = rect.bottom - rect.top;
	d3d9->CreateDevice(D3DADAPTER_DEFAULT, D3DDEVTYPE_HAL, hwnd, D3DCREATE_SOFTWARE_VERTEXPROCESSING, &d3dpp, &g_device);

	D3DXCreateLine(g_device, &g_line);


	if ((D3DXCreateSprite(g_device, &g_pSprite)))
	{
		return E_FAIL;
	}

	if ((D3DXCreateTextureFromFile(g_device, L"1.png", &g_pTex)))
	{
		return E_FAIL;
	}

    return true;
}


void d3d9::drawHollowRect(int left, int top, int width, int height, float weight, D3DCOLOR color) {
	hollowRect hr;
	hr.left = left;
	hr.top = top;
	hr.width = width;
	hr.height = height;
	hr.weight = weight;
	hr.color = color;
	hollowRects.push_back(hr);
}

unsigned  threadId[2];
unsigned __stdcall showWindowThread(LPVOID lpParam) {
	ShowWindow(hwnd, TRUE);
	UpdateWindow(hwnd);

	MSG msg;
	while (GetMessage(&msg, hwnd, NULL, NULL))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	UnregisterClassA("ChessAI", NULL);

	return 0;
}

unsigned __stdcall runafterWindowThread(LPVOID lpParam) {

	while (true)
	{
		HWND gameHwnd = ((HWND)0x00172636);
		RECT rect;
		GetWindowRect(gameHwnd, &rect);
		SetWindowPos(::hwnd, HWND_TOPMOST, rect.left, rect.top, rect.right - rect.left, rect.bottom - rect.top, NULL);

		Sleep(10);
	}
	return 0;
}

boolean d3d9::showWindow() {

	_beginthreadex(NULL, 0, showWindowThread,NULL, 0, &threadId[0]);
	_beginthreadex(NULL, 0, runafterWindowThread,NULL, 0, &threadId[1]);
	
	return true;
}

