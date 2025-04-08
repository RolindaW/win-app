
#ifndef UNICODE  // Warning! Redundant if already set in project configuration
#define UNICODE
#endif

#ifndef _UNICODE  // Warning! Redundant if already set in project configuration
#define	_UNICODE
#endif

// Includes
#include <Windows.h>
//#include <tchar.h>  // Optional - build sinlge-byte, Multibyte Character Set (MBCS) and Unicode applications from same source; define "_TCHAR" data type, "_T"/"_TEXT" macro, "_tcslen" macro

#include "wglextcustom.h"

// Comment pragma - The linker searches for this library the same way as if specified on the command line
#pragma comment (lib, "opengl32.lib")

// Function prototypes
int WINAPI wWinMain(HINSTANCE, HINSTANCE, PWSTR, int);
LRESULT CALLBACK HelperWndProc(HWND, UINT, WPARAM, LPARAM);
LRESULT CALLBACK MainWndProc(HWND, UINT, WPARAM, LPARAM);
BOOL InitializeHelperWindow(void);
void TerminateHelperWindow(void);
BOOL InitializeMainWindow(void);
void TerminateMainWindow(void);
void* GetAnyGLFunctionAddress(const char*);
int IsWGLExtensionSupported(const char*);
BOOL LoadWGLExtensions(void);
BOOL CreateAdvancedRenderingContext(void);
void TerminateAdvancedRenderingContext(void);

// Typedefs
typedef struct _Window
{
	ATOM windowClass;
	HWND window;
	HDC deviceContext;
	int pixelFormatIndex;
	HGLRC renderingContext;
};

// Warning! Current architecture only support single main window
typedef struct _WindowBundle
{
	HINSTANCE instance;
	_Window helperWindow;
	_Window mainWindow;
	BOOL mainWindowShouldClose;
};

typedef struct _WGL
{
	PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB;
	PFNWGLGETEXTENSIONSSTRINGEXTPROC GetExtensionsStringEXT;
	PFNWGLGETPIXELFORMATATTRIBIVARBPROC GetPixelFormatAttribivARB;
	PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB;
	int ARB_pixel_format_supported;
	int ARB_create_context_supported;
};

#define wglGetExtensionsStringARB _wgl.GetExtensionsStringARB
#define wglGetExtensionsStringEXT _wgl.GetExtensionsStringEXT
#define wglGetPixelFormatAttribivARB _wgl.GetPixelFormatAttribivARB
#define wglCreateContextAttribsARB _wgl.CreateContextAttribsARB

// Global variables
_WindowBundle _windowBundle;
_WGL _wgl;

// Application entry-point function
int WINAPI wWinMain(HINSTANCE hInstance, HINSTANCE hPrevInstance, PWSTR pCmdLine, int nCmdShow)
{
	_windowBundle.instance = hInstance;

	// Initialize windows
	if (!InitializeHelperWindow()) {
		return EXIT_FAILURE;
	}

	if (!InitializeMainWindow()) {
		return EXIT_FAILURE;
	}

	// TODO: Load WGL extensions required to create advanced rendering context - Warning! Require "dummy" rendering context
	if (!LoadWGLExtensions()) {
		return EXIT_FAILURE;
	}

	// TODO: Create advanced rendering context using loaded extensions
	//if (!CreateAdvancedRenderingContext()) {
	//	return EXIT_FAILURE;
	//}

	// Enter main window message loop (i.e. the game loop)
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

		// Check if main window close requested
		running = !_windowBundle.mainWindowShouldClose;

	} while (running);

	// Release resources
	TerminateHelperWindow();
	TerminateAdvancedRenderingContext();
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
	if (!_windowBundle.helperWindow.windowClass) {
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

	if (!_windowBundle.helperWindow.window) {
		MessageBox(NULL, TEXT("Cannot create helper window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	// Hide helper window
	ShowWindow(_windowBundle.helperWindow.window, SW_HIDE);

	// Helper window message loop - Warning! Exit when no message is peeked
	MSG msg = {};

	while (PeekMessage(&msg, _windowBundle.helperWindow.window, 0, 0, PM_REMOVE)) {
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
	if (!_windowBundle.mainWindow.windowClass) {
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
		NULL);  // TODO: Pass per-window configuration data

	if (!_windowBundle.mainWindow.window) {
		MessageBox(NULL, TEXT("Cannot create main window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return EXIT_FAILURE;
	}

	SetPropW(_windowBundle.mainWindow.window, L"_WB", (HANDLE)&_windowBundle);

	// Show main window
	ShowWindow(_windowBundle.mainWindow.window, SW_SHOWDEFAULT);

	// Warning! Do not enter main window message loop HERE

	return TRUE;
}

// Main window class window procedure
LRESULT CALLBACK MainWndProc(HWND hWnd, UINT uMsg, WPARAM wParam, LPARAM lParam)
{
	_WindowBundle* windowBundle = (_WindowBundle*)GetProp(hWnd, L"_WB");

	if (uMsg == WM_NCCREATE) {
		// TODO: Handle per-window configuration data

		return DefWindowProc(hWnd, uMsg, wParam, lParam);
	}
	
	switch (uMsg)
	{
		// TODO: Handle required messages for input handling

		case WM_PAINT:
		{
			// TODO: Remove - Warning! Not necessary
			PAINTSTRUCT ps;
			HDC hdc = BeginPaint(hWnd, &ps);

			FillRect(hdc, &ps.rcPaint, (HBRUSH)(COLOR_WINDOW + 1));

			EndPaint(hWnd, &ps);

			return 0;
		}

		case WM_CLOSE:
			if (MessageBox(hWnd, TEXT("Quit application?"), TEXT("Exit!"), MB_OKCANCEL) == IDOK)
			{
				windowBundle->mainWindowShouldClose = FALSE;
			}

			return 0;
	}

	return DefWindowProc(hWnd, uMsg, wParam, lParam);
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

void* GetAnyGLFunctionAddress(const char* name)
{
	// Try get OpenGL function pointer from version above 1.1
	void* p = (void*)wglGetProcAddress(name);

	// Check error - Warning! Return value may be implementation-specific on failure: NULL - 0 - (default), 1, 2, 3 or -1
	if (p == 0 ||
		(p == (void*)0x1) || (p == (void*)0x2) || (p == (void*)0x3) ||
		(p == (void*)-1))
	{
		// Try get OpenGL function pointer from version 1.1 (i.e. from "opengl32.dll" itself)
		HMODULE module = LoadLibraryA("opengl32.dll");
		p = (void*)GetProcAddress(module, name);
	}

	return p;
}

int IsWGLExtensionSupported(const char* extension)
{
	const char* extensionList = NULL;  // Warning! Space-separated extension list expected

	if (_wgl.GetExtensionsStringARB)  // Warning! 
		extensionList = wglGetExtensionsStringARB(wglGetCurrentDC());
	else if (_wgl.GetExtensionsStringEXT)
		extensionList = wglGetExtensionsStringEXT();

	if (!extensionList)
		return FALSE;

	// TODO: Check if requested extension is on the extension list
	return IsStringInExtensionString(extension, extensionList);
}

BOOL LoadWGLExtensions(void)
{
	// 1 - Create helper window "dummy" rendering context - Warning! Not necessary to store any information (i.e. pixel format - and chosen index, rendering context)
	
	HDC helperWindowDeviceContext = GetDC(_windowBundle.helperWindow.window);

	PIXELFORMATDESCRIPTOR simplePixelFormat = {};

	simplePixelFormat.nSize = sizeof(PIXELFORMATDESCRIPTOR);
	simplePixelFormat.nVersion = 1;
	simplePixelFormat.dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER;
	simplePixelFormat.iPixelType = PFD_TYPE_RGBA;
	simplePixelFormat.cColorBits = 24;

	int simplePixelFormatIndex = ChoosePixelFormat(helperWindowDeviceContext, &simplePixelFormat);
	if (!simplePixelFormatIndex) {
		MessageBox(NULL, TEXT("Cannot match an appropriate pixel format for helper window device context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	if (!SetPixelFormat(helperWindowDeviceContext, simplePixelFormatIndex, &simplePixelFormat)) {
		MessageBox(NULL, TEXT("Cannot set the pixel format to helper window device context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	HGLRC dummyRenderingContext = wglCreateContext(helperWindowDeviceContext);
	if (!dummyRenderingContext) {
		MessageBox(NULL, TEXT("Cannot create dummy rendering context for helper window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	if (!wglMakeCurrent(helperWindowDeviceContext, dummyRenderingContext)) {
		MessageBox(NULL, TEXT("Cannot make helper window dummy rendering context current"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// TODO: 2 - Load required WGL extensions

	// Load WGL extension functions
	_wgl.GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
		GetAnyGLFunctionAddress("wglGetExtensionsStringARB");  // Warning! Old and widely-implemented extension - unlikely not to be found
	_wgl.GetExtensionsStringEXT = (PFNWGLGETEXTENSIONSSTRINGEXTPROC)
		GetAnyGLFunctionAddress("wglGetExtensionsStringEXT");
	_wgl.GetPixelFormatAttribivARB = (PFNWGLGETPIXELFORMATATTRIBIVARBPROC)
		GetAnyGLFunctionAddress("wglGetPixelFormatAttribivARB");
	_wgl.CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
		GetAnyGLFunctionAddress("wglCreateContextAttribsARB");

	// TODO: Check if WGL extensions are available
	_wgl.ARB_pixel_format_supported =
		IsWGLExtensionSupported("WGL_ARB_pixel_format");
	_wgl.ARB_create_context_supported =
		IsWGLExtensionSupported("WGL_ARB_create_context");

	// 3 - Delete helper window "dummy" rendering context (no longer necessary after loading WGL extensions)  
	//wglMakeCurrent(helperWindowDeviceContext, NULL);
	wglDeleteContext(_windowBundle.helperWindow.renderingContext);  // Warning! Also makes rendering context not current if required

	return TRUE;
}

BOOL CreateAdvancedRenderingContext(void)
{
	// TODO: Get advanced pixel format



	// TODO: Create advanced rendering context



	return TRUE;
}

void TerminateAdvancedRenderingContext(void)
{
	wglDeleteContext(_windowBundle.mainWindow.renderingContext);
}
