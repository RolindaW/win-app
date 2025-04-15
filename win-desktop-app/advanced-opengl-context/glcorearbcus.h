// Custom "glcorearb.h" header

#pragma once

#include <Windows.h>  // Required for Windwos typdefs (e.g. "HDC", "UINT", "HGLRC") on WGL extension functions

// Core OpenGL and modern OpenGL extensions

// GL_VERSION_1_0

// Enumerations
#define GL_FALSE 0
#define GL_TRUE 1
#define GL_TRIANGLES 0x0004
#define GL_COLOR 0x1800

// Typedefs
typedef unsigned int GLenum;
typedef float GLfloat;
typedef int GLint;
typedef int GLsizei;
typedef unsigned int GLuint;

// Function pointer typdefs
typedef void (WINAPI* PFNGLDRAWARRAYSPROC) (GLenum mode, GLint first, GLsizei count);

typedef void (WINAPI* PFNTITIKAKA) (GLenum mode, GLint first, GLsizei count);

// GL_VERSION_2_0

// Enumerations
#define GL_FRAGMENT_SHADER 0x8B30
#define GL_VERTEX_SHADER 0x8B31

// Typedefs
typedef char GLchar;

// Function pointer typdefs
typedef GLuint(WINAPI* PFNGLCREATEPROGRAMPROC) (void);
typedef void (WINAPI* PFNGLLINKPROGRAMPROC) (GLuint program);
typedef void (WINAPI* PFNGLUSEPROGRAMPROC) (GLuint program);
typedef void (WINAPI* PFNGLDELETEPROGRAMPROC) (GLuint program);

typedef GLuint(WINAPI* PFNGLCREATESHADERPROC) (GLenum type);
typedef void (WINAPI* PFNGLSHADERSOURCEPROC) (GLuint shader, GLsizei count, const GLchar* const* string, const GLint* length);
typedef void (WINAPI* PFNGLCOMPILESHADERPROC) (GLuint shader);
typedef void (WINAPI* PFNGLATTACHSHADERPROC) (GLuint program, GLuint shader);
typedef void (WINAPI* PFNGLDELETESHADERPROC) (GLuint shader);

// GL_VERSION_3_0

// Function pointer typdefs
typedef void (WINAPI* PFNGLBINDVERTEXARRAYPROC) (GLuint array);
typedef void (WINAPI* PFNGLDELETEVERTEXARRAYSPROC) (GLsizei n, const GLuint* arrays);
typedef void (WINAPI* PFNGLCLEARBUFFERFVPROC) (GLenum buffer, GLint drawbuffer, const GLfloat* value);

// GL_VERSION_4_5

// Function pointer typdefs 
typedef void (WINAPI* PFNGLCREATEVERTEXARRAYSPROC)(GLsizei n, GLuint* arrays);

// WGL window system extensions

// Typedef - WGL extension function pointers
typedef const char* (WINAPI* PFNWGLGETEXTENSIONSSTRINGARBPROC)(HDC);
typedef BOOL(WINAPI* PFNWGLCHOOSEPIXELFORMATARBPROC)(HDC, const int*, const FLOAT*, UINT, int*, UINT*);
typedef HGLRC(WINAPI* PFNWGLCREATECONTEXTATTRIBSARBPROC)(HDC, HGLRC, const int*);

// Typedef - WGL extension enumerations

// Pixel format

// WGL_ARB_pixel_format
#define WGL_DRAW_TO_WINDOW_ARB 0x2001
#define WGL_SUPPORT_OPENGL_ARB 0x2010
#define WGL_DOUBLE_BUFFER_ARB 0x2011
#define WGL_PIXEL_TYPE_ARB 0x2013
#define WGL_COLOR_BITS_ARB 0x2014
#define WGL_DEPTH_BITS_ARB 0x2022
#define WGL_STENCIL_BITS_ARB 0x2023

#define WGL_TYPE_RGBA_ARB 0x202B

// WGL_ARB_multisample
#define WGL_SAMPLES_ARB 0x2042

// WGL_ARB_framebuffer_sRGB
#define WGL_FRAMEBUFFER_SRGB_CAPABLE_ARB 0x20A9

// Rendering context

// WGL_ARB_create_context, WGL_ARB_create_context_profile
#define WGL_CONTEXT_MAJOR_VERSION_ARB 0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB 0x2092
#define WGL_CONTEXT_LAYER_PLANE_ARB 0x2093
#define WGL_CONTEXT_FLAGS_ARB 0x2094
#define WGL_CONTEXT_PROFILE_MASK_ARB 0x9126

#define WGL_CONTEXT_DEBUG_BIT_ARB 0x0001
#define WGL_CONTEXT_FORWARD_COMPATIBLE_BIT_ARB 0x0002

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB 0x00000001
#define WGL_CONTEXT_COMPATIBILITY_PROFILE_BIT_ARB 0x00000002

// WGL_ARB_create_context_robustness
#define WGL_CONTEXT_ROBUST_ACCESS_BIT_ARB 0x00000004

#define WGL_CONTEXT_RESET_NOTIFICATION_STRATEGY_ARB 0x8256

#define WGL_NO_RESET_NOTIFICATION_ARB 0x8261
#define WGL_LOSE_CONTEXT_ON_RESET_ARB 0x8252

// WGL_ARB_create_context_no_error
#define WGL_CONTEXT_OPENGL_NO_ERROR_ARB 0x31B3

// WGL_ARB_context_flush_control
#define WGL_CONTEXT_RELEASE_BEHAVIOR_ARB 0x2097

#define WGL_CONTEXT_RELEASE_BEHAVIOR_NONE_ARB 0x0000
#define WGL_CONTEXT_RELEASE_BEHAVIOR_FLUSH_ARB 0x2098
