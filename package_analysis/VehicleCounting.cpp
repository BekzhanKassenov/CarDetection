#include "VehicleCounting.h"

/*
 ========================================
 = Vehicle counting definition          =
 ========================================
 */
VehicleCounting::VehicleCounting(const std::string& config_location) :
        classifier(cv::makePtr<cv::CascadeClassifier>()),
        countAB(0),
        countBA(0),
        config_location(config_location) {

    loadConfig();
    std::cout << "VehicleCounting()" << std::endl;
}

VehicleCounting::VehicleCounting() { }

VehicleCounting::~VehicleCounting() {
    std::cout << "~VehicleCounting()" << std::endl;
}

void VehicleCounting::process(const cv::Mat &img_input, const cvb::CvTracks &tracks,
                              std::vector<cv::Rect> &result) {
    if (img_input.empty())
        return;

    result.clear();

    bool detected = false;
    std::vector <cv::Rect> cars;

    cv::Mat toShow;
    for (const std::pair <cvb::CvID, cvb::CvTrack*> &it: tracks) {
        cvb::CvID id = it.first;
        cvb::CvTrack *track = it.second;

        CvPoint2D64f centroid = track->centroid;

        if (!track->inactive) {
            if (positions.count(id) > 0) {
                auto it2 = positions.find(id);
                VehiclePosition old_position = it2->second;

                VehiclePosition current_position = separator.getVehiclePosition(centroid);

                if (current_position != old_position) {
                    if (!detected) {
                        cv::Mat res1, res2;
                        cv::transpose(img_input, res1);
                        cv::flip(res1, res2, 0);
                        classifier->detectMultiScale(
                                res2,
                                cars,
                                1.05,
                                1,
                                0,
                                cv::Size(0, 0),
                                img_input.size()
                        );

                        std::cout << "Detected " << cars.size() << " cars." << std::endl;
                        res1 = img_input;
                        for (const auto& rect: cars) {
                            cv::rectangle(res2, rect, CV_RGB(255, 0, 0));
                        }

                        cv::flip(res2, res1, 0);
                        cv::transpose(res1, res2);
                        toShow = res2;
                        detected = true;
                    }

                    cv::Rect borders;

                    if (old_position == VP_A && current_position == VP_B) {
                        if (pointDetected(cars, centroid, borders)) {
                            countAB++;
                            result.push_back(borders);
                        }
                    }

                    if (old_position == VP_B && current_position == VP_A) {
                        if (pointDetected(cars, centroid, borders)) {
                            countBA++;
                            result.push_back(borders);
                        }
                    }

                    positions.erase(it2);
                }
            }
            else {
                positions[id] = separator.getVehiclePosition(centroid);
            }
        }
        else {
            if (positions.count(id) > 0) {
                positions.erase(positions.find(id));
            }
        }
    }

    if (toShow.empty()) {
        cv::imshow("Counting", img_input);
    } else {
        cv::imshow("Counting", toShow);
    }

    cv::waitKey(1);
}

bool VehicleCounting::pointDetected(const std::vector<cv::Rect>& objects, const CvPoint2D64f& p_input, cv::Rect& result) {
    cv::Point p(p_input.x, p_input.y);

    for (const auto& rect: objects) {
        if (rect.contains(p)) {
            result = rect;
            return true;
        }
    }

    return false;
}

void VehicleCounting::loadConfig() {
    classifier->load(config_location + "/HaarFeaturesCars.xml");

    cv::FileStorage storage(config_location + "/VehicleCounting.xml", cv::FileStorage::READ);

    int from_x, from_y;
    int to_x, to_y;

    storage["from_x"] >> from_x;
    storage["from_y"] >> from_y;

    storage["to_x"] >> to_x;
    storage["to_y"] >> to_y;

    storage.release();

    separator.setPoints(from_x, from_y, to_x, to_y);
}
