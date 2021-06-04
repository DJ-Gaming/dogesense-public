#pragma once
#include "../../dependencies/utilities/csgo.hpp"
#include "../menu/variables.hpp"

#define tick_interval			(interfaces::globals->interval_per_tick)
#define time_to_ticks( dt )		( (int)( 0.5f + (float)(dt) / tick_interval ) )

namespace misc {
	namespace movement {
		void bunny_hop(c_usercmd* cmd);
		void edgeJump(c_usercmd* cmd);
		void auto_strafe(c_usercmd* cmd);
		void DoJumpBug(c_usercmd* cmd);
		void edgebug(c_usercmd* cmd);
	};
	namespace visual {
		void radar();
		void noflash();
	};
	void chatspam();
	void clantag();
	void fpsboost();
}

namespace visuals
{
	struct box {
		int x, y, w, h;
		box() = default;
		box(int x, int y, int w, int h) {
			this->x = x;
			this->y = y;
			this->w = w;
			this->h = h;
		}
	};

	bool get_player_box(player_t* ent, box& in);
	void draw_box(player_t* ent, box bbox);
	void draw_name(player_t* ent, box bbox);
	void draw_gun(player_t* entity, box bbox);
	void draw_ammo(player_t* entity, box bbox);
	void draw_hp(player_t* entity, box bbox);
	void spectator_list();
	void loop_entities();
	void nightmode();
	void watermark();
}

struct tick_info
{
	tick_info()
	{
		head_pos = vec3_t{};
		simulation_time = -1.f;
		bone_matrix_built = false;
	}
	tick_info(player_t* player)
	{
		head_pos = player->get_hitbox_position(hitbox_head);
		simulation_time = player->simulation_time();
		bone_matrix_built = false;
		if (player->setup_bones(bone_matrix, 128, BONE_USED_BY_ANYTHING, interfaces::globals->cur_time))
			bone_matrix_built = true;
	}
	vec3_t head_pos;
	float simulation_time;
	bool bone_matrix_built;
	matrix_t bone_matrix[128];
};

namespace backtracking {
	extern std::vector<tick_info> records[65];
	extern int closest_player;
	extern int closest_tick;
	bool is_tick_valid(float simulation_time, float backtrack_time);
	void store(c_usercmd* cmd);
	void run(c_usercmd* cmd);
}

namespace recoil {
	void rcs(c_usercmd* cmd);
}

namespace chams {
	void init();
	void shutdown();
	void run(hooks::draw_model_execute::fn ofunc, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix);
}

namespace glow {
	void run();
}

struct aimbot_ctx_t {
	int mode;
	int hitpoint;
	float smooth;
	float fov;
	vec3_t hitbox_pos;
};

namespace aimbot {
	extern aimbot_ctx_t ctx;
	void weapon_cfg(weapon_t* weapon);
	int get_hitbox(int hitpoint);
	int get_hitbox(player_t* player, matrix_t* matrix, vec3_t eye_pos);
	void run(c_usercmd* cmd);
}

namespace antiaim {
	void run(c_usercmd* cmd);
}

namespace skinchanger {
	void run();
	void animfix_hook();
	void animfix_unhook();
}

struct triggerbot_ctx {
	bool enabled = false;
	int hitpoint = 0;
	int delay = 0;
	int minimim_damage = 0;
	float burst_time = 0.f;
	vec3_t hitbox_pos;
	bool ready = false;
	int fov = 0;
};
namespace triggerbot {
	extern triggerbot_ctx ctx_t;
	void weapon_cfg(weapon_t* weapon);
	void run(c_usercmd* cmd);
	void magnet(c_usercmd* cmd);
	int get_hitbox(int hitpoint);
	int get_hitbox(player_t* player, matrix_t* matrix, vec3_t eye_pos);
}

namespace animations {
	void animation_fix(int stage);
	void manage_local_fake_animstate();
}

struct resolver_t {
	float final_feet_yaw[65];
	float old_feet_yaw[65];
	float eye_lby_delta[65];
};
extern resolver_t resolver_ctx;

namespace resolver {
	void resolver_yaw(int stage);
};

struct fake_state_t {
	bool m_should_update_fake = false;
	std::array< animation_layer, 13 > m_fake_layers;
	std::array< float, 24 > m_fake_poses;
	anim_state* m_fake_state = nullptr;
	float m_fake_rotation = 0.f;
	bool init_fake_anim = false;
	float m_fake_spawntime = 0.f;
	float m_fake_delta = 0.f;
	matrix_t m_fake_matrix[128];
	matrix_t m_fake_position_matrix[128];
	bool m_got_fake_matrix = false;
	float m_real_y_ang = 0.f;
};
extern fake_state_t fake_state;

/* Math Shit */

namespace math {

	

}