#include <thread>

#include <imgui.h>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include <nfd.h>

#include "application.h"
#include "defer.h"
#include "logging.h"
#include "gui.h"

static void
folder_picker_thread(std::string &asset_folder_path)
{
    nfdchar_t *out_path = 0;
    nfdresult_t result = NFD_PickFolder(0, &out_path);
    if ( result == NFD_OKAY ) {
        asset_folder_path = std::string(out_path);
    }
    else if ( result == NFD_CANCEL ) {
        logi("User pressed cancel.");
    }
    else {
        loge("%s", NFD_GetError());
    }
}

static const char *
capture_api_as_cstr(int video_capture_api)
{
    switch (video_capture_api) {
    case cv::CAP_ANY:   return "Auto";
    case cv::CAP_DSHOW: return "DirectShow";
    default:            return "Unknown";
    }
}

GUI::GUI()
    : show_stats_window(false)
    , show_settings_window(true)
    , show_skin_tone_detector_settings(false)
    , failed_to_open_camera(false)
{}

void
GUI::render_stats(const Application &app)
{
    ImGui::Begin("Stats");

    ImGui::Text("Game");
    ImGui::Text("FPS: %d", app.fps);
    ImGui::Text("Asset folder: \"%s\"", app.asset_folder_path.c_str());

    ImGui::Text("Camera");
    ImGui::Text("ID:          %d", app.camera_id);
    ImGui::Text("API backend: %s", capture_api_as_cstr(app.camera_api));

    ImGui::End();
}

void
GUI::render_settings(Application &app)
{
    ImGui::SetNextWindowPos({300,200}, ImGuiCond_Once);
    ImGui::Begin("Settings");

    bool camera_is_open = app.webcam.is_open();

    ImGui::BeginDisabled(camera_is_open);

        ImGui::PushItemWidth(100);
        ImGui::InputInt("Camera id", &app.camera_id);

        ImGui::PushItemWidth(100);
        if (ImGui::BeginCombo("Video capture API", capture_api_as_cstr(app.camera_api))) {
            if (ImGui::Selectable(capture_api_as_cstr(cv::CAP_ANY),
                app.camera_api == cv::CAP_ANY)) {
                app.camera_api = cv::CAP_ANY;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable(capture_api_as_cstr(cv::CAP_DSHOW),
                app.camera_api == cv::CAP_DSHOW)) {
                app.camera_api = cv::CAP_DSHOW;
                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Open camera")) {
            app.webcam.open(
                app.camera_id,
                app.camera_api);

            failed_to_open_camera = !app.webcam.is_open();
            if (!failed_to_open_camera) {
                app.state = CALIBRATING_SKIN_TONE;
            }
        }

        if (failed_to_open_camera) {
            ImGui::PushStyleColor(ImGuiCol_Text, IM_COL32(255,12,44,255));
            ImGui::Text("Failed to open camera");
            ImGui::PopStyleColor();
        }

    ImGui::EndDisabled();

    ImGui::BeginDisabled(!camera_is_open);
        if (ImGui::Button("Close camera")) {
            app.webcam.close();
        }
    ImGui::EndDisabled();

    ImGui::Text("Asset folder: %s", app.asset_folder_path.c_str());
    if (ImGui::Button("Pick asset folder")) {
        std::thread t(
            folder_picker_thread,
            std::ref(app.asset_folder_path));
        t.detach();
    }

    ImGui::BeginDisabled(!camera_is_open || app.asset_folder_path.empty());
        if (ImGui::Button("Start")) {
            show_settings_window = false;
        }
    ImGui::EndDisabled();

    ImGui::End();
}

void
GUI::render_skin_tone_detector_settings(SkinToneDetector &skin_tone_detector)
{
    ImGui::Begin("Hand detector settings");

    ImGui::Text("{h=%.3f, s=%.3f, v=%.3f}",
        skin_tone_detector.skin_tone[0],
        skin_tone_detector.skin_tone[1],
        skin_tone_detector.skin_tone[2]);

    ImGui::InputDouble("h low delta", &skin_tone_detector.h_low_delta);
    ImGui::InputDouble("h high delta", &skin_tone_detector.h_high_delta);
    ImGui::InputDouble("s low delta", &skin_tone_detector.s_low_delta);
    ImGui::InputDouble("s high delta", &skin_tone_detector.s_high_delta);

    ImGui::End();
}

void
GUI::render(Application &app, SkinToneDetector &skin_tone_detector)
{
    if (show_stats_window) {
        render_stats(app);
    }

    if (show_settings_window)  {
        render_settings(app);
    }

    if (show_skin_tone_detector_settings) {
        render_skin_tone_detector_settings(skin_tone_detector);
    }
}
