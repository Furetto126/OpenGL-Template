#pragma once

#include "Window.h"

namespace AppGL
{
        class App
        {
        public:
                Window window;
                explicit App(AppOptions options);
                virtual ~App() = default;
                
                virtual void onStart()   = 0;
                virtual void onUpdate()  = 0;
                virtual void onDestroy() = 0;
                
                void run();
                void close() const;
        };
        
        extern App* InitApp();
}