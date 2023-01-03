#include "rect_helper.h"

static bool
point_in_line_shadow(const cv::Point2d &p1, const cv::Point2d &p2, const cv::Point2d &q)
{
    // Compute the segment length and direction
    double segment_length = cv::norm(p2 - p1);
    cv::Point2d segment_dir = (p2 - p1) / segment_length;

    // Compute the projection of q onto the line formed by p1 and p2
    double projection = (q - p1).dot(segment_dir);

    // Return true if the projection is within the segment, false otherwise
    return 0 < projection && projection < segment_length;
}

static double
get_min_distance_to_segment(const cv::Point2d &p1, const cv::Point2d &p2, const cv::Point2d &q)
{
    // Check if q is within the shadow cast by the line formed by p1 and p2
    if (point_in_line_shadow(p1, p2, q)) {
        // Compute the cross product of the segment direction and q - p1
        double cross_product = (p2 - p1).cross(q - p1);

        // Return the Euclidean norm of the cross product
        return cross_product;
    } else {
        // Return the minimum of the distances from q to p1 and p2
        return std::min(cv::norm(q - p1), cv::norm(q - p2));
    }
}


bool
rects_intersect(const cv::Rect &r1, const cv::Rect &r2)
{
    return (r2.x < r1.x + r1.width  &&
            r2.x + r2.width > r1.x  &&
            r2.y < r1.y + r1.height &&
            r2.y + r2.height > r1.y);
}

double
get_min_distance_rectangles(const cv::Rect &r1, const cv::Rect &r2)
{
    std::vector<double> distances;

    std::array<cv::Point2d, 4> r1_points {
        cv::Point2d(r1.x, r1.y),
        cv::Point2d(r1.x + r1.width, r1.y),
        cv::Point2d(r1.x + r1.width, r1.y + r1.height),
        cv::Point2d(r1.x, r1.y + r1.height)
    };

    std::array<std::pair<cv::Point2d, cv::Point2d>, 4> r1_segments{
        std::make_pair(r1_points[0], r1_points[1]),
        std::make_pair(r1_points[1], r1_points[2]),
        std::make_pair(r1_points[2], r1_points[3]),
        std::make_pair(r1_points[3], r1_points[0])
    };

    std::array<cv::Point2d, 4> r2_points {
        cv::Point2d(r2.x, r2.y),
        cv::Point2d(r2.x + r2.width, r2.y),
        cv::Point2d(r2.x + r2.width, r2.y + r1.height),
        cv::Point2d(r2.x, r2.y + r2.height)
    };

    std::array<std::pair<cv::Point2d, cv::Point2d>, 4> r2_segments{
        std::make_pair(r2_points[0], r2_points[1]),
        std::make_pair(r2_points[1], r2_points[2]),
        std::make_pair(r2_points[2], r2_points[3]),
        std::make_pair(r2_points[3], r2_points[0])
    };

    for (const auto &[p1, p2] : r1_segments) {
        for (const auto &q : r2_points) {
            distances.push_back(get_min_distance_to_segment(p1, p2, q));
        }
    }

    for (const auto &[p1, p2] : r2_segments) {
        for (const auto &q : r1_points) {
            distances.push_back(get_min_distance_to_segment(p1, p2, q));
        }
    }

    return *std::min_element(distances.begin(), distances.end());
}

std::vector<cv::Rect>
merge_rects(const std::vector<cv::Rect> &input, bool (*predicate)(const cv::Rect &, const cv::Rect &))
{
    std::vector<cv::Rect> merged_boxes;
    for (const cv::Rect &rect : input) {
        bool merged = false;
        for (cv::Rect &merged_rect : merged_boxes) {
            if (predicate(rect, merged_rect)) {
                int x_min = std::min(rect.x, merged_rect.x);
                int y_min = std::min(rect.y, merged_rect.y);
                int x_max = std::max(rect.x + rect.width, merged_rect.x + merged_rect.width);
                int y_max = std::max(rect.y + rect.height, merged_rect.y + merged_rect.height);

                merged_rect.x = x_min;
                merged_rect.y = y_min;
                merged_rect.width  = x_max - x_min;
                merged_rect.height = y_max - y_min;

                merged = true;
            }
        }

        if (!merged) {
            merged_boxes.push_back(rect);
        }
    }

    return merged_boxes;
}
