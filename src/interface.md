# API Reference

## Module basic

## Class basic.Position

|    Constructors   |         Description         |
| ----------------- | --------------------------- |
| Position(x, y, z) | Create a position (x, y, z) |

| Properties | Type | Description  |
| ---------- | ---- | ------------ |
| x          | int  | x coordinate |
| y          | int  | y coordinate |
| z          | int  | z coordinate |



## Module map

|    Methods     | Return Value |    Description     |
| -------------- | ------------ | ------------------ |
| load(filename) | Map          | Load map from file |



## Class map.Map

|    Methods     |    Return Value   |                Description                |
| -------------- | ----------------- | ----------------------------------------- |
| row()          | int               | Return row number of the map              |
| col()          | int               | Return column number of the map           |
| map_type(x, y) | int               | Return map type on (x, y)                 |
| elements()     | [(int, Position)] | Return type and position of every element |



## Module gamebody

|    Properties   | Type |       Description        |
| --------------- | ---- | ------------------------ |
| STATE_CONTINUE  | int  | The game should continue |
| STATE_TEAM0_WON | int  | Team 0 won               |
| STATE_TEAM1_WON | int  | Team 1 won               |
| STATE_TIE       | int  | Tie                      |



## Class gamebody.State
Describe the state of a certain element, including building, resource and unit.
Any invalid data should be set to an invalid state (e.g. -1)

|   Properties  |   Type   |          Description           |
| ------------- | -------- | ------------------------------ |
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

| Properties |   Type  |                     Description                     |
| ---------- | ------- | --------------------------------------------------- |
| elements   | [State] | Everything in the map that can be seen by this team |



## Class gamebody.ProductionEntry

| Properties | Type |            Description            |
| ---------- | ---- | --------------------------------- |
| unit_type  | int  | type of the unit to be produced   |
| round_left | int  | rounds needed to produce the unit |



## Class gamebody.GameBody

|               Methods               |            Return Value            |               Description                |
| ----------------------------------- | ---------------------------------- | ---------------------------------------- |
| team_name()                         | (team0_name, team1_name)           | Return team names                        |
| score()                             | (team0_score, team1_score)         | Return current score                     |
| round()                             | int                                | Return current round                     |
|                                     |                                    |                                          |
| buildings()                         |                                    |                                          |
| resources()                         |                                    |                                          |
| units()                             |                                    |                                          |
| production_list()                   | (ProductionEntry, ProductionEntry) | Return production lists of the two teams |
|                                     |                                    |                                          |
| team_game_info()                    | (team0_game_info, team1_game_info) |                                          |
| run(team0_commands, team1_commands) | (state, max_round_reached)         |                                          |



## Class gamebody.Command

|   Properties  |   Type   |      Description       |
| ------------- | -------- | ---------------------- |
| element_index | int      |                        |
| order         | int      | Type of the order      |
| target        | Position | Position of the target |
| type          | int      | Used for PRODUCE       |
