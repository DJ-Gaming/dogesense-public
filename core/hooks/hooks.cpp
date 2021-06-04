#include "../../dependencies/utilities/csgo.hpp"
#include "../features/features.hpp"
#include "../features/misc/engine_prediction.hpp"
#include "../menu/menu.hpp"
#include "../../dependencies/xor.h"

hooks::create_move::fn create_move_original = nullptr;
hooks::paint_traverse::fn paint_traverse_original = nullptr;
hooks::lock_cursor::fn lock_cursor_original = nullptr;
hooks::in_key_event::fn in_key_event_original = nullptr;
hooks::do_post_screen_effects::fn do_post_screen_effects_original = nullptr;
hooks::draw_model_execute::fn draw_model_execute_original = nullptr;
hooks::frame_stage_notify::fn frame_stage_notify_original = nullptr;
hooks::sv_cheats::fn sv_cheats_original = nullptr;
hooks::setup_bones::fn setup_bones_original = nullptr;
hooks::loosefiles::fn loosefiles_original = nullptr;
hooks::hkCheckFileCRCsWithServer::fn CheckFileCRCsWithServer_original = nullptr;

bool hooks::initialize() {
	const auto create_move_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 24));
	const auto paint_traverse_target = reinterpret_cast<void*>(get_virtual(interfaces::panel, 41));
	auto lock_cursor_target = reinterpret_cast<void*>(get_virtual(interfaces::surface, 67));
	auto in_key_event_target = reinterpret_cast<void*>(get_virtual(interfaces::client, 21));
	auto do_post_screen_effects_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 44));
	auto draw_model_execute_target = reinterpret_cast<void*>(get_virtual(interfaces::model_render, 21));
	auto frame_stage_notify_target = reinterpret_cast<void*>(get_virtual(interfaces::client, 37));
	auto viewmodel_fov_target = reinterpret_cast<void*>(get_virtual(interfaces::clientmode, 35));
	auto sv_cheats_target = reinterpret_cast<void*>(get_virtual(interfaces::console->get_convar(XorStr("sv_cheats")), 13));
	auto setup_bones_target = reinterpret_cast<void*>(utilities::pattern_scan(XorStr("client.dll"), XorStr("55 8B EC 83 E4 F8 8B 4D 08 BA ? ? ? ? E8 ? ? ? ? 85 C0 75 12")));
	auto g_pFileSystem = **reinterpret_cast<void***>(utilities::pattern_scan(XorStr("engine.dll"), XorStr("8B 0D ? ? ? ? 8D 95 ? ? ? ? 6A 00 C6")) + 0x2);
	auto svpureloosefiles_target = reinterpret_cast<void*>(get_virtual(g_pFileSystem, 128));
	auto CheckFileCRCsWithServer_target = reinterpret_cast<void*>(utilities::pattern_scan(XorStr("engine.dll"), XorStr("55 8B EC 81 EC ? ? ? ? 53 8B D9 89 5D F8 80")));

	if (MH_Initialize() != MH_OK)
		throw std::runtime_error(XorStr("failed to initialize MH_Initialize."));

	if (MH_CreateHook(create_move_target, &create_move::hook, reinterpret_cast<void**>(&create_move_original)) != MH_OK)
		throw std::runtime_error(XorStr("failed to initialize create_move. (outdated index?)"));

	if (MH_CreateHook(paint_traverse_target, &paint_traverse::hook, reinterpret_cast<void**>(&paint_traverse_original)) != MH_OK)
		throw std::runtime_error(XorStr("failed to initialize paint_traverse. (outdated index?)"));

	if (MH_CreateHook(lock_cursor_target, &lock_cursor::hook, reinterpret_cast<void**>(&lock_cursor_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize lock_cursor. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(in_key_event_target, &in_key_event::hook, reinterpret_cast<void**>(&in_key_event_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize in_key_event. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(do_post_screen_effects_target, &do_post_screen_effects::hook, reinterpret_cast<void**>(&do_post_screen_effects_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize do_post_screen_effects. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(draw_model_execute_target, &draw_model_execute::hook, reinterpret_cast<void**>(&draw_model_execute_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize draw_model_execute. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(frame_stage_notify_target, &frame_stage_notify::hook, reinterpret_cast<void**>(&frame_stage_notify_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize frame_stage_notify. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(viewmodel_fov_target, &viewmodel_fov::hook, NULL) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize viewmodel_fov_hook. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(sv_cheats_target, &sv_cheats::hook, reinterpret_cast<void**>(&sv_cheats_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize sv_cheats. (outdated index?)"));
		return false;
	}
	
	if (MH_CreateHook(setup_bones_target, &setup_bones::hook, reinterpret_cast<void**>(&setup_bones_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize setup_bones. (outdated index?)"));
		return false;
	}

	if (MH_CreateHook(svpureloosefiles_target, &loosefiles::hook, reinterpret_cast<void**>(&loosefiles_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize svpureloosefiles. (outdated index?)"));
		return false;
	}
	
	if (MH_CreateHook(CheckFileCRCsWithServer_target, &hkCheckFileCRCsWithServer::hook, reinterpret_cast<void**>(&CheckFileCRCsWithServer_original)) != MH_OK) {
		throw std::runtime_error(XorStr("failed to initialize CheckFileCRCsWithServer. (outdated index?)"));
		return false;
	}

	if (MH_EnableHook(MH_ALL_HOOKS) != MH_OK)
		throw std::runtime_error(XorStr("failed to enable hooks."));

	chams::init();
	skinchanger::animfix_hook();

	console::log(XorStr("[setup] hooks initialized!\n"));
	return true;
}

void hooks::release() {
	MH_Uninitialize();
	skinchanger::animfix_unhook();
	chams::shutdown();

	MH_DisableHook(MH_ALL_HOOKS);
}

#define sig_player_by_index "85 C9 7E 2A A1"
#define sig_draw_server_hitboxes "55 8B EC 81 EC ? ? ? ? 53 56 8B 35 ? ? ? ? 8B D9 57 8B CE"

void draw_server_hitboxes() {
	if (!interfaces::engine->is_connected() || !interfaces::engine->is_in_game())
		return;

	if (!csgo::local_player || !csgo::local_player->is_alive())
		return;

	if (!interfaces::input->m_fCameraInThirdPerson)
		return;

	auto get_player_by_index = [](int index) -> player_t* { //i dont need this shit func for anything else so it can be lambda
		typedef player_t* (__fastcall* player_by_index)(int);
		static auto player_index = reinterpret_cast<player_by_index>(utilities::pattern_scan(XorStr("server.dll"), sig_player_by_index));

		if (!player_index)
			return false;

		return player_index(index);
	};

	static auto fn = reinterpret_cast<uintptr_t>(utilities::pattern_scan(XorStr("server.dll"), sig_draw_server_hitboxes));
	auto duration = 1.f;
	PVOID entity = nullptr;

	entity = get_player_by_index(csgo::local_player->index());

	if (!entity)
		return;

	__asm {
		pushad
		movss xmm1, duration
		push 0 // 0 - colored, 1 - blue
		mov ecx, entity
		call fn
		popad
	}
}

bool __stdcall hooks::create_move::hook(float input_sample_frametime, c_usercmd* cmd) {
	create_move_original(input_sample_frametime, cmd);

	if (!cmd || !cmd->command_number)
		return false;

	csgo::local_player = static_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));
	csgo::cmd = cmd;

	uintptr_t* frame_pointer;
	__asm mov frame_pointer, ebp;
	csgo::send_packet = true;

	misc::movement::bunny_hop(cmd);

	misc::movement::auto_strafe(cmd);

	misc::clantag();

	misc::chatspam();

	misc::visual::noflash();

	recoil::rcs(cmd);

	if (variables::bullrush)
		cmd->buttons |= in_bullrush;

	prediction::start(cmd); {

		misc::movement::edgeJump(cmd);
		misc::movement::DoJumpBug(cmd);
		misc::movement::edgebug(cmd);

		auto old_viewangles = cmd->viewangles;
		auto old_forwardmove = cmd->forwardmove;
		auto old_sidemove = cmd->sidemove;
		backtracking::store(cmd);
		backtracking::run(cmd);
		aimbot::run(cmd);
		triggerbot::magnet(cmd);
		triggerbot::run(cmd);
		antiaim::run(cmd);
		math::correct_movement(old_viewangles, cmd, old_forwardmove, old_sidemove);
	} prediction::end();

	if (variables::nade) {
		interfaces::console->get_convar(XorStr("cl_grenadepreview"))->set_value(1);
	}
	else {
		interfaces::console->get_convar(XorStr("cl_grenadepreview"))->set_value(0);
	}

	static auto race = interfaces::console->get_convar(XorStr("r_skin"));

	switch (variables::race)
	{
	case 0:
		race->set_value(0); // default
		break;
	case 1:
		race->set_value(1); // who was in paris?
		break;
	case 2:
		race->set_value(2); // insert mexican joke here
		break;
	case 3:
		race->set_value(3); // force this to always on
		break;
	case 4:
		race->set_value(4); // insert indian joke here
		break;
	}

	cmd->forwardmove = std::clamp(cmd->forwardmove, -450.0f, 450.0f);
	cmd->sidemove = std::clamp(cmd->sidemove, -450.0f, 450.0f);
	cmd->upmove = std::clamp(cmd->upmove, -320.0f, 320.0f);
	/*
	if (variables::antiaim)
		animations::manage_local_fake_animstate();
	*/
	*(bool*)(*frame_pointer - 0x1C) = csgo::send_packet;

	cmd->viewangles.normalize();
	cmd->viewangles.x = std::clamp(cmd->viewangles.x, -89.0f, 89.0f);
	cmd->viewangles.y = std::clamp(cmd->viewangles.y, -180.0f, 180.0f);
	cmd->viewangles.z = 0.0f;

	return false;
}

void __stdcall hooks::paint_traverse::hook(unsigned int panel, bool force_repaint, bool allow_force) {
	auto panel_to_draw = fnv::hash(interfaces::panel->get_panel_name(panel));

	if (variables::removescope && strcmp(XorStr("HudZoom"), interfaces::panel->get_panel_name(panel)) == 0)
		return;

	misc::fpsboost();

	switch (panel_to_draw) {
	case fnv::hash("MatSystemTopPanel"):

		if (variables::watermark)
			visuals::watermark(); // sexy

		//menu::toggle();
		menu::render();
		visuals::loop_entities();

		int screen_x, screen_y;

		interfaces::engine->get_screen_size(screen_x, screen_y);

		if (interfaces::engine->is_in_game() && interfaces::engine->is_connected() && csgo::local_player) {
			weapon_t* Weapon = csgo::local_player->active_weapon();

			if (Weapon && variables::removescope && (Weapon->is_sniper() || Weapon->is_auto()) && csgo::local_player->is_scoped() && csgo::local_player)
			{
				render::draw_line(screen_x / 2, 0, screen_x / 2, screen_y, color(0, 0, 0, 150));
				render::draw_line(0, screen_y / 2, screen_x, screen_y / 2, color(0, 0, 0, 150));
			}

			vec3_t ViewAngles;
			interfaces::engine->get_view_angles(ViewAngles);
			ViewAngles += (csgo::local_player->aim_punch_angle() * 2.f);

			vec3_t fowardVec;
			math::angle_vectors(ViewAngles, fowardVec);
			fowardVec *= 10000;

			vec3_t start = csgo::local_player->get_eye_pos();
			vec3_t end = start + fowardVec, endScreen;

			if (interfaces::debug_overlay->world_to_screen(end, endScreen) && csgo::local_player->is_alive() && variables::recoil_crosshair)
			{
				render::draw_line(endScreen.x - 5, endScreen.y, endScreen.x + 5, endScreen.y, color(255, 255, 255, 255));
				render::draw_line(endScreen.x, endScreen.y - 5, endScreen.x, endScreen.y + 5, color(255, 255, 255, 255));
			}
		}

		if (variables::velocity_toggles) {

			player_t* local = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

			if (!local)
				return;

			int vel = local->velocity().length_2d();

			color Velocity = color(255, 255, 255);

			if (vel >= 100)
				Velocity = color(255, 255, 59);

			if (vel >= 300)
				Velocity = color(0, 255, 59);

			render::text(screen_x / 2, screen_y / 2 + (screen_y / 2.5), render::fonts::velocity_font, std::to_string(vel), false, Velocity);

			int add = 0;

			if (GetAsyncKeyState(variables::jump_bug_key) && variables::jump_bug) {
				render::text(screen_x / 2, screen_y / 2 + (screen_y / 2.8), render::fonts::velocity_font, XorStr("jb"), false, color::white(255));
				add = 20;
			}
			if (GetAsyncKeyState(variables::edge_bug_key) && variables::edge_bug)
				render::text(screen_x / 2, screen_y / 2 + (screen_y / 2.8) + add, render::fonts::velocity_font, XorStr("eb"), false, color::white(255));
		}

		break;
	}
	paint_traverse_original(interfaces::panel, panel, force_repaint, allow_force);
}

void __stdcall hooks::lock_cursor::hook() {
	if (variables::menu::opened) {
		interfaces::surface->unlock_cursor();
		interfaces::inputsystem->enable_input(false);
		return;
	}
	else {
		interfaces::inputsystem->enable_input(true);
	}

	return lock_cursor_original(interfaces::surface);
}

int __fastcall hooks::in_key_event::hook(void* ecx, int edx, int event_code, int key_num, const char* current_binding) {
	if (variables::menu::opened)
		return 0;

	return in_key_event_original(event_code, key_num, current_binding);
}

bool __stdcall hooks::do_post_screen_effects::hook(int value) {

	//TODO: Move to visuals class
	static auto disablePostProcessing = *(bool**)(utilities::pattern_scan(XorStr("client.dll"), XorStr("83 EC 4C 80 3D")) + 5);
	*disablePostProcessing = variables::post;

	if (interfaces::engine->is_in_game() && interfaces::engine->is_connected() && csgo::local_player && variables::glow) {
		glow::run();
	}

	return do_post_screen_effects_original(interfaces::clientmode, value);
}

void __stdcall hooks::draw_model_execute::hook(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix) {
	if (interfaces::model_render->is_material_overriden() &&
		!strstr(info.model->name, XorStr("arms")) &&
		!strstr(info.model->name, XorStr("weapons/v_"))) {
		return draw_model_execute_original(interfaces::model_render, ctx, state, info, matrix);
	}
	chams::run(draw_model_execute_original, ctx, state, info, matrix);
}

void __stdcall hooks::frame_stage_notify::hook(int frame_stage) {
	static int nightmode_value = 0;
	if (!interfaces::engine->is_in_game() || !interfaces::engine->is_connected()) {
		frame_stage_notify_original(interfaces::client, frame_stage);
		nightmode_value = 0;
		return;
	}

	animations::animation_fix(frame_stage);
	resolver::resolver_yaw(frame_stage);

	if (frame_stage == FRAME_RENDER_START && csgo::local_player) {

		static bool enabledtp = false, check = false;
		if (variables::thirdperson) {

			if (GetAsyncKeyState(variables::thirdperson_key) && csgo::local_player && csgo::local_player->is_alive())
			{
				if (!check)
					enabledtp = !enabledtp;
				check = true;
			}
			else
				check = false;

			if (csgo::local_player && csgo::local_player->is_alive())
			{
				if (enabledtp)
				{
					*(bool*)((DWORD)interfaces::input + 0xAD) = true;
					*(float*)((DWORD)interfaces::input + 0xA8 + 0x8 + 0x8) = 150.f;
				}
				else
				{
					*(bool*)((DWORD)interfaces::input + 0xAD) = false;
					*(float*)((DWORD)interfaces::input + 0xA8 + 0x8 + 0x8) = 0.f;
				}
			}
			else
			{
				*(bool*)((DWORD)interfaces::input + 0xAD) = false;
				*(float*)((DWORD)interfaces::input + 0xA8 + 0x8 + 0x8) = 0.f;
			}
		}
		else {
			*(bool*)((DWORD)interfaces::input + 0xAD) = false;
			*(float*)((DWORD)interfaces::input + 0xA8 + 0x8 + 0x8) = 0.f;
		}
	}
	if (frame_stage == FRAME_NET_UPDATE_POSTDATAUPDATE_START) {
		if (variables::skinchanger) {
			skinchanger::run();
		}
	}
	if (nightmode_value != variables::nightmode) {
		visuals::nightmode();
		nightmode_value = variables::nightmode;
	}
	frame_stage_notify_original(interfaces::client, frame_stage);
}

float __stdcall hooks::viewmodel_fov::hook() {
	return 68 + float(std::clamp(variables::viewmodel_fov, 0.f, 30.f));
}

bool __fastcall hooks::sv_cheats::hook(PVOID convar, int edx) {
	if (!convar)
		return false;

	static DWORD CAM_THINK = (DWORD)utilities::pattern_scan(XorStr("client.dll"), XorStr("85 C0 75 30 38 86"));

	if (variables::thirdperson) {
		if ((DWORD)_ReturnAddress() == CAM_THINK)
			return true;
	}

	if (!sv_cheats_original(convar))
		return false;

	return sv_cheats_original(convar);
}

bool __fastcall hooks::setup_bones::hook(void* ecx, void* edx, matrix_t* bone_to_world_out, int max_bones, int bone_mask, float curtime) {
	const auto player = (player_t*)((uintptr_t)ecx - 0x4);
	if (player && player->client_class()->class_id == ccsplayer && !player->dormant()) {
		if (player->index() == interfaces::engine->get_local_player()) {
			const auto backup_effects = *(uint32_t*)((uintptr_t)player + 0xF0);
			const auto backup_shit = *(uint32_t*)((uintptr_t)player + 0xA68);
			*(uint32_t*)((uintptr_t)player + 0xA68) = 0;
			*(uint32_t*)((uintptr_t)player + 0xF0) |= 0x8;
			return setup_bones_original(ecx, bone_to_world_out, max_bones, bone_mask, curtime);
			*(uint32_t*)((uintptr_t)player + 0xF0) = backup_effects;
			*(uint32_t*)((uintptr_t)player + 0xA68) = backup_shit;
		}
	}
	return setup_bones_original(ecx, bone_to_world_out, max_bones, bone_mask, curtime);
}

bool __fastcall hooks::loosefiles::hook(void* ecx, void* edx)
{
	return true;
}

void __fastcall hooks::hkCheckFileCRCsWithServer::hook(void* ecx, void* edx)
{
	return;
}