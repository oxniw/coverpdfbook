// ==============================================================================
// File: .//Description-of-file/math_geometry.md
// Overview: Pure C implementation of geometry and quadrant sector classification.
// ==============================================================================

#include "math_geometry.h"
#include <math.h>

#ifndef M_PI
#define M_PI 3.14159265358979323846
#endif

double math_distance(double x1, double y1, double x2, double y2) {
    double dx = x2 - x1;
    double dy = y2 - y1;
    return sqrt(dx * dx + dy * dy);
}

double math_angle_deg(double x0, double y0, double x, double y) {
    double dx = x - x0;
    double dy = y - y0;
    double rad = atan2(dy, dx);
    double deg = rad * (180.0 / M_PI);
    if (deg < 0.0) {
        deg += 360.0;
    }
    return deg;
}

GeometryResult math_resolve_sector_n(double origin_x, double origin_y,
                                     double current_x, double current_y,
                                     double inner_radius,
                                     int sector_count) {
    GeometryResult res;
    double dx = current_x - origin_x;
    double dy = current_y - origin_y;
    res.distance = sqrt(dx * dx + dy * dy);
    res.angle_deg = math_angle_deg(origin_x, origin_y, current_x, current_y);

    if (sector_count < 2) sector_count = 2;
    if (sector_count > RADIAL_MAX_SECTORS) sector_count = RADIAL_MAX_SECTORS;

    // Center dead-zone
    if (res.distance <= inner_radius) {
        res.sector = RADIAL_SECTOR_NOTHING;
        return res;
    }

    // Measure angle clockwise starting from 12 o'clock (-90 degrees / 270 degrees)
    // 12 o'clock is angle_deg = 270.
    // Normalized offset angle [0, 360) clockwise from 12 o'clock:
    double offset_angle = res.angle_deg + 90.0;
    while (offset_angle >= 360.0) offset_angle -= 360.0;
    while (offset_angle < 0.0) offset_angle += 360.0;

    double sweep = 360.0 / (double)sector_count;
    int index = 1 + (int)(offset_angle / sweep);
    if (index > sector_count) index = sector_count;
    if (index < 1) index = 1;

    res.sector = (RadialSector)index;
    return res;
}

GeometryResult math_resolve_sector(double origin_x, double origin_y,
                                   double current_x, double current_y,
                                   double inner_radius) {
    return math_resolve_sector_n(origin_x, origin_y, current_x, current_y, inner_radius, 4);
}
