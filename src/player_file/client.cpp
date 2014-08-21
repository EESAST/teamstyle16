#include "basic.h"

struct Command
{
    int element_index;  // 操作对象索引号
    int order;

    Position target;
    int value;          // 各项指令的value
};

