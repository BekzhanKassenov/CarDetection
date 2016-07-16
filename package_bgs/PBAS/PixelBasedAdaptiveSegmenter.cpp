#include "PixelBasedAdaptiveSegmenter.h"

PixelBasedAdaptiveSegmenter::PixelBasedAdaptiveSegmenter(const std::string& config_location) :
        first_time(true), show_output(false), enable_input_blur(true),
        enable_output_blur(true),
        alpha(7.0), beta(1.0), N(20), Raute_min(2), R_incdec(0.05),
        R_lower(18),
        R_scale(5), T_dec(0.05), T_inc(1), T_init(18), T_lower(2),
        T_upper(200), config_location(config_location) {
    std::cout << "PixelBasedAdaptiveSegmenter()" << std::endl;
}

PixelBasedAdaptiveSegmenter::~PixelBasedAdaptiveSegmenter() {
    std::cout << "~PixelBasedAdaptiveSegmenter()" << std::endl;
}

void PixelBasedAdaptiveSegmenter::process(const cv::Mat &img_input, cv::Mat &img_output) {
    if (img_input.empty())
        return;

    loadConfig();

    if (first_time) {
        pbas.setAlpha(alpha);
        pbas.setBeta(beta);
        pbas.setN(N);
        pbas.setRaute_min(Raute_min);
        pbas.setR_incdec(R_incdec);
        pbas.setR_lower(R_lower);
        pbas.setR_scale(R_scale);
        pbas.setT_dec(T_dec);
        pbas.setT_inc(T_inc);
        pbas.setT_init(T_init);
        pbas.setT_lower(T_lower);
        pbas.setT_upper(T_upper);

        saveConfig();
    }

    cv::Mat img_input_new;
    if (enable_input_blur)
        cv::GaussianBlur(img_input, img_input_new, cv::Size(5, 5), 1.5);
    else
        img_input.copyTo(img_input_new);

    cv::Mat img_foreground;
    pbas.process(&img_input_new, &img_foreground);

    if (enable_output_blur)
        cv::medianBlur(img_foreground, img_foreground, 5);

    if (show_output)
        cv::imshow("PBAS", img_foreground);

    img_foreground.copyTo(img_output);

    first_time = false;
}

void PixelBasedAdaptiveSegmenter::saveConfig() {
    std::string config_path = config_location + "/PixelBasedAdaptiveSegmenter.xml";
    CvFileStorage *fs = cvOpenFileStorage(config_path.c_str(), 0, CV_STORAGE_WRITE);

    assert(fs != nullptr);

    cvWriteInt(fs, "enable_input_blur", enable_input_blur);
    cvWriteInt(fs, "enable_output_blur", enable_output_blur);

    cvWriteReal(fs, "alpha", alpha);
    cvWriteReal(fs, "beta", beta);
    cvWriteInt(fs, "N", N);
    cvWriteInt(fs, "Raute_min", Raute_min);
    cvWriteReal(fs, "R_incdec", R_incdec);
    cvWriteInt(fs, "R_lower", R_lower);
    cvWriteInt(fs, "R_scale", R_scale);
    cvWriteReal(fs, "T_dec", T_dec);
    cvWriteInt(fs, "T_inc", T_inc);
    cvWriteInt(fs, "T_init", T_init);
    cvWriteInt(fs, "T_lower", T_lower);
    cvWriteInt(fs, "T_upper", T_upper);

    cvWriteInt(fs, "show_output", show_output);

    cvReleaseFileStorage(&fs);
}

void PixelBasedAdaptiveSegmenter::loadConfig() {
    std::string config_path = config_location + "/PixelBasedAdaptiveSegmenter.xml";
    CvFileStorage *fs = cvOpenFileStorage(config_path.c_str(), 0, CV_STORAGE_READ);

    enable_input_blur = cvReadIntByName(fs, 0, "enable_input_blur", true);
    enable_output_blur = cvReadIntByName(fs, 0, "enable_output_blur", true);

    alpha = cvReadRealByName(fs, 0, "alpha", 7.0);
    beta = cvReadRealByName(fs, 0, "beta", 1.0);
    N = cvReadIntByName(fs, 0, "N", 20);
    Raute_min = cvReadIntByName(fs, 0, "Raute_min", 2);
    R_incdec = cvReadRealByName(fs, 0, "R_incdec", 0.05);
    R_lower = cvReadIntByName(fs, 0, "R_lower", 18);
    R_scale = cvReadIntByName(fs, 0, "R_scale", 5);
    T_dec = cvReadRealByName(fs, 0, "T_dec", 0.05);
    T_inc = cvReadIntByName(fs, 0, "T_inc", 1);
    T_init = cvReadIntByName(fs, 0, "T_init", 18);
    T_lower = cvReadIntByName(fs, 0, "T_lower", 2);
    T_upper = cvReadIntByName(fs, 0, "T_upper", 200);

    show_output = cvReadIntByName(fs, 0, "show_output", true);

    cvReleaseFileStorage(&fs);
}