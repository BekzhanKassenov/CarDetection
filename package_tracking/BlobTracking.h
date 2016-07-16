#pragma once

#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "cvblob/cvblob.h"

class BlobTracking {
private:
    bool first_time;
    int min_area;
    int max_area;

    bool debug_track;
    bool debug_blob;
    bool show_blob_mask;
    bool show_output;

    std::string config_location;
    cvb::CvTracks tracks;

    void saveConfig();

    void loadConfig();

    // Disallow creation of BlobTracking without config
    BlobTracking();
public:

    BlobTracking(const std::string& config_location);

    ~BlobTracking();

    void process(const cv::Mat &img_input, const cv::Mat &img_mask, cv::Mat &img_output);

    const cvb::CvTracks getTracks();
};

