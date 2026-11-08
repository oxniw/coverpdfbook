// ==============================================================================
// File: .//Description-of-file/math_geometry.md
// Overview: Header for pure mathematical geometry and sector resolution routines.
// ==============================================================================

#ifndef MATH_GEOMETRY_H
#define MATH_GEOMETRY_H

#include "../include/radial_types.h"

#ifdef __cplusplus
extern "C" {
#endif

/**
 * @brief Calculate Euclidean distance between two 2D points.
 */
double math_distance(double x1, double y1, double x2, double y2);

/**
 * @brief Calculate angle in degrees [0, 360) from origin (x0, y0) to point (x, y).
 * 0 degrees = positive X axis (3 o'clock), 90 deg = positive Y (screen down / 6 o'clock).
 */
double math_angle_deg(double x0, double y0, double x, double y);

/**
 * @brief Classify cursor position into RadialSector for dynamic N outer sectors.
 *
 * @param origin_x Center X coordinate where gesture started.
 * @param origin_y Center Y coordinate where gesture started.
 * @param current_x Current cursor X coordinate.
 * @param current_y Current cursor Y coordinate.
 * @param inner_radius Radius threshold for center "Nothing" circle.
 * @param sector_count Total number of outer sectors N (e.g. 4, 6, 8).
 * @return GeometryResult containing distance, angle, and resolved sector (0=Nothing, 1..N).
 */
GeometryResult math_resolve_sector_n(double origin_x, double origin_y,
                                     double current_x, double current_y,
                                     double inner_radius,
                                     int sector_count);

/**
 * @brief Classify cursor position into RadialSector (4 default quadrants).
 *
 * @param origin_x Center X coordinate where gesture started.
 * @param origin_y Center Y coordinate where gesture started.
 * @param current_x Current cursor X coordinate.
 * @param current_y Current cursor Y coordinate.
 * @param inner_radius Radius threshold for center "Nothing" circle.
 * @return GeometryResult containing distance, angle, and resolved sector.
 */
GeometryResult math_resolve_sector(double origin_x, double origin_y,
                                   double current_x, double current_y,
                                   double inner_radius);

#ifdef __cplusplus
}
#endif

#endif // MATH_GEOMETRY_H
