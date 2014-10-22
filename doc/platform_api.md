# API Reference
平台组API

## Module platform
包含平台的设置和少量功能

异常继承关系

    IOError
        AIError
            AIFileError      AI文件错误
            AIConnectError   AI连接错误

## Module battle
一般性对战，指令完全由调用者提供

|      方法      | 返回值 |   描述   |
|----------------|--------|----------|
| load(filename) | Battle | 载入游戏 |

类继承关系

    object
        Battle

### Class Battle
对战基类

指令添加方式由子类提供

|                      构造函数                      |        描述        |
|----------------------------------------------------|--------------------|
| Battle(map_info, team0_name=None, team1_name=None) | 用地图信息构造战斗 |

|                      方法                     |        返回值        |                                描述                                |
|-----------------------------------------------|----------------------|--------------------------------------------------------------------|
| **基本信息**                                  |                      |                                                                    |
| map_info()                                    | MapInfo              | 当前的地图信息                                                     |
| team_name(team)                               | str                  | 队伍名                                                             |
| **游戏状态**                                  |                      |                                                                    |
| round()                                       | int                  | 返回当前回合数                                                     |
| state()                                       | int                  | 返回当前的游戏状态                                                 |
| score(team)                                   | int                  | 返回分数                                                           |
| elements(team)                                | {int:Element}        | 返回某队所有元素                                                   |
| vision(team)                                  | set(Position)[3]     | 返回某队三层视野的点集                                             |
| view_elements(perspective)                    | {int:Element}        | 返回某队视角下的所有元素                                           |
| production_list(team)                         | [[kind, round_left]] | 返回生产列表                                                       |
| population(team)                              | int                  | 返回该队伍的人口数                                                 |
| **统计数据**                                  |                      |                                                                    |
| score_history()                               | [int[2]]             | 分数历史                                                           |
| unit_num_history()                            | [int[2]]             | 单位数历史                                                         |
| population_history()                          | [int[2]]             | 人口历史                                                           |
| command_history()                             | [[Command][2]]       | 指令历史                                                           |
| event_history()                               | [[Event]]            | 事件历史                                                           |
| **运行/指令**                                 |                      |                                                                    |
| set_command(team, command)                    | bool                 | 添加指令，指令无效则返回False                                      |
| commands(team)                                | [Command]            | 返回该队伍当前的指令集                                             |
| next_round()                                  | [Event]              | 进行一回合                                                         |
| **保存/载入**                                 |                      |                                                                    |
| save(filename, compact=False, compress=False) | None                 | 保存游戏, compact为True则将省略空白符, compress=True则会以压缩存档 |
| save_event_strings(filename)                  | None                 | 保存事件字符串                                                     |


## Module ai_battle
AI对战，指令均来自于AI

|                                               方法                                               |  返回值  |                      描述                     |
|--------------------------------------------------------------------------------------------------|----------|-----------------------------------------------|
| load(filename, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT, ai0_filename=None, ai1_filename=None) | AIBattle | 载入游戏, 可能抛出AIFileError, AIConnectError |

类继承关系

    object
        Battle
            AIBattle

### Class AIBattle
AI对战

|                                               构造函数                                               |                 描述                |
|------------------------------------------------------------------------------------------------------|-------------------------------------|
| AIBattle(map_info, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT, ai0_filename=None, ai1_filename=None) | 可能抛出AIFileError, AIConnectError |

|                方法               | 返回值 |            描述            |
|-----------------------------------|--------|----------------------------|
| feed_ai_commands(sleep_time=None) | None   | 从AI获取指令，填充至指令集 |
| run_until_end()                   | None   | 运行直至比赛结束           |

上述两方法可能抛出 AIConnectError


## Module human_ai_battle
人机对战，指令部分来自于AI

|                                                        方法                                                       |    返回值     |                      描述                     |
|-------------------------------------------------------------------------------------------------------------------|---------------|-----------------------------------------------|
| load(filename, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT, human_team_name=None, ai_filename=None, ai_team_num=1) | HumanAIBattle | 载入游戏, 可能抛出AIFileError, AIConnectError |

类继承关系

    object
        Battle
            HumanAIBattle

### Class HumanAIBattle
人机对战

|                                                          构造函数                                                          |                 描述                |
|----------------------------------------------------------------------------------------------------------------------------|-------------------------------------|
| HumanAIBattle(map_info, port=DEFAULT_PORT, timeout=DEFAULT_TIMEOUT, human_team_name=None, ai_filename=None, ai_team_num=1) | 可能抛出AIFileError, AIConnectError |

|                方法               | 返回值 |                         描述                        |
|-----------------------------------|--------|-----------------------------------------------------|
| feed_ai_commands(sleep_time=None) | None   | 从AI获取指令，填充至指令集, 可能抛出 AIConnectError |
| add_command(command)              | bool   | 向选手指令集中增添指令                              |


## Module replayer
回放器，指令来自游戏存档

|      方法      |  返回值  |   描述   |
|----------------|----------|----------|
| load(filename) | Replayer | 载入游戏 |

类继承关系

    object
        Battle
            Replayer

## Class Replayer
回放器

回放器的统计数据不会变化

|    属性   | 类型 |    描述    |
|-----------|------|------------|
| max_round | int  | 最大回合数 |

|     方法     | 返回值 |       描述       |
|--------------|--------|------------------|
| goto_begin() | int    | 返回回放开始回合 |
| goto_end()   | int    | 返回回放结束回合 |
| goto(round)  | int    | 前往指定回合     |
