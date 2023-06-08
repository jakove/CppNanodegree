#pragma once

#include <string>

#include <opencv2/opencv.hpp>
#include <opencv2/imgcodecs.hpp>
#include <opencv2/dnn.hpp>


namespace ImageProcessing
{

    void ReorderRect(std::vector<cv::Point2f> &points);

    std::vector<cv::Point2f> TransformToUnrotated(std::vector<cv::Point2f> &rotated_points);

    void DownScaleToMaxSize(cv::Mat &image, int max_width, int max_height);

    std::optional<std::vector<cv::Point2f>> FindSudokuField(const cv::Mat &image);

    cv::Mat CropSudokuField(const cv::Mat &src, std::vector<cv::Point2f> &edges);

    std::vector<std::pair<cv::Point, cv::Rect>> FilterNumberSegments(cv::Mat &src);

    std::optional<int> GetBiggestNonBorderContour(std::vector<std::vector<cv::Point>> &contours, int segment_width, int segment_height);

    int RecognizeNumber(const cv::Mat &input, cv::dnn::dnn4_v20221220::Net &model, const cv::Size& size, double scale_factor);

    void DrawPolygon(cv::Mat& image, const std::vector<cv::Point2f>& poly_points, const cv::Scalar& color, int thickness = 1, bool is_closed = false);

}
