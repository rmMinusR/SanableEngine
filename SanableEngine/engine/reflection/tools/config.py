import logging

formatter = logging.Formatter(fmt="%(levelname)-9s %(message)s")

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(formatter)

global logger
logger = logging.Logger("STIX")
logger.setLevel("INFO")
logger.addHandler(consoleHandler)
