#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>
#include <omp.h>  // OpenMP

int main() {
    auto total_start = std::chrono::high_resolution_clock::now();

    std::string image_path = "/home/nvidia/Desktop/mcap/mcap2/image/exampleinput/example_input/boat.jpg";
    cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
    if (img.empty()) {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return -1;
    }

    auto t1 = std::chrono::high_resolution_clock::now();
    cv::Mat filtered_8u;
    cv::medianBlur(img, filtered_8u, 11);
    auto t2 = std::chrono::high_resolution_clock::now();

    cv::Mat float_img;
    filtered_8u.convertTo(float_img, CV_32F, 1.0 / 255.0);

    // Compute min/max manually using OpenMP
    auto t3 = std::chrono::high_resolution_clock::now();
    double minVal = 1.0, maxVal = 0.0;
    #pragma omp parallel for reduction(min:minVal) reduction(max:maxVal)
    for (int i = 0; i < float_img.rows; ++i) {
        for (int j = 0; j < float_img.cols; ++j) {
            float val = float_img.at<float>(i, j);
            if (val < minVal) minVal = val;
            if (val > maxVal) maxVal = val;
        }
    }

    cv::Mat rescaled = float_img.clone();
    #pragma omp parallel for
    for (int i = 0; i < float_img.rows; ++i) {
        for (int j = 0; j < float_img.cols; ++j) {
            rescaled.at<float>(i, j) = (float_img.at<float>(i, j) - minVal) / (maxVal - minVal);
        }
    }
    auto t4 = std::chrono::high_resolution_clock::now();

    cv::Mat low_pass;
    cv::blur(rescaled, low_pass, cv::Size(9, 9));

    // High-pass + Sharpen using OpenMP
    auto t5 = std::chrono::high_resolution_clock::now();
    cv::Mat sharpened = rescaled.clone();
    #pragma omp parallel for
    for (int i = 0; i < rescaled.rows; ++i) {
        for (int j = 0; j < rescaled.cols; ++j) {
            float val = rescaled.at<float>(i, j) + (rescaled.at<float>(i, j) - low_pass.at<float>(i, j));
            val = std::min(1.0f, std::max(0.0f, val)); // clip to [0,1]
            sharpened.at<float>(i, j) = val;
        }
    }
    auto t6 = std::chrono::high_resolution_clock::now();

    cv::Mat output_img;
    sharpened.convertTo(output_img, CV_8U, 255.0);

    cv::imwrite("enigma_out.jpg", output_img);
    cv::imshow("Original", img);
    cv::imshow("Sharpened", output_img);
    cv::waitKey(0);
    cv::destroyAllWindows();

    auto total_end = std::chrono::high_resolution_clock::now();

    // Timing
    auto ms = [](auto start, auto end) {
        return std::chrono::duration<double, std::milli>(end - start).count();
    };

    std::cout << "Median Filter Time: " << ms(t1, t2) << " ms\n";
    std::cout << "Rescale + Sharpen (OpenMP) Time: " << ms(t3, t6) << " ms\n";
    std::cout << "Total Execution Time: " << ms(total_start, total_end) << " ms\n";

    return 0;
}

