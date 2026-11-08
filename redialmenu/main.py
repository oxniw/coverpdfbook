# ==============================================================================
# File: .//Description-of-file/main_py.md
# Overview: Python entry point orchestrating C++ DLL bridge, callbacks, and lifecycle.
# ==============================================================================

import signal
import sys
import time
from radial_bridge import RadialMenuBridge
from action_handlers import dispatch_action
from logger import setup_logger

logger = setup_logger("Main")

def main() -> int:
    logger.info("Starting Radial Menu Gesture System...")

    bridge = RadialMenuBridge()

    if not bridge.initialize():
        logger.error("Initialization failed. Terminating.")
        return 1

    # Configure dynamic N sectors and labels from config
    from config import ACTIONS_CONFIG
    sector_count = len(ACTIONS_CONFIG)
    bridge.set_sector_count(sector_count)
    for i, action in enumerate(ACTIONS_CONFIG, start=1):
        bridge.set_sector_label(i, action["label"])

    bridge.set_callback(dispatch_action)

    if not bridge.start_hook():
        logger.error("Could not install mouse hook. Terminating.")
        bridge.shutdown()
        return 1

    def handle_signal(sig, frame):
        logger.info("Signal received. Exiting cleanly...")
        bridge.shutdown()
        sys.exit(0)

    signal.signal(signal.SIGINT, handle_signal)
    signal.signal(signal.SIGTERM, handle_signal)

    logger.info("System active: Hold right-click and drag to display the radial wheel!")
    logger.info("Releasing in the center circle cancels. Releasing in a quadrant executes.")

    try:
        # Non-blocking poll loop allowing responsive Python Ctrl+C handling
        while True:
            ret = bridge.poll_events()
            if ret == -1:
                break
            time.sleep(0.005)
    except KeyboardInterrupt:
        logger.info("Keyboard interrupt received.")
    finally:
        bridge.shutdown()
        logger.info("Radial Menu system shut down successfully.")

    return 0

if __name__ == "__main__":
    sys.exit(main())
