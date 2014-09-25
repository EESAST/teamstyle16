# -*- coding: UTF-8 -*-
# main.py

from basic import *
from gamebody import *
import map

map = map.Map(types, elements)
Game = GameBody(map, max_population, max_round, team_name)
while True:
    Event = Game.run()
    Status = Game.status()
    if Status == STATE_TIE:
        pass
    elif Status == STATE_CONTINUE:
        pass
    else:
        pass
    
