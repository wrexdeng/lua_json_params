#ifndef LUA_JSON_PARAMS_H
#define LUA_JSON_PARAMS_H

#include <json/json.h>

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
};


namespace lua_json_params{
/*
关于lua栈大小的要求：
    每次push/pop一个table时，都是逐个元素进行的，并不会因为table过大而导致大量堆栈占用
    但是每个table层级，都会占一个堆栈位置。例如data["key1"]["key2"]["key3"]["key4"]在push/pop时会占用4个对战位置
    lua默认栈大小是20个。如果需要push/pop大量参数，同时有非常高层级的table，需要调用者手动扩大lua栈大小（请调用函数lua_checkstack）
*/


/*
功能：
    深度遍历data，将其内容（包含各级子数组、子对象）全部压入lua栈中
*/
void push_lua_param(lua_State *L, Json::Value &data);


/*
功能：
    深度遍历lu栈顶的对象，将其内容（包含各级子table）全部弹出到data中

返回值：
    0正常；lua栈顶为nil时也认为正常
    -1 lua返回了我们不支持的参数，例如LUA_TFUNCTION
    -2 lua返回的table中的key有我们不支持的参数。例如数组和hash混在一起，或者采用浮点数作为key
*/
int pop_lua_param(lua_State *L, Json::Value &data);

}

#endif