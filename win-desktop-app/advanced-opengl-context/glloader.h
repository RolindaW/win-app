#pragma once

#include "glcorearbcus.h"

// Core OpenGL and modern OpenGL extensions

// GL_VERSION_1_0

// Function pointers
extern PFNGLDRAWARRAYSPROC DrawArrays;

#define glDrawArrays DrawArrays

// GL_VERSION_2_0

// Function pointers
extern PFNGLCREATEPROGRAMPROC CreateProgram;
extern PFNGLLINKPROGRAMPROC LinkProgram;
extern PFNGLUSEPROGRAMPROC UseProgram;
extern PFNGLDELETEPROGRAMPROC DeleteProgram;

extern PFNGLCREATESHADERPROC CreateShader;
extern PFNGLSHADERSOURCEPROC ShaderSource;
extern PFNGLCOMPILESHADERPROC CompileShader;
extern PFNGLATTACHSHADERPROC AttachShader;
extern PFNGLDELETESHADERPROC DeleteShader;

#define glCreateProgram CreateProgram
#define glLinkProgram LinkProgram
#define glUseProgram UseProgram
#define glDeleteProgram DeleteProgram

#define glCreateShader CreateShader
#define glShaderSource ShaderSource
#define glCompileShader CompileShader
#define glAttachShader AttachShader
#define glDeleteShader DeleteShader

// GL_VERSION_3_0

// Function pointer
extern PFNGLBINDVERTEXARRAYPROC BindVertexArray;
extern PFNGLDELETEVERTEXARRAYSPROC DeleteVertexArrays;
extern PFNGLCLEARBUFFERFVPROC ClearBufferfv;

#define glBindVertexArray BindVertexArray
#define glDeleteVertexArrays DeleteVertexArrays
#define glClearBufferfv ClearBufferfv

// GL_VERSION_4_5

// Function pointers
extern PFNGLCREATEVERTEXARRAYSPROC CreateVertexArrays;

#define glCreateVertexArrays CreateVertexArrays

// WGL window system extensions

extern PFNWGLGETEXTENSIONSSTRINGARBPROC GetExtensionsStringARB;
extern PFNWGLCHOOSEPIXELFORMATARBPROC ChoosePixelFormatARB;
extern PFNWGLCREATECONTEXTATTRIBSARBPROC CreateContextAttribsARB;

#define wglGetExtensionsStringARB GetExtensionsStringARB
#define wglChoosePixelFormatARB ChoosePixelFormatARB
#define wglCreateContextAttribsARB CreateContextAttribsARB

// Function prototypes
BOOL LoadWGL();
BOOL LoadModernGL();
