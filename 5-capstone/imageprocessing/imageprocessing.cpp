#include "imageprocessing.h"

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/dnn.hpp>
#include <string>
#include <fstream>
#include <cmath>

namespace ImageProcessing
{

    void ReorderRect(std::vector<cv::Point2f> &points)
    {
        auto buffer = points;

        auto [upper_left, lower_right] = std::minmax_element(points.begin(), points.end(), [](const auto &pt1, const auto &pt2)
                                                             { return pt1.x + pt1.y < pt2.x + pt2.y; });
        auto [lower_left, upper_right] = std::minmax_element(points.begin(), points.end(), [](const auto &pt1, const auto &pt2)
                                                             { return pt1.x - pt1.y < pt2.x - pt2.y; });

        buffer[0] = *upper_left;
        buffer[1] = *lower_left;
        buffer[2] = *lower_right;
        buffer[3] = *upper_right;
        points = buffer;
    }

    std::vector<cv::Point2f> TransformToUnrotated(std::vector<cv::Point2f> &rotated_points)
    {
        std::vector<cv::Point2f> unrotated(4);

        // not a reatngle with 4 points
        if (!rotated_points.size() == 4)
        {
            return unrotated;
        }

        ReorderRect(rotated_points);

        // calculate euclidean distances and find maximum width of the rectangle
        auto width_1 = cv::norm(rotated_points[0] - rotated_points[3]);
        auto width_2 = cv::norm(rotated_points[1] - rotated_points[2]);
        auto width = std::max(width_1, width_2);

        // calculate euclidian distances and find maximum height of the rectangle
        auto height_1 = cv::norm(rotated_points[0] - rotated_points[1]);
        auto height_2 = cv::norm(rotated_points[3] - rotated_points[2]);
        auto height = std::max(height_1, height_2);

        auto length = 0.25 * (height_1 + height_2 + width_1 + width_2);

        // calculate all 4 edges for unrotated rect
        unrotated[0] = cv::Point2f(0, 0);
        unrotated[1] = cv::Point2f(0, height);
        unrotated[2] = cv::Point2f(width, height);
        unrotated[3] = cv::Point2f(width, 0);

        return unrotated;
    }

    void DownScaleToMaxSize(cv::Mat &image, int max_width, int max_height)
    {
        if (image.rows > max_height || image.cols > max_width)
        {
            double scale_factor = std::min(static_cast<double>(max_width) / image.cols, static_cast<double>(max_height) / image.rows);
            cv::resize(image, image, cv::Size(0, 0), scale_factor, scale_factor, cv::INTER_LANCZOS4);
        }
    }

    std::optional<std::vector<cv::Point2f>> FindSudokuField(const cv::Mat &image)
    {
        std::vector<cv::Point2f> edges;

        cv::Mat modified_image;
        image.copyTo(modified_image);
        cv::adaptiveThreshold(modified_image, modified_image, 255, cv::ADAPTIVE_THRESH_MEAN_C, cv::THRESH_BINARY_INV, 7, 2);
        std::vector<std::vector<cv::Point>> contours{};
        cv::findContours(modified_image, contours, cv::RETR_TREE, cv::CHAIN_APPROX_SIMPLE);



        if (contours.size() == 0)
        {
            return std::optional<std::vector<cv::Point2f>>{};
        }

        auto max_cnt = std::max_element(contours.begin(), contours.end(), [](const auto &cnt1, const auto &cnt2)
                                        { return cv::contourArea(cnt1) < cv::contourArea(cnt2); });
        auto max_idx = std::distance(contours.begin(), max_cnt);
        cv::approxPolyDP(*max_cnt, edges, 20, true);

        // no recangle found. Return innvalid result
        if (edges.size() != 4)
        {
            return std::optional<std::vector<cv::Point2f>>{};
        }
        return edges;
    }

    cv::Mat CropSudokuField(const cv::Mat &src, std::vector<cv::Point2f> &edges)
    {
        cv::Mat dst;
        std::vector<cv::Point2f> unrotated_points = TransformToUnrotated(edges);
        cv::Mat m_rot = cv::getPerspectiveTransform(edges, unrotated_points);
        cv::warpPerspective(src, dst, m_rot, cv::Size2f(unrotated_points[2].x, unrotated_points[2].y));
        return dst;
    }

    std::optional<int> GetBiggestNonBorderContour(std::vector<std::vector<cv::Point>> &contours, int segment_width, int segment_height)
    {
        std::sort(contours.begin(), contours.end(), [](const auto &cnt1, const auto &cnt2)
                  { return cv::contourArea(cnt1) > cv::contourArea(cnt2); });

        for (int idx = 0; idx < contours.size(); idx++)
        {
            const auto &contour = contours[idx];
            bool is_border = false;
            for (auto &point : contour)
            {
                is_border = point.x == 0 || point.y == 0 || point.x >= segment_width - 1 || point.y >= segment_height - 1;
                if (is_border)
                {
                    break;
                }
            }
            if (!is_border)
            {
                return idx;
            }
        }
        return {};
    }

    std::vector<std::pair<cv::Point, cv::Rect>> FilterNumberSegments(cv::Mat &src)
    {
        std::vector<std::pair<cv::Point, cv::Rect>> rois;
        float segment_height = src.rows / 9.0;
        float segment_width = src.cols / 9.0;

        for (int i = 0; i < 9; i++)
        {
            for (int j = 0; j < 9; j++)
            {
                cv::Rect roi(segment_width * i, segment_height * j, segment_width, segment_height);
                cv::Mat segment = src(roi);

                // use otsu thresholding for automatically detecting the threshold
                cv::threshold(segment, segment, 0, 255, cv::THRESH_OTSU | cv::THRESH_BINARY_INV);

                std::vector<std::vector<cv::Point>> contours{};
                cv::findContours(segment, contours, cv::RETR_LIST, cv::CHAIN_APPROX_NONE);

                if (contours.size() == 0)
                {
                    segment = 0;
                    continue;
                }

                auto most_likely_idx = GetBiggestNonBorderContour(contours, segment_width, segment_height);

                // no valid contour found or the contour found whith a size wich is smaller than one percent of the segements size
                if (!most_likely_idx.has_value() || cv::contourArea(contours[most_likely_idx.value()]) < segment_width * segment_height * 0.01)
                {
                    segment = 0;
                    continue;
                }

                cv::Mat mask = cv::Mat::zeros(segment.size(), 0);
                cv::drawContours(mask, contours, most_likely_idx.value(), 255, cv::FILLED);

                cv::Mat filtered_segment;
                cv::bitwise_and(segment, segment, filtered_segment, mask);
                filtered_segment.copyTo(segment);

                cv::Point roi_position{j, i};
                rois.emplace_back(roi_position, roi);
            }
        }
        return rois;
    }

    int RecognizeNumber(const cv::Mat &input, cv::dnn::Net &model, const cv::Size& size, double scale_factor)
    {
        cv::Mat input_mat;
        input.convertTo(input_mat, CV_32FC1);
        //cv::resize(input_mat, input_mat, cv::Size(28, 28), 0.0, 0.0, cv::INTER_LINEAR);
        auto input_blob = cv::dnn::blobFromImage(input_mat, scale_factor, size, 0.0, false, false, CV_32F);
        model.setInput(input_blob);
        auto feature_vector_dnn = model.forward();
        
        cv::Point max_idx;
        cv::minMaxLoc(feature_vector_dnn, NULL, NULL, NULL, &max_idx);
        return max_idx.x;
    }

    void DrawPolygon(cv::Mat& image, const std::vector<cv::Point2f>& poly_points, const cv::Scalar& color, int thickness, bool is_closed) {
        if(poly_points.size() < 2) {
            return;
        }
        
        for(int point_idx = 1; point_idx < poly_points.size(); point_idx++) {
            cv::line(image, poly_points[point_idx], poly_points[point_idx - 1], color, thickness);
        }

        if(is_closed) {
            cv::line(image, poly_points[0], poly_points[poly_points.size() - 1], color, thickness);
        }
    }
}