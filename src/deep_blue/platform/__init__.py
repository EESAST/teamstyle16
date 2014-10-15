import logging
logging.basicConfig(level=logging.INFO)

from logic import *
import battle
import ai_battle
import replayer
from ai_proxy import AIError, AIFileError, AIConnectError
