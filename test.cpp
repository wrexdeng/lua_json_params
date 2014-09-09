#include "stdio.h"
#include <json/json.h>

extern "C"{
#include "lua.h"
#include "lauxlib.h"
#include "lualib.h"
};

#include "lua_json_params.h"

/*
测试lua_json_params api功能
在c++和lua中导入导出多级table

g++ test.cpp lua_json_params.cpp -ljsoncpp -llua -ldl
*/

using namespace lua_json_params;

int test_c_api(lua_State *L)
{
    Json::Value input;
    int ret = pop_lua_param(L, input);
    if(ret != 0)
        return ret;
    
    printf("c-printf   name is %s\n", input["basic"]["user"].asCString());
    
    push_lua_param(L, input);    
    return 1;
}

int get_user_data(Json::Value &user_data)
{
    user_data["basic"]["user"] = "邓小超";
    user_data["basic"]["level"] = 10;
    user_data["basic"]["gold"] = 3790;
    user_data["basic"]["hp"] = 100;
    user_data["basic"]["attack"] = 30;

    user_data["process"][0] = "铁剑";
    user_data["process"][1] = 49.24;
    user_data["process"][2] = 35;
    user_data["process"][3] = "优良";
    user_data["process"][4] = "非绑定";
    
    return 0;
}

int main()
{
    Json::Value user_data1;
    Json::Value user_data2;
    get_user_data(user_data1);
    get_user_data(user_data2);

    lua_State *L;
    L = luaL_newstate();
    luaL_openlibs(L);
    luaL_dofile(L, "./my.lua");
    
    lua_register(L, "test_c_api", test_c_api);

    lua_getglobal(L, "wrex_lua");
    
    push_lua_param(L, user_data1);
    push_lua_param(L, user_data2);
    
    int ret = lua_pcall(L, 2, 2, 0);
    if(ret != 0)
    {
        printf("call lua error %d\n", ret);
        return -1;
    }
    
    ret = pop_lua_param(L, user_data1);
    if(ret != 0)
    {
        printf("pop_lua_param error %d\n", ret);
        return -1;
    }
    ret = pop_lua_param(L, user_data2);
    if(ret != 0)
    {
        printf("pop_lua_param error %d\n", ret);
        return -1;
    }
    
    printf("c-printf   %d\n", user_data1["basic"]["hp"].asInt());
    
    lua_close(L);
    return 0;
}

