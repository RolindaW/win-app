
#ifndef UNICODE  // Warning! Redundant if already set in project configuration
#define UNICODE
#endif

#ifndef _UNICODE  // Warning! Redundant if already set in project configuration
#define	_UNICODE
#endif

#include <Windows.h>
//#include <tchar.h>  // Optional - build sinlge-byte, Multibyte Character Set (MBCS) and Unicode applications from same source; define "_TCHAR" data type, "_T"/"_TEXT" macro, "_tcslen" macro

// Warning! Declaration can be omitted if definition is placed before it is referenced
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam);  
void OnSize(HWND hwnd, UINT flag, int width, int height);

int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Warning! Use corresponding data type/function variants (e.g. WNDCLASSEX, RegisterClassEx(), CreateWindowEx()) to create a window with an extended window style

	// Register the window class
	const TCHAR WINDOW_CLASS_NAME[] = TEXT("Sample Window Class");

	WNDCLASS wc = {};

	wc.lpfnWndProc = WndProc;  // Important! Function pointer to window-procedure function
	wc.hInstance = hInstance;
	wc.lpszClassName = WINDOW_CLASS_NAME;

	if (!RegisterClass(&wc))
	{
		MessageBox(NULL, TEXT("Cannot register window class"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	// Create and display the window
	const TCHAR WINDOW_TITLE[] = TEXT("Windows Desktop App from Scratch");

	HWND hWnd = CreateWindow(
		WINDOW_CLASS_NAME,  // Important! Reference corresponding window class by name
		WINDOW_TITLE,
		WS_OVERLAPPEDWINDOW,  // Window style
		CW_USEDEFAULT,  // Initial position and size
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		CW_USEDEFAULT,
		NULL,  // Parent window
		NULL,  // Menu bar
		hInstance,  // Instance handle
		NULL);  // Additional application data

	if (hWnd == NULL)
	{
		MessageBox(NULL, TEXT("Cannot create application window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	ShowWindow(hWnd, nCmdShow);
	UpdateWindow(hWnd);

	// Message loop
	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
}

LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		// TODO: Handle per-window configuration data
		MessageBox(NULL, TEXT("Application window created"), TEXT("Success!"), MB_OK | MB_ICONINFORMATION);
	}

	switch (uMsg)
	{
		case WM_SIZE:
		{
			int width = LOWORD(lParam);  // Macro to get the low-order word
			int height = HIWORD(lParam);  // Macro to get the high-order word

			// Important! Handle message in a separate function to make code more modular
			// Warning! Use built-in multitasking facilities to avoid blocking current thread
			OnSize(hWnd, (UINT)wParam, width, height);
			break;
		}

		case WM_PAINT:
		{
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);  // Important! All painting occurs here, between "BeginPaint()" and "EndPaint()"

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			const TCHAR greeting[] = TEXT("Hello, there!");
			TextOut(hdc, 5, 5, greeting, wcslen(greeting));  // Warning! Use "_tcslen" (from "tchar.h") for encoding compatibility

			EndPaint(hWnd, &ps);
			break;
		}

		case WM_CLOSE:
			if (MessageBox(hWnd, TEXT("Quit application?"), TEXT("Exit!"), MB_OKCANCEL) == IDOK)
			{
				DestroyWindow(hWnd);  // Put "WM_DESTROY" message on the queue - Warning! Default behavior if message unhandled
			}
			break;
		
		case WM_DESTROY:  // Important! Message sent aftert window is removed from screen but before destruction occurs
			PostQuitMessage(0);  // Put "WM_QUIT" message on the queue - Important! Cause "GetMessage()" return 0 i.e. break out of message loop
			break;

		default:
			return DefWindowProc(hWnd, uMsg, wParam, lParam);  // Default window procedure for unhandled message
	}

	return 0;  // Indicate message handled
}

void OnSize(HWND hwnd, UINT flag, int width, int height)
{
	// TODO: Handle resizing
}