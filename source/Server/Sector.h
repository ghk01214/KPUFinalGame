#pragma once

#include "Session.h"
#include "Object.h"

class Sector
{
private:
	using c_map = concurrency::concurrent_unordered_map<int, Session*>;

public:
	Sector();
	Sector(POS x, POS z);
	~Sector();

	Sector(const Sector& right);
	Sector& operator=(const Sector& right);

	Sector(Sector&& right) noexcept;
	Sector& operator=(Sector&& right) noexcept;

	void EnterSector(ID id);
	void LeaveSector(ID id);

	void MakeNewPlayerViewList(c_set* new_list, Session* client, c_map* players, c_map* npcs);

	bool OutOfSectorXMax(POS x) { return lt_x + SECTOR_RANGE <= x; }
	bool OutOfSectorXMin(POS x) { return x < lt_x; }
	bool OutOfSectorZMax(POS z) { return lt_z + SECTOR_RANGE <= z; }
	bool OutOfSectorZMin(POS z) { return z < lt_z; }

	POS GetLBX() { return lt_x; }
	POS GetLBZ() { return lt_z; }

private:
	c_set objects;
	std::shared_mutex sector_lock;

	POS lt_x, lt_z;
};

