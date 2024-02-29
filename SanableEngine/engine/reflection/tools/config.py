import logging

formatter = logging.Formatter(fmt="%(levelname)-8s >> %(message)")

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(formatter)

global logger
logger = logging.Logger("Logger")
#logger.addHandler(consoleHandler)
