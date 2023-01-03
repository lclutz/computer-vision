#pragma once

#include <vector>

#include <opencv2/core.hpp>

bool rects_intersect(const cv::Rect &r1, const cv::Rect &r2);

double get_min_distance_rectangles(const cv::Rect &r1, const cv::Rect &r2);

std::vector<cv::Rect> merge_rects(const std::vector<cv::Rect> &input, bool (*predicate)(const cv::Rect &, const cv::Rect &));
