# ==============================================================================
# File: .//Description-of-file/test_suite_py.md
# Overview: Comprehensive unit and integration test suite validating pure geometry math and native DLL bindings.
# ==============================================================================

import unittest # Import standard unittest framework
import logging # Import logging module
from config import AlignmentMode, Point2D, Rect2D, MOVE_WINDOW_SUCCESS, MOVE_WINDOW_ERR_NOT_FOUND # Import configuration
from geometry_calc import calculate_window_target_position, clamp_position_to_bounds # Import domain geometry math
from move_window_api import MoveWindowNativeAPI # Import native library wrapper

logger = logging.getLogger("test_suite") # Initialize test suite logger

class TestGeometryCalculations(unittest.TestCase): # Define pure math test case
    def test_center_alignment(self) -> None: # Test center alignment calculation
        mouse = Point2D(x=500, y=500) # Define cursor position
        pos = calculate_window_target_position(mouse, window_width=200, window_height=100, mode=AlignmentMode.CENTER) # Calculate center position
        self.assertEqual(pos.x, 400) # Verify centered horizontal position (500 - 100)
        self.assertEqual(pos.y, 450) # Verify centered vertical position (500 - 50)

    def test_top_left_alignment(self) -> None: # Test top-left alignment calculation
        mouse = Point2D(x=300, y=400) # Define cursor position
        pos = calculate_window_target_position(mouse, window_width=200, window_height=100, mode=AlignmentMode.TOP_LEFT) # Calculate top-left position
        self.assertEqual(pos.x, 300) # Verify horizontal coordinate snaps to cursor X
        self.assertEqual(pos.y, 400) # Verify vertical coordinate snaps to cursor Y

    def test_custom_offset_alignment(self) -> None: # Test custom offset calculation
        mouse = Point2D(x=300, y=400) # Define cursor position
        pos = calculate_window_target_position(mouse, window_width=200, window_height=100, mode=AlignmentMode.CUSTOM_OFFSET, offset_x=25, offset_y=-15) # Calculate offset position
        self.assertEqual(pos.x, 325) # Verify X offset (300 + 25)
        self.assertEqual(pos.y, 385) # Verify Y offset (400 - 15)

    def test_boundary_clamping_inside(self) -> None: # Test clamping when point is already inside bounds
        bounds = Rect2D(x=0, y=0, width=1920, height=1080) # Define 1080p monitor rectangle
        desired = Point2D(x=100, y=100) # Point inside monitor bounds
        clamped = clamp_position_to_bounds(desired, window_width=400, window_height=300, bounds=bounds) # Apply clamp
        self.assertEqual(clamped.x, 100) # Verify X untouched
        self.assertEqual(clamped.y, 100) # Verify Y untouched

    def test_boundary_clamping_overflow(self) -> None: # Test clamping when window bleeds beyond borders
        bounds = Rect2D(x=0, y=0, width=1920, height=1080) # Define monitor boundaries
        desired_left_overflow = Point2D(x=-50, y=-20) # Overflow on top-left
        clamped_left = clamp_position_to_bounds(desired_left_overflow, window_width=400, window_height=300, bounds=bounds) # Apply clamp
        self.assertEqual(clamped_left.x, 0) # Verify clamped to left edge
        self.assertEqual(clamped_left.y, 0) # Verify clamped to top edge

        desired_right_overflow = Point2D(x=1800, y=900) # Overflow on bottom-right (1800+400 > 1920, 900+300 > 1080)
        clamped_right = clamp_position_to_bounds(desired_right_overflow, window_width=400, window_height=300, bounds=bounds) # Apply clamp
        self.assertEqual(clamped_right.x, 1520) # Verify clamped to right edge (1920 - 400)
        self.assertEqual(clamped_right.y, 780) # Verify clamped to bottom edge (1080 - 300)

class TestNativeLibraryBindings(unittest.TestCase): # Define native binding integration test case
    @classmethod # Class method decorator
    def setUpClass(cls) -> None: # Set up test fixtures
        cls.api = MoveWindowNativeAPI() # Initialize native API wrapper

    def test_cursor_query(self) -> None: # Test native cursor coordinate retrieval
        cursor = self.api.get_cursor_position() # Query cursor via Win32 API
        self.assertIsNotNone(cursor) # Verify cursor is not None
        if cursor: # Guard check
            self.assertIsInstance(cursor.x, int) # Verify X coordinate is integer
            self.assertIsInstance(cursor.y, int) # Verify Y coordinate is integer

    def test_foreground_window_query(self) -> None: # Test native foreground window query
        hwnd = self.api.get_foreground_window() # Fetch active foreground window
        if hwnd: # If active window exists
            bounds = self.api.get_window_bounds(hwnd) # Query its bounds
            self.assertIsNotNone(bounds) # Verify bounds retrieved

    def test_invalid_window_handle_reposition(self) -> None: # Test error handling with dummy handle
        fake_hwnd = 0xFFFFFF # Non-existent window handle
        status = self.api.move_window_to_cursor(fake_hwnd) # Attempt repositioning
        self.assertEqual(status, MOVE_WINDOW_ERR_NOT_FOUND) # Verify error code matches not found

if __name__ == "__main__": # Test execution entry
    unittest.main() # Run all unit tests
