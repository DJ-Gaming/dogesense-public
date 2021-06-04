#define NOMINMAX
#include "../features.hpp"

/* Backtrack */

float lerp_time() {

	int ud_rate = interfaces::console->get_convar("cl_updaterate")->get_int();
	convar* min_ud_rate = interfaces::console->get_convar("sv_minupdaterate");
	convar* max_ud_rate = interfaces::console->get_convar("sv_maxupdaterate");

	if (min_ud_rate && max_ud_rate)
		ud_rate = max_ud_rate->get_int();

	float ratio = interfaces::console->get_convar("cl_interp_ratio")->get_float();

	if (ratio == 0)
		ratio = 1.0f;

	float lerp = interfaces::console->get_convar("cl_interp")->get_float();
	convar* c_min_ratio = interfaces::console->get_convar("sv_client_min_interp_ratio");
	convar* c_max_ratio = interfaces::console->get_convar("sv_client_max_interp_ratio");

	if (c_min_ratio && c_max_ratio && c_min_ratio->get_float() != 1)
		ratio = std::clamp(ratio, c_min_ratio->get_float(), c_max_ratio->get_float());

	return std::max(lerp, ratio / ud_rate);
}

bool backtracking::is_tick_valid(float simulation_time, float backtrack_time) {
	i_net_channel_info* nci = interfaces::engine->get_net_channel_info_new();
	if (!nci) return true;
	float correct = 0;
	correct += nci->get_latency(0);
	correct += nci->get_latency(1);

	std::clamp(correct, 0.f, interfaces::console->get_convar("sv_maxunlag")->get_float());

	float delta_time = correct - (interfaces::globals->cur_time - simulation_time);

	float time_limit = std::clamp(backtrack_time, 0.001f, 0.2f);

	if (fabsf(delta_time) > time_limit)
		return false;

	return true;
}
std::vector<tick_info> backtracking::records[65];
int backtracking::closest_player = -1;
int backtracking::closest_tick = -1;
void backtracking::store(c_usercmd* cmd) {
	if (!csgo::local_player || !variables::backtrack )
		return;

	if (!csgo::local_player->is_alive())
		return;

	vec3_t eye_pos = csgo::local_player->get_eye_pos();

	closest_player = -1;
	float max_player_fov = FLT_MAX;
	for (int i = 1; i <= 64; i++) {
		auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!player ||
			player->dormant() ||
			!player->is_player() ||
			!player->is_alive() ||
			player->team() == csgo::local_player->team()) {
			if (records[i].size() > 0)
				records[i].clear();
			continue;
		}

		records[i].insert(records[i].begin(), tick_info(player));

		for (auto tick : records[i])
			if (!is_tick_valid(tick.simulation_time, float(200) / 1000.f))
				records[i].pop_back();
	}
}
void backtracking::run(c_usercmd* cmd) {
	if (!csgo::local_player || !variables::backtrack )
		return;

	if (!csgo::local_player->is_alive())
		return;

	vec3_t eye_pos = csgo::local_player->get_eye_pos();
	float max_player_fov = FLT_MAX;
	for (int i = 1; i <= 64; i++) {
		auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!player ||
			player->dormant() ||
			!player->is_player() ||
			!player->is_alive() ||
			player->team() == csgo::local_player->team()) {
			if (records[i].size() > 0)
				records[i].clear();
			continue;
		}

		vec3_t view_angles{};
		math::angle_vectors((cmd->viewangles + (csgo::local_player->aim_punch_angle() * interfaces::console->get_convar("weapon_recoil_scale")->get_float())), view_angles);
		float fov_distance = math::distance_point_to_line(records[i].front().head_pos, eye_pos, view_angles);

		if (max_player_fov > fov_distance) {
			max_player_fov = fov_distance;
			closest_player = i;
		}
	}
	closest_tick = -1;
	float max_tick_fov = FLT_MAX;
	if (closest_player != -1) {
		for (int t = 0; t < records[closest_player].size(); t++) {
			vec3_t view_angles2{};
			math::angle_vectors((cmd->viewangles + (csgo::local_player->aim_punch_angle() * interfaces::console->get_convar("weapon_recoil_scale")->get_float())), view_angles2);
			float fov_distance2 = math::distance_point_to_line(records[closest_player].at(t).head_pos, eye_pos, view_angles2);

			if (max_tick_fov > fov_distance2) {
				max_tick_fov = fov_distance2;
				closest_tick = t;
			}
		}
		if (closest_tick != -1 && records[closest_player].at(closest_tick).simulation_time != -1.f && (cmd->buttons & in_attack))
			cmd->tick_count = time_to_ticks(records[closest_player].at(closest_tick).simulation_time);
	}
}

/* RCS */

void recoil::rcs(c_usercmd* cmd) {

	if (!variables::rcs)
		return;

	if (!csgo::local_player)
		return;

	weapon_t* weapon = csgo::local_player->active_weapon();

	if (!weapon)
		return;

	if (!weapon->is_pistol() && !weapon->is_sniper())
	{
		static vec3_t vOldPunch = { 0.0f, 0.0f, 0.0f };
		vec3_t vNewPunch = csgo::local_player->aim_punch_angle();

		vNewPunch *= 2.f;

		vNewPunch -= vOldPunch;
		vNewPunch += vOldPunch;

		vec3_t vFinal = cmd->viewangles - (vNewPunch - vOldPunch);

		math::sanitize(vFinal);

		if (!math::sanitize(vFinal))
			return;

		math::Clamp(vFinal);

		interfaces::engine->set_view_angles(vFinal);
		
		vOldPunch = vNewPunch;
	}
}

/* Aimbot */

aimbot_ctx_t aimbot::ctx;

void aimbot::weapon_cfg(weapon_t* weapon) {
	ctx.hitbox_pos = vec3_t{ 0.0f, 0.0f, 0.0f };
	ctx.hitpoint = 3;
	ctx.mode == 1;

	if (weapon->is_pistol()) {
		ctx.fov = variables::aim_fov_pistol;
		ctx.smooth = variables::smooth_pistol;
	}
	else if (weapon->is_sniper()) {
		ctx.fov = variables::aim_fov_sniper;
		ctx.smooth = variables::smooth_sniper;
	}
	else {
		ctx.fov = variables::aim_fov_rifle;
		ctx.smooth = variables::smooth_rifle;
	}
}

int aimbot::get_hitbox(int hitpoint)
{
	if (hitpoint != hitbox_head && hitpoint != hitbox_neck)
		return hitbox_chest;

	return hitpoint;
}
int aimbot::get_hitbox(player_t* player, matrix_t* matrix, vec3_t eye_pos) {
	vec3_t view_angle;
	interfaces::engine->get_view_angles(view_angle);
	float max_fov = ctx.fov;
	int closest_hitbox = hitbox_chest;
	for (int i = 0; i <= 6; i++)
	{
		float fov_distance = math::get_fov(view_angle + (csgo::local_player->aim_punch_angle() * interfaces::console->get_convar("weapon_recoil_scale")->get_float()), math::calculate_angle(eye_pos, player->get_hitbox_position(i, matrix)));

		if (max_fov > fov_distance)
		{
			max_fov = fov_distance;
			closest_hitbox = i;
		}
	}
	return closest_hitbox;
}

void aimbot::run(c_usercmd* cmd)
{
	if (!csgo::local_player ||
		!csgo::local_player->is_alive())
		return;

	auto weapon = csgo::local_player->active_weapon();

	if (!weapon ||
		weapon->is_knife() ||
		weapon->is_nade() ||
		weapon->is_c4() ||
		weapon->is_taser() ||
		!(cmd->buttons & in_attack))
		return;

	weapon_cfg(weapon);

	if (!variables::aimbot)
		return;

	float max_player_fov = ctx.fov;
	vec3_t aim_angle;
	vec3_t view_angle;
	interfaces::engine->get_view_angles(view_angle);
	vec3_t eye_pos = csgo::local_player->get_eye_pos();
	int closest_player = -1;

	for (int i = 1; i <= 64; i++) {
		auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (!player ||
			player->dormant() ||
			!player->is_player() ||
			!player->is_alive() ||
			player->team() == csgo::local_player->team())
			continue;

		vec3_t current_hitbox;

		matrix_t bone_matrix[128];
		if (player->setup_bones(bone_matrix, 128, BONE_USED_BY_HITBOX, interfaces::globals->cur_time) && bone_matrix) {
			vec3_t temp_hitbox = player->get_hitbox_position((ctx.hitpoint == 3) ?
				get_hitbox(player, bone_matrix, eye_pos)
				: get_hitbox(ctx.hitpoint), bone_matrix);
			if (!utilities::goes_through_smoke(eye_pos, temp_hitbox))
				current_hitbox = temp_hitbox;
		}
		if (current_hitbox.is_valid()) {
			float fov_distance = math::get_fov(view_angle + (csgo::local_player->aim_punch_angle() * interfaces::console->get_convar("weapon_recoil_scale")->get_float()), math::calculate_angle(eye_pos, current_hitbox));
			if (max_player_fov > fov_distance)
			{
				if (csgo::local_player->can_see_player_pos(player, current_hitbox))
				{
					max_player_fov = fov_distance;
					closest_player = i;
					ctx.hitbox_pos = current_hitbox;
				}
			}
		}
	}
	if (closest_player != -1 && ctx.hitbox_pos.is_valid())
	{
		auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(closest_player));
		if (!player) return;
		aim_angle = math::calculate_angle(eye_pos, ctx.hitbox_pos);
		math::sanitize_angle(aim_angle);
		vec3_t recoil_angle = csgo::local_player->aim_punch_angle() * interfaces::console->get_convar("weapon_recoil_scale")->get_float();
		math::sanitize_angle(recoil_angle);
		aim_angle -= recoil_angle;
		math::sanitize_angle(view_angle);
		vec3_t delta_angle = view_angle - aim_angle;
		math::sanitize_angle(delta_angle);
		float smoothing = (ctx.smooth > 1.f) ? ctx.smooth : 1.f;
		vec3_t final_angle = view_angle - delta_angle / smoothing;
		if (!math::sanitize_angle(final_angle))
			return;
		cmd->viewangles = final_angle;
		if (ctx.mode != 2)
			interfaces::engine->set_view_angles(cmd->viewangles);
	}
}