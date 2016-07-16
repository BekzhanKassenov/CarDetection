#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "package_bgs/PBAS/PixelBasedAdaptiveSegmenter.h"
#include "package_tracking/BlobTracking.h"
#include "package_analysis/VehicleCounting.h"

namespace CarDetection {
    const cv::String keys =
        "{help h |            | print this message}"
        "{config | \"config\" | path to the config}"
        "{video  | \"video\"  | path to the video file}";

    std::string config_location;
    std::string video_file;

    void parseCommandLineArguments(int argc, const char * const * argv) {
        cv::CommandLineParser parser(argc, argv, keys.c_str());
        config_location = parser.get<std::string>("config");
        video_file = parser.get<std::string>("video");
    }
}

void drawRectangles(cv::Mat& image, const std::vector<cv::Rect>& objects) {
    for (const auto& rect: objects) {
        cv::rectangle(image, rect, CV_RGB(225, 0, 0));
    }
}

int main(int argc, const char * const * argv) {
    std::cout << "Using OpenCV " << CV_MAJOR_VERSION << "." << CV_MINOR_VERSION << "." << CV_SUBMINOR_VERSION <<
    std::endl;

    CarDetection::parseCommandLineArguments(argc, argv);

    /* Open video file */
    cv::VideoCapture capture(CarDetection::video_file);
    if (!capture.isOpened()) {
        std::cerr << "Cannot open video!" << std::endl;
        return 1;
    }

    /* Background Subtraction Algorithm */
    cv::Ptr<IBGS> bgs = cv::makePtr<PixelBasedAdaptiveSegmenter>(CarDetection::config_location);

    /* Blob Tracking Algorithm */
    cv::Mat img_blob;
    cv::Ptr<BlobTracking> blobTracking = cv::makePtr<BlobTracking>(CarDetection::config_location);

    /* Vehicle Counting Algorithm */
    cv::Ptr<VehicleCounting> vehicleCounting = cv::makePtr<VehicleCounting>(CarDetection::config_location);

    std::cout << "Press 'q' to quit..." << std::endl;
    int key = 0;
    cv::Mat img_input;
    while (key != 'q') {
        capture >> img_input;

        if (img_input.empty()) {
            break;
        }


        //cv::imshow("Input", img_input);

        // bgs->process(...) internally process and show the foreground mask image
        cv::Mat img_mask;
        bgs->process(img_input, img_mask);

        if (!img_mask.empty()) {
            // Perform blob tracking
            blobTracking->process(img_input, img_mask, img_blob);

            // Perform vehicle counting
            std::vector <cv::Rect> objects;
            vehicleCounting->process(img_blob, blobTracking->getTracks(), objects);

            //drawRectangles(img_input, objects);
            if (!objects.empty()) {
                cv::imwrite("result.jpg", img_input);
            }
        }

        cv::imshow("Video", img_input);

        key = cvWaitKey(1);
    }

    cv::destroyAllWindows();
    capture.release();

    return 0;
}
