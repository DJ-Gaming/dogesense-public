#pragma once

namespace hooks {
	bool initialize();
	void release();

	inline unsigned int get_virtual(void* _class, unsigned int index) { return static_cast<unsigned int>((*static_cast<int**>(_class))[index]); }

	namespace create_move {
		using fn = bool(__stdcall*)(float, c_usercmd*);
		bool __stdcall hook(float input_sample_frametime, c_usercmd* cmd);
	};

	namespace paint_traverse {
		using fn = void(__thiscall*)(i_panel*, unsigned int, bool, bool);
		void __stdcall hook(unsigned int panel, bool force_repaint, bool allow_force);
	}

	namespace lock_cursor {
		using fn = void(__thiscall*)(void*);
		void __stdcall hook();
	}

	namespace in_key_event {
		using fn = int(__stdcall*)(int, int, const char*);
		int __fastcall hook(void* ecx, int edx, int event_code, int key_num, const char* current_binding);
	}

	namespace do_post_screen_effects {
		using fn = bool(__thiscall*)(i_client_mode*, int);
		bool __stdcall hook(int value);
	}

	namespace draw_model_execute {
		using fn = void(__thiscall*)(iv_model_render*, i_mat_render_context*, const draw_model_state_t&, const model_render_info_t&, matrix_t*);
		void __stdcall hook(i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix);
	}

	namespace frame_stage_notify {
		using fn = void(__thiscall*)(i_base_client_dll*, int);
		void __stdcall hook(int frame_stage);
	}

	namespace viewmodel_fov {
		float __stdcall hook();
	}

	namespace sv_cheats {
		using fn = bool(__thiscall*)(void*);
		bool __fastcall hook(PVOID convar, int edx);
	}
	
	namespace setup_bones {
		using fn = bool(__thiscall*) (void*, matrix_t*, int, int, float);
		bool __fastcall hook(void* ecx, void* edx, matrix_t* bone_to_world_out, int max_bones, int bone_mask, float curtime);
	}

	namespace loosefiles {
		using fn = bool(__thiscall*) (void*);
		bool __fastcall hook(void* ecx, void* edx);
	}

	namespace hkCheckFileCRCsWithServer {
		using fn = void(__thiscall*) (void*);
		void __fastcall hook(void* ecx, void* edx);
	}
}
