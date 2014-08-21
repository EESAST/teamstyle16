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

| 属性 |   类型   |     描述     |
|------|----------|--------------|
| kind | int      | 元素类型     |
| pos  | Position | 该元素的位置 |

### Class Resource
|  属性  | 类型 |   描述   |
|--------|------|----------|
| remain | int  | 剩余资源 |

### Class UnitBase
|   属性  |  类型  |    描述    |
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
| attacks     | int[2] | 攻击       |
| defences    | int[2] | 防御       |

### Class Building
| 属性 |         类型         |     描述     |
|------|----------------------|--------------|
| size | (x_length, y_length) | 建筑物的大小 |

### Class Unit
|  属性 | 类型 |   描述   |
|-------|------|----------|
| speed | int  | 移动速度 |

### Class Formation
|    属性    |  类型  |     描述    |
|------------|--------|-------------|
| plane_nums | int[4] | 4机种的数目 |


## Module map
定义用于设置初始状态的地图，及相关的函数
类继承关系

    object
        Map

|      方法      | 返回值 |                描述               |
|----------------|--------|-----------------------------------|
| load(filename) | Map    | 从文件中载入地图，存放在Map对象中 |

### Class Map

|      方法      |      返回值       |           描述           |
|----------------|-------------------|--------------------------|
| row()          | int               | 返回地图行数             |
| col()          | int               | 返回地图列数             |
| map_type(x, y) | int               | 返回(x, y)处的地形       |
| elements()     | [(int, Position)] | 返回所有元素的类型和位置 |



## Module gamebody

|       属性      | 类型 |           描述           |
|-----------------|------|--------------------------|
| STATE_CONTINUE  | int  | The game should continue |
| STATE_TEAM0_WON | int  | Team 0 won               |
| STATE_TEAM1_WON | int  | Team 1 won               |
| STATE_TIE       | int  | Tie                      |



## Class gamebody.State
Describe the state of a certain element, including building, resource and unit.
Any invalid data should be set to an invalid state (e.g. -1)

|      属性     |   类型   |              描述              |
|---------------|----------|--------------------------------|
| element_index | int      | Index of the element           |
|               |          |                                |
| pos           | Position | Position of the element        |
| type          | int      |                                |
| team          | int      |                                |
| visible       | bool     | Whether the element is visible |
|               |          |                                |
| health        | int      |                                |
| fuel          | int      |                                |
| ammo          | int      |                                |
| metal         | int      |                                |
|               |          |                                |
| destination   | Position |                                |



## Class gamebody.TeamGameInfo
Provide game information for a certain team

|   属性   |   类型  |                         描述                        |
|----------|---------|-----------------------------------------------------|
| elements | [State] | Everything in the map that can be seen by this team |



## Class gamebody.ProductionEntry

|    属性    | 类型 |                描述               |
|------------|------|-----------------------------------|
| unit_type  | int  | type of the unit to be produced   |
| round_left | int  | rounds needed to produce the unit |



## Class gamebody.GameBody

|                 方法                |                 返回值                 |                   描述                   |
|-------------------------------------|----------------------------------------|------------------------------------------|
| team_name()                         | (team0_name, team1_name)               | Return team names                        |
| score()                             | (team0_score, team1_score)             | Return current score                     |
| round()                             | int                                    | Return current round                     |
|                                     |                                        |                                          |
| elements()                          |                                        |                                          |
| production_list()                   | ([ProductionEntry], [ProductionEntry]) | Return production lists of the two teams |
|                                     |                                        |                                          |
| team_game_info()                    | (team0_game_info, team1_game_info)     |                                          |
| run(team0_commands, team1_commands) | (state, max_round_reached)             |                                          |





## Class gamebody.Command

|      属性     |   类型   |          描述          |
|---------------|----------|------------------------|
| element_index | int      |                        |
| order         | int      | Type of the order      |
| target        | Position | Position of the target |
| type          | int      | Used for PRODUCE       |
