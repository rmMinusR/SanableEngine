import logging

formatter = logging.Formatter(fmt="%(levelname)-8s >> %(message)")

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(formatter)

logger = logging.Logger("Logger")
#logger.addHandler(consoleHandler)
logger.setLevel(logging.WARNING)
