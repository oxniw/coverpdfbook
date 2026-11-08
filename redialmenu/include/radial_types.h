// ==============================================================================
// File: .//Description-of-file/radial_types.md
// Overview: Core type definitions, enums, and callback prototypes for radial menu.
// ==============================================================================

#ifndef RADIAL_TYPES_H
#define RADIAL_TYPES_H

#include <stdint.h>
#include <stdbool.h>

#ifdef __cplusplus
extern "C" {
#endif

#define RADIAL_MAX_SECTORS 16
#define RADIAL_DEFAULT_SECTOR_COUNT 4

/**
 * @brief Quadrant / Sector indices:
 * - 0: Center circle ("Nothing" / Dead-zone / Cancel)
 * - 1..N: Outer sectors ("function1" .. "functionN")
 */
typedef enum {
    RADIAL_SECTOR_NOTHING = 0,
    RADIAL_SECTOR_FUNCTION1 = 1,
    RADIAL_SECTOR_FUNCTION2 = 2,
    RADIAL_SECTOR_FUNCTION3 = 3,
    RADIAL_SECTOR_FUNCTION4 = 4
} RadialSector;

/**
 * @brief Gesture interaction state machine.
 */
typedef enum {
    GESTURE_STATE_IDLE = 0,
    GESTURE_STATE_PENDING = 1,
    GESTURE_STATE_ACTIVE = 2
} GestureState;

/**
 * @brief Geometry calculation result.
 */
typedef struct {
    double distance;
    double angle_deg;
    RadialSector sector;
} GeometryResult;

/**
 * @brief 2D Point structure.
 */
typedef struct {
    int32_t x;
    int32_t y;
} RadialPoint;

/**
 * @brief Callback invoked when a sector action is confirmed upon release.
 * @param sector The selected RadialSector enum.
 * @param user_data Opaque pointer passed during callback registration.
 */
typedef void (*RadialActionCallback)(int32_t sector, void* user_data);

#ifdef __cplusplus
}
#endif

#endif // RADIAL_TYPES_H
