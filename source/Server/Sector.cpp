#include "pch.h"
#include "Sector.h"

Sector::Sector() :
	lt_x{ VAR::WORLD_X_MIN },
	lt_z{ VAR::WORLD_Z_MIN }
{
}

Sector::Sector(POS x, POS z) :
	lt_x{ x },
	lt_z{ z }
{
}

Sector::~Sector()
{
}

Sector::Sector(const Sector& right) :
	lt_x{ right.lt_x },
	lt_z{ right.lt_z }
{
	std::memcpy(&objects, &right.objects, right.objects.size());
}

Sector& Sector::operator=(const Sector& right)
{
	if (this != &right)
	{
		lt_x = right.lt_x;
		lt_z = right.lt_z;

		std::memcpy(&objects, &right.objects, right.objects.size());
	}

	return *this;
}

Sector::Sector(Sector&& right) noexcept :
	lt_x{ right.lt_x },
	lt_z{ right.lt_z }
{
	right.lt_x = VAR::WORLD_X_MIN;
	right.lt_z = VAR::WORLD_Z_MIN;

	sector_lock.lock();
	right.sector_lock.lock();

	objects.swap(right.objects);
	right.objects.clear();

	right.sector_lock.unlock();
	sector_lock.unlock();
}

Sector& Sector::operator=(Sector&& right) noexcept
{
	if (this != &right)
	{
		lt_x = right.lt_x;
		lt_z = right.lt_z;

		right.lt_x = VAR::WORLD_X_MIN;
		right.lt_z = VAR::WORLD_Z_MIN;

		sector_lock.lock();
		right.sector_lock.lock();

		objects.swap(right.objects);
		right.objects.clear();

		right.sector_lock.unlock();
		sector_lock.unlock();
	}

	return *this;
}

void Sector::EnterSector(ID id)
{
	objects.insert(id);
}

void Sector::LeaveSector(ID id)
{
	sector_lock.lock();
	objects.unsafe_erase(id);
	sector_lock.unlock();
}

void Sector::MakeNewPlayerViewList(c_set* new_list, Session* client, c_map* players, c_map* npcs)
{
	c_map obj;

	for (auto& [id, player] : *players)
	{
		obj[id] = player;
	}
	
	for (auto& [id, npc] : *npcs)
	{
		obj[id] = npc;
	}

	if (not objects.empty())
	{
		for (auto& obj_id : objects)
		{
			if (not client->IsMyID(obj_id))
			{
				auto dis1{ client->GetX() - obj[obj_id]->GetX() };
				auto dis2{ client->GetZ() - obj[obj_id]->GetZ() };

				if (::IsInSight(dis1, dis2))
				{
					new_list->insert(obj_id);
				}
			}
		}
	}
}