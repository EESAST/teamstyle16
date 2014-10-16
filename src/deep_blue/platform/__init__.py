import logging
logging.basicConfig(level=logging.INFO)

DEFAULT_PORT = 8067
DEFAULT_TIMEOUT = 5

from logic import *
import battle
import ai_battle
import human_ai_battle
import replayer
from ai_proxy import AIError, AIFileError, AIConnectError
