//
// Created by bekzhan on 5/2/16.
//

#pragma once

#include <cassert>
#include <cv.h>

enum VehiclePosition {
    VP_A = 1,
    VP_B = 2
};

class Lane {
private:
    int a, b, c;

public:
    Lane();

    ~Lane();

    void setPoints(int xFrom, int yFrom, int xTo, int yTo);

    VehiclePosition getVehiclePosition(const CvPoint2D64f&);
};
