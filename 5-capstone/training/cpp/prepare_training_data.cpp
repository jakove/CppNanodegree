#include <iostream>

#include <opencv2/opencv.hpp>

#include "imageprocessing.h"

int main()
{
    unsigned int counter = 1641;

    std::string path_to_images = "resources/images/training/";
    std::string out_path_prefix = "resources/images/segments";

    std::vector<std::string> image_paths;

    cv::glob(path_to_images + "*", image_paths);

    for (auto& path_to_image : image_paths)
    {
        auto orig_image = cv::imread(path_to_image);

        if (orig_image.empty())
        {
            std::cout << "Could not load image\n";
            continue;
        }

        //int max_rows = 1024, max_cols = 1024;
        //ImageProcessing::DownScaleToMaxSize(orig_image, max_rows, max_cols);

        cv::Mat processed_image;
        cv::cvtColor(orig_image, processed_image, cv::COLOR_BGR2GRAY);

        auto rect_edges = ImageProcessing::FindSudokuField(processed_image);
        if (!rect_edges.has_value())
        {
            "The Sudokufield could not be found\n";
            continue;
        }

        const auto &rect_points = rect_edges.value();
        // from cropping the sodkufield the points are sorted
        ImageProcessing::DrawPolygon(orig_image, rect_points, cv::Scalar(0, 0, 255), 3, true);

        // Todo: show image in QT
        cv::imshow("rect", orig_image);
        cv::waitKey(0);

        auto sudoku_image = ImageProcessing::CropSudokuField(processed_image, rect_edges.value());

        auto rois = ImageProcessing::FilterNumberSegments(sudoku_image);
        auto model = cv::dnn::readNetFromTensorflow("resources/frozen_mnist_retrained.pb");

        bool valid = true;
        for (auto &[position, roi] : rois)
        {
            auto number = ImageProcessing::RecognizeNumber(sudoku_image(roi), model, cv::Size(28, 28), 1 / 255.0);
            std::string path = out_path_prefix + "/" + std::to_string(number) + "/" + std::to_string(counter) + ".png";

            std::cout << path << ": " << number << "\n";

            cv::imwrite(path, sudoku_image(roi));
            counter++;
        }
    }

    return 0;
}