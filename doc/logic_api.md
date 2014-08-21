# API Reference

## Module basic
定义游戏相关的常量和类

类继承关系

    object
        Position
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

|   属性  |   类型   |      描述      |
|---------|----------|----------------|
| index   | int      | 元素的索引号   |
| kind    | int      | 元素类型       |
| pos     | Position | 该元素的位置   |
| visible | bool     | 该元素是否可见 |

### Class Resource
|  属性  | 类型 |   描述   |
|--------|------|----------|
| remain | int  | 剩余资源 |

### Class UnitBase
|     属性    |  类型  |    描述    |
|-------------|--------|------------|
| team        | int    | 所属队伍   |
| sight_range | int[3] | 视野范围   |
| fire_range  | int[3] | 攻击范围   |
| health      | int    | 生命值     |
| health_max  | int    | 生命值上限 |
| fuel        | int    | 燃料       |
| fuel_max    | int    | 燃料上限   |
| ammo        | int    | 弹药       |
| ammo_max    | int    | 弹药上限   |
| metal       | int    | 金属       |
| metal_max   | int    | 金属上限   |
| attacks     | int[2] | 攻击       |
| defences    | int[2] | 防御       |

### Class Building
| 属性 |         类型         |     描述     |
|------|----------------------|--------------|
| size | (x_length, y_length) | 建筑物的大小 |

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


## Module map
定义地图及相关的函数

类继承关系

    object
        Map

|         方法        | 返回值 |                 描述                |
|---------------------|--------|-------------------------------------|
| save(filename, Map) | None   | 保存地图至文件                      |
| saves(Map)          | str    | 保存地图至字符串                    |
| load(filename)      | Map    | 从文件中载入地图，存放在Map对象中   |
| loads(s)            | Map    | 从字符串中载入地图，存放在Map对象中 |

### Class Map

|      方法      |      返回值       |           描述           |
|----------------|-------------------|--------------------------|
| row()          | int               | 返回地图行数             |
| col()          | int               | 返回地图列数             |
| map_type(x, y) | int               | 返回(x, y)处的地形       |
| elements()     | [(int, Position)] | 返回所有元素的类型和位置 |


## Module command
定义玩家/AI所下达的指令

类继承关系

    object
        Command
            Attack
            ChangeDest
            Collect
            Explode
            Fix
            Produce
            Supply

### Class Command
|        构造函数        | 描述 |
|------------------------|------|
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
|               构造函数               | 描述 |
|--------------------------------------|------|
| Supply(operand, target, metal, ammo) | 补给 |

|  属性  | 类型 |              描述              |
|--------|------|--------------------------------|
| target | int  | 补给对象的索引号               |
| metal  | int  | 要补给的金属量，为负则尽力补给 |
| ammo   | int  | 要补给的弹药量，为负则尽力补给 |

## Module event
定义

类继承关系

    object
        Event

TODO


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

### Class GameBody
游戏主体

只有在设置map后（即map()返回值不为None）才能运行

|               构造函数              |     描述     |
|-------------------------------------|--------------|
| GameBody(team_names=None, map=None) | 构造游戏主体 |


|            方法           |        返回值        |                               描述                               |
|---------------------------|----------------------|------------------------------------------------------------------|
| elements(perspective=GOD) | [Element]            | 返回上帝/某队视角下的所有元素                                    |
| map()                     | Map                  | 返回当前地图                                                                 |
| production_list(team)     | [(kind, round_left)] | 返回生产列表                                                     |
| round()                   | int                  | 返回当前回合数                                                   |
| run(commands0, commands1) | (state, [Event])     | 接收两队发出的指令，运行一回合，返回游戏状态及该回合内发生的事件 |
| score(team)               | int                  | 返回分数                                                         |
| team_name(team)           | str                  | 返回队伍名                                                       |
|                           |                      |                                                                  |
| load_map(map)             | None                 | 加载地图。若成功，则清空当前状态                                 |
| set_team_name(team, name) | None                 | 设置队伍名                                                       |
| save(filename)            | None                 | 保存当前游戏状态（包括地图）                                     |
| saves()                   | str                  | 保存当前游戏状态（包括地图）至字符串                             |
| load(filename)            | None                 | 载入游戏状态（包括地图）                                         |
| loads(s)                  | None                 | 从字符串中载入游戏状态（包括地图）                               |
