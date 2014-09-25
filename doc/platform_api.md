# API Reference
平台组API

## Module platform
包含平台的设置和少量功能

|          方法         |  返回值  |     描述     |
|-----------------------|----------|--------------|
| load_game(filename)   | Battle   | 载入游戏     |
| load_replay(filename) | Replayer | 载入回放文件 |

## Module battle

类继承关系

    object
        Battle
            AIBattle
            HumanAIBattle
            Replayer

异常继承关系

    IOError
        AIProxyError
            AIFileError      AI文件错误
            AIConnectError   AI连接错误
            ParseError       指令解析错误


### Class Battle
对战基类

指令添加方式由子类提供

|     构造函数     |        描述        |
|------------------|--------------------|
| Battle(map_info) | 用地图信息构造战斗 |

|                   方法                   |        返回值        |               描述               |
|------------------------------------------|----------------------|----------------------------------|
| **基本信息**                             |                      |                                  |
| map_info()                               | MapInfo              | 当前的地图信息                   |
| team_name(team)                          | str                  | 队伍名                           |
| **游戏状态**                             |                      |                                  |
| round()                                  | int                  | 返回当前回合数                   |
| status()                                 | int                  | 返回当前的游戏状态               |
| score(team)                              | int                  | 返回分数                         |
| elements(perspective)                    | [Element]            | 返回某队视角下的所有元素         |
| production_list(team)                    | [(kind, round_left)] | 返回生产列表                     |
| population(team)                         | int                  | 返回该队伍的人口数               |
| **统计数据**                             |                      |                                  |
| score_history()                          | [int[2]]             | 分数历史                         |
| unit_num_history()                       | [int[2]]             | 单位数历史                       |
| population_history()                     | [int[2]]             | 人口历史                         |
| command_history()                        | [[Command][2]]       | 指令历史 (攻击指令经过修饰)      |
| **运行/指令**                            |                      |                                  |
| commands(team)                           | [Command]            | 返回该队伍当前的指令集           |
| next_round()                             | [gamebody.Event]     | 进行一回合                       |
| **保存/载入**                            |                      |                                  |
| save_game(filename)                      | None                 | 保存游戏                         |
| load_game(filename)                      | None                 | 载入游戏                         |
| save_replay(filename, begin=0, end=None) | None                 | 保存[begin, end]回合的回放至文件 |


### Class AIBattle
AI对战

|                         构造函数                         |                 描述                |
|----------------------------------------------------------|-------------------------------------|
| AIBattle(map_info, ai0_filename, ai1_filename, port=...) | 可能抛出AIFileError, AIConnectError |

|        方法        | 返回值 |            描述            |
|--------------------|--------|----------------------------|
| feed_ai_commands() | None   | 从AI获取指令，填充至指令集 |
| run_until_end()    | None   | 运行直至比赛结束           |

上述两方法可能抛出 AIConnectError, ParseError


### Class HumanAIBattle
人机对战

|             构造函数            |                 描述                |
|---------------------------------|-------------------------------------|
| HumanAIBattle(map, ai_filename, port=...) | 可能抛出AIFileError, AIConnectError |

|         方法         | 返回值 |            描述            |
|----------------------|--------|----------------------------|
| feed_ai_commands()   | None   | 从AI获取指令，填充至指令集 |
| add_command(command) | None   | 向选手指令集中增添指令     |


## Class Replayer
回放器

回放器的统计数据不会变化

|     方法    | 返回值 |       描述       |
|-------------|--------|------------------|
| begin()     | int    | 返回回放开始回合 |
| end()       | int    | 返回回放结束回合 |
| goto(round) | int    | 前往指定回合     |


# 平台组内部

## Module ai_proxy
AI代理，处理游戏程序与AI间的通信


### Class AIProxy

|           构造函数          |                       描述                       |
|-----------------------------|--------------------------------------------------|
| AIProxy(team_num, filename) | 运行该AI程序，并与其连接，获取队名，传递稳定信息。 |

|               方法               |        返回值        |                         描述                         |
|----------------------------------|----------------------|------------------------------------------------------|
| team_num()                       | int                  | 返回队伍编号                                         |
| team_name()                      | str                  | 返回队伍名称                                         |
| get_commands(Battle, time_limit) | ([Command], runtime) | 向选手传递信息，在时限内从选手AI获取指令，并记录耗时 |




