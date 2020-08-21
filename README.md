
Navigation 教程链接地址,navmesh文件的生成

https://github.com/ketoo/NoahGameFrame/blob/master/NFComm/NFNavigationPlugin

https://github.com/bluesky7290/NFrame_unity3d_nav

使用例子:
```lua
local recastnavigation = require("recastnavigation")
print("recastnavigation:", inspect(recastnavigation), "\n")

local path = "./srv_demo.navmesh"
local navmesh = recastnavigation.navmesh(1, path)
print("navmesh:", inspect(navmesh), "\n")

local res, path = navmesh:FindStraightPath(0,0,0,23,0,5)
if res then
    print("navmesh FindStraightPath:", inspect(path), "\n")
end

navmesh = nil

collectgarbage()

while true do
end
```
