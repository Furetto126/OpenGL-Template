#include <utility>

#include "AppGL/OpenGL/App.h"
#include "AppGL/shader/Shader.h"

class MyApp : public AppGL::App
{
public:
        explicit MyApp(AppGL::AppOptions options): AppGL::App(std::move(options)) {}
    
private:
        std::shared_ptr<Shader> shader;
        float frames = 0.0f;
        
        void onStart() override
        {
                // Init Shader with fragment and vertex code.
                const char vertCode[] = {
                #embed "../src/shadersource/vertex.vert"
                , '\0' };
                
                const char fragCode[] = {
                #embed "../src/shadersource/fragment.frag"
                , '\0' };
                
                shader = std::make_shared<Shader>(vertCode, fragCode);
                shader->addUniform("frames", frames);
        }
       
        void onUpdate() override
        {
                frames++;
                
                shader->setUniform("frames", frames);
                shader->drawFullScreenQuad();
        }
        
        void onDestroy() override {}
};

AppGL::App* AppGL::InitApp()
{
        AppOptions options;
        options.width = 800;
        options.height = 600;
        options.title = "OpenGL template window!";
        
        AppGL::App* app = new MyApp(options);
        return app;
}