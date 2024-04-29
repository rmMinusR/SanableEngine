import logging

# Add custom log level

LOG_USER_LEVEL = 100
logging.addLevelName(LOG_USER_LEVEL, "USER")
def log_user_message(self, message, *args, **kws):
    if self.isEnabledFor(LOG_USER_LEVEL): self._log(LOG_USER_LEVEL, message, args, **kws) 
logging.Logger.user = log_user_message


# Add custom formatting

formatter = logging.Formatter(fmt="%(levelname)-10s %(message)s")

consoleHandler = logging.StreamHandler()
consoleHandler.setFormatter(formatter)

global logger
logger = logging.Logger("STIX")
logger.setLevel("INFO")
logger.addHandler(consoleHandler)
