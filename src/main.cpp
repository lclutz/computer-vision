#include <SDL.h>

#include <imgui.h>
#include <imgui_impl_sdl.h>
#include <imgui_impl_sdlrenderer.h>

#include <opencv2/opencv.hpp>

#include "application.h"
#include "defer.h"
#include "logging.h"

#include "gui.h"
#include "motion_detector.h"
#include "skin_tone_calibrator.h"
#include "hand_detector.h"

constexpr int WINDOW_WIDTH  = 800;
constexpr int WINDOW_HEIGHT = 600;

static SDL_Texture *
bgr_mat_to_sdl_texture(const cv::Mat &mat,
                       SDL_Renderer *renderer,
                       SDL_Texture *texture)
{
    SDL_Texture *result;
    bool texture_size_fits = false;
    if (texture) {
        int texture_width, texture_height;
        SDL_QueryTexture(texture, 0, 0, &texture_width, &texture_height);
        texture_size_fits = texture_width == mat.cols && texture_height == mat.rows;
    }

    if (texture && texture_size_fits) {
        result = texture;
    } else {
        if (texture) { SDL_DestroyTexture(texture); }
        result = SDL_CreateTexture(
            renderer,
            SDL_PIXELFORMAT_BGR24,
            SDL_TEXTUREACCESS_STREAMING,
            mat.cols, mat.rows);
    }

    if (!result) {
        logw("Failed to create output texture: %s", SDL_GetError());
    }

    if (SDL_UpdateTexture(result, 0, mat.data, 3*mat.cols) != 0) {
        logw("Failed to update output texture: %s", SDL_GetError());
    }

    return result;
}

int main(int argc, char *argv[])
{
    (void)argc;
    (void)argv;

    Application app;
    GUI gui;
    MotionDetector motion_detector;
    SkinToneCalibrator skin_tone_calibrator;
    HandDetector hand_detector;

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

    while (app.state != SHOULD_CLOSE) {
        SDL_Event event;
        while (SDL_PollEvent(&event)) {
            ImGui_ImplSDL2_ProcessEvent(&event);
            switch (event.type) {
                case SDL_QUIT: {
                    app.state = SHOULD_CLOSE;
                } break;

                case SDL_KEYDOWN: {
                    switch (event.key.keysym.sym) {
                        case SDLK_F1: {
                            gui.show_settings_window = !gui.show_settings_window;
                        } break;
                        case SDLK_F2: {
                            gui.show_stats_window = !gui.show_stats_window;
                        } break;
                        case SDLK_F3: {
                            gui.show_hand_detector_settings = !gui.show_hand_detector_settings;
                        } break;
                        case SDLK_RETURN: {
                            if (app.state == CALIBRATING_SKIN_TONE) {
                                hand_detector = HandDetector(skin_tone_calibrator.calibrate());
                                app.state = INITIALIZATION_DONE;
                            }
                        } break;
                    }
                } break;
            }
        }

        {
            ImGui_ImplSDLRenderer_NewFrame();
            ImGui_ImplSDL2_NewFrame();
            SDL_SetRenderDrawColor(renderer, 0x1d, 0x22, 0x27, 0);
            SDL_RenderClear(renderer);

            if (app.webcam.is_open()) {
                if (app.webcam.new_frame_available()) {
                    cv::Mat image = app.webcam.read();
#if 0
                    cv::Mat motion = motion_detector.apply(image);
                    main_window_texture = bgr_mat_to_sdl_texture(motion, renderer, main_window_texture);
#endif
                    if (app.state == CALIBRATING_SKIN_TONE) {
                        cv::Mat hand = skin_tone_calibrator.apply(image);
                        main_window_texture = bgr_mat_to_sdl_texture(hand, renderer, main_window_texture);
                    }

                    if (app.state >= INITIALIZATION_DONE) {
                        cv::Mat output = hand_detector.apply(image);
                        main_window_texture = bgr_mat_to_sdl_texture(output, renderer, main_window_texture);
                    }
                }
                SDL_RenderCopy(renderer, main_window_texture, 0, 0);
            }

            ImGui::NewFrame();
            gui.render(app, hand_detector);
            ImGui::Render();
            ImGui_ImplSDLRenderer_RenderDrawData(ImGui::GetDrawData());
        }

        uint64_t now = SDL_GetTicks64();
        if (now - fps_timer >= 1000) {
            app.fps = fps_counter;
            fps_counter = 0;
            fps_timer = now;
        }

        ++fps_counter;
        SDL_RenderPresent(renderer);
    }

    return 0;
}
