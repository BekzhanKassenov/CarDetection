#pragma once

#include <iostream>
#include <cv.h>
#include <highgui.h>

#include "PBAS.h"
#include "../IBGS.h"

class PixelBasedAdaptiveSegmenter : public IBGS {
private:
    PBAS pbas;

    bool first_time;
    bool show_output;
    bool enable_input_blur;
    bool enable_output_blur;

    float alpha;
    float beta;
    int N;
    int Raute_min;
    float R_incdec;
    int R_lower;
    int R_scale;
    float T_dec;
    int T_inc;
    int T_init;
    int T_lower;
    int T_upper;

    std::string config_location;

    // Disallow creation of PixelBasedAdaptiveSegmenter without
    // config_location
    PixelBasedAdaptiveSegmenter();
public:

    PixelBasedAdaptiveSegmenter(const std::string& config_location);

    ~PixelBasedAdaptiveSegmenter();

    void process(const cv::Mat &img_input, cv::Mat &img_output);

private:
    void saveConfig();

    void loadConfig();
};