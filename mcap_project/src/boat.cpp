#include <opencv2/opencv.hpp>
#include <iostream>
#include <chrono>

int main() {
    auto total_start = std::chrono::high_resolution_clock::now();

    // Load grayscale image
    std::string image_path = "/home/nvidia/Desktop/mcap/mcap2/image/exampleinput/example_input/boat.jpg";
    auto t1 = std::chrono::high_resolution_clock::now();
    cv::Mat img = cv::imread(image_path, cv::IMREAD_GRAYSCALE);
    auto t2 = std::chrono::high_resolution_clock::now();

    if (img.empty()) {
        std::cerr << "Failed to load image: " << image_path << std::endl;
        return -1;
    }

    // Save input image
    cv::imwrite("boat_input.jpg", img);

    // Apply median filter on original 8-bit image
    auto t3 = std::chrono::high_resolution_clock::now();
    cv::Mat filtered_8u;
    cv::medianBlur(img, filtered_8u, 11);
    auto t4 = std::chrono::high_resolution_clock::now();

    // Convert to float [0, 1]
    auto t5 = std::chrono::high_resolution_clock::now();
    cv::Mat float_img;
    filtered_8u.convertTo(float_img, CV_32F, 1.0 / 255.0);
    auto t6 = std::chrono::high_resolution_clock::now();

    // Rescale intensity
    auto t7 = std::chrono::high_resolution_clock::now();
    double minVal, maxVal;
    cv::minMaxLoc(float_img, &minVal, &maxVal);
    cv::Mat rescaled;
    float_img.convertTo(rescaled, CV_32F, 1.0 / (maxVal - minVal), -minVal / (maxVal - minVal));
    auto t8 = std::chrono::high_resolution_clock::now();

    // Low-pass blur
    auto t9 = std::chrono::high_resolution_clock::now();
    cv::Mat low_pass;
    cv::blur(rescaled, low_pass, cv::Size(9, 9));
    auto t10 = std::chrono::high_resolution_clock::now();

    // High-pass enhancement
    auto t11 = std::chrono::high_resolution_clock::now();
    cv::Mat high_pass = rescaled - low_pass;
    cv::Mat sharpened = rescaled + high_pass;
    auto t12 = std::chrono::high_resolution_clock::now();

    // Clip values to [0,1] and convert back to 8-bit
    auto t13 = std::chrono::high_resolution_clock::now();
    cv::Mat clipped;
    cv::threshold(sharpened, clipped, 1.0, 1.0, cv::THRESH_TRUNC);   // Clip upper bound
    cv::threshold(clipped, clipped, 0.0, 0.0, cv::THRESH_TOZERO);    // Clip lower bound
    cv::Mat output_img;
    clipped.convertTo(output_img, CV_8U, 255.0);
    auto t14 = std::chrono::high_resolution_clock::now();

    // Save output image
    cv::imwrite("boat_output.jpg", output_img);

    // Display input and output images
    cv::imshow("Original Image", img);
    cv::imshow("Sharpened Image", output_img);

    // Wait indefinitely until any key is pressed
    cv::waitKey(0);
    //cv::destroyAllWindows();

    auto total_end = std::chrono::high_resolution_clock::now();

    // Timing helper
    auto ms = [](auto start, auto end) {
        return std::chrono::duration<double, std::milli>(end - start).count();
    };

    // Print timing summary
    std::cout << "\n=== Execution Time Summary ===\n";
    std::cout << "Image Load Time:              " << ms(t1, t2) << " ms\n";
    std::cout << "Median Filter Time:           " << ms(t3, t4) << " ms\n";
    std::cout << "Float Conversion Time:        " << ms(t5, t6) << " ms\n";
    std::cout << "Rescale Intensity Time:       " << ms(t7, t8) << " ms\n";
    std::cout << "Low-pass Blur Time:           " << ms(t9, t10) << " ms\n";
    std::cout << "High-pass Sharpening Time:    " << ms(t11, t12) << " ms\n";
    std::cout << "Clip & Convert to 8-bit Time: " << ms(t13, t14) << " ms\n";
    std::cout << "Total Execution Time:         " << ms(total_start, total_end) << " ms\n";

    return 0;
}

