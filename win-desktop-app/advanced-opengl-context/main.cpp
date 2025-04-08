
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
LRESULT CALLBACK HelperWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitializeHelperWindow(void);
void TerminateHelperWindow(void);
BOOL InitializeMainWindow(void);
void TerminateMainWindow(void);
BOOL CreateDummyRenderingContext(void);

// Typedefs
typedef struct _Window
{
	ATOM windowClass;
	HWND window;
	HDC deviceContext;
	int pixelFormatIndex;
	HGLRC renderingContext;
};

typedef struct _WindowBundle
{
	HINSTANCE instance;
	_Window helperWindow;
	_Window mainWindow;
};

// Global variables
_WindowBundle _windowBundle;

// Application entry-point function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	_windowBundle.instance = hInstance;

	// Initialize windows
	if (!InitializeHelperWindow())
	{
		return EXIT_FAILURE;
	}

	if (!InitializeMainWindow())
	{
		return EXIT_FAILURE;
	}

	// TODO: Create "dummy" rendering context and load required extensions


	// TODO: Create advanced rendering context using loaded extensions


	// TODO: Enter main window message loop (i.e. the game loop) - Warning! Do not use conventional window message loop

	BOOL running = TRUE;

	do
	{
		// TODO: Render

		// Swap buffers
		SwapBuffers(_windowBundle.mainWindow.deviceContext);

		// Pool main window events
		MSG msg = {};

		while (PeekMessage(&msg, _windowBundle.mainWindow.window, 0, 0, PM_REMOVE))
		{
			TranslateMessage(&msg);
			DispatchMessage(&msg);
		}

		// TODO: Check if running (i.e. main window closed)
		running = FALSE;

	} while (running);

	// Terminate windows
	TerminateHelperWindow();
	TerminateMainWindow();

	return EXIT_SUCCESS;
}

BOOL InitializeHelperWindow(void)
{
	// Register helper window class
	WNDCLASS wc = {};

	wc.style = CS_OWNDC;
	wc.lpfnWndProc = HelperWndProc;
	wc.hInstance = _windowBundle.instance;
	wc.lpszClassName = TEXT("Helper Window Class");

	_windowBundle.helperWindow.windowClass = RegisterClass(&wc);
	if (!_windowBundle.helperWindow.windowClass)
	{
		MessageBox(NULL, TEXT("Cannot register helper window class"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// Create helper window
	_windowBundle.helperWindow.window = CreateWindow(
		MAKEINTATOM(_windowBundle.helperWindow.windowClass),
		TEXT("Helper Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		1, 1,
		NULL,
		NULL,
		_windowBundle.instance,
		NULL);

	if (!_windowBundle.helperWindow.window)
	{
		MessageBox(NULL, TEXT("Cannot create helper window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	// Hide helper window
	ShowWindow(_windowBundle.helperWindow.window, SW_HIDE);

	// Helper window message loop - Warning! Exit when no message is peeked
	MSG msg = {};

	while (PeekMessage(&msg, _windowBundle.helperWindow.window, 0, 0, PM_REMOVE))
	{
		TranslateMessage(&msg);
		DispatchMessage(&msg);
	}

	return TRUE;
}

// Helper window class window procedure
LRESULT CALLBACK HelperWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	return DefWindowProc(hWnd, uMsg, wParam, lParam);
}

void TerminateHelperWindow(void)
{
	if (_windowBundle.helperWindow.window) {
		DestroyWindow(_windowBundle.helperWindow.window);
	}
	if (_windowBundle.helperWindow.windowClass) {
		UnregisterClass(MAKEINTATOM(_windowBundle.helperWindow.windowClass), _windowBundle.instance);
	}
}

BOOL InitializeMainWindow(void)
{
	// Register main window class
	WNDCLASS wc = {};

	wc.style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC;
	wc.lpfnWndProc = MainWndProc;
	wc.hInstance = _windowBundle.instance;
	wc.lpszClassName = TEXT("Main Window Class");

	_windowBundle.mainWindow.windowClass = RegisterClass(&wc);
	if (!_windowBundle.mainWindow.windowClass)
	{
		MessageBox(NULL, TEXT("Cannot register main window class"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// Create main window
	_windowBundle.mainWindow.window = CreateWindow(
		MAKEINTATOM(_windowBundle.mainWindow.windowClass),
		TEXT("Main Window"),
		WS_OVERLAPPEDWINDOW,
		CW_USEDEFAULT, CW_USEDEFAULT,
		CW_USEDEFAULT, CW_USEDEFAULT,
		NULL,
		NULL,
		_windowBundle.instance,
		NULL);  // TODO: Provide per-window data

	if (!_windowBundle.mainWindow.window)
	{
		MessageBox(NULL, TEXT("Cannot create main window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	// Show main window
	ShowWindow(_windowBundle.mainWindow.window, SW_SHOWDEFAULT);

	// Warning! Do not enter main window message loop HERE

	return TRUE;
}

// Main window class window procedure
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
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
		//OnSize(hWnd, (UINT)wParam, width, height);
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
		// EN GLFW ellos solo usan WM_COSE; y lo hacen para poder a true el flag "should_close", que podemos consultar desde "fuera" en el game loop
		// para saber cuando el suuario quiere cerrar la ventana.
		// Creo que para esto tendriamos que crear la estructura de configuracion que se para per-window al crear para poder interactuar y leer el flag
		// desde fuera.
		//
		if (MessageBox(hWnd, TEXT("Quit application?"), TEXT("Exit!"), MB_OKCANCEL) == IDOK)
		{
			// TODO: Check this part because collides with Destroy on main Terminate procedure
			//
			// !!!
			// !!!
			// Aqui lo unico que habria que hacer seria poner el flag "should_close" a true y nada mas.
			// Lo del message box preguntando es opcional, lo podemos dejar sin mas.
			//
			// Esto no hay que hacer, el destroy se hace fuera
			//DestroyWindow(hWnd);  // Put "WM_DESTROY" message on the queue - Warning! Default behavior if message unhandled
		}
		break;

	// TODO: este tampoco hace falta, lo podemos borrar porque vamos  aterminar la ejecuicion con el flag que pondremos en WM_CLOSE
	// y ademas no deneceitamos el WM_QUIT para salir del loop porque no vamos a usar la funcion GetMesssage - y de hecho
	// no podemos poruqe  sino nos quedariamos de forma indefinida en el loop del polling de los mensajes de ventana en el game loop
	case WM_DESTROY:  // Important! Message sent after window is removed from screen but before destruction occurs
		PostQuitMessage(0);  // Put "WM_QUIT" message on the queue - Important! Cause "GetMessage()" return 0 i.e. break out of message loop
		break;

	default:
		return DefWindowProc(hWnd, uMsg, wParam, lParam);  // Default window procedure for unhandled message
	}

	return 0;  // Indicate message handled
}

void TerminateMainWindow(void)
{
	if (_windowBundle.mainWindow.window) {
		DestroyWindow(_windowBundle.mainWindow.window);
	}
	if (_windowBundle.mainWindow.windowClass) {
		UnregisterClass(MAKEINTATOM(_windowBundle.mainWindow.windowClass), _windowBundle.instance);
	}
}
