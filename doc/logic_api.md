# API Reference

## Module basic
定义游戏相关的常量和类

类继承关系

    object
        Position
        Rectangle
        Element
            Resource
                Mine
                Oilfield
            UnitBase
                Building
                    Base
                    Fort
                Unit
                    Submarine
                    Ship
                        Cargo
                        Carrier
                        Destroyer
                    Plane
                        Fighter
                        Scout

TODO：游戏常量


### Class Position

|      构造函数     |             描述            |
|-------------------|-----------------------------|
| Position(x, y, z) | 构造坐标为 (x, y, z) 的位置 |

| 属性 | 类型 |  描述  |
|------|------|--------|
| x    | int  | x 坐标 |
| y    | int  | y 坐标 |
| z    | int  | z 坐标 |

### Class Element
地图上所有元素的抽象

注意：下面所有的属性，只代表可以在其末端基类中访问到这些属性，并且可能是类属性

例如，可能Element类和对象里没有kind和size属性，而Unit却有类属性size（Unit都是1x1），Submarine有类属性kind（潜艇的类型是常量）

|   属性   |         类型         |      描述      |
| -------- | -------------------- | -------------- |
| index    | int                  | 元素的索引号   |
| kind     | int                  | 元素类型       |
| position | Position             | 该元素的位置   |
| size     | (x_length, y_length) | 该元素的大小   |
| visible  | bool                 | 该元素是否可见 |

### Class Resource
|    属性    | 类型 |   描述   |
| ---------- | ---- | -------- |
| metal/fuel | int  | 剩余资源 |

### Class UnitBase
|     属性     |  类型  |       描述       |
|--------------|--------|------------------|
| team         | int    | 所属队伍         |
| sight_ranges | int[3] | 视野范围         |
| fire_ranges  | int[3] | 攻击范围         |
| health       | int    | 生命值           |
| health_max   | int    | 生命值上限       |
| fuel         | int    | 燃料             |
| fuel_max     | int    | 燃料上限         |
| ammo         | int    | 弹药             |
| ammo_once    | int    | 单次攻击消耗弹药 |
| ammo_max     | int    | 弹药上限         |
| metal        | int    | 金属             |
| metal_max    | int    | 金属上限         |
| attacks      | int[2] | 攻击             |
| defences     | int[2] | 防御             |

### Class Unit
|     属性    |   类型   |      描述      |
| ----------- | -------- | -------------- |
| speed       | int      | 移动速度       |
| dest        | Position | 当前目的地     |
| cost        | int      | 生产所需金属   |
| population  | int      | 占有人口数     |
| build_round | int      | 生产所需回合数 |


## Module map_info
定义地图信息及相关的函数

类继承关系

    object
        MapInfo

|      方法      | 返回值  |        描述        |
|----------------|---------|--------------------|
| load(filename) | MapInfo | 从文件中载入地图   |
| loads(s)       | MapInfo | 从字符串中载入地图 |

### Class MapInfo
地图信息, 代表可作为游戏开始初始状态的地图 (如普通地图, 战役, 游戏存档等) 以及相应配置

|                                                  构造函数                                                 |   描述   |
| --------------------------------------------------------------------------------------------------------- | -------- |
| MapInfo(x_max, y_max, max_population=..., max_round=..., record_interval=..., time_per_round=..., weather=..., **kwargs) | 构造地图 |

|       属性      |      类型     |       描述       |
| --------------- | ------------- | ---------------- |
| types           | [[int]]       | 地形二维数组     |
| x_max           | int           | 地图行数         |
| y_max           | int           | 地图列数         |
| elements        | {int:Element} | 地图上所有元素   |
| max_polulation  | int           | 人口上限         |
| max_round       | int           | 最大回合数       |
| record_interval | int           | 统计数据记录间隔 |
| time_per_round  | float         | 每回合时间       |
| weather         | int           | 天气             |

|             方法             |   返回值   |                       描述                       |
| ---------------------------- | ---------- | ------------------------------------------------ |
| map_type(x, y)               | int        | 查询地形                                         |
| set_map_type(x, y, map_type) | bool       | 设置地形                                         |
| element(pos)                 | Element    | 返回pos位置的元素, 若无元素则返回None            |
| add_element(element)         | int        | 添加元素并设置索引, 若位置无效或被占用则返回None |
| save(filename)               | None       | 保存地图信息至文件                               |
| saves()                      | str        | 保存地图至字符串                                 |
| saves_elements()             | str        | 保存单位状态至字符串                             |
| loads_elements(str)          | None       | 从字符串读取elements信息, 并更新当前地图         |
| path_finding(origin, dest)   | [Position] | 返回包含起点和终点在内的路径结点列表             |



## Module command
定义玩家/AI所下达的指令

类继承关系

    object
        Command
            AttackPos
            AttackUnit
            ChangeDest
            Fix
            Produce
            Supply
            Cancel

### Class Command
|     构造函数     |              描述             |
| ---------------- | ----------------------------- |
| Command(operand) | 指令, operand为操作对象索引号 |

### Class AttackPos
|         构造函数        |   描述   |
| ----------------------- | -------- |
| AttackPos(operand, pos) | 攻击坐标 |

### Class AttackUnit
|           构造函数          |   描述   |
| --------------------------- | -------- |
| AttackUnit(operand, target) | 攻击单位 |


### Class ChangeDest
|          构造函数         |    描述    |
| ------------------------- | ---------- |
| ChangeDest(operand, dest) | 更改目的地 |

### Class Fix
|       构造函数       | 描述 |
| -------------------- | ---- |
| Fix(operand, target) | 维修 |

### Class Produce
|    构造函数   | 描述 |
| ------------- | ---- |
| Produce(kind) | 生产 |

### Class Supply
|                                构造函数                               |          描述          |
| --------------------------------------------------------------------- | ---------------------- |
| Supply(operand, target, fuel=INFINITY, ammo=INFINITY, metal=INFINITY) | 补给, 量为负则尽力补给 |



## Module event
定义游戏中发生的事件

类继承关系

    object
        Event
            AddProductionEntry
            AttackPos
            AttackUnit
            Supply
            Fix
            Collect
            ChangeDest
            Move
            Create
            Destroy
            Capture

### Class Event
|      方法     | 返回值 |           描述          |
|---------------|--------|-------------------------|
| description() | str    | 返回描述该Event的字符串 |

### Class AddProductionEntry
| 属性 | 类型 |        描述        |
|------|------|--------------------|
| team | int  | 添加生产条目的队伍 |
| kind | int  | 要生产单位的类型   |

### Class AttackPos
|  属性  |   类型   |     描述     |
|--------|----------|--------------|
| index  | int      | 攻击者索引号 |
| target | Position | 攻击目标     |
| damage | int      | 造成的伤害   |

### Class AttackUnit
|  属性  | 类型 |      描述      |
|--------|------|----------------|
| index  | int  | 攻击者索引号   |
| target | int  | 被攻击者索引号 |
| damage | int  | 造成的伤害     |

### Class Supply
|  属性  | 类型 |        描述        |
| ------ | ---- | ------------------ |
| index  | int  | 补给发起方的索引号 |
| target | int  | 补给接受方的索引号 |
| fuel   | int  | 补给的燃料量       |
| ammo   | int  | 补给的弹药量       |
| metal  | int  | 补给的金属量       |

### Class Fix
|       属性      |   类型   |        描述        |
|-----------------|----------|--------------------|
| index           | int      | 维修发起方的索引号 |
| target          | int      | 维修接受方的索引号 |
| metal           | int      | 维修消耗的金属     |
| health_increase | int      | 生命值增量         |

### Class Collect
|  属性  | 类型 |      描述      |
|--------|------|----------------|
| index  | int  | 收集方索引号   |
| target | int  | 资源索引号     |
| fuel   | int  | 收集到的燃料量 |
| metal  | int  | 收集到的金属量 |

### Class ChangeDest
|  属性 | 类型 |    描述    |
|-------|------|------------|
| index | int  | 单位索引号 |
| dest  | int  | 新目的地   |

### Class Move
|  属性 |    类型    |       描述       |
| ----- | ---------- | ---------------- |
| index | int        | 单位索引号       |
| nodes | [Position] | 移动路径上的拐点 |
| steps | int        | 总步数           |

### Class Create
|  属性 | 类型 |    描述    |
|-------|------|------------|
| index | int  | 单位索引号 |

### Class Destroy
|  属性 | 类型 |       描述       |
|-------|------|------------------|
| index | int  | 被毁灭单位索引号 |

### Class Capture
|  属性 | 类型 |        描述        |
|-------|------|--------------------|
| index | int  | 被占领堡垒的索引号 |
| team  | int  | 队伍               |

## Module gamebody
提供游戏主体

|      属性      | 类型 |           描述           |
| -------------- | ---- | ------------------------ |
| STATE_CONTINUE | int  | 值为-1，表示游戏应当继续 |
| STATE_TIE      | int  | 值为2，表示平局          |

|           方法           |  返回值  |         描述         |
| ------------------------ | -------- | -------------------- |
| load_game(filename)      | GameBody | 从文件读取游戏主体   |
| loads_game(game_str)     | GameBody | 从字符串读取游戏主体 |

类继承关系

    object
        GameBody

### Class GameBody
游戏主体

|            构造函数            |     描述     |
| ------------------------------ | ------------ |
| GameBody(map_info, team_names) | 构造游戏主体 |

|       属性       |          类型          |                           描述                          |
| ---------------- | ---------------------- | ------------------------------------------------------- |
| **attr**         |                        |                                                         |
| map_info         | MapInfo                | 地图信息                                                |
| round            | int                    | 当前回合数, 从0开始                                     |
| scores           | [int]                  | 两队积分                                                |
| populations      | [int, int]             | 两队当前人口                                            |
| production_lists | [[[kind, round_left]]] | 两队生产列表                                            |
| commands         | [[Command]]            | 两队当前指令                                            |
| **property**     |                        |                                                         |
| map              | [[int]]                | 地形图                                                  |
| max_population   | int                    | 人口上限                                                |
| max_round        | int                    | 回合上限                                                |
| record_interval  | int                    | 统计数据记录间隔                                        |
| time_per_round   | float                  | 每回合时间                                              |
| weather          | int                    | 天气                                                    |
| status           | int                    | 当前游戏状态, 0或1表示该队伍胜利, -1仍未分出胜负, 2平局 |

|            方法            |        返回值        |                            描述                           |
| -------------------------- | -------------------- | --------------------------------------------------------- |
| **游戏状态**               |                      |                                                           |
| score(team)                | int                  | 分数                                                      |
| elements(team)             | {int:Element}        | 某队所有元素                                              |
| vision(team)               | [set(Position)]      | 某队共享三层视野                                          |
| view_elements(perspective) | {int:Element}        | 某队视角下的所有元素                                      |
| production_list(team)      | [[kind, round_left]] | 生产列表                                                  |
| population(team)           | int                  | 该队伍的人口数                                            |
| **运行相关**               |                      |                                                           |
| set_command(team, command) | bool                 | 为该队伍添加一条指令，并更新指令列表, 指令无效则返回False |
| run()                      | [Event]              | 运行一回合，返回该回合内发生的事件                        |
| **设置/保存相关**          |                      |                                                           |
| save(filename)             | None                 | 保存当前游戏状态至文件                                    |
| saves()                    | str                  | 保存当前游戏状态至字符串                                  |
