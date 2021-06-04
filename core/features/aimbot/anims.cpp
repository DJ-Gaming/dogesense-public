#include "../features.hpp"

void animations::animation_fix(int stage) {
	
	static float proper_abs = 0.f;

	if (interfaces::engine->is_in_game() && interfaces::engine->is_connected() && stage == FRAME_RENDER_START && csgo::cmd && csgo::local_player != nullptr && csgo::local_player->is_alive()) {
		auto state = csgo::local_player->get_anim_state();
		auto layers = csgo::local_player->anim_overlays();
		if (state && layers) {
			auto fresh_tick = []() -> bool {
				static int old_tick = 0;
				if (old_tick != interfaces::globals->tick_count) {
					old_tick = interfaces::globals->tick_count;
					return true;
				}
				return false;
			};

			static animation_layer backup_layers[15];
			static auto sent_pose_params = csgo::local_player->pose_param();
			if (fresh_tick())
			{
				std::memcpy(backup_layers, layers, (sizeof(animation_layer) * 15));
				csgo::local_player->animations(true);
				csgo::local_player->update_state(state, csgo::cmd->viewangles);
				if (csgo::send_packet)
				{
					proper_abs = csgo::local_player->abs_angles().y;
					sent_pose_params = csgo::local_player->pose_param();
				}
			}
			csgo::local_player->animations(false);
			csgo::local_player->setup_bones(nullptr, 128, BONE_USED_BY_ANYTHING, interfaces::globals->cur_time);
			csgo::local_player->invalidate_bone_cache();
			csgo::local_player->update_client_side_animations();
			csgo::local_player->set_abs_angles(vec3_t(0, proper_abs, 0));
			std::memcpy(layers, backup_layers, (sizeof(animation_layer) * 15));
			csgo::local_player->pose_param() = sent_pose_params;
		}
		if (interfaces::clientstate->delta_tick != -1 && !interfaces::input->m_fCameraInThirdPerson)
			csgo::local_player->animations(true);
	}

	if (interfaces::engine->is_in_game() && interfaces::engine->is_connected() && stage == FRAME_RENDER_START && csgo::cmd && csgo::local_player != nullptr && csgo::local_player->is_alive()) {
	}
}

fake_state_t fake_state;
void animations::manage_local_fake_animstate()
{
	if (!csgo::local_player || !csgo::local_player->is_alive() || !csgo::cmd)
		return;

	if (fake_state.m_fake_spawntime != csgo::local_player->spawn_time() || fake_state.m_should_update_fake)
	{
		fake_state.init_fake_anim = false;
		fake_state.m_fake_spawntime = csgo::local_player->spawn_time();
		fake_state.m_should_update_fake = false;
	}

	if (!fake_state.init_fake_anim)
	{
		fake_state.m_fake_state = static_cast<anim_state*> (interfaces::mem_alloc->alloc(sizeof(anim_state)));

		if (fake_state.m_fake_state != nullptr)
			csgo::local_player->create_state(fake_state.m_fake_state);

		fake_state.init_fake_anim = true;
	}

	if (csgo::send_packet)
	{
		std::array<animation_layer, 15> networked_layers;
		std::memcpy(&networked_layers, csgo::local_player->anim_overlays(), sizeof(animation_layer) * 15);
		auto backup_poses = csgo::local_player->pose_param();
		auto backup_abs_angles = csgo::local_player->abs_angles();

		csgo::local_player->update_state(fake_state.m_fake_state, csgo::cmd->viewangles);
		fake_state.m_fake_rotation = reinterpret_cast<anim_state*> (fake_state.m_fake_state)->m_flGoalFeety;
		csgo::local_player->set_abs_angles(vec3_t(0, fake_state.m_fake_rotation, 0));
		csgo::local_player->invalidate_bone_cache();
		fake_state.m_got_fake_matrix = csgo::local_player->setup_bones(fake_state.m_fake_matrix, 128, BONE_USED_BY_ANYTHING & ~BONE_USED_BY_ATTACHMENT, interfaces::globals->cur_time);
		const auto org_tmp = csgo::local_player->renderable()->get_render_origin();
		if (fake_state.m_got_fake_matrix)
		{
			for (auto& i : fake_state.m_fake_matrix)
			{
				i[0][3] -= org_tmp.x;
				i[1][3] -= org_tmp.y;
				i[2][3] -= org_tmp.z;
			}
		}
		csgo::local_player->set_abs_angles(backup_abs_angles);
		std::memcpy(csgo::local_player->anim_overlays(), &networked_layers, sizeof(animation_layer) * 15);
		csgo::local_player->pose_param() = backup_poses;
	}
}

/* Thanks UC <3 */

resolver_t resolver_ctx;

void resolver::resolver_yaw(int stage) {

	if (!interfaces::engine->is_in_game() || !interfaces::engine->is_connected() || stage != FRAME_NET_UPDATE_END || !variables::resolver)
		return;

	// looping on the weekend like usual
	for (int i = 1; i <= interfaces::globals->max_clients; i++) {
		auto entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

		if (!entity
			|| !csgo::local_player
			|| !entity->is_alive()
			|| entity == csgo::local_player
			|| entity->team() == csgo::local_player->team())
			continue;

		player_info_t player_info;
		interfaces::engine->get_player_info(entity->index(), &player_info);

		if (player_info.fakeplayer) // skip bots
			continue;

		auto anim_state = entity->get_anim_state();
		if (!anim_state)
			return;

		// dont forget to include fakelag checks, 
		// some p2cs require up to 3 ticks in order to lby desync,
		// but you can do it with just 1

		// take their feetyaw and normaclampalize for extra safety
		resolver_ctx.final_feet_yaw[entity->index()] = std::clamp( math::NormalizeAngleFloat(anim_state->m_flGoalFeety) , -180.f, 180.f);

		// can be used in the future if u want
		resolver_ctx.old_feet_yaw[entity->index()] = std::clamp( math::NormalizeAngleFloat(resolver_ctx.old_feet_yaw[entity->index()]), -180.f, 180.f);

		// calculate their lby delta men!
		resolver_ctx.eye_lby_delta[entity->index()] = math::NormalizeAngleFloat(entity->eye_angles().y - entity->lower_body_y());

		// make sure their lby is broken and their standing. once again p2cs can do things differently,
		// they usually jitter their speed between 0.0f and 1.1f so make sure we are above that interval.
		// Other cheats that actually flick to break lby can be checked with 0.0f speed
		if (fabs(resolver_ctx.eye_lby_delta[entity->index()]) > 35.f && entity->velocity().length_2d() <= 2.f /* && choking[entity->index()] */)
		{
			// now we go, clamped their feetyaw to max range using the opposite of their lby delta, 
			// pretty basic, but its really all you gotta do
			resolver_ctx.final_feet_yaw[entity->index()] += std::clamp(resolver_ctx.eye_lby_delta[entity->index()], -58.f, 58.f);

			// maybe we can save set this old_feet_yaw for future usage,
			// possibly saving their standing desync angle and using it while moving afterwards
			resolver_ctx.old_feet_yaw[entity->index()] = resolver_ctx.final_feet_yaw[entity->index()];
		}

		// finally set their feet_yaw
		anim_state->m_flGoalFeety = resolver_ctx.final_feet_yaw[entity->index()];
	}

}