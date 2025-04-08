
#ifndef UNICODE  // Warning! Redundant if already set in project configuration
#define UNICODE
#endif

#ifndef _UNICODE  // Warning! Redundant if already set in project configuration
#define	_UNICODE
#endif

#include <Windows.h>
#include <gl/GL.h>
//#include <tchar.h>  // Optional - build sinlge-byte, Multibyte Character Set (MBCS) and Unicode applications from same source; define "_TCHAR" data type, "_T"/"_TEXT" macro, "_tcslen" macro

// Comment pragma - The linker searches for this library the same way as if specified on the command line
//#pragma comment (lib, "gdi32.lib")
#pragma comment (lib, "opengl32.lib")

// Function prototypes
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int);
LRESULT CALLBACK WndProc(HWND, UINT, WPARAM, LPARAM);
void OnSize(HWND, UINT, int, int);

// Application entry-point function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	// Register the window class
	const TCHAR WINDOW_CLASS_NAME[] = TEXT("Sample Window Class");

	WNDCLASS wc = {};

	wc.style = CS_OWNDC;  // Important! Private display device context
	wc.lpfnWndProc = WndProc;  // Important! Function pointer to window-procedure function
	wc.hInstance = hInstance;  // Important! Window class owner
	wc.lpszClassName = WINDOW_CLASS_NAME;  // Important! Window class identifier

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
		MessageBox(NULL, TEXT("Cannot create application main window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	ShowWindow(hWnd, nCmdShow);  // "SW_SHOWDEFAULT" by default
	UpdateWindow(hWnd);  // Send first "WM_PAINT" message

	// Message loop
	MSG msg = {};

	while (GetMessage(&msg, NULL, 0, 0))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return EXIT_SUCCESS;
}

// Main window class window procedure
LRESULT CALLBACK WndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	if (uMsg == WM_NCCREATE)
	{
		// TODO: Handle per-window configuration data

		return DefWindowProc(hWnd, uMsg, wParam, lParam);  // Warning! Some lower level aspects of the window are uninitialized before WM_NCCREATE is processed - thus probably call before own processing
	}

	if (uMsg == WM_CREATE)
	{
		// Create an OpenGL rendering context
		
		// Get the private display device context of the window
		HDC hDeviceContext = GetDC(hWnd);

		// Describe required pixel format
		PIXELFORMATDESCRIPTOR requiredPixelFormat =
		{
			sizeof(PIXELFORMATDESCRIPTOR),	// size of this pdf
			1,								// version of this pdf
			PFD_DRAW_TO_WINDOW |			// pixel buffer property flags: support window
			PFD_SUPPORT_OPENGL |			// support OpenGL
			PFD_DOUBLEBUFFER,				// double buffering
			PFD_TYPE_RGBA,					// pixel data type: RGBA
			32,								// pixel color depth
			0, 0, 0, 0, 0, 0,				// color bits: ignored
			0, 0,							// alpha buffer: ignored
			0,								// accumulation buffer: ignored
			0, 0, 0, 0,						// accumulation bits: ignored
			24,								// z-buffer depth
			8,								// stencil buffer depth
			0,								// number of auxiliar buffers: ignored
			PFD_MAIN_PLANE,					// main layer
			0,								// number of overlay and underlay planes: ignored
			0, 0, 0							// layer masks: ignored
		};

		// Get the index of the pixel format supported by the device context that is the closest match to required pixel format description
		int indexPixelFormat = ChoosePixelFormat(hDeviceContext, &requiredPixelFormat);

		// Set the index to be the pixel format of the device context
		SetPixelFormat(hDeviceContext, indexPixelFormat, &requiredPixelFormat);

		// Create a rendering context
		if (HGLRC hRenderingContext = wglCreateContext(hDeviceContext))
		{
			// Make the rendering context (and corresponding device context) current
			if (wglMakeCurrent(hDeviceContext, hRenderingContext))
			{
				// Use OpenGL core functionality - Warning! Microsoft OpenGL implementation for Windows (support up to version 1.1)
				const GLubyte* aOpenGLVersion = glGetString(GL_VERSION);
				
				// TODO: Convert char string to wide char string - Warning! Can not just cast "(wchar_t*)aOpenGLVersion"
				MessageBox(NULL, (wchar_t*)aOpenGLVersion, TEXT("OpenGL Version"), MB_OK | MB_ICONINFORMATION);

				// Make the rendering context not current
				wglMakeCurrent(hDeviceContext, NULL);
			}
			else
			{
				MessageBox(NULL, TEXT("Cannot make the rendering context current"), TEXT("Error!"), MB_OK | MB_ICONERROR);
			}

			// Delete the rendering context - Warning! It also makes the rendering context not current if required
			wglDeleteContext(hRenderingContext);
		}
		else
		{
			MessageBox(NULL, TEXT("Cannot create a rendering context"), TEXT("Error!"), MB_OK | MB_ICONERROR);
		}

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

		case WM_DESTROY:  // Important! Message sent after window is removed from screen but before destruction occurs
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
