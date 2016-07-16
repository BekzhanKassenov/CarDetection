#include "BlobTracking.h"

BlobTracking::BlobTracking() { }

BlobTracking::BlobTracking(const std::string& config_location) :
        first_time(true), min_area(500), max_area(20000), debug_track(false),
        debug_blob(false), show_blob_mask(false), show_output(false), config_location(config_location) {
    std::cout << "BlobTracking()" << std::endl;
}

BlobTracking::~BlobTracking() {
    std::cout << "~BlobTracking()" << std::endl;
}

const cvb::CvTracks BlobTracking::getTracks() {
    return tracks;
}

void BlobTracking::process(const cv::Mat &img_input, const cv::Mat &img_mask, cv::Mat &img_output) {
    if (img_input.empty() || img_mask.empty())
        return;

    loadConfig();

    if (first_time)
        saveConfig();

    IplImage *frame = new IplImage(img_input);
    cvConvertScale(frame, frame, 1, 0);

    IplImage *segmentated = new IplImage(img_mask);

    IplConvKernel *morphKernel = cvCreateStructuringElementEx(5, 5, 1, 1, CV_SHAPE_RECT, NULL);
    cvMorphologyEx(segmentated, segmentated, NULL, morphKernel, CV_MOP_OPEN, 1);

    if (show_blob_mask)
        cvShowImage("Blob Mask", segmentated);

    IplImage *labelImg = cvCreateImage(cvGetSize(frame), IPL_DEPTH_LABEL, 1);

    cvb::CvBlobs blobs;
    unsigned int result = cvb::cvLabel(segmentated, labelImg, blobs);

    //cvb::cvFilterByArea(blobs, 500, 1000000);
    cvb::cvFilterByArea(blobs, min_area, max_area);

    //cvb::cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX);
    if (debug_blob)
        cvb::cvRenderBlobs(labelImg, blobs, frame, frame,
                           CV_BLOB_RENDER_BOUNDING_BOX | CV_BLOB_RENDER_CENTROID | CV_BLOB_RENDER_ANGLE |
                           CV_BLOB_RENDER_TO_STD);
//  else
//    cvb::cvRenderBlobs(labelImg, blobs, frame, frame, CV_BLOB_RENDER_BOUNDING_BOX|CV_BLOB_RENDER_CENTROID|CV_BLOB_RENDER_ANGLE);

    cvb::cvUpdateTracks(blobs, tracks, 200., 5);

    if (debug_track)
        cvb::cvRenderTracks(tracks, frame, frame,
                            CV_TRACK_RENDER_ID | CV_TRACK_RENDER_BOUNDING_BOX | CV_TRACK_RENDER_TO_STD);
//  else
//    cvb::cvRenderTracks(tracks, frame, frame, CV_TRACK_RENDER_ID|CV_TRACK_RENDER_BOUNDING_BOX);
//
    //std::map<CvID, CvTrack *> CvTracks

    if (show_output)
        cvShowImage("Blob Tracking", frame);

    cv::Mat img_result(frame);
    img_result.copyTo(img_output);

    //cvReleaseImage(&frame);
    //cvReleaseImage(&segmentated);
    cvReleaseImage(&labelImg);
    delete frame;
    delete segmentated;
    cvReleaseBlobs(blobs);
    cvReleaseStructuringElement(&morphKernel);

    first_time = false;
}

void BlobTracking::saveConfig() {
    std::string config_path = config_location + "/BlobTracking.xml";
    CvFileStorage *fs = cvOpenFileStorage(config_path.c_str(), 0, CV_STORAGE_WRITE);

    cvWriteInt(fs, "min_area", min_area);
    cvWriteInt(fs, "max_area", max_area);

    cvWriteInt(fs, "debug_track", debug_track);
    cvWriteInt(fs, "debug_blob", debug_blob);
    cvWriteInt(fs, "show_blob_mask", show_blob_mask);
    cvWriteInt(fs, "show_output", show_output);

    cvReleaseFileStorage(&fs);
}

void BlobTracking::loadConfig() {
    std::string config_path = config_location + "/BlobTracking.xml";
    CvFileStorage *fs = cvOpenFileStorage(config_path.c_str(), 0, CV_STORAGE_READ);

    min_area = cvReadIntByName(fs, 0, "min_area", 500);
    max_area = cvReadIntByName(fs, 0, "max_area", 20000);

    debug_track = cvReadIntByName(fs, 0, "debug_track", false);
    debug_blob = cvReadIntByName(fs, 0, "debug_blob", false);
    show_blob_mask = cvReadIntByName(fs, 0, "show_blob_mask", false);
    show_output = cvReadIntByName(fs, 0, "show_output", true);

    cvReleaseFileStorage(&fs);
}