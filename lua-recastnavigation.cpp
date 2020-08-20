#define LUA_LIB

#ifdef __cplusplus
extern "C"
{
#endif
#include <lua.h>
#include <lauxlib.h>

    LUAMOD_API int luaopen_recastnavigation(lua_State *L);

#ifdef __cplusplus
}
#endif

#include "recastnavigation.h"

static void *
check_userdata(lua_State *L, int idx)
{
    void *ret = lua_touserdata(L, idx);
    luaL_argcheck(L, ret != NULL, idx, "Userdata should not be NULL");
    return ret;
}

static void
create_meta(lua_State *L, luaL_Reg *l, const char *name, lua_CFunction tostring, lua_CFunction gcfunc)
{
    int n = 0;
    while (l[n].name)
        ++n;
    lua_newtable(L);
    lua_createtable(L, 0, n);
    int i;
    for (i = 0; i < n; i++)
    {
        lua_pushcfunction(L, l[i].func);
        lua_setfield(L, -2, l[i].name);
    }
    lua_setfield(L, -2, "__index");
    lua_pushstring(L, name);
    lua_setfield(L, -2, "__metatable");
    if (tostring)
    {
        lua_pushcfunction(L, tostring);
        lua_setfield(L, -2, "__tostring");
    }

    if (gcfunc)
    {
        lua_pushcfunction(L, gcfunc);
        lua_setfield(L, -2, "__gc");
    }
}

struct s_navigation
{
    int64_t scene;
    RecastNavigationHandle *handle;
};

static int
lnew(lua_State *L)
{
    int64_t scene = luaL_checknumber(L, 1);

    size_t l;
    const char *respath = luaL_checklstring(L, 2, &l);

    struct s_navigation *nav = (struct s_navigation *)lua_newuserdata(L, sizeof(struct s_navigation));
    nav->scene = scene;
    nav->handle = NULL;

    nav->handle = RecastNavigationHandle::Create(respath);
    if (!nav->handle)
    {
        lua_pushnil(L);
        return 1;
    }

    lua_pushvalue(L, lua_upvalueindex(1));
    lua_setmetatable(L, -2);
    return 1;
}

static int
lrelease(lua_State *L)
{
    struct s_navigation *nav = (struct s_navigation *)check_userdata(L, 1);
    printf("recastnavigation release [%lld]\n", nav->scene);

    if (nav->handle)
    {
        delete nav->handle;
        nav->handle = NULL;
    }
    else
    {
        printf("recastnavigation release handle is null");
    }
    return 0;
}

static int
lFindStraightPath(lua_State *L)
{
    struct s_navigation *nav = (struct s_navigation *)check_userdata(L, 1);
    float start_x = luaL_checknumber(L, 2);
    float start_y = luaL_checknumber(L, 3);
    float start_z = luaL_checknumber(L, 4);

    float end_x = luaL_checknumber(L, 5);
    float end_y = luaL_checknumber(L, 6);
    float end_z = luaL_checknumber(L, 7);

    NFVector3 start(start_x, start_y, start_z);
    NFVector3 end(end_x, end_y, end_z);

    std::vector<NFVector3> paths;
    int pos = nav->handle->FindStraightPath(start, end, paths);
    if (pos <= 0)
    {
        lua_pushboolean(L, false);
        return 1;
    }
    lua_pushboolean(L, true);
    lua_newtable(L);
    for (size_t i = 0; i < paths.size(); i++)
    {
        lua_createtable(L, 0, 3);
        lua_pushinteger(L, paths[i].X());
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, paths[i].Y());
        lua_rawseti(L, -2, 2);
        lua_pushinteger(L, paths[i].Z());
        lua_rawseti(L, -2, 3);

        lua_rawseti(L, -2, i + 1);
    }
    return 2;
}

static int
lFindRandomPointAroundCircle(lua_State *L)
{
    struct s_navigation *nav = (struct s_navigation *)check_userdata(L, 1);
    float center_x = luaL_checknumber(L, 2);
    float center_y = luaL_checknumber(L, 3);
    float center_z = luaL_checknumber(L, 4);

    int max_points = luaL_checknumber(L, 5);
    float maxRadius = luaL_checknumber(L, 6);

    NFVector3 center(center_x, center_y, center_z);
    std::vector<NFVector3> paths;

    int size = nav->handle->FindRandomPointAroundCircle(center, paths, max_points, maxRadius);
    if (size <= 0)
    {
        lua_pushboolean(L, false);
        return 1;
    }

    lua_pushboolean(L, true);

    lua_newtable(L);
    for (size_t i = 0; i < paths.size(); i++)
    {
        lua_newtable(L);
        lua_createtable(L, 0, 3);
        lua_pushinteger(L, paths[i].X());
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, paths[i].Y());
        lua_rawseti(L, -2, 2);
        lua_pushinteger(L, paths[i].Z());
        lua_rawseti(L, -2, 3);

        lua_rawseti(L, -2, i + 1);
    }

    return 2;
}

static int
lRaycast(lua_State *L)
{
    struct s_navigation *nav = (struct s_navigation *)check_userdata(L, 1);
    float start_x = luaL_checknumber(L, 2);
    float start_y = luaL_checknumber(L, 3);
    float start_z = luaL_checknumber(L, 4);

    float end_x = luaL_checknumber(L, 5);
    float end_y = luaL_checknumber(L, 6);
    float end_z = luaL_checknumber(L, 7);

    NFVector3 start(start_x, start_y, start_z);
    NFVector3 end(end_x, end_y, end_z);

    std::vector<NFVector3> hitPointVec;
    int res = nav->handle->Raycast(start, end, hitPointVec);
    lua_pushinteger(L, res);
    lua_newtable(L);
    for (size_t i = 0; i < hitPointVec.size(); i++)
    {
        lua_createtable(L, 0, 3);
        lua_pushinteger(L, hitPointVec[i].X());
        lua_rawseti(L, -2, 1);
        lua_pushinteger(L, hitPointVec[i].Y());
        lua_rawseti(L, -2, 2);
        lua_pushinteger(L, hitPointVec[i].Z());
        lua_rawseti(L, -2, 3);

        lua_rawseti(L, -2, i + 1);
    }
    return 2;
}

static void
lnavmesh(lua_State *L)
{
    luaL_Reg l[] = {
        {"FindStraightPath", lFindStraightPath},
        {"FindRandomPointAroundCircle", lFindRandomPointAroundCircle},
        {"Raycast", lRaycast},
        {NULL, NULL},
    };
    create_meta(L, l, "navmesh", NULL, lrelease);
    lua_pushcclosure(L, lnew, 1);
}

LUAMOD_API int
luaopen_recastnavigation(lua_State *L)
{
    luaL_checkversion(L);
    lua_newtable(L);

    lnavmesh(L);
    lua_setfield(L, -2, "navmesh");

    return 1;
}