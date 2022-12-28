#include <deque>
#include <vector>

#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <opencv2/opencv.hpp>

#include "application.h"
#include "defer.h"
#include "event_receiver.h"
#include "gui.h"
#include "hand_detector.h"
#include "logging.h"
#include "message_queue.h"
#include "sdl_opencv_helper.h"

constexpr int WINDOW_WIDTH  = 800;
constexpr int WINDOW_HEIGHT = 600;

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    if (SDL_Init(SDL_INIT_VIDEO) != 0) {
        loge("Failed to initialize SDL: %s", SDL_GetError());
        return 1;
    }
    defer(SDL_Quit());

    SDL_Window *window = SDL_CreateWindow("Computer Vision",
        SDL_WINDOWPOS_UNDEFINED, SDL_WINDOWPOS_UNDEFINED,
        WINDOW_WIDTH, WINDOW_HEIGHT,
        SDL_WINDOW_SHOWN|SDL_WINDOW_RESIZABLE|SDL_WINDOW_ALLOW_HIGHDPI);
    if (!window) {
        loge("Failed to create a window: %s", SDL_GetError());
        return 1;
    }
    defer(SDL_DestroyWindow(window));

    SDL_Renderer *renderer = SDL_CreateRenderer(window, -1,
        SDL_RENDERER_PRESENTVSYNC|SDL_RENDERER_ACCELERATED);
    if (!renderer) {
        loge("Failed to create a renderer: %s", SDL_GetError());
        return 1;
    }
    defer(SDL_DestroyRenderer(renderer));

    if (!IMGUI_CHECKVERSION()) {
        loge("IMGUI_CHECKVERSION failed");
        return 1;
    }

    if (!ImGui::CreateContext()) {
        loge("Failed to create ImGui context");
        return 1;
    }
    defer(ImGui::DestroyContext());

    ImGuiIO &io = ImGui::GetIO();
    io.IniFilename = 0;

    ImGui::StyleColorsDark();

    if (!ImGui_ImplSDL2_InitForSDLRenderer(
        window, renderer)) {
        loge("Failed to initialize ImGui for SDL");
        return 1;
    }
    defer(ImGui_ImplSDL2_Shutdown());

    if (!ImGui_ImplSDLRenderer_Init(renderer)) {
        loge("Failed to initialize ImGui for SDL renderer");
        return 1;
    }
    defer(ImGui_ImplSDLRenderer_Shutdown());

    uint64_t fps_timer = SDL_GetTicks64();
    int fps_counter = 0;
    SDL_Texture *main_window_texture = 0;
    cv::Mat camera_input;

    std::vector<EventReceiver *> event_receivers;

    Application &app = Application::instance();

    GUI &gui = GUI::instance();
    event_receivers.push_back(&gui);

    Webcam &webcam = Webcam::instance();
    event_receivers.push_back(&webcam);

    HandDetector hand_detector;
    event_receivers.push_back(&hand_detector);

    MessageQueue &message_queue = MessageQueue::instance();

    while (!app.should_close) {
        { // Handle SDL events
            SDL_Event event;
            while (SDL_PollEvent(&event)) {
                ImGui_ImplSDL2_ProcessEvent(&event);
                switch (event.type) {
                    case SDL_QUIT: {
                        app.should_close = true;
                    } break;

                    case SDL_KEYDOWN: {
                        switch (event.key.keysym.sym) {
                        case SDLK_F1: {
                            message_queue.push(Event(EVENT_TOGGLE_SETTINGS_WINDOW_VISIBILITY));
                        } break;
                        case SDLK_F2: {
                            message_queue.push(Event(EVENT_TOGGLE_STATS_WINDOW_VISIBILITY));
                        } break;
                        case SDLK_RETURN: {
                            message_queue.push(Event(EVENT_CALIBRATE_SKIN_TONE));
                        } break;
                        }
                    } break;
                }
            }
        }

        { // Handle our own events
            Event event;
            while (message_queue.poll(&event)) {
                for (EventReceiver *event_receiver : event_receivers)
                {
                    event_receiver->handle_event(event);
                }
            }
        }

        { // Rendering the main window
            ImGui_ImplSDLRenderer_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            SDL_SetRenderDrawColor(renderer, 0x1d, 0x22, 0x27, 0);
            SDL_RenderClear(renderer);

            if (std::optional<cv::Mat> camera_frame = webcam.read()) {
                cv::Mat output = hand_detector.apply(camera_frame.value());

                if (!output.empty()) {
                    main_window_texture = bgr_mat_to_sdl_texture(output, renderer, main_window_texture);
                }
            }

            if (main_window_texture) {
                paint_sdL_texture_to_renderer(renderer, window, main_window_texture);
            }

            gui.render();

            // Swap buffers
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
            SDL_RenderPresent(renderer);
        }

        { // Measure FPS
            uint64_t now = SDL_GetTicks64();
            if (now - fps_timer >= 1000) {
                app.fps = fps_counter;
                fps_counter = 0;
                fps_timer = now;
            }

            ++fps_counter;
        }
    }

    return 0;
}
