# ==============================================================================
# File: .//Description-of-file/logger_py.md
# Overview: Production-grade structured logging system adhering to prevent-print.
# ==============================================================================

import logging
import sys

def setup_logger(name: str = "RadialMenu") -> logging.Logger:
    """
    Configures and returns a structured logger with timestamps and log levels.
    """
    logger = logging.getLogger(name)
    if not logger.handlers:
        logger.setLevel(logging.INFO)
        formatter = logging.Formatter(
            fmt="[%(asctime)s] [%(levelname)s] [%(name)s]: %(message)s",
            datefmt="%Y-%m-%d %H:%M:%S"
        )
        handler = logging.StreamHandler(sys.stderr)
        handler.setFormatter(formatter)
        logger.addHandler(handler)
    return logger
