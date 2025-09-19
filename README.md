# OpenGL Template  

This template repository let's you have a ready-to-go simple OpenGL application already set up and initialized, complete with shaders.  
Useful to quickly test things without the hassle of having to scroll through [Learn OpenGL](https://learnopengl.com/) for the 500th time.

## Set up
Inside of ```CMakeLists.txt``` simply set the ```USE_WIN32``` to ```OFF``` if you want to use standard GLFW for window management, or to ```ON``` to use direct win32 API calls _(when switching from one to the other you should clear your CMake cache by deleting the build folder)_.  

Inside of ```src/``` you will find two source files starting with "MyApp", keep the one corresponding to your target platform(s) and remove its final extension to be able to use it.  
You are now free to delete non-used folders inside ```src/AppGL/OpenGL/``` based on the choosen target platform(s).  

## How to use
An ```App``` is made of
- ```onStart()``` to insert custom code to execute as soon as the App is **ready**.
- ```onUpdate()``` for custom logic to be executed **every frame**.
- ```onDestroy()``` for instruction to run right before the App is **destroyed**.

You must also provide a function with signature specified in the base ```App``` of your choice, which will serve as the **entry point** of your application.   
After setting up the flags with AppOptions you must **return a pointer to your ```App``` class' instance**.  

Inside of ```src/shadersource``` you can find both the fragment and vertex shaders, which you can modify to your likings.
