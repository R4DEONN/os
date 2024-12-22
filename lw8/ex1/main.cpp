#include <windows.h>
#include <cstdlib>
#include <ctime>

COLORREF ellipseColor = RGB(0, 0, 255); //TODO: Избавиться от глобальной переменной
int EXIT_ID = 1;

void DrawEllipse(HDC hdc, RECT rect);

LRESULT CALLBACK WindowProc(HWND hwnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	switch (uMsg)
	{
	case WM_DESTROY:
		PostQuitMessage(0);
		return 0;

	case WM_PAINT:
	{
		PAINTSTRUCT ps;
		HDC hdc = BeginPaint(hwnd, &ps);
		RECT rect;
		GetClientRect(hwnd, &rect);
		FillRect(hdc, &rect, (HBRUSH)(COLOR_WINDOWFRAME));
		DrawEllipse(hdc, rect);
		EndPaint(hwnd, &ps);
		return 0;
	}

	case WM_SIZE:
		InvalidateRect(hwnd, nullptr, TRUE);
		return 0;

	case WM_COMMAND:
		if (LOWORD(wParam) == EXIT_ID)
		{
			PostQuitMessage(0);
		}
		return 0;

	case WM_LBUTTONDOWN:
	{
		POINT pt;
		GetCursorPos(&pt);
		ScreenToClient(hwnd, &pt);

		RECT rect;
		GetClientRect(hwnd, &rect);

		if (pt.x >= rect.left && pt.x <= rect.right &&
			pt.y >= rect.top && pt.y <= rect.bottom)
		{
			ellipseColor = RGB(rand() % 256, rand() % 256, rand() % 256);
			InvalidateRect(hwnd, nullptr, TRUE);
		}
		return 0;
	}
	}


	return DefWindowProc(hwnd, uMsg, wParam, lParam);
}

HMENU CreateMenus()
{
	HMENU hMenu = CreateMenu();
	HMENU hSubMenu = CreatePopupMenu();
	AppendMenu(hSubMenu, MF_STRING, EXIT_ID, "Exit");
	AppendMenu(hMenu, MF_STRING | MF_POPUP, (UINT_PTR)hSubMenu, "File");

	return hMenu;
}

void DrawEllipse(HDC hdc, RECT rect)
{
	HBRUSH brush = CreateSolidBrush(ellipseColor);
	//TODO: Перед Select нужно запомнить прежнее значение
	SelectObject(hdc, brush);
	Ellipse(hdc, rect.left, rect.top, rect.right, rect.bottom);
	DeleteObject(brush);
}

int WINAPI WinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, LPSTR lpCmdLine, int nShowCmd)
{
	const char CLASS_NAME[] = "Sample Window Class";

	WNDCLASS wc = {};
	wc.lpfnWndProc = WindowProc;
	wc.hInstance = hInstance;
	wc.lpszClassName = CLASS_NAME;

	RegisterClass(&wc);

	HWND hwnd = CreateWindowEx(
		0,
		CLASS_NAME,
		"App",
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		nullptr,
		CreateMenus(),
		hInstance,
		nullptr
	);

	ShowWindow(hwnd, nShowCmd); //TODO: nShowCmd???

	srand(static_cast<unsigned int>(time(nullptr)));

	MSG msg; //TODO: Как операционка узнает в какую процецуру отправлять сообщение
	while (GetMessage(&msg, nullptr, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return 0;
}

//TODO: только по эллипсу кликать