#include <json/json.h>

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
};


namespace lua_json_params{

/*
关于lua栈的大小要求：
每次push/pop一个table时，都是逐个元素进行的，并不会因为table过大而导致大量堆栈占用
但是每个table层级，都会占一个堆栈位置。例如data["key1"]["key2"]["key3"]["key4"]在push/pop时会占用4个对战位置
lua默认栈大小是20个。如果需要push/pop大量参数，同时有非常高层级的table，需要调用者手动扩大lua栈大小（请调用函数lua_checkstack）
*/
void push_lua_param(lua_State *L, Json::Value &data)
{
    int type = data.type();
    if(type == Json::nullValue)
    {
        lua_pushnil(L);
        return;
    }
    else if(type == Json::intValue)
    {
        lua_pushinteger(L, data.asInt());
        return;
    }
    else if(type == Json::uintValue)
    {
        lua_pushinteger(L, data.asUInt());
        return;
    }
    else if(type == Json::realValue)
    {
        lua_pushinteger(L, data.asDouble());
        return;
    }
    else if(type == Json::booleanValue)
    {
        lua_pushboolean(L, data.asBool());
        return;
    }
    else if(type == Json::stringValue)
    {
        lua_pushstring(L, data.asCString());
        return;
    }
    else if(type == Json::arrayValue)
    {
        unsigned int size = data.size();
        lua_createtable(L, 0, size);
        for(unsigned int i = 0; i < size; i++)   
        {
            lua_pushinteger(L, i);
            push_lua_param(L, data[i]);
            lua_settable(L, -3);
        }
    }
    else if(type == Json::objectValue)
    {
        std::vector<std::string> mem = data.getMemberNames();
        unsigned int size = mem.size();
        lua_createtable(L, size, 0);
        for(unsigned int i=0; i<size; i++)   
        {
            push_lua_param(L, data[mem[i]]);
            lua_setfield(L, -2, mem[i].c_str());
        }
    }
}

/*
返回值：
0正常；lua无参数返回时也认为正常
-1 lua返回了我们不支持的参数，例如LUA_TFUNCTION
-2 lua返回的table中的key有我们不支持的参数。例如数组和hash混在一起，或者采用浮点数作为key
*/
int pop_lua_param(lua_State *L, Json::Value &data)
{
    int type = lua_type(L, -1);
    if(type == LUA_TNONE)
        return 0;
    else if(type == LUA_TNIL)
    {
        lua_pop(L, 1);
        return 0;
    }
    else if(type == LUA_TNUMBER)
    {
        data = lua_tonumber(L, -1);
        lua_pop(L, 1);
        return 0;
    }
    else if(type == LUA_TBOOLEAN)
    {
        data = lua_toboolean(L, -1);
        lua_pop(L, 1);
        return 0;
    }
    else if(type == LUA_TSTRING)
    {
        data = lua_tostring(L, -1);
        lua_pop(L, 1);
        return 0;
    }
    else if(type == LUA_TTABLE)
    {
        lua_pushnil(L);
        int key_type = LUA_TNONE;
        int ret = 0;
        while(lua_next(L, -2) != 0)
        {
            int next_key_type = lua_type(L, -2);
            if(key_type == LUA_TNONE)
                key_type = next_key_type;
            else if(key_type != next_key_type)
            {
                //恢复堆栈，弹出key-value和table本身
                lua_pop(L, 3);
                return -2;
            }
            
            if(key_type == LUA_TNUMBER)
            {
                float src_key = lua_tonumber(L, -2);
                int int_key = (int)src_key;
                if(int_key != src_key)
                {
                    //恢复堆栈，弹出key-value和table本身
                    lua_pop(L, 3);
                    return -2;
                }
                ret = pop_lua_param(L, data[int_key]);
            }
            else if(key_type == LUA_TSTRING)
                ret = pop_lua_param(L, data[lua_tostring(L, -2)]);
            else
            {
                //恢复堆栈，弹出key-value和table本身
                lua_pop(L, 3);
                return -2;
            }
            
            if(ret < 0)
            {
                //恢复堆栈，value已经在递归中弹出，仅需弹出key和table本身
                lua_pop(L, 2);
                return ret;
            }
        }
        lua_pop(L, 1);
        return 0;
    }
    else if(type ==  LUA_TFUNCTION)
    {
        return -1;
    }
    else if(type == LUA_TUSERDATA)
    {
        return -1;
    }
    else if(type == LUA_TTHREAD)
    {
        return -1;
    }
    else if(type == LUA_TLIGHTUSERDATA)
    {
        return -1;
    }
}

}

