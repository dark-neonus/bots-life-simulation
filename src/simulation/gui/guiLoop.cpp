// Dear ImGui: standalone example application for GLFW + OpenGL 3, using programmable pipeline
// (GLFW is a cross-platform general purpose library for handling windows, inputs, OpenGL/Vulkan/Metal graphics context creation, etc.)

// Learn about Dear ImGui:
// - FAQ                  https://dearimgui.com/faq
// - Getting Started      https://dearimgui.com/getting-started
// - Documentation        https://dearimgui.com/docs (same as your local docs/ folder).
// - Introduction, links and more at the top of imgui.cpp

#include <random>

#include "imgui.h"
#include "imgui_impl_glfw.h"
#include "imgui_impl_opengl3.h"
#include <stdio.h>
#define GL_SILENCE_DEPRECATION
#if defined(IMGUI_IMPL_OPENGL_ES2)
#include <GLES2/gl2.h>
#endif
#include <GLFW/glfw3.h> // Will drag system OpenGL headers
#include <iostream>

#include "gui.h"
#include "simulation.h"

#include "objects/Food.h"

// [Win32] Our example includes a copy of glfw3.lib pre-compiled with VS2010 to maximize ease of testing and compatibility with old VS compilers.
// To link with VS2010-era libraries, VS2015+ requires linking with legacy_stdio_definitions.lib, which we do using this pragma.
// Your own project should not be affected, as you are likely to link with a newer binary of GLFW that is adequate for your version of Visual Studio.
#if defined(_MSC_VER) && (_MSC_VER >= 1900) && !defined(IMGUI_DISABLE_WIN32_FUNCTIONS)
#pragma comment(lib, "legacy_stdio_definitions")
#endif

// This example can also compile and run with Emscripten! See 'Makefile.emscripten' for details.
#ifdef __EMSCRIPTEN__
#include "../libs/emscripten/emscripten_mainloop_stub.h"
#endif

static void glfw_error_callback(int error, const char* description)
{
    fprintf(stderr, "GLFW Error %d: %s\n", error, description);
}

int guiLoop(std::shared_ptr<Simulation> simulation)
{
    glfwSetErrorCallback(glfw_error_callback);
    if (!glfwInit())
        return 1;

    // Decide GL+GLSL versions
#if defined(IMGUI_IMPL_OPENGL_ES2)
    // GL ES 2.0 + GLSL 100 (WebGL 1.0)
    const char* glsl_version = "#version 100";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 2);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(IMGUI_IMPL_OPENGL_ES3)
    // GL ES 3.0 + GLSL 300 es (WebGL 2.0)
    const char* glsl_version = "#version 300 es";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    glfwWindowHint(GLFW_CLIENT_API, GLFW_OPENGL_ES_API);
#elif defined(__APPLE__)
    // GL 3.2 + GLSL 150
    const char* glsl_version = "#version 150";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 2);
    glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // Required on Mac
#else
    // GL 3.0 + GLSL 130
    const char* glsl_version = "#version 130";
    glfwWindowHint(GLFW_CONTEXT_VERSION_MAJOR, 3);
    glfwWindowHint(GLFW_CONTEXT_VERSION_MINOR, 0);
    //glfwWindowHint(GLFW_OPENGL_PROFILE, GLFW_OPENGL_CORE_PROFILE);  // 3.2+ only
    //glfwWindowHint(GLFW_OPENGL_FORWARD_COMPAT, GL_TRUE);            // 3.0+ only
#endif

    // Create window with graphics context
    GLFWwindow* window = glfwCreateWindow(1280, 720, "Dear ImGui GLFW+OpenGL3 example", nullptr, nullptr);
    if (window == nullptr)
        return 1;
    glfwMakeContextCurrent(window);
    glfwSwapInterval(1); // Enable vsync

    // Setup Dear ImGui context
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO(); (void)io;
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;     // Enable Keyboard Controls
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableGamepad;      // Enable Gamepad Controls
    io.ConfigFlags |= ImGuiConfigFlags_DockingEnable;         // Enable Docking
    io.ConfigFlags |= ImGuiConfigFlags_ViewportsEnable;       // Enable Multi-Viewport / Platform Windows
    //io.ConfigViewportsNoAutoMerge = true;
    //io.ConfigViewportsNoTaskBarIcon = true;

    // Setup Dear ImGui style
    ImGui::StyleColorsDark();
    //ImGui::StyleColorsLight();

    // When viewports are enabled we tweak WindowRounding/WindowBg so platform windows can look identical to regular ones.
    ImGuiStyle& style = ImGui::GetStyle();
    if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
    {
        style.WindowRounding = 0.0f;
        style.Colors[ImGuiCol_WindowBg].w = 1.0f;
    }

    // Setup Platform/Renderer backends
    ImGui_ImplGlfw_InitForOpenGL(window, true);
#ifdef __EMSCRIPTEN__
    ImGui_ImplGlfw_InstallEmscriptenCallbacks(window, "#canvas");
#endif
    ImGui_ImplOpenGL3_Init(glsl_version);

    // Load Fonts
    // - If no fonts are loaded, dear imgui will use the default font. You can also load multiple fonts and use ImGui::PushFont()/PopFont() to select them.
    // - AddFontFromFileTTF() will return the ImFont* so you can store it if you need to select the font among multiple.
    // - If the file cannot be loaded, the function will return a nullptr. Please handle those errors in your application (e.g. use an assertion, or display an error and quit).
    // - The fonts will be rasterized at a given size (w/ oversampling) and stored into a texture when calling ImFontAtlas::Build()/GetTexDataAsXXXX(), which ImGui_ImplXXXX_NewFrame below will call.
    // - Use '#define IMGUI_ENABLE_FREETYPE' in your imconfig file to use Freetype for higher quality font rendering.
    // - Read 'docs/FONTS.md' for more instructions and details.
    // - Remember that in C/C++ if you want to include a backslash \ in a string literal you need to write a double backslash \\ !
    // - Our Emscripten build process allows embedding fonts to be accessible at runtime from the "fonts/" folder. See Makefile.emscripten for details.
    //io.Fonts->AddFontDefault();
    //io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\segoeui.ttf", 18.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/DroidSans.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Roboto-Medium.ttf", 16.0f);
    //io.Fonts->AddFontFromFileTTF("../../misc/fonts/Cousine-Regular.ttf", 15.0f);
    //ImFont* font = io.Fonts->AddFontFromFileTTF("c:\\Windows\\Fonts\\ArialUni.ttf", 18.0f, nullptr, io.Fonts->GetGlyphRangesJapanese());
    //IM_ASSERT(font != nullptr);

    // Our state
    // bool show_demo_window = true;
    // bool show_another_window = false;
    ImVec4 clear_color = ImVec4(0.45f, 0.55f, 0.60f, 1.00f);

    std::random_device rd;
    std::mt19937 gen;
    std::uniform_int_distribution<int> spawnChance = std::uniform_int_distribution<int>(0, 10);
    std::uniform_int_distribution<int> distX = std::uniform_int_distribution<int>(0, simulation->chunkManager->mapWidth - 1);
    std::uniform_int_distribution<int> distY = std::uniform_int_distribution<int>(0, simulation->chunkManager->mapHeight - 1);
    

    // Init of simulation here
    // Create a shared pointer to the simulation
    // std::shared_ptr<Simulation> simulation = std::make_shared<Simulation>(6, 6, 35);

    // Add SimulationObjects
    // Add BaseObjects
    // simulation->addObject(SimulationObjectType::BaseObject, std::make_shared<SimulationObject>(simulation, Vec2(0, 0), 3, colorInt(255, 0, 0)));
    // simulation->addObject(SimulationObjectType::BaseObject, std::make_shared<SimulationObject>(simulation, Vec2(100, 100), 5, colorInt(255, 255, 0)));
    // simulation->addObject(SimulationObjectType::BaseObject, std::make_shared<SimulationObject>(simulation, Vec2(100, 150), 5, colorInt(255, 255, 0)));
    // simulation->addObject(SimulationObjectType::BaseObject, std::make_shared<SimulationObject>(simulation, Vec2(150, 100), 5, colorInt(255, 255, 0)));
    // simulation->addObject(SimulationObjectType::BaseObject, std::make_shared<SimulationObject>(simulation, Vec2(50, 110), 10, colorInt(0, 0, 255, 120)));
    // simulation->addObject(SimulationObjectType::BaseObject, std::make_shared<SimulationObject>(simulation, Vec2(60, 70), 30, colorInt(0, 125, 0)));

    // // Add FoodObjects
    // simulation->addObject(SimulationObjectType::FoodObject, std::make_shared<FoodObject>(simulation, Vec2(200, 200), colorInt(100, 0, 0), simulation->unit * 10, simulation->unit * 5, 1.0f, 2.0f, true));
    // simulation->addObject(SimulationObjectType::FoodObject, std::make_shared<FoodObject>(simulation, Vec2(250, 200), colorInt(100, 0, 0), simulation->unit * 20, simulation->unit * 10, 1.0f, 2.0f, true));
    // simulation->addObject(SimulationObjectType::FoodObject, std::make_shared<FoodObject>(simulation, Vec2(300, 200), colorInt(100, 0, 0), simulation->unit * 30, simulation->unit * 15, 1.0f, 2.0f, true));
    // simulation->addObject(SimulationObjectType::FoodObject, std::make_shared<FoodObject>(simulation, Vec2(350, 200), colorInt(100, 0, 0), simulation->unit * 40, simulation->unit * 20, 1.0f, 2.0f, true));

    // // Add TreeObjects
    // simulation->addObject(SimulationObjectType::TreeObject, std::make_shared<TreeObject>(simulation, Vec2(200, 150), 3, 700.0f, 1.0f, 0.5f, 500.0f, true));
    // simulation->addObject(SimulationObjectType::TreeObject, std::make_shared<TreeObject>(simulation, Vec2(250, 150), 5, 300.0f, 1.2f, 2.5f, 400.0f, false));
    // simulation->addObject(SimulationObjectType::TreeObject, std::make_shared<TreeObject>(simulation, Vec2(350, 150), 10, 200.0f, 1.0f, 1.5f, 700.0f, false));

    // // Add BotObjects
    // simulation->addObject(SimulationObjectType::BotObject, std::make_shared<BotObject>(simulation, Vec2(300, 300), 100, 80, 30, 5.0f, 8));
    // simulation->addObject(SimulationObjectType::BotObject, std::make_shared<BotObject>(simulation, Vec2(400, 300), 200, 40, 20, 2.0f, 20));


    // Spawn 400 bots for crash test

    // for (int y = 0; y < 20; y++) {
    //     for (int x = 0; x < 20; x++) {
    //         simulation->addObject(SimulationObjectType::BotObject, std::make_shared<BotObject>(simulation, Vec2(200 + x*40, 200 + y*40), 200, 80, 100, 2.0f, 20));
    //     }
    // 
    // Main loop
#ifdef __EMSCRIPTEN__
    // For an Emscripten build we are disabling file-system access, so let's not attempt to do a fopen() of the imgui.ini file.
    // You may manually call LoadIniSettingsFromMemory() to load settings from your own storage.
    io.IniFilename = nullptr;
    EMSCRIPTEN_MAINLOOP_BEGIN
#else
    while (!glfwWindowShouldClose(window))
#endif
    {
        // Poll and handle events (inputs, window resize, etc.)
        // You can read the io.WantCaptureMouse, io.WantCaptureKeyboard flags to tell if dear imgui wants to use your inputs.
        // - When io.WantCaptureMouse is true, do not dispatch mouse input data to your main application, or clear/overwrite your copy of the mouse data.
        // - When io.WantCaptureKeyboard is true, do not dispatch keyboard input data to your main application, or clear/overwrite your copy of the keyboard data.
        // Generally you may always pass all inputs to dear imgui, and hide them from your application based on those two flags.
        glfwPollEvents();
        if (glfwGetWindowAttrib(window, GLFW_ICONIFIED) != 0)
        {
            ImGui_ImplGlfw_Sleep(10);
            continue;
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplGlfw_NewFrame();
        ImGui::NewFrame();

        ///////////////////////////////////////////////////
        ///////////////////////////////////////////////////
        ///////////////////////////////////////////////////

        // if (spawnChance(gen) == 7) {
        for (int i = 0; i < 20; i++) {
            simulation->addObject(
                SimulationObjectType::FoodObject,
                std::dynamic_pointer_cast<SimulationObject> (
                    std::make_shared<FoodObject>(
                        simulation,
                        Vec2(distX(gen), distY(gen)),
                        colorInt(100, 0, 0),
                        450,
                        550,
                        0.5f,
                        1.0f,
                        false
                    )
                )
            );
        }

        createGui(simulation, io);

        // Rendering
        ImGui::Render();
        int display_w, display_h;
        glfwGetFramebufferSize(window, &display_w, &display_h);
        glViewport(0, 0, display_w, display_h);
        glClearColor(clear_color.x * clear_color.w, clear_color.y * clear_color.w, clear_color.z * clear_color.w, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());

        // Update and Render additional Platform Windows
        // (Platform functions may change the current OpenGL context, so we save/restore it to make it easier to paste this code elsewhere.
        //  For this specific demo app we could also call glfwMakeContextCurrent(window) directly)
        if (io.ConfigFlags & ImGuiConfigFlags_ViewportsEnable)
        {
            GLFWwindow* backup_current_context = glfwGetCurrentContext();
            ImGui::UpdatePlatformWindows();
            ImGui::RenderPlatformWindowsDefault();
            glfwMakeContextCurrent(backup_current_context);
        }

        glfwSwapBuffers(window);
    }
#ifdef __EMSCRIPTEN__
    EMSCRIPTEN_MAINLOOP_END;
#endif

    // Cleanup
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplGlfw_Shutdown();
    ImGui::DestroyContext();

    glfwDestroyWindow(window);
    glfwTerminate();

    return 0;
}

