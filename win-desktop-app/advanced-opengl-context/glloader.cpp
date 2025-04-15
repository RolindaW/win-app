#include "glloader.h"

PFNGLDRAWARRAYSPROC DrawArrays = nullptr;

PFNGLCREATEPROGRAMPROC CreateProgram = nullptr;
PFNGLLINKPROGRAMPROC LinkProgram = nullptr;
PFNGLUSEPROGRAMPROC UseProgram = nullptr;
PFNGLDELETEPROGRAMPROC DeleteProgram = nullptr;

PFNGLCREATESHADERPROC CreateShader = nullptr;
PFNGLSHADERSOURCEPROC ShaderSource = nullptr;
PFNGLCOMPILESHADERPROC CompileShader = nullptr;
PFNGLATTACHSHADERPROC AttachShader = nullptr;
PFNGLDELETESHADERPROC DeleteShader = nullptr;

PFNGLBINDVERTEXARRAYPROC BindVertexArray = nullptr;
PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays = nullptr;
PFNGLCLEARBUFFERFVPROC ClearBufferfv = nullptr;

PFNGLCREATEVERTEXARRAYSPROC CreateVertexArrays = nullptr;

PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB = nullptr;
PFNWGLCHOOSEPIXELFORMATARBPROC ChoosePixelFormatARB = nullptr;
PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB = nullptr;

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

int IsExtensionInExtensionList(const char* extension, const char* extensionList)
{
	// TODO: Code here
	return TRUE;
}

int IsWGLExtensionSupported(const char* extension)
{
	const char* extensionList = NULL;  // Warning! Space-separated extension list expected

	if (GetExtensionsStringARB)
		extensionList = wglGetExtensionsStringARB(wglGetCurrentDC());

	if (!extensionList)
		return FALSE;

	// Check if requested extension is on the extension list
	return IsExtensionInExtensionList(extension, extensionList);
}

// WGL window system extensions
BOOL LoadWGL()
{
	// Warning! This is used to check if an WGL extension is available
	GetExtensionsStringARB = (PFNWGLGETEXTENSIONSSTRINGARBPROC)
		GetAnyGLFunctionAddress("wglGetExtensionsStringARB"); // Warning! Old and widely-implemented extension - unlikely not to be available

	// TODO: Check if corresponding WGL extension (WGL_ARB_pixel_format) is availalbe ("IsWGLExtensionSupported" function); report error on failure
	//if (!IsWGLExtensionSupported("WGL_ARB_pixel_format")) {
	//	MessageBox(NULL, TEXT("Cannot find WGL_ARB_pixel_format WGL extension"), TEXT("Error!"), MB_OK | MB_ICONERROR);

	//	return FALSE;
	//}
	ChoosePixelFormatARB = (PFNWGLCHOOSEPIXELFORMATARBPROC)
		GetAnyGLFunctionAddress("wglChoosePixelFormatARB");

	// TODO: Check if corresponding WGL extension (WGL_ARB_create_context) is availalbe ("IsWGLExtensionSupported" function); report error on failure
	CreateContextAttribsARB = (PFNWGLCREATECONTEXTATTRIBSARBPROC)
		GetAnyGLFunctionAddress("wglCreateContextAttribsARB");

	return TRUE;
}

// Load modern OpenGL extensions
BOOL LoadModernGL()
{
	// TODO: Need to check if functions are available before try loading?
	DrawArrays = (PFNGLDRAWARRAYSPROC)
		GetAnyGLFunctionAddress("glDrawArrays");

	CreateProgram = (PFNGLCREATEPROGRAMPROC)
		GetAnyGLFunctionAddress("glCreateProgram");
	LinkProgram = (PFNGLLINKPROGRAMPROC)
		GetAnyGLFunctionAddress("glLinkProgram");
	UseProgram = (PFNGLUSEPROGRAMPROC)
		GetAnyGLFunctionAddress("glUseProgram");
	DeleteProgram = (PFNGLDELETEPROGRAMPROC)
		GetAnyGLFunctionAddress("glDeleteProgram");

	CreateShader = (PFNGLCREATESHADERPROC)
		GetAnyGLFunctionAddress("glCreateShader");
	ShaderSource = (PFNGLSHADERSOURCEPROC)
		GetAnyGLFunctionAddress("glShaderSource");
	CompileShader = (PFNGLCOMPILESHADERPROC)
		GetAnyGLFunctionAddress("glCompileShader");
	AttachShader = (PFNGLATTACHSHADERPROC)
		GetAnyGLFunctionAddress("glAttachShader");
	DeleteShader = (PFNGLDELETESHADERPROC)
		GetAnyGLFunctionAddress("glDeleteShader");

	BindVertexArray = (PFNGLBINDVERTEXARRAYPROC)
		GetAnyGLFunctionAddress("glBindVertexArray");
	DeleteVertexArrays = (PFNGLDELETEVERTEXARRAYSPROC)
		GetAnyGLFunctionAddress("glDeleteVertexArrays");
	ClearBufferfv = (PFNGLCLEARBUFFERFVPROC)
		GetAnyGLFunctionAddress("glClearBufferfv");

	CreateVertexArrays = (PFNGLCREATEVERTEXARRAYSPROC)
		GetAnyGLFunctionAddress("glCreateVertexArrays");

	return TRUE;
}
