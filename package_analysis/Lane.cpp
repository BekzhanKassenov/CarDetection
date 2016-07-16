//
// Created by bekzhan on 5/2/16.
//

#include "Lane.h"

Lane::Lane() : a(0), b(0), c(0) { }

Lane::~Lane() { }

void Lane::setPoints(int xFrom, int yFrom, int xTo, int yTo) {
    a = yTo - yFrom;
    b = xFrom - xTo;
    c = -xFrom * yTo + yFrom * xTo;

    if (a < 0 || (a == 0 && b < 0)) {
        a *= -1, b *= -1, c *= -1;
    }

    assert(a != 0 || b != 0);
}

VehiclePosition Lane::getVehiclePosition(const CvPoint2D64f &centroid) {
    double num = a * centroid.x + b * centroid.y + c;

    return (num <= 0) ? VP_A : VP_B;
}
