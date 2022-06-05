﻿#include "pch.h"
#include "NPC.h"
#include "Player.h"
#include "Session.h"

using namespace std::chrono;

Session::Session() :
	recv_ex{},
	send_ex{},
	state{ STATE::FREE },
	sock{ INVALID_SOCKET },
	id{ -1 },
	object{ new Object{} },
	next_move_time{ steady_clock::now() + seconds(1) },
	remain{ 0 },
	flag{ 0 }
{
}

Session::Session(Object* obj) :
	recv_ex{},
	send_ex{},
	state{ STATE::FREE },
	sock{ INVALID_SOCKET },
	id{ -1 },
	object{ obj },
	next_move_time{ steady_clock::now() + seconds(1) },
	remain{ 0 },
	flag{ 0 }
{
}

Session::~Session()
{
	closesocket(sock);
	
	if (object)
	{
		delete object;
		object = nullptr;
	}
}

void Session::Reset()
{
	closesocket(sock);
	state.store(STATE::FREE, std::memory_order_seq_cst);
}

void Session::Recv()
{
	flag = 0;
	recv_ex.wsa.len = VAR::DATA - remain;
	recv_ex.wsa.buf = recv_ex.data + remain;

	ZeroMemory(&recv_ex.over, sizeof(recv_ex.over));
	WSARecv(sock, &recv_ex.wsa, 1, 0, &flag, &recv_ex.over, nullptr);
}

void Session::Send(void* packet)
{
	send_ex.Set(reinterpret_cast<char*>(packet));

	WSASend(sock, &send_ex.wsa, 1, 0, 0, &send_ex.over, nullptr);
}

void Session::SendLogin()
{
	sc_login.size = sizeof(SC::P::LOGIN);
	sc_login.type = SC::LOGIN;
	sc_login.id = id;
	sc_login.x = object->GetX();
	sc_login.y = object->GetY();
	sc_login.z = object->GetZ();

	Send(&sc_login);
}

void Session::SendAdd(Session* client)
{
	sc_add_obj.size = sizeof(SC::P::ADD_OBJ);
	sc_add_obj.type = SC::ADD_OBJ;
	sc_add_obj.id = client->id;
	strcpy_s(sc_add_obj.name, VAR::NAME, client->object->GetName());
	sc_add_obj.x = client->object->GetX();
	sc_add_obj.y = client->object->GetY();
	sc_add_obj.z = client->object->GetZ();

	if (client->id < MAX_USER)
	{
		auto player{ dynamic_cast<Player*>(client->object) };
		auto look{ player->GetLook() };
		auto right{ player->GetRight() };
		auto up{ player->GetUp() };

		sc_add_obj.look_x = look.x;
		sc_add_obj.look_y = look.y;
		sc_add_obj.look_z = look.z;

		sc_add_obj.right_x = right.x;
		sc_add_obj.right_y = right.y;
		sc_add_obj.right_z = right.z;

		sc_add_obj.up_x = up.x;
		sc_add_obj.up_y = up.y;
		sc_add_obj.up_z = up.z;
	}
	else
	{
		auto npc{ dynamic_cast<NPC*>(client->object) };
		auto look{ npc->GetLook() };
		auto right{ npc->GetRight() };
		auto up{ npc->GetUp() };

		sc_add_obj.look_x = look.x;
		sc_add_obj.look_y = look.y;
		sc_add_obj.look_z = look.z;

		sc_add_obj.right_x = right.x;
		sc_add_obj.right_y = right.y;
		sc_add_obj.right_z = right.z;

		sc_add_obj.up_x = up.x;
		sc_add_obj.up_y = up.y;
		sc_add_obj.up_z = up.z;
	}

	Send(&sc_add_obj);
}

void Session::SendRemove(ID target_id)
{
	sc_delete_obj.size = sizeof(SC::P::REMOVE_OBJ);
	sc_delete_obj.type = SC::REMOVE_OBJ;
	sc_delete_obj.id = target_id;

	Send(&sc_delete_obj);
}

void Session::SendMove(Session* client, int time)
{
	sc_move_obj.size = sizeof(SC::P::MOVE_OBJ);
	sc_move_obj.type = SC::MOVE_OBJ;
	sc_move_obj.id = client->id;
	sc_move_obj.x = client->object->GetX();
	sc_move_obj.y = client->object->GetY();
	sc_move_obj.z = client->object->GetZ();
	sc_move_obj.client_time = time;
	
	Send(&sc_move_obj);
}

void Session::SendRotate(Session* client)
{
	sc_rotate_obj.size = sizeof(SC::P::ROTATE_OBJ);
	sc_rotate_obj.type = SC::ROTATE_OBJ;
	sc_rotate_obj.id = client->id;

	if (client->id < MAX_USER)
	{
		auto player{ dynamic_cast<Player*>(client->object) };
		auto look{ player->GetLook() };
		auto right{ player->GetRight() };
		auto up{ player->GetUp() };

		sc_rotate_obj.look_x = look.x;
		sc_rotate_obj.look_y = look.y;
		sc_rotate_obj.look_z = look.z;

		sc_rotate_obj.right_x = right.x;
		sc_rotate_obj.right_y = right.y;
		sc_rotate_obj.right_z = right.z;

		sc_rotate_obj.up_x = up.x;
		sc_rotate_obj.up_y = up.y;
		sc_rotate_obj.up_z = up.z;
	}
	else
	{
		auto npc{ dynamic_cast<NPC*>(client->object) };
		auto look{ npc->GetLook() };
		auto right{ npc->GetRight() };
		auto up{ npc->GetUp() };

		sc_rotate_obj.look_x = look.x;
		sc_rotate_obj.look_y = look.y;
		sc_rotate_obj.look_z = look.z;

		sc_rotate_obj.right_x = right.x;
		sc_rotate_obj.right_y = right.y;
		sc_rotate_obj.right_z = right.z;

		sc_rotate_obj.up_x = up.x;
		sc_rotate_obj.up_y = up.y;
		sc_rotate_obj.up_z = up.z;
	}

	Send(&sc_rotate_obj);
}

void Session::SendPlayerAttack(Session* client)
{
}

void Session::AddToViewList(ID id)
{
	view_list.insert(id);
}

void Session::RemoveFromViewList(ID id)
{
	view_lock.lock();
	view_list.unsafe_erase(id);
	view_lock.unlock();
}
