
SKYNET_ROOT ?= ../../skynet
include $(SKYNET_ROOT)/platform.mk

PLAT ?= none
TARGET ?= ../../luaclib/recastnavigation.so

CXX=g++

ifeq ($(PLAT), macosx)
	CXXFLAGS = -g -O2 -pedantic -bundle -undefined dynamic_lookup -std=c++17
else
ifeq ($(PLAT), linux)
	CXXFLAGS = -g -O2 -shared -fPIC -std=c++17
endif
endif

LUA_INC ?= $(SKYNET_ROOT)/3rd/lua/

RECAST_NAVIGATION_DIR = ../../thirdparty/recastnavigation

DETOUR_INC = $(RECAST_NAVIGATION_DIR)/Detour/Include
DETOUR_SRC = DetourAlloc.cpp DetourCommon.cpp DetourNavMesh.cpp DetourNavMeshBuilder.cpp \
			DetourAssert.cpp DetourNavMeshQuery.cpp DetourNode.cpp

RECAST_INC = $(RECAST_NAVIGATION_DIR)/Recast/Include
RECAST_SRC = Recast.cpp RecastAlloc.cpp RecastArea.cpp RecastContour.cpp RecastFilter.cpp RecastLayers.cpp \
			RecastAssert.cpp RecastMesh.cpp RecastMeshDetail.cpp RecastRasterization.cpp RecastRegion.cpp

DETOUR_TILECACHE_INC = $(RECAST_NAVIGATION_DIR)/DetourTileCache/Include
DETOUR_TILECACHE_SRC = DetourTileCache.cpp DetourTileCacheBuilder.cpp

LRECAST_NAVIGATION = lua-recastnavigation.cpp

.PHONY: all clean

all: $(TARGET)

$(TARGET): $(foreach v, $(DETOUR_SRC), $(RECAST_NAVIGATION_DIR)/Detour/Source/$(v)) \
					$(foreach v, $(RECAST_SRC), $(RECAST_NAVIGATION_DIR)/Recast/Source/$(v)) \
					$(foreach v, $(DETOUR_TILECACHE_SRC), $(RECAST_NAVIGATION_DIR)/DetourTileCache/Source/$(v)) \
					$(foreach v, $(LRECAST_NAVIGATION), $(v))
	$(CXX) $(CXXFLAGS) -o $@ $^ -I$(LUA_INC) -I$(RECAST_INC) -I$(DETOUR_INC) -I$(DETOUR_TILECACHE_INC) 

clean:
	rm -f *.o $(TARGET)