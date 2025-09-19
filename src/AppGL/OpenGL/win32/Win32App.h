#pragma once

#ifndef APPGLNATIVE
#define APPGLNATIVE
#endif

#include "Win32Window.h"

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_create_context.txt for all values
#define WGL_CONTEXT_MAJOR_VERSION_ARB             0x2091
#define WGL_CONTEXT_MINOR_VERSION_ARB             0x2092
#define WGL_CONTEXT_PROFILE_MASK_ARB              0x9126

#define WGL_CONTEXT_CORE_PROFILE_BIT_ARB          0x00000001

// See https://www.khronos.org/registry/OpenGL/extensions/ARB/WGL_ARB_pixel_format.txt for all values
#define WGL_DRAW_TO_WINDOW_ARB                    0x2001
#define WGL_ACCELERATION_ARB                      0x2003
#define WGL_SUPPORT_OPENGL_ARB                    0x2010
#define WGL_DOUBLE_BUFFER_ARB                     0x2011
#define WGL_PIXEL_TYPE_ARB                        0x2013
#define WGL_COLOR_BITS_ARB                        0x2014
#define WGL_DEPTH_BITS_ARB                        0x2022
#define WGL_STENCIL_BITS_ARB                      0x2023

#define WGL_FULL_ACCELERATION_ARB                 0x2027
#define WGL_TYPE_RGBA_ARB                         0x202B

namespace AppGL
{
        class Win32App
        {
        public:
                Win32Window window;
                
                explicit Win32App(HINSTANCE hInstance, AppOptions options);
                virtual ~Win32App() = default;
                
                virtual void onStart()   = 0;
                virtual void onUpdate()  = 0;
                virtual void onDestroy() = 0;
                
                void run();
                void close();
        };
        
        extern Win32App* InitApp(HINSTANCE hinstance);
}