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
                        Battleship
                        Cargo
                        Carrier
                        Cruiser
                        Destroyer
                    Formation

TODO：游戏常量

|            方法           | 返回值 |       描述       |
|---------------------------|--------|------------------|
| kind_to_formation(kind)   | int[4] | 类型值->机群组成 |
| formation_to_kind(int[4]) | int    | 机群组成->类型值 |

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

例如，可能Element类和对象里没有kind和size属性，而Unit却有类属性size（Unit都是1x1），Submarine有类属性kind（潜艇的类型是常量），Formation有实例属性kind（Formation的类型不确定）

|   属性  |         类型         |      描述      |
|---------|----------------------|----------------|
| index   | int                  | 元素的索引号   |
| kind    | int                  | 元素类型       |
| pos     | Position             | 该元素的位置   |
| size    | (x_length, y_length) | 该元素的大小   |
| visible | bool                 | 该元素是否可见 |

### Class Resource
|  属性  | 类型 |   描述   |
|--------|------|----------|
| remain | int  | 剩余资源 |

### Class UnitBase
|     属性    |  类型  |       描述       |
|-------------|--------|------------------|
| team        | int    | 所属队伍         |
| sight_range | int[3] | 视野范围         |
| fire_range  | int[3] | 攻击范围         |
| health      | int    | 生命值           |
| health_max  | int    | 生命值上限       |
| fuel        | int    | 燃料             |
| fuel_max    | int    | 燃料上限         |
| ammo        | int    | 弹药             |
| ammo_once   | int    | 单次攻击消耗弹药 |
| ammo_max    | int    | 弹药上限         |
| metal       | int    | 金属             |
| metal_max   | int    | 金属上限         |
| attacks     | int[2] | 攻击             |
| defences    | int[2] | 防御             |

### Class Unit
|    属性    |   类型   |     描述     |
|------------|----------|--------------|
| speed      | int      | 移动速度     |
| dest       | Position | 当前目的地   |
| cost       | int      | 生产所需金属 |
| population | int      | 占有人口数   |

### Class Formation
|    属性    |  类型  |     描述    |
|------------|--------|-------------|
| plane_nums | int[4] | 4机种的数目 |


## Module command
定义玩家/AI所下达的指令

类继承关系

    object
        Command
            Attack
            ChangeDest
            Collect
            Fix
            Produce
            Supply
            Cancel

### Class Command
|     构造函数     | 描述 |
|------------------|------|
| Command(operand) | 指令 |

|   属性  | 类型 |      描述      |
|---------|------|----------------|
| operand | int  | 操作对象索引号 |

### Class Attack/Collect/Fix
|       构造函数       |      描述      |
|----------------------|----------------|
| XXX(operand, target) | 攻击/收集/维修 |

|  属性  | 类型 |       描述       |
|--------|------|------------------|
| target | int  | 指令目标的索引号 |

### Class ChangeDest
|          构造函数         |    描述    |
|---------------------------|------------|
| ChangeDest(operand, dest) | 更改目的地 |

| 属性 |   类型   |      描述      |
|------|----------|----------------|
| dest | Position | 新目的地的坐标 |

### Class Produce
|        构造函数        | 描述 |
|------------------------|------|
| Produce(operand, kind) | 生产 |

| 属性 | 类型 |        描述        |
|------|------|--------------------|
| kind | int  | 要生产的对象的类型 |

### Class Supply

|                       构造函数                      | 描述 |
|-----------------------------------------------------|------|
| Supply(operand, target, fuel=-1, metal=-1, ammo=-1) | 补给 |

|  属性  | 类型 |              描述              |
|--------|------|--------------------------------|
| target | int  | 补给对象的索引号               |
| fuel   | int  | 要补给的燃料量，为负则尽力补给 |
| metal  | int  | 要补给的金属量，为负则尽力补给 |
| ammo   | int  | 要补给的弹药量，为负则尽力补给 |


## Module event
定义游戏中发生的事件

类继承关系

    object
        Event
            AddProductionEntry
            Attack
            Crash
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

### Class Attack
|  属性  | 类型 |      描述      |
|--------|------|----------------|
| index  | int  | 攻击者索引号   |
| target | int  | 被攻击者索引号 |
| hit    | bool | 是否命中       |
| damage | int  | 造成的伤害     |

### Class Crash
|  属性  | 类型 |                  描述                  |
|--------|------|----------------------------------------|
| index  | int  | 坠毁单位索引号                         |
| target | int  | 受到坠机伤害的单位索引（没有时为None） |
| damage | int  | 受到的坠机伤害                         |

### Class Supply
|  属性 | 类型 |        描述        |
|-------|------|--------------------|
| index | int  | 补给发起方的索引号 |
| target | int  | 补给接受方的索引号 |
| fuel  | int  | 补给的燃料量       |
| metal | int  | 补给的金属量       |
| ammo  | int  | 补给的弹药量       |

### Class Fix
|       属性      | 类型 |        描述        |
|-----------------|------|--------------------|
| index           | int  | 维修发起方的索引号 |
| target          | int  | 维修接受方的索引号 |
| metal           | int  | 维修消耗的金属     |
| health_increase | int  | 生命值增量         |

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
|  属性 |   类型   |    描述    |
|-------|----------|------------|
| index | int      | 单位索引号 |
| from  | Position | 移动前位置 |
| to    | Position | 移动后位置 |

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

类继承关系

    object
        GameBody

|      属性      | 类型 |             描述             |
|----------------|------|------------------------------|
| GOD            | int  | 值不为0或1，表示上帝视角     |
| STATE_CONTINUE | int  | 值不为0或1，表示游戏应当继续 |
| STATE_TIE      | int  | 值不为0或1，表示平局         |

|      方法      |  返回值  |        描述        |
|----------------|----------|--------------------|
| load(filename) | GameBody | 载入游戏           |
| loads(s)       | GameBody | 从字符串中载入游戏 |


### Class GameBody
游戏主体

|                     构造函数                     |     描述     |
|--------------------------------------------------|--------------|
| GameBody(filename, max_polulation=..., max_round=...) | 构造游戏主体 |

|            方法            |        返回值        |                描述                |
|----------------------------|----------------------|------------------------------------|
| map()                      | Map                  | 返回当前地图                       |
| max_polulation()           | int                  | 返回最大人口数                     |
| team_name(team)            | str                  | 返回队伍名                         |
| **游戏状态**               |                      |                                    |
| round()                    | int                  | 返回当前回合数                     |
| score(team)                | int                  | 返回分数                           |
| elements(perspective=GOD)  | [Element]            | 返回上帝/某队视角下的所有元素      |
| production_list(team)      | [(kind, round_left)] | 返回生产列表                       |
| population(team)           | int                  | 返回该队伍的人口数                 |
| weather()                  | int                  | 返回当前的天气                     |
| **运行相关**               |                      |                                    |
| commands(team)             | [Command]            | 返回该队伍当前的指令               |
| set_command(team, command) | None                 | 为该队伍添加一条指令，并处理冲突   |
| run()                      | [Event]              | 运行一回合，返回该回合内发生的事件 |
| status()                   | int                  | 返回当前的游戏状态                 |
| **设置/保存相关**          |                      |                                    |
| set_team_name(team, name)  | None                 | 设置队伍名                         |
| save(filename)             | None                 | 保存游戏（包括地图）               |
| saves()                    | str                  | 保存游戏（包括地图）至字符串       |
