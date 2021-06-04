#include "../features.hpp"
#include  "engine_prediction.hpp"
#include  "../../../dependencies/xor.h"

#define D3DX_PI 3.1415926535897932384626

void misc::movement::bunny_hop(c_usercmd* cmd) {
	if (!variables::bhop)
		return;

	if (variables::jump_bug && GetAsyncKeyState(variables::jump_bug_key))
		return;

	const int move_type = csgo::local_player->move_type();

	if (move_type == movetype_ladder || move_type == movetype_noclip || move_type == movetype_observer)
		return;

	if (!(csgo::local_player->flags() & fl_onground))
		cmd->buttons &= ~in_jump;
};

void misc::movement::auto_strafe(c_usercmd* cmd) {

	if (!variables::auto_strafe)
		return;

	if ((csgo::local_player->flags() & fl_onground))
		return;

	static auto side = 1.f;
	side = -side;

	auto velocity = csgo::local_player->velocity();

	vec3_t wish_angle = cmd->viewangles;

	auto speed = velocity.length_2d();
	auto ideal_strafe = std::clamp(RAD2DEG(atan(15.f / speed)), 0.f, 90.f);

	cmd->forwardmove = 0.f;

	static auto cl_sidespeed =interfaces::console->get_convar(XorStr("cl_sidespeed"));

	static float old_y = 0.f;
	auto y_delta = std::remainderf(wish_angle.y - old_y, 360.f);
	auto abs_y_delta = abs(y_delta);
	old_y = wish_angle.y;

	const auto cl_sidespeed_value = cl_sidespeed->get_float();

	if (abs_y_delta <= ideal_strafe || abs_y_delta >= 30.f)
	{
		vec3_t velocity_direction;
		math::VectorAngles(velocity, velocity_direction);
		auto velocity_delta = std::remainderf(wish_angle.y - velocity_direction.y, 360.0f);
		auto retrack = std::clamp(RAD2DEG(atan(30.f / speed)), 0.f, 90.f) * 2.f;
		if (velocity_delta <= retrack || speed <= 15.f)
		{
			if (-retrack <= velocity_delta || speed <= 15.0f)
			{
				wish_angle.y += side * ideal_strafe;
				cmd->sidemove = cl_sidespeed_value * side;
			}
			else
			{
				wish_angle.y = velocity_direction.y - retrack;
				cmd->sidemove = cl_sidespeed_value;
			}
		}
		else
		{
			wish_angle.y = velocity_direction.y + retrack;
			cmd->sidemove = -cl_sidespeed_value;
		}

		math::CorrectMovement(cmd, wish_angle, cmd->viewangles);
	}
	else if (y_delta > 0.f)
		cmd->sidemove = -cl_sidespeed_value;
	else
		cmd->sidemove = cl_sidespeed_value;
}

bool unduck;

void misc::movement::DoJumpBug(c_usercmd* cmd)
{
	if (!variables::jump_bug || !GetAsyncKeyState(variables::jump_bug_key))
		return;

	if (!csgo::local_player || !csgo::local_player->is_alive())
		return;

	float max_radias = D3DX_PI * 2;
	float step = max_radias / 128; // normal is 128
	float xThick = 25;


	if (variables::jump_bug && GetAsyncKeyState(variables::jump_bug_key) && (csgo::local_player->flags() & (1 << 0)))
	{
		int screenWidth, screenHeight;
		interfaces::engine->get_screen_size(screenWidth, screenHeight);
		if (unduck)
		{
			cmd->buttons &= ~in_duck; // duck
			cmd->buttons |= in_jump; // jump
			unduck = false;
		}
		vec3_t pos = csgo::local_player->origin();
		for (float a = 0.f; a < max_radias; a += step)
		{
			vec3_t pt;
			pt.x = (xThick * cos(a)) + pos.x;
			pt.y = (xThick * sin(a)) + pos.y;
			pt.z = pos.z;


			vec3_t pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			ray_t ray;
			ray.initialize(pt, pt2);

			trace_filter flt;
			flt.skip = csgo::local_player;
			interfaces::trace_ray->trace_ray(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.flFraction != 1.f && fag.flFraction != 0.f)
			{
				cmd->buttons |= in_duck; // duck
				cmd->buttons &= ~in_jump; // jump
				unduck = true;
			}
		}
		for (float a = 0.f; a < max_radias; a += step)
		{
			vec3_t pt;
			pt.x = ((xThick - 2.f) * cos(a)) + pos.x;
			pt.y = ((xThick - 2.f) * sin(a)) + pos.y;
			pt.z = pos.z;

			vec3_t pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			ray_t ray;
			ray.initialize(pt, pt2);

			trace_filter flt;
			flt.skip = csgo::local_player;
			interfaces::trace_ray->trace_ray(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.flFraction != 1.f && fag.flFraction != 0.f)
			{
				cmd->buttons |= in_duck; // duck
				cmd->buttons &= ~in_jump; // jump
				unduck = true;
			}
		}
		for (float a = 0.f; a < max_radias; a += step)
		{
			vec3_t pt;
			pt.x = ((xThick - 20.f) * cos(a)) + pos.x;
			pt.y = ((xThick - 20.f) * sin(a)) + pos.y;
			pt.z = pos.z;

			vec3_t pt2 = pt;
			pt2.z -= 6;

			trace_t fag;

			ray_t ray;
			ray.initialize(pt, pt2);

			trace_filter flt;
			flt.skip = csgo::local_player;
			interfaces::trace_ray->trace_ray(ray, MASK_SOLID_BRUSHONLY, &flt, &fag);

			if (fag.flFraction != 1.f && fag.flFraction != 0.f)
			{
				cmd->buttons |= in_duck; // duck
				cmd->buttons &= ~in_jump; // jump
				unduck = true;
			}
		}
	}
}

void misc::movement::edgebug(c_usercmd* cmd)
{
	if (!variables::edge_bug || !GetAsyncKeyState(variables::edge_bug_key))
		return;

	if (variables::jump_bug == true && GetAsyncKeyState(variables::jump_bug_key))
		return;

	if (!csgo::local_player || !csgo::local_player->is_alive())
		return;

	if (!(prediction::getFlags() & fl_onground) && csgo::local_player->flags() & fl_onground)
		cmd->buttons |= in_duck;
}

void misc::movement::edgeJump(c_usercmd* cmd) {
	if (!(variables::edge_jump)) return;

	player_t* localPlayer = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));


	static int moveTipo = localPlayer->move_type();


	if (moveTipo == movetype_ladder) return;

	if ((prediction::getFlags() & 1) && !(localPlayer->flags() & 1))
		cmd->buttons |= in_jump;
}

void misc::visual::radar() {

	if (!variables::radar)
		return;

	for (int i = 1; i <= interfaces::globals->max_clients; i++) {

		entity_t* ent = reinterpret_cast<entity_t*>(interfaces::entity_list->get_client_entity(i));

		if (!ent)
			return;

		if (ent->team() != csgo::local_player->team() && ent->index() != csgo::local_player->index()) {
			ent->spotted() = true;
		}
	}
}

void misc::visual::noflash() {

	player_t* local = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (variables::noflash) {
		local->flash_alpha() = 0.f;
	}
	else {
		local->flash_alpha() = 255.f;
	}
}

void set_clantag(const char* tag)
{
	static auto ret = (int(__fastcall*)(const char*, const char*))utilities::pattern_scan(XorStr("engine.dll"), XorStr("53 56 57 8B DA 8B F9 FF 15"));

	ret(tag, tag);
}

void misc::clantag() {
	static bool reset = false;
	static float old_curtime = interfaces::globals->cur_time;

	if (!variables::clantag && reset == false) {
		reset = true;
		set_clantag(XorStr(" "));
	}
	else if (variables::clantag) {
		if (interfaces::globals->cur_time > old_curtime + 0.25f) {
			switch (int(old_curtime) % 19) {
			case 0:set_clantag(XorStr("          D")); break;
			case 1:set_clantag(XorStr("         Do")); break;
			case 2:set_clantag(XorStr("        Dog")); break;
			case 3:set_clantag(XorStr("       Doge")); break;
			case 4:set_clantag(XorStr("      DogeS")); break;
			case 5:set_clantag(XorStr("     DogeSe")); break;
			case 6:set_clantag(XorStr("    DogeSen")); break;
			case 7:set_clantag(XorStr("   DogeSens")); break;
			case 8:set_clantag(XorStr(" DogeSense")); break;
			case 9:set_clantag(XorStr("DogeSense ")); break;
			case 10:set_clantag(XorStr("ogeSense  ")); break;
			case 11:set_clantag(XorStr("geSense   ")); break;
			case 12:set_clantag(XorStr("eSense    ")); break;
			case 13:set_clantag(XorStr("Sense     ")); break;
			case 14:set_clantag(XorStr("ense      ")); break;
			case 15:set_clantag(XorStr("nse       ")); break;
			case 16:set_clantag(XorStr("se        ")); break;
			case 17:set_clantag(XorStr("e         ")); break;
			case 18:set_clantag(XorStr("          ")); break;
			case 19:set_clantag(XorStr("          ")); break;
			}
			old_curtime = interfaces::globals->cur_time;
		}
		reset = false;
	}
}

void misc::chatspam() {
	static float old_curtime = interfaces::globals->cur_time;
	if (variables::chatspam) {
		if (interfaces::globals->cur_time > old_curtime + 2.f) {
			interfaces::engine->execute_cmd(XorStr("say Get Good Get DogeSense"));
			old_curtime = interfaces::globals->cur_time;
		}
	}
}

bool once = false;

void misc::fpsboost() {
	static auto postprocess = interfaces::console->get_convar(XorStr("mat_postprocess_enable"));
	static auto blur = interfaces::console->get_convar(XorStr("@panorama_disable_blur"));
	static auto postprocess_enable = interfaces::console->get_convar("cl_csm_enabled");

	if (variables::fpsboost && !once) {
		postprocess->set_value(0);
		postprocess_enable->set_value(0);
		blur->set_value(1);
		once = true;
	}
	else if (once && !variables::fpsboost)
	{
		postprocess->set_value(1);
		postprocess_enable->set_value(1);
		blur->set_value(0);
		once = false;
	}
}