#pragma once

#include <memory>

#include <opencv2/opencv.hpp>

#include "event_receiver.h"
#include "background_estimator.h"
#include "motion_detector.h"
#include "skin_tone_calibrator.h"
#include "skin_tone_detector.h"

struct HandDetector : public EventReceiver {
    bool started;

    std::unique_ptr<BackgroundEstimator> bg_estimator_brightness;
    std::unique_ptr<BackgroundEstimator> bg_estimator_hue;
    std::unique_ptr<MotionDetector>      motion_detector;
    std::unique_ptr<SkinToneCalibrator>  skin_tone_calibrator;
    std::unique_ptr<SkinToneDetector>    skin_tone_detector;

    cv::Mat apply(const cv::Mat input);

    void handle_event(const Event &e) override;

    HandDetector()
        : started(false)
        , bg_estimator_brightness(std::make_unique<BackgroundEstimator>(BG_ESTIMATOR_BRIGHTNESS))
        , bg_estimator_hue(std::make_unique<BackgroundEstimator>(BG_ESTIMATOR_HUE))
        , motion_detector(std::make_unique<MotionDetector>())
        , skin_tone_calibrator(std::make_unique<SkinToneCalibrator>())
        , skin_tone_detector(nullptr)
    {}
};
