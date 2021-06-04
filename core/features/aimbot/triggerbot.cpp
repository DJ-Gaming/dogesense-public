#include "../features.hpp"

/* Triggerbot */

triggerbot_ctx triggerbot::ctx_t;

void triggerbot::weapon_cfg(weapon_t* weapon) {

	ctx_t.hitbox_pos = vec3_t{ 0.0f, 0.0f, 0.0f };
	ctx_t.hitpoint = 0;
	ctx_t.enabled = variables::triggerbot;
	ctx_t.fov = variables::trigger_fov;
	ctx_t.delay = variables::trigger_delay;
	ctx_t.minimim_damage = variables::trigger_min_damage;
}
constexpr float get_damage_multiplier(int hitGroup) noexcept
{
	switch (hitGroup) {
	case 1:
		return 4.0f;
	case 3:
		return 1.25f;
	case 6:
	case 7:
		return 0.75f;
	default:
		return 1.0f;
	}
}
constexpr bool is_armored(int hitGroup, bool helmet) noexcept
{
	switch (hitGroup) {
	case 1:
		return helmet;

	case 2:
	case 3:
	case 4:
	case 5:
		return true;
	default:
		return false;
	}
}

float RandomFloat(float min, float max)
{
	static auto ranFloat = reinterpret_cast<float(*)(float, float)>(GetProcAddress(GetModuleHandleW(L"vstdlib.dll"), "RandomFloat"));
	if (ranFloat)
	{
		return ranFloat(min, max);
	}
	else
	{
		return 0.f;
	}
}

void triggerbot::run(c_usercmd* cmd) {
	if (!csgo::local_player || !csgo::local_player->is_alive() || csgo::local_player->next_attack() > interfaces::globals->cur_time)
		return;

	const auto activeWeapon = csgo::local_player->active_weapon();
	if (!activeWeapon || !activeWeapon->clip1_count() || activeWeapon->next_primary_attack() > interfaces::globals->cur_time)
		return;

	weapon_cfg(activeWeapon);

	if (!ctx_t.enabled)
		return;

	static auto lastTime = 0.0f;
	static auto lastContact = 0.0f;

	const auto now = interfaces::globals->cur_time;

	lastContact = 0.0f;

	if (!GetAsyncKeyState(variables::triggerbot_bind))
		return;

	if (now - lastTime < ctx_t.delay / 1000.0f)
		return;

	const auto weaponData = activeWeapon->get_weapon_data();
	if (!weaponData)
		return;

	const auto aimPunch = csgo::local_player->aim_punch_angle();

	const vec3_t viewAngles{ std::cos(DEG2RAD(cmd->viewangles.x + aimPunch.x)) * std::cos(DEG2RAD(cmd->viewangles.y + aimPunch.y)) * weaponData->weapon_range,
							 std::cos(DEG2RAD(cmd->viewangles.x + aimPunch.x)) * std::sin(DEG2RAD(cmd->viewangles.y + aimPunch.y)) * weaponData->weapon_range,
							-std::sin(DEG2RAD(cmd->viewangles.x + aimPunch.x)) * weaponData->weapon_range };
	trace_t trace;
	ray_t ray;
	ray.initialize(csgo::local_player->get_eye_pos(), csgo::local_player->get_eye_pos() + viewAngles);
	trace_filter filter;
	filter.skip = csgo::local_player;
	interfaces::trace_ray->trace_ray(ray, 0x46004009, &filter, &trace);
	if (trace.entity && trace.entity->client_class()->class_id == class_ids::ccsplayer
		&& trace.entity->team() != csgo::local_player->team()
		&& !trace.entity->has_gun_game_immunity()
		&& (!ctx_t.hitpoint
			|| trace.hitGroup == ctx_t.hitpoint)
		&& (!utilities::goes_through_smoke(csgo::local_player->get_eye_pos(), csgo::local_player->get_eye_pos() + viewAngles))
		&& (!csgo::local_player->flash_duration())) {

		float damage = (activeWeapon->item_definition_index() != WEAPON_TASER ? get_damage_multiplier(trace.hitGroup) : 1.0f) * weaponData->weapon_damage * std::pow(weaponData->weapon_range_mod, trace.flFraction * weaponData->weapon_range / 500.0f);

		if (float armorRatio{ weaponData->weapon_armor_ratio / 2.0f }; activeWeapon->item_definition_index() != WEAPON_TASER && is_armored(trace.hitGroup, trace.entity->has_helmet()))
			damage -= (trace.entity->armor() < damage * armorRatio / 2.0f ? trace.entity->armor() * 4.0f : damage) * (1.0f - armorRatio);

		if (damage >= (trace.entity->health() < ctx_t.minimim_damage ? trace.entity->health() : ctx_t.minimim_damage) && ctx_t.ready && variables::trigger_magnet) {
			cmd->buttons |= in_attack;
			lastTime = 0.0f;
			lastContact = now;
		}
		else if (damage >= (trace.entity->health() < ctx_t.minimim_damage ? trace.entity->health() : ctx_t.minimim_damage)) {
			cmd->buttons |= in_attack;
			lastTime = 0.0f;
			lastContact = now;
		}
	}
	else {
		lastTime = now;
	}

}

int triggerbot::get_hitbox(int hitpoint)
{
	if (hitpoint != hitbox_head && hitpoint != hitbox_neck)
		return hitbox_chest;

	return hitpoint;
}
int triggerbot::get_hitbox(player_t* player, matrix_t* matrix, vec3_t eye_pos) {
	vec3_t view_angle;
	interfaces::engine->get_view_angles(view_angle);
	float max_fov = ctx_t.fov; // fov
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

void StopMovement(c_usercmd* cmd)
{
	if (csgo::local_player->move_type() != movetype_walk)
		return;

	vec3_t hvel = csgo::local_player->velocity();
	hvel.z = 0;
	float speed = hvel.length_2d();

	// Homework: Get these dynamically
	float accel = interfaces::console->get_convar("sv_accelerate")->get_float();
	float maxSpeed = interfaces::console->get_convar("sv_maxspeed")->get_float();
	float playerSurfaceFriction = 1.0f;
	float max_accelspeed = accel * interfaces::globals->interval_per_tick * maxSpeed * playerSurfaceFriction;

	float wishspeed{};

	// Only do custom deceleration if it won't end at zero when applying max_accel
	// Gamemovement truncates speed < 1 to 0
	if (speed - max_accelspeed <= -1.f)
	{
		wishspeed = max_accelspeed / (speed / (accel * interfaces::globals->interval_per_tick));
	}
	else // Full deceleration, since it won't overshoot
	{
		// Or use max_accelspeed, doesn't matter
		wishspeed = max_accelspeed;
	}

	vec3_t ndir{};
	math::vec3_t_angles((hvel * -1.f), ndir);
	ndir.y = cmd->viewangles.y - ndir.y;
	vec3_t vndir2;
	math::angle_vectors(ndir, vndir2);

	cmd->forwardmove = vndir2.x * wishspeed;
	cmd->sidemove = vndir2.y * wishspeed;
}

void triggerbot::magnet(c_usercmd* cmd)
{
	if (!csgo::local_player ||
		!csgo::local_player->is_alive())
		return;

	auto weapon = csgo::local_player->active_weapon();

	if (!weapon ||
		weapon->is_knife() ||
		weapon->is_nade() ||
		weapon->is_c4() ||
		weapon->is_taser())
		return;

	if (!weapon->clip1_count() || weapon->next_primary_attack() > interfaces::globals->cur_time)
		return;

	weapon_cfg(weapon);

	if (!variables::trigger_magnet)
		return;

	float max_player_fov = ctx_t.fov; // fov
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
			vec3_t temp_hitbox = player->get_hitbox_position((ctx_t.hitpoint == 3) ?
				get_hitbox(player, bone_matrix, eye_pos)
				: get_hitbox(ctx_t.hitpoint), bone_matrix);
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
					ctx_t.hitbox_pos = current_hitbox;
				}
			}
		}
	}
	if (closest_player != -1 && ctx_t.hitbox_pos.is_valid())
	{
		if (!GetAsyncKeyState(variables::triggerbot_bind))
			return;

		auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(closest_player));
		if (!player) return;

		// StopMovement(cmd);

		aim_angle = math::calculate_angle(eye_pos, ctx_t.hitbox_pos);
		math::sanitize_angle(aim_angle);
		vec3_t recoil_angle = csgo::local_player->aim_punch_angle() * interfaces::console->get_convar("weapon_recoil_scale")->get_float();
		math::sanitize_angle(recoil_angle);
		aim_angle -= recoil_angle;
		math::sanitize_angle(view_angle);
		vec3_t delta_angle = view_angle - aim_angle;
		math::sanitize_angle(delta_angle);
		float smoothing = (variables::trigger_smooth > 1.f) ? variables::trigger_smooth : 1.f;
		vec3_t final_angle = view_angle - delta_angle / smoothing;
		if (!math::sanitize_angle(final_angle))
			return;
		cmd->viewangles = final_angle;
		interfaces::engine->set_view_angles(cmd->viewangles);
		ctx_t.ready = true;
	}
}