#ifndef _RECASTNAVIGATION_H_
#define _RECASTNAVIGATION_H_

#include <iostream>
#include <vector>
#include <cstring>

#include "DetourNavMeshBuilder.h"
#include "DetourNavMeshQuery.h"
#include "DetourCommon.h"
#include "DetourNavMesh.h"

// https://github.com/ketoo/NoahGameFrame/blob/master/NFComm/NFNavigationPlugin

/** 安全的释放一个指针内存 */
#define SAFE_RELEASE(i) \
	if (i)              \
	{                   \
		delete i;       \
		i = NULL;       \
	}

/** 安全的释放一个指针数组内存 */
#define SAFE_RELEASE_ARRAY(i) \
	if (i)                    \
	{                         \
		delete[] i;           \
		i = NULL;             \
	}

// Returns a random number [0..1)
static float frand()
{
	return (float)rand() / (float)RAND_MAX;
}

struct NavMeshSetHeader
{
	int version;
	int tileCount;
	dtNavMeshParams params;
};

struct NavMeshTileHeader
{
	dtTileRef tileRef;
	int dataSize;
};

class NFVector3
{
private:
	float x, y, z;
	void InitData()
	{
		x = 0.0f;
		y = 0.0f;
		z = 0.0f;
	}

public:
	// construction
	NFVector3()
	{
		InitData();
	}

	NFVector3(float x, float y, float z)
	{
		this->x = x;
		this->y = y;
		this->z = z;
	}

	NFVector3(float coordinate[3])
	{
		this->x = coordinate[0];
		this->y = coordinate[1];
		this->z = coordinate[2];
	}

	NFVector3(double coordinate[3])
	{
		this->x = (float)coordinate[0];
		this->y = (float)coordinate[1];
		this->z = (float)coordinate[2];
	}

	NFVector3(const NFVector3 &v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;
	}

	//----------------------------------------------------------------------------
	bool operator<(const NFVector3 &v) const
	{
		return this->Length() < v.Length();
	}

	bool operator>(const NFVector3 &v) const
	{
		return this->Length() > v.Length();
	}

	NFVector3 &operator=(const NFVector3 &v)
	{
		this->x = v.x;
		this->y = v.y;
		this->z = v.z;

		return *this;
	}

	bool operator==(const NFVector3 &v) const
	{
		return std::abs(this->x - v.x) < 0.001f && std::abs(this->y - v.y) < 0.001f && std::abs(this->z - v.z) < 0.001f;
	}

	bool operator!=(const NFVector3 &v) const
	{
		return std::abs(this->x - v.x) >= 0.001f || std::abs(this->y - v.y) >= 0.001f || std::abs(this->z - v.z) >= 0.001f;
	}

	//----------------------------------------------------------------------------
	// Arithmetic Operations
	NFVector3 operator+(const NFVector3 &v) const
	{
		NFVector3 xV;

		xV.x = this->x + v.x;
		xV.y = this->y + v.y;
		xV.z = this->z + v.z;
		return xV;
	}

	NFVector3 operator-(const NFVector3 &v) const
	{
		NFVector3 xV;

		xV.x = this->x - v.x;
		xV.y = this->y - v.y;
		xV.z = this->z - v.z;
		return xV;
	}

	NFVector3 operator-() const
	{
		return NFVector3(-x, -y, -z);
	}

	NFVector3 operator*(float s) const
	{
		return NFVector3(x * s, y * s, z * s);
	}

	NFVector3 operator/(float s) const
	{
		if (std::abs(s) > 0.001f)
		{
			return NFVector3(x / s, y / s, z / s);
		}

		return Zero();
	}

	//----------------------------------------------------------------------------
	// Arithmetic Updates
	NFVector3 &operator+=(const NFVector3 &v)
	{
		x += v.x;
		y += v.y;
		z += v.z;
		return *this;
	}

	NFVector3 &operator-=(const NFVector3 &v)
	{
		x -= v.x;
		y -= v.y;
		z -= v.z;
		return *this;
	}

	NFVector3 &operator*=(float s)
	{
		x *= s;
		y *= s;
		z *= s;
		return *this;
	}

	NFVector3 operator/=(float s)
	{
		return NFVector3(x / s, y / s, z / s);
	}

	//----------------------------------------------------------------------------
	float X() const
	{
		return this->x;
	}

	float Y() const
	{
		return this->y;
	}

	float Z() const
	{
		return this->z;
	}

	void SetX(float x)
	{
		this->x = x;
	}

	void SetY(float y)
	{
		this->y = y;
	}

	void SetZ(float z)
	{
		this->z = z;
	}

	//----------------------------------------------------------------------------
	bool IsZero() const
	{
		return std::abs(x) < 0.001f && std::abs(y) < 0.001f && std::abs(z) < 0.001f;
	}
	//----------------------------------------------------------------------------
	inline float SquaredMagnitude() const
	{
		return x * x + y * y + z * z;
	}

	//----------------------------------------------------------------------------
	inline float SquaredLength() const
	{
		return SquaredMagnitude();
	}

	//----------------------------------------------------------------------------
	inline float Magnitude() const
	{
		return sqrtf(x * x + y * y + z * z);
	}

	//----------------------------------------------------------------------------
	inline float Length() const
	{
		return Magnitude();
	}

	//----------------------------------------------------------------------------
	inline NFVector3 Direction() const
	{
		if (this->IsZero())
		{
			return Zero();
		}

		float lenSquared = SquaredMagnitude();
		float invSqrt = 1.0f / sqrtf(lenSquared);
		return NFVector3(x * invSqrt, y * invSqrt, z * invSqrt);
	}

	//----------------------------------------------------------------------------
	inline NFVector3 Normalized() const
	{
		return Direction();
	}

	//----------------------------------------------------------------------------
	float Distance(const NFVector3 &v) const
	{
		NFVector3 vX = *this - v;
		return vX.Length();
	}

	// Special values.
	inline static const NFVector3 &Zero()
	{
		static NFVector3 v(0, 0, 0);
		return v;
	}
	inline static const NFVector3 &One()
	{
		static NFVector3 v(1, 1, 1);
		return v;
	}
	inline static const NFVector3 &UnitX()
	{
		static NFVector3 v(1, 0, 0);
		return v;
	}
	inline static const NFVector3 &UnitY()
	{
		static NFVector3 v(0, 1, 0);
		return v;
	}
	inline static const NFVector3 &UnitZ()
	{
		static NFVector3 v(0, 0, 1);
		return v;
	}
};

class RecastNavigationHandle
{
public:
	static const int NAV_ERROR = -1;

	static const int MAX_POLYS = 256;
	static const int NAV_ERROR_NEARESTPOLY = -2;

	static const long RCN_NAVMESH_VERSION = 1;
	static const int INVALID_NAVMESH_POLYREF = 0;

	struct NavmeshLayer
	{
		dtNavMesh *pNavmesh;
		dtNavMeshQuery *pNavmeshQuery;
	};

public:
	RecastNavigationHandle(){};

	virtual ~RecastNavigationHandle()
	{
		dtFreeNavMesh(navmeshLayer.pNavmesh);
		dtFreeNavMeshQuery(navmeshLayer.pNavmeshQuery);
	};

	int FindStraightPath(const NFVector3 &start, const NFVector3 &end, std::vector<NFVector3> &paths)
	{
		dtNavMeshQuery *navmeshQuery = navmeshLayer.pNavmeshQuery;

		float spos[3];
		spos[0] = start.X();
		spos[1] = start.Y();
		spos[2] = start.Z();

		float epos[3];
		epos[0] = end.X();
		epos[1] = end.Y();
		epos[2] = end.Z();

		dtQueryFilter filter;
		filter.setIncludeFlags(0xffff);
		filter.setExcludeFlags(0);

		const float extents[3] = {2.f, 4.f, 2.f};

		dtPolyRef startRef = INVALID_NAVMESH_POLYREF;
		dtPolyRef endRef = INVALID_NAVMESH_POLYREF;

		float startNearestPt[3];
		float endNearestPt[3];
		navmeshQuery->findNearestPoly(spos, extents, &filter, &startRef, startNearestPt);
		navmeshQuery->findNearestPoly(epos, extents, &filter, &endRef, endNearestPt);

		if (!startRef || !endRef)
		{
			//debuf_msg("NavMeshHandle::findStraightPath({%s}): Could not find any nearby poly's ({%d}, {%d})\n", resPath.c_str(), startRef, endRef);
			return NAV_ERROR_NEARESTPOLY;
		}

		dtPolyRef polys[MAX_POLYS];
		int npolys;
		float straightPath[MAX_POLYS * 3];
		unsigned char straightPathFlags[MAX_POLYS];
		dtPolyRef straightPathPolys[MAX_POLYS];
		int nstraightPath;
		int pos = 0;

		navmeshQuery->findPath(startRef, endRef, startNearestPt, endNearestPt, &filter, polys, &npolys, MAX_POLYS);
		nstraightPath = 0;

		if (npolys)
		{
			float epos1[3];
			dtVcopy(epos1, endNearestPt);

			if (polys[npolys - 1] != endRef)
				navmeshQuery->closestPointOnPoly(polys[npolys - 1], endNearestPt, epos1, 0);

			navmeshQuery->findStraightPath(startNearestPt, endNearestPt, polys, npolys, straightPath, straightPathFlags, straightPathPolys, &nstraightPath, MAX_POLYS);

			NFVector3 currpos;
			for (int i = 0; i < nstraightPath * 3;)
			{
				currpos.SetX(straightPath[i++]);
				currpos.SetY(straightPath[i++]);
				currpos.SetZ(straightPath[i++]);
				paths.push_back(currpos);
				pos++;
			}
		}

		return pos;
	}

	int FindRandomPointAroundCircle(const NFVector3 &centerPos, std::vector<NFVector3> &points, int maxPoints, float maxRadius)
	{
		dtNavMeshQuery *navmeshQuery = navmeshLayer.pNavmeshQuery;

		dtQueryFilter filter;
		filter.setIncludeFlags(0xffff);
		filter.setExcludeFlags(0);

		if (maxRadius <= 0.0001f)
		{
			NFVector3 currpos;

			for (int i = 0; i < maxPoints; i++)
			{
				float pt[3];
				dtPolyRef ref;
				dtStatus status = navmeshQuery->findRandomPoint(&filter, frand, &ref, pt);
				if (dtStatusSucceed(status))
				{
					currpos.SetX(pt[0]);
					currpos.SetY(pt[1]);
					currpos.SetZ(pt[2]);

					points.push_back(currpos);
				}
			}

			return (int)points.size();
		}

		const float extents[3] = {2.f, 4.f, 2.f};

		dtPolyRef startRef = INVALID_NAVMESH_POLYREF;

		float spos[3];
		spos[0] = centerPos.X();
		spos[1] = centerPos.Y();
		spos[2] = centerPos.Z();

		float startNearestPt[3];
		navmeshQuery->findNearestPoly(spos, extents, &filter, &startRef, startNearestPt);

		if (!startRef)
		{
			//debuf_msg("NavMeshHandle::findRandomPointAroundCircle({%s}): Could not find any nearby poly's ({%d})\n", resPath, startRef);
			return NAV_ERROR_NEARESTPOLY;
		}

		NFVector3 currpos;
		bool done = false;
		int itry = 0;

		while (itry++ < 3 && points.size() == 0)
		{
			maxPoints -= (int)points.size();

			for (int i = 0; i < maxPoints; i++)
			{
				float pt[3];
				dtPolyRef ref;
				dtStatus status = navmeshQuery->findRandomPointAroundCircle(startRef, spos, maxRadius, &filter, frand, &ref, pt);

				if (dtStatusSucceed(status))
				{
					done = true;
					currpos.SetX(pt[0]);
					currpos.SetY(pt[1]);
					currpos.SetZ(pt[2]);

					NFVector3 v = centerPos - currpos;
					float dist_len = v.Length();
					if (dist_len > maxRadius)
						continue;

					points.push_back(currpos);
				}
			}

			if (!done)
				break;
		}

		return (int)points.size();
	}

	int Raycast(const NFVector3 &start, const NFVector3 &end, std::vector<NFVector3> &hitPointVec)
	{
		dtNavMeshQuery *navmeshQuery = navmeshLayer.pNavmeshQuery;

		float hitPoint[3];

		float spos[3];
		spos[0] = start.X();
		spos[1] = start.Y();
		spos[2] = start.Z();

		float epos[3];
		epos[0] = end.X();
		epos[1] = end.Y();
		epos[2] = end.Z();

		dtQueryFilter filter;
		filter.setIncludeFlags(0xffff);
		filter.setExcludeFlags(0);

		const float extents[3] = {2.f, 4.f, 2.f};

		dtPolyRef startRef = INVALID_NAVMESH_POLYREF;

		float nearestPt[3];
		navmeshQuery->findNearestPoly(spos, extents, &filter, &startRef, nearestPt);

		if (!startRef)
		{
			return NAV_ERROR_NEARESTPOLY;
		}

		float t = 0;
		float hitNormal[3];
		memset(hitNormal, 0, sizeof(hitNormal));

		dtPolyRef polys[MAX_POLYS];
		int npolys;

		navmeshQuery->raycast(startRef, spos, epos, &filter, &t, hitNormal, polys, &npolys, MAX_POLYS);

		if (t > 1)
		{
			// no hit
			return NAV_ERROR;
		}
		else
		{
			// Hit
			hitPoint[0] = spos[0] + (epos[0] - spos[0]) * t;
			hitPoint[1] = spos[1] + (epos[1] - spos[1]) * t;
			hitPoint[2] = spos[2] + (epos[2] - spos[2]) * t;
			if (npolys)
			{
				float h = 0;
				navmeshQuery->getPolyHeight(polys[npolys - 1], hitPoint, &h);
				hitPoint[1] = h;
			}
		}

		hitPointVec.push_back(NFVector3(hitPoint[0], hitPoint[1], hitPoint[2]));
		return 1;
	}

	static RecastNavigationHandle *Create(std::string resPath)
	{
		FILE *fp = fopen(resPath.c_str(), "rb");
		if (!fp)
		{
			printf("RecastNavigationHandle::create: open({%s}) is error!\n", resPath.c_str());
			return NULL;
		}

		printf("RecastNavigationHandle::create: ({%s}), layer={%d}\n", resPath.c_str(), 0);

		bool safeStorage = true;
		int pos = 0;
		int size = sizeof(NavMeshSetHeader);

		fseek(fp, 0, SEEK_END);
		size_t flen = ftell(fp);
		fseek(fp, 0, SEEK_SET);

		uint8_t *data = new uint8_t[flen];
		if (data == NULL)
		{
			printf("RecastNavigationHandle::create: open({%s}), memory(size={%d}) error!\n", resPath.c_str(), (int)flen);

			fclose(fp);
			SAFE_RELEASE_ARRAY(data);
			return NULL;
		}

		size_t readsize = fread(data, 1, flen, fp);
		if (readsize != flen)
		{
			printf("RecastNavigationHandle::create: open({%s}), read(size={%d} != {%d}) error!\n", resPath.c_str(), (int)readsize, (int)flen);

			fclose(fp);
			SAFE_RELEASE_ARRAY(data);
			return NULL;
		}

		if (readsize < sizeof(NavMeshSetHeader))
		{
			printf("RecastNavigationHandle::create: open({%s}), NavMeshSetHeader is error!\n", resPath.c_str());

			fclose(fp);
			SAFE_RELEASE_ARRAY(data);
			return NULL;
		}

		NavMeshSetHeader header;
		memcpy(&header, data, size);

		pos += size;

		if (header.version != RecastNavigationHandle::RCN_NAVMESH_VERSION)
		{
			printf("NFNavigationHandle::create: navmesh version({%d}) is not match({%d})!\n", header.version, ((int)RecastNavigationHandle::RCN_NAVMESH_VERSION));

			fclose(fp);
			SAFE_RELEASE_ARRAY(data);
			return NULL;
		}

		dtNavMesh *mesh = dtAllocNavMesh();
		if (!mesh)
		{
			printf("NavMeshHandle::create: dtAllocNavMesh is failed!\n");
			fclose(fp);
			SAFE_RELEASE_ARRAY(data);
			return NULL;
		}

		dtStatus status = mesh->init(&header.params);
		if (dtStatusFailed(status))
		{
			printf("NFNavigationHandle::create: mesh init is error({%d})!\n", status);
			fclose(fp);
			SAFE_RELEASE_ARRAY(data);
			return NULL;
		}

		// Read tiles.
		bool success = true;
		for (int i = 0; i < header.tileCount; ++i)
		{
			NavMeshTileHeader tileHeader;
			size = sizeof(NavMeshTileHeader);
			memcpy(&tileHeader, &data[pos], size);
			pos += size;

			size = tileHeader.dataSize;
			if (!tileHeader.tileRef || !tileHeader.dataSize)
			{
				success = false;
				status = DT_FAILURE + DT_INVALID_PARAM;
				break;
			}

			unsigned char *tileData =
				(unsigned char *)dtAlloc(size, DT_ALLOC_PERM);
			if (!tileData)
			{
				success = false;
				status = DT_FAILURE + DT_OUT_OF_MEMORY;
				break;
			}
			memcpy(tileData, &data[pos], size);
			pos += size;

			status = mesh->addTile(tileData, size, (safeStorage ? DT_TILE_FREE_DATA : 0), tileHeader.tileRef, 0);

			if (dtStatusFailed(status))
			{
				success = false;
				break;
			}
		}

		fclose(fp);
		SAFE_RELEASE_ARRAY(data);

		if (!success)
		{
			printf("NavMeshHandle::create:  error({%d})!\n", status);
			dtFreeNavMesh(mesh);
			return NULL;
		}

		RecastNavigationHandle *pNavMeshHandle = new RecastNavigationHandle();
		dtNavMeshQuery *pNavmeshQuery = new dtNavMeshQuery();

		pNavmeshQuery->init(mesh, 1024);
		pNavMeshHandle->resPath = resPath;
		pNavMeshHandle->navmeshLayer.pNavmeshQuery = pNavmeshQuery;
		pNavMeshHandle->navmeshLayer.pNavmesh = mesh;

		uint32_t tileCount = 0;
		uint32_t nodeCount = 0;
		uint32_t polyCount = 0;
		uint32_t vertCount = 0;
		uint32_t triCount = 0;
		uint32_t triVertCount = 0;
		uint32_t dataSize = 0;

		const dtNavMesh *navmesh = mesh;
		for (int i = 0; i < navmesh->getMaxTiles(); ++i)
		{
			const dtMeshTile *tile = navmesh->getTile(i);
			if (!tile || !tile->header)
				continue;

			tileCount++;
			nodeCount += tile->header->bvNodeCount;
			polyCount += tile->header->polyCount;
			vertCount += tile->header->vertCount;
			triCount += tile->header->detailTriCount;
			triVertCount += tile->header->detailVertCount;
			dataSize += tile->dataSize;
		}

		printf("\t==> ----------------RecastNavigationHandle Create------------------------\n");
		printf("\t==> resPath: {%s}\n", resPath.c_str());
		printf("\t==> tiles loaded: {%d}\n", tileCount);
		printf("\t==> BVTree nodes: {%d}\n", nodeCount);
		printf("\t==> {%d} polygons ({%d} vertices)\n", polyCount, vertCount);
		printf("\t==> {%d} triangles ({%d} vertices)\n", triCount, triVertCount);
		printf("\t==> {%f:.2f} MB of data (not including pointers)\n", (((float)dataSize / sizeof(unsigned char)) / 1048576));
		printf("\t==> ----------------RecastNavigationHandle Create------------------------\n");

		return pNavMeshHandle;
	}

	NavmeshLayer navmeshLayer;
	std::string resPath;
};

#endif