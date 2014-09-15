#include "basic.h"

enum OrderType
{
    ATTACK,   // 攻击索引号为value的单位
    CHANGE_DEST,  // 更改目的地为target
    COLLECT,  // 收集索引号为value的单位
    EXPLODE,  // 自爆
    FIX,      // 维修索引号为value的单位
    PRODUCE,   // 生产value类型的单位
    SUPPLY   // 补给索引号为value的单位
};
