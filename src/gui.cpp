#include <thread>

#include <imgui.h>

#include <opencv2/opencv.hpp>
#include <opencv2/videoio.hpp>

#include "application.h"
#include "message_queue.h"
#include "gui.h"


void
GUI::handle_event(const Event &e)
{
    switch (e.type)
    {
    case EVENT_TOGGLE_SETTINGS_WINDOW_VISIBILITY:
        show_settings_window = !show_settings_window;
        break;

    case EVENT_TOGGLE_STATS_WINDOW_VISIBILITY:
        show_stats_window = !show_stats_window;
        break;

    default:
        break;
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
GUI::render()
{
    ImGui::NewFrame();
    if (show_stats_window) {
        render_stats();
    }

    if (show_settings_window)  {
        render_settings();
    }
    ImGui::Render();
}

void
GUI::render_stats()
{
    Application &app = Application::instance();
    Webcam &webcam = Webcam::instance();

    ImGui::Begin("Stats");

    ImGui::Text("Game");
    ImGui::Text("FPS: %d", app.fps);

    ImGui::Text("Camera");
    ImGui::Text("ID:          %d", webcam.camera_id);
    ImGui::Text("API backend: %s", capture_api_as_cstr(webcam.camera_api));

    ImGui::End();
}

void
GUI::render_settings()
{
    MessageQueue &message_queue = MessageQueue::instance();
    Webcam &webcam = Webcam::instance();

    ImGui::SetNextWindowPos({300,200}, ImGuiCond_Once);
    ImGui::Begin("Settings");

    bool camera_is_open = webcam.is_open();

    ImGui::BeginDisabled(camera_is_open);

        ImGui::PushItemWidth(100);
        ImGui::InputInt("Camera id", &(webcam.camera_id));

        ImGui::PushItemWidth(100);
        if (ImGui::BeginCombo("Video capture API", capture_api_as_cstr(webcam.camera_api))) {
            if (ImGui::Selectable(capture_api_as_cstr(cv::CAP_ANY),
                webcam.camera_api == cv::CAP_ANY)) {
                webcam.camera_api = cv::CAP_ANY;
                ImGui::SetItemDefaultFocus();
            }
            if (ImGui::Selectable(capture_api_as_cstr(cv::CAP_DSHOW),
                webcam.camera_api == cv::CAP_DSHOW)) {
                webcam.camera_api = cv::CAP_DSHOW;
                ImGui::SetItemDefaultFocus();
            }
            ImGui::EndCombo();
        }

        if (ImGui::Button("Open camera")) {
            webcam.open();

            failed_to_open_camera = !webcam.is_open();
            if (!failed_to_open_camera) {
                // TODO:
                // app.state = CALIBRATING_SKIN_TONE;
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
            webcam.close();
        }
    ImGui::EndDisabled();

    ImGui::BeginDisabled(!camera_is_open);
        if (ImGui::Button("Start")) {
            show_settings_window = false;
            message_queue.push(Event(EVENT_START));
        }
    ImGui::EndDisabled();

    ImGui::End();
}
