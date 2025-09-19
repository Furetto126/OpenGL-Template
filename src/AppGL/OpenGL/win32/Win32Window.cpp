#include "Win32Window.h"

#include <glad/glad.h>

#include <stdexcept>
#include <utility>

// Mostly from https://gist.github.com/nickrolfe/1127313ed1dbf80254b614a721b3ee9c
// ------------------------------------------------------------------------------

typedef HGLRC WINAPI wglCreateContextAttribsARB_type(HDC hdc, HGLRC hShareContext,
                                                     const int *attribList);
wglCreateContextAttribsARB_type *wglCreateContextAttribsARB;

typedef BOOL WINAPI wglChoosePixelFormatARB_type(HDC hdc, const int *piAttribIList,
                                                 const FLOAT *pfAttribFList, UINT nMaxFormats, int *piFormats, UINT *nNumFormats);
wglChoosePixelFormatARB_type *wglChoosePixelFormatARB;

namespace AppGL
{
        Win32Window::Win32Window(HINSTANCE hInstance, AppOptions options): options(std::move(options))
        {
                windowHandle  = createWindow(hInstance);
                deviceContext = GetDC(windowHandle);
                GLContext     = initGL();
                
                ShowWindow(windowHandle, SW_SHOW);
                UpdateWindow(windowHandle);
        }
        
        Win32Window::~Win32Window()
        {
                if (windowHandle) DestroyWindow(windowHandle);
        }
        
        void Win32Window::pollEvents()
        {
                MSG msg;
                while (PeekMessageA(&msg, nullptr, 0, 0, PM_REMOVE)) {
                        if (msg.message == WM_QUIT) shouldClose = true;
                        else {
                                TranslateMessage(&msg);
                                DispatchMessageA(&msg);
                        }
                }
        }
        
        LRESULT CALLBACK Win32Window::windowCallback(HWND hwnd, UINT msg, WPARAM wParam, LPARAM lParam)
        {
               LRESULT result = 0;
               switch (msg) {
                       case WM_CLOSE:
                       case WM_DESTROY:
                               PostQuitMessage(0);
                               break;
                       default:
                               result = DefWindowProcA(hwnd, msg, wParam, lParam);
                               break;
               }
               
               return result;
        }
        
        HWND Win32Window::createWindow(HINSTANCE instance) const
        {
                WNDCLASSA windowClass = {
                        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                        .lpfnWndProc = windowCallback,
                        .hInstance = instance,
                        .hCursor = LoadCursor(nullptr, IDC_ARROW),
                        .hbrBackground = nullptr,
                        .lpszClassName = "WGLWindow"
                };
                
                if (!RegisterClassA(&windowClass))
                        throw std::runtime_error("Failed to register window.");
                
                RECT rect = {
                        .right  = (LONG)(options.width),
                        .bottom = (LONG)options.height
                };
                DWORD windowStyle = WS_OVERLAPPEDWINDOW;
                AdjustWindowRect(&rect, windowStyle, false);
                
                HWND handle = CreateWindowExA(
                        0,
                        windowClass.lpszClassName,
                        options.title,
                        windowStyle,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        rect.right - rect.left,
                        rect.bottom - rect.top,
                        nullptr,
                        nullptr,
                        instance,
                        nullptr
                );
                
                if (!handle)
                        throw std::runtime_error("Failed to create window.");
                
                return handle;
        }
        
        HGLRC Win32Window::initGL()
        {
                initGLExtensions();
                
                if (!options.pixelFormatAttributes.has_value())
                        throw std::runtime_error("Creating an OpenGL window without pixel format is kinda meh.");
                std::vector<int> pfaVector{};
                for (const auto& [flag, value] : options.pixelFormatAttributes.value()) {
                        pfaVector.push_back(flag);
                        pfaVector.push_back(value);
                }
                pfaVector.push_back(0); // Must terminate with a zero
                
                int pixelFormat;
                UINT numFormats;
                if (!wglChoosePixelFormatARB(deviceContext, pfaVector.data(), nullptr, 1, &pixelFormat, &numFormats)
                        || numFormats == 0)
                        throw std::runtime_error("Failed to set the OpenGL pixel format.");
                
                PIXELFORMATDESCRIPTOR pfd;
                DescribePixelFormat(deviceContext, pixelFormat, sizeof(pfd), &pfd);
                if (!SetPixelFormat(deviceContext, pixelFormat, &pfd))
                        throw std::runtime_error("Failed to set the OpenGL pixel format.");
                
                
                if (!options.GLAttributes.has_value())
                        throw std::runtime_error("Creating an OpenGL window without OpenGL attributes is kinda sad.");
                std::vector<int> glaVector{};
                for (const auto& [flag, value] : options.GLAttributes.value()) {
                        glaVector.push_back(flag);
                        glaVector.push_back(value);
                }
                glaVector.push_back(0); // Must terminate with a zero
                
                HGLRC hglrc = wglCreateContextAttribsARB(deviceContext, nullptr, glaVector.data());
                if (!hglrc)
                        throw std::runtime_error("Failed to create OpenGL context.");
                if (!wglMakeCurrent(deviceContext, hglrc))
                        throw std::runtime_error("Failed to activate OpenGL rendering context.");
                
                return hglrc;
        }
        
        void Win32Window::initGLExtensions()
        {
                WNDCLASSA window_class = {
                        .style = CS_HREDRAW | CS_VREDRAW | CS_OWNDC,
                        .lpfnWndProc = DefWindowProcA,
                        .hInstance = GetModuleHandle(nullptr),
                        .lpszClassName = "DummyWGL",
                };
                
                if (!RegisterClassA(&window_class)) {
                        throw std::runtime_error("Failed to register dummy OpenGL window.");
                }
                
                HWND dummyWindow = CreateWindowExA(
                        0,
                        window_class.lpszClassName,
                        "Dummy OpenGL Window",
                        0,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        CW_USEDEFAULT,
                        nullptr,
                        nullptr,
                        window_class.hInstance,
                        nullptr
                );
                
                if (!dummyWindow) {
                        throw std::runtime_error("Failed to create dummy OpenGL window.");
                }
                
                HDC dummyDC = GetDC(dummyWindow);
                
                PIXELFORMATDESCRIPTOR pfd = {
                        .nSize = sizeof(pfd),
                        .nVersion = 1,
                        .dwFlags = PFD_DRAW_TO_WINDOW | PFD_SUPPORT_OPENGL | PFD_DOUBLEBUFFER,
                        .iPixelType = PFD_TYPE_RGBA,
                        .cColorBits = 32,
                        .cAlphaBits = 8,
                        .cDepthBits = 24,
                        .cStencilBits = 8,
                        .iLayerType = PFD_MAIN_PLANE,
                };
                
                int pixelFormat = ChoosePixelFormat(dummyDC, &pfd);
                if (!pixelFormat)
                        throw std::runtime_error("Failed to find a suitable pixel format.");
                if (!SetPixelFormat(dummyDC, pixelFormat, &pfd))
                        throw std::runtime_error("Failed to set the pixel format.");
                
                HGLRC dummyContext = wglCreateContext(dummyDC);
                if (!dummyContext)
                        throw std::runtime_error("Failed to create a dummy OpenGL rendering context.");
                if (!wglMakeCurrent(dummyDC, dummyContext))
                        throw std::runtime_error("Failed to activate dummy OpenGL rendering context.");
                
                wglCreateContextAttribsARB = (wglCreateContextAttribsARB_type*)wglGetProcAddress(
                        "wglCreateContextAttribsARB");
                wglChoosePixelFormatARB = (wglChoosePixelFormatARB_type*)wglGetProcAddress(
                        "wglChoosePixelFormatARB");
                
                wglMakeCurrent(dummyDC, nullptr);
                wglDeleteContext(dummyContext);
                ReleaseDC(dummyWindow, dummyDC);
                DestroyWindow(dummyWindow);
        }
}