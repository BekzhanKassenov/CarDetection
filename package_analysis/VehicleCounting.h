#pragma once

#include <iostream>
#include <string>

#include <cv.h>
#include <highgui.h>

#include "../package_tracking/cvblob/cvblob.h"
#include "Lane.h"


class VehicleCounting {
private:
    cv::Ptr <cv::CascadeClassifier> classifier;

    std::map<cvb::CvID, VehiclePosition> positions;

    Lane separator;

    long countAB;
    long countBA;

    std::string config_location;

public:
    VehicleCounting(const std::string& config_location);
    ~VehicleCounting();

    void process(const cv::Mat &, const cvb::CvTracks &, std::vector<cv::Rect> &result);

private:
    // Disallow creation of VehicleCounting without config location
    VehicleCounting();
    bool pointDetected(const std::vector <cv::Rect>&, const CvPoint2D64f&, cv::Rect&);
    void loadConfig();
};
