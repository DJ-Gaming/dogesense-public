#include "../features.hpp"

/* AntiAim */

float server_curtime(c_usercmd* cmd) {
	const auto v1 = static_cast<i_net_channel_info*> (interfaces::engine->get_net_channel_info_new());

	const auto v3 = v1->get_latency(1); //local player
	const auto v4 = v1->get_latency(0); //generic

	return v3 + v4 + ticks_to_time(1) + ticks_to_time(cmd->tick_count);
}

bool lby_update(c_usercmd* cmd) {

	static auto next_lby_update_time = -1.f;

	auto spawn_time = csgo::local_player->spawn_time();
	auto anim_state = csgo::local_player->get_anim_state();

	if (!(csgo::local_player->flags() & fl_onground) && anim_state->flUpVelocity < 100.f)
		return false;

	const auto current_time = server_curtime(cmd);
	{
		if (spawn_time != csgo::local_player->spawn_time())
		{
			spawn_time = csgo::local_player->spawn_time();
			next_lby_update_time = -1.f;
		}

		if (anim_state->speed_2d > 0.1 || !(csgo::local_player->flags() & fl_onground) && anim_state->flUpVelocity > 100.f)
			next_lby_update_time = current_time + 0.22f;
		else if (next_lby_update_time == -1.f || current_time >= next_lby_update_time)
		{
			csgo::send_packet = false;
			next_lby_update_time = current_time + 1.1f;
			return true;
		}
	}

	return next_lby_update_time - current_time <= interfaces::globals->interval_per_tick;
}

void antiaim::run(c_usercmd* cmd) {

	if (!variables::antiaim) return;
	if (!csgo::local_player) return;
	if (!csgo::local_player->is_alive()) return;
	weapon_t* weapon = csgo::local_player->active_weapon();
	if (!weapon) return;

	if (csgo::local_player->move_type() == movetype_ladder) return;
	if (cmd->buttons & in_use) return;
	if (cmd->buttons & in_attack) return;
	if (cmd->buttons & in_attack2 && weapon->is_knife()) return;
	if (cmd->buttons & in_attack2 && weapon->item_definition_index() == WEAPON_REVOLVER) return;
	if (weapon->is_nade()) return;

	csgo::send_packet = interfaces::engine->get_net_channel_info()->choked_packets >= 3;

	static auto sidemove_direction = 0.f, sidemove_speed = 0.f;

	sidemove_speed = cmd->buttons & in_duck ? -3.3f : -1.2f;

	if (variables::lby) {
		if (GetKeyState(0x5A)) {
			if (lby_update(cmd)) {
				csgo::send_packet = false;
				cmd->viewangles.y += 120;
			}
			else if (!csgo::send_packet)
				cmd->viewangles.y += 120;
		}
		else {
			if (lby_update(cmd)) {
				csgo::send_packet = false;
				cmd->viewangles.y -= 120;
			}
			else if (!csgo::send_packet)
				cmd->viewangles.y -= 120;
		}
	}
	else {
		if (GetKeyState(0x5A)) {
			if (!csgo::send_packet)
				cmd->viewangles.y += 58.f;
		}
		else {
			if (!csgo::send_packet)
				cmd->viewangles.y -= 58.f;
		}
	}

	if (csgo::send_packet && variables::micro) {
		sidemove_direction *= -1.f;

		
		if (std::fabsf(cmd->sidemove) < 4.0f && std::fabsf(cmd->sidemove) < 4.0f)
			cmd->sidemove = sidemove_direction * sidemove_speed;
	}

	cmd->buttons &= ~(in_forward | in_back | in_moveright | in_moveleft);
}