
#ifndef UNICODE  // Warning! Redundant if already set in project configuration
#define UNICODE
#endif

#ifndef _UNICODE  // Warning! Redundant if already set in project configuration
#define	_UNICODE
#endif

// Includes
#include <Windows.h>
//#include <tchar.h>  // Optional - build sinlge-byte, Multibyte Character Set (MBCS) and Unicode applications from same source; define "_TCHAR" data type, "_T"/"_TEXT" macro, "_tcslen" macro

#include "glloader.h"
#include "triangle.h"

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
BOOL LoadWGLExtensions(void);
BOOL CreateAdvancedRenderingContext(void);
void TerminateAdvancedRenderingContext(void);

// Typedefs

typedef struct _RenderingContextDescriptor
{
	int majorVersion = 4;
	int minorVersion = 6;
	BOOL coreProfile = TRUE;
	BOOL forward;
	BOOL debug;
	BOOL robustness;
	BOOL noError;
	BOOL flush;
};

typedef struct _Window
{
	ATOM windowClass;
	HWND window;
	HDC deviceContext;
	int pixelFormatIndex;
	HGLRC renderingContext;
	_RenderingContextDescriptor renderingContextDescriptor;
};

// Warning! Current architecture only support single main window
typedef struct _WindowBundle
{
	HINSTANCE instance;
	_Window helperWindow;
	_Window mainWindow;
	BOOL mainWindowShouldClose;
};

// Global variables
_WindowBundle _windowBundle;

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

	// Load WGL extensions required to create advanced rendering context - Warning! Require "dummy" rendering context
	if (!LoadWGLExtensions()) {
		return EXIT_FAILURE;
	}

	// Create advanced rendering context using loaded extensions
	if (!CreateAdvancedRenderingContext()) {
		return EXIT_FAILURE;
	}

	InitTriangle();

	// Enter main window message loop (i.e. the game loop)
	BOOL running = TRUE;

	do
	{
		// Render
		DrawTriangle();

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
	TerminateTriangle();

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
				windowBundle->mainWindowShouldClose = TRUE;
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

BOOL LoadWGLExtensions(void)
{
	// 1 - Create helper window "dummy" rendering context - Warning! Not necessary to store any information (i.e. pixel format - and chosen index, rendering context)
	
	// 1.1 - Describe required pixel format, find best match within supported by helper window device context and set to it
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

	// 1.2 - Create helper window "dummy" rendering context and make it current
	HGLRC dummyRenderingContext = wglCreateContext(helperWindowDeviceContext);
	if (!dummyRenderingContext) {
		MessageBox(NULL, TEXT("Cannot create dummy rendering context for helper window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	if (!wglMakeCurrent(helperWindowDeviceContext, dummyRenderingContext)) {
		MessageBox(NULL, TEXT("Cannot make helper window dummy rendering context current"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// 2 - Load required WGL extension functions
	if (!LoadWGL()) {
		MessageBox(NULL, TEXT("Cannot load WGL extension functions on helper window dummy rendering context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// 3 - Delete helper window "dummy" rendering context - Warning! No longer necessary once WGL extensions loaded
	//wglMakeCurrent(helperWindowDeviceContext, NULL);
	wglDeleteContext(_windowBundle.helperWindow.renderingContext);  // Warning! Also makes rendering context not current if required

	return TRUE;
}

BOOL CreateAdvancedRenderingContext(void)
{
	// 1 - Describe required pixel format - Warning! Use loaded WGL extensions, find best match within supported by main window device context and set to it

	// TODO: Use enumerations from WGL_ARB_multisample and WGL_ARB_framebuffer_sRGB WGL extensions if available
	const int pixelFormatAttribList[] =
	{
		WGL_DRAW_TO_WINDOW_ARB, GL_TRUE,
		WGL_SUPPORT_OPENGL_ARB, GL_TRUE,
		WGL_DOUBLE_BUFFER_ARB, GL_TRUE,
		WGL_PIXEL_TYPE_ARB, WGL_TYPE_RGBA_ARB,
		WGL_COLOR_BITS_ARB, 32,
		WGL_DEPTH_BITS_ARB, 24,
		WGL_STENCIL_BITS_ARB, 8,
		0, // Terminate
	};

	_windowBundle.mainWindow.deviceContext = GetDC(_windowBundle.mainWindow.window);
	int advancedPixelFormatIndex;
	UINT numFormats;

	if (!wglChoosePixelFormatARB(_windowBundle.mainWindow.deviceContext, pixelFormatAttribList, NULL, 1, &advancedPixelFormatIndex, &numFormats)) {
		MessageBox(NULL, TEXT("Cannot match an appropriate pixel format for main window device context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	PIXELFORMATDESCRIPTOR advancedPixelFormat = {};

	if (!DescribePixelFormat(_windowBundle.mainWindow.deviceContext, advancedPixelFormatIndex, sizeof(PIXELFORMATDESCRIPTOR), &advancedPixelFormat)) {
		MessageBox(NULL, TEXT("Cannot describe matched pixel format for main window device context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	if (!SetPixelFormat(_windowBundle.mainWindow.deviceContext, advancedPixelFormatIndex, &advancedPixelFormat)) {
		MessageBox(NULL, TEXT("Cannot set the pixel format to main window device context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// TODO: 2 - Create main window advanced rendering context - Warning! Use loaded WGL extensions - and make it current

	_RenderingContextDescriptor* renderingContextDescriptor = &_windowBundle.mainWindow.renderingContextDescriptor;
	int flags = 0;
	int profileMask = 0;

	// Warning! Corresponding WGL extension WGL_ARB_create_context availability already checked
	if (renderingContextDescriptor->forward) {
		flags |= WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB;
	}
	
	// TODO: Check if corresponding extension WGL_ARB_create_context_profile is available; otherwise, report error
	if (renderingContextDescriptor->coreProfile)
		profileMask |= WGL_CONTEXT_CORE_PROFILE_BIT_ARB;
	else
		profileMask |= WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB;

	// Warning! Corresponding WGL extension WGL_ARB_create_context availability already checked
	if (renderingContextDescriptor->debug) {
		flags |= WGL_CONTEXT_DEBUG_BIT_ARB;
	}

	if (renderingContextDescriptor->robustness)
	{
		// TODO: Check if corresponding extension WGL_ARB_create_context_robustness is available; otherwise, report error
		// TODO: Add reset notification strategy rendering context attribute into attribute list
		flags |= WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB;
	}

	if (renderingContextDescriptor->flush)
	{
		// TODO: Check if corresponding extension WGL_ARB_context_flush_control is available; otherwise, report error
		// TODO: Add context release behavior rendering context attribute into attribute list
	}

	if (renderingContextDescriptor->noError)
	{
		// TODO: Check if corresponding extension WGL_ARB_create_context_no_error is available; otherwise, report error
		// TODO: Add context opengl no error rendering context attribute into attribute list
	}

	const int renderingContextAttribList[] =
	{
		WGL_CONTEXT_MAJOR_VERSION_ARB, renderingContextDescriptor->majorVersion,
		WGL_CONTEXT_MINOR_VERSION_ARB, renderingContextDescriptor->minorVersion,
		WGL_CONTEXT_FLAGS_ARB, flags,
		WGL_CONTEXT_PROFILE_MASK_ARB, profileMask,
		0, // Terminate
	};

	_windowBundle.mainWindow.renderingContext = wglCreateContextAttribsARB(_windowBundle.mainWindow.deviceContext, NULL, renderingContextAttribList);
	if (!_windowBundle.mainWindow.renderingContext)
	{
		// TODO: Call "GetLastError" GL function to get error message
		MessageBox(NULL, TEXT("Cannot create advanced rendering context for main window"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	if (!wglMakeCurrent(_windowBundle.mainWindow.deviceContext, _windowBundle.mainWindow.renderingContext)) {
		MessageBox(NULL, TEXT("Cannot make main window advanced rendering context current"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	// EXTRA - Load required WGL extension functions
	if (!LoadModernGL()) {
		MessageBox(NULL, TEXT("Cannot load modern GL functions on main window advanced rendering context"), TEXT("Error!"), MB_OK | MB_ICONERROR);

		return FALSE;
	}

	return TRUE;
}

void TerminateAdvancedRenderingContext(void)
{
	wglDeleteContext(_windowBundle.mainWindow.renderingContext);
}
