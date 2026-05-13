// Gactorio - Energy Drink Factory Simulator
// Phase 2: MVC 뼈대 + AppUI 컴포지터.
//
// 부트스트랩 순서: Model → Controller(Model&) → View(Controller&) → AppUI에 등록.
// ImGui/SDL2/OpenGL3 초기화 및 메인 루프는 여기에 그대로 둔다.

#include <SDL.h>
#include <SDL_opengl.h>
#include <imgui.h>
#include <backends/imgui_impl_sdl2.h>
#include <backends/imgui_impl_opengl3.h>

#include "models/Factory.h"
#include "controllers/FactoryController.h"
#include "views/AppUI.h"
#include "views/FactoryPanel.h"

#include <cstdio>
#include <memory>

int main(int /*argc*/, char** /*argv*/) {
    // -------------------------------------------------------------------------
    // SDL2 + OpenGL 컨텍스트 셋업
    // -------------------------------------------------------------------------
    if (SDL_Init(SDL_INIT_VIDEO | SDL_INIT_TIMER) != 0) {
        std::fprintf(stderr, "SDL_Init failed: %s\n", SDL_GetError());
        return 1;
    }

    const char* glsl_version = "#version 130";
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_PROFILE_MASK, SDL_GL_CONTEXT_PROFILE_CORE);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MAJOR_VERSION, 3);
    SDL_GL_SetAttribute(SDL_GL_CONTEXT_MINOR_VERSION, 0);
    SDL_GL_SetAttribute(SDL_GL_DOUBLEBUFFER, 1);
    SDL_GL_SetAttribute(SDL_GL_DEPTH_SIZE, 24);
    SDL_GL_SetAttribute(SDL_GL_STENCIL_SIZE, 8);

    const SDL_WindowFlags window_flags = static_cast<SDL_WindowFlags>(
        SDL_WINDOW_OPENGL | SDL_WINDOW_RESIZABLE | SDL_WINDOW_ALLOW_HIGHDPI);

    SDL_Window* window = SDL_CreateWindow(
        "Gactorio - Energy Drink Factory",
        SDL_WINDOWPOS_CENTERED, SDL_WINDOWPOS_CENTERED,
        1280, 720, window_flags);
    if (!window) {
        std::fprintf(stderr, "SDL_CreateWindow failed: %s\n", SDL_GetError());
        SDL_Quit();
        return 1;
    }

    SDL_GLContext gl_context = SDL_GL_CreateContext(window);
    SDL_GL_MakeCurrent(window, gl_context);
    SDL_GL_SetSwapInterval(1);

    // -------------------------------------------------------------------------
    // ImGui 셋업
    // -------------------------------------------------------------------------
    IMGUI_CHECKVERSION();
    ImGui::CreateContext();
    ImGuiIO& io = ImGui::GetIO();
    io.ConfigFlags |= ImGuiConfigFlags_NavEnableKeyboard;
    ImGui::StyleColorsDark();

    ImGui_ImplSDL2_InitForOpenGL(window, gl_context);
    ImGui_ImplOpenGL3_Init(glsl_version);

    // -------------------------------------------------------------------------
    // MVC 부트스트랩
    // -------------------------------------------------------------------------
    Factory            factory;            // 1. Model
    FactoryController  ctrl(factory);      // 2. Controller(Model&)
    AppUI              app;                // 3. UI 컴포지터
    app.addPanel(std::make_unique<FactoryPanel>(ctrl));   // 4. View(Controller&)

    // -------------------------------------------------------------------------
    // 메인 루프
    // -------------------------------------------------------------------------
    bool running = true;
    const ImVec4 clear_color = ImVec4(0.10f, 0.10f, 0.12f, 1.0f);

    while (running) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            if (event.type == SDL_QUIT) running = false;
            if (event.type == SDL_WINDOWEVENT &&
                event.window.event == SDL_WINDOWEVENT_CLOSE &&
                event.window.windowID == SDL_GetWindowID(window)) {
                running = false;
            }
        }

        ImGui_ImplOpenGL3_NewFrame();
        ImGui_ImplSDL2_NewFrame();
        ImGui::NewFrame();

        app.renderAll();   // <-- 패널들이 알아서 그린다

        ImGui::Render();
        glViewport(0, 0,
            static_cast<int>(io.DisplaySize.x),
            static_cast<int>(io.DisplaySize.y));
        glClearColor(clear_color.x, clear_color.y, clear_color.z, clear_color.w);
        glClear(GL_COLOR_BUFFER_BIT);
        ImGui_ImplOpenGL3_RenderDrawData(ImGui::GetDrawData());
        SDL_GL_SwapWindow(window);
    }

    // -------------------------------------------------------------------------
    // 정리 (생성 역순)
    // -------------------------------------------------------------------------
    ImGui_ImplOpenGL3_Shutdown();
    ImGui_ImplSDL2_Shutdown();
    ImGui::DestroyContext();

    SDL_GL_DeleteContext(gl_context);
    SDL_DestroyWindow(window);
    SDL_Quit();

    return 0;
}
