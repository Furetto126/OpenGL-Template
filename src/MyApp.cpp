#include <utility>

#include "AppGL/OpenGL/glfw/App.h"
#include "AppGL/shader/Shader.h"
#include "AppGL/shader/ComputeShader.h"

#include <memory>

class MyApp : public AppGL::App
{
public:
        explicit MyApp(AppGL::AppOptions options): AppGL::App(std::move(options)) {}
    
private:
        std::shared_ptr<ComputeShader> computeShader;

        float deltaTime = 0.0f;
        float lastFrame = 0.0f;
        float frames = 0.0f;

        void onStart() override
        {
                // Init Shader with fragment and vertex code.
                const char computeCode[] = {
                #embed "../src/shadersource/compute.comp"
                , '\0' };

                const char vertCode[] = {
                #embed "../src/shadersource/vertex.vert"
                , '\0' };

                const char fragCode[] = {
                #embed "../src/shadersource/fragment.frag"
                , '\0' };

                computeShader = std::make_shared<ComputeShader>(computeCode, vertCode, fragCode);
                computeShader->addUniform("resolution", glm::vec2(0.0));
                computeShader->addUniform("time", 0.0f);
        }

        void onUpdate() override
        {
                float currentFrame = this->window.getTime();
                deltaTime = currentFrame - lastFrame;
                lastFrame = currentFrame;
                if (frames > 500) {
                        std::cout << "FPS: " << 1 / deltaTime << std::endl;
                        frames = 0;
                } else frames++;


                computeShader->setUniform("resolution", this->window.getResolution());
                computeShader->setUniform("time", currentFrame);
                computeShader->drawFullScreenQuad(800, 600, 0);
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