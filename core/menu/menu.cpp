#include "menu.hpp"
#include "../../dependencies/xor.h"
#include "../features/skinchanger/skin_list.h"

std::vector<const char*> search_results = { "Skins" };
void skin_input(std::string& search_str, int& skinid, const char* control_name) {
	zgui::text_input(control_name, search_str, 5);
	if (search_str.size() > 0) {
		std::string::size_type sz;
		int i_dec = std::stoi(search_str, &sz);
		if (i_dec >= 0 && i_dec <= 920)
			skinid = i_dec;
	}
}

void menu::render()
{
	zgui::poll_input("Counter-Strike: Global Offensive");

	static int MenuTab = 1;
	static int VisualsTab = 0;
	static int RageTab = 0;
	static int SkinTab = 0;
	variables::menu::opened = false;
	if (zgui::begin_window("", { 405, 398 }, render::fonts::watermark_font)) // 500, 365
	{
		variables::menu::opened = true;

		zgui::begin_groupbox(XorStr("DogeSense"), { 385, 42 });
		{
			if (zgui::tab_button(XorStr("Legitbot#tab"), { 80, 20 })) MenuTab = 1;
			zgui::next_column(100, 17.5);
			if (zgui::tab_button(XorStr("Visuals#tab"), { 80, 20 })) MenuTab = 2;
			if (MenuTab == 2)
			{
				VisualsTab = 0;
			}
			zgui::next_column(200, 17.5);
			if (zgui::tab_button(XorStr("Misc#tab"), { 80, 20 })) MenuTab = 3;
			zgui::next_column(300, 17.5);
			if (zgui::tab_button(XorStr("Skins#tab"), { 80, 20 })) MenuTab = 4;
			if (MenuTab == 4)
			{
				SkinTab = 0;
			}
			zgui::end_groupbox();
		}
		zgui::next_column(0, 45); // 95 , 0
		if (MenuTab == 0)
		{
			if (RageTab == 0) {

				zgui::begin_groupbox(XorStr("Aimbot"), { 190,150 });
				{
					zgui::end_groupbox();
				}

				zgui::begin_groupbox(XorStr("Autosniper Settings"), { 190,48 });
				{
					zgui::end_groupbox();
				}
				zgui::begin_groupbox(XorStr("Scout Settings"), { 190,48 });
				{
					zgui::end_groupbox();
				}
				zgui::begin_groupbox(XorStr("AWP Settings"), { 190,48 });
				{
					zgui::end_groupbox();
				}
				zgui::next_column(289, 0);
				zgui::begin_groupbox(XorStr("Pistol Settings"), { 190,48 });
				{
					zgui::end_groupbox();
				}
				zgui::begin_groupbox(XorStr("Deagle Settings"), { 190,48 });
				{
					zgui::end_groupbox();
				}
				zgui::begin_groupbox(XorStr("Other Settings"), { 190,48 });
				{
					zgui::end_groupbox();
				}
			}
			else if (RageTab == 1) {
				zgui::begin_groupbox(XorStr("Antiaim"), { 191,205 });
				{
					zgui::end_groupbox();
				}
			}
		}
		else if (MenuTab == 1)
		{
			zgui::begin_groupbox(XorStr("Aimbot"), { 190, 325 });
			{
				zgui::combobox(XorStr("Aimbot Selection"), { XorStr("Rifles"), XorStr("Pistols"), XorStr("Snipers") }, variables::weapon_config);
				zgui::checkbox(XorStr("Aimbot"), variables::aimbot); // 175
				switch (variables::weapon_config) {
				case 0: {
					zgui::slider_float(XorStr("Aimbot FOV"), 0.f, 10.f, variables::aim_fov_rifle);
					zgui::slider_float(XorStr("Aimbot Smooth"), 0.f, 30.f, variables::smooth_rifle);
					break;
				}
				case 1: {
					zgui::slider_float(XorStr("Aimbot FOV"), 0.f, 10.f, variables::aim_fov_pistol);
					zgui::slider_float(XorStr("Aimbot Smooth"), 0.f, 30.f, variables::smooth_pistol);
					break;
				}
				case 2: {
					zgui::slider_float(XorStr("Aimbot FOV"), 0.f, 10.f, variables::aim_fov_sniper);
					zgui::slider_float(XorStr("Aimbot Smooth"), 0.f, 30.f, variables::smooth_sniper);
					break;
				}
				}
				zgui::checkbox(XorStr("Backtrack"), variables::backtrack);
				zgui::checkbox(XorStr("Recoil Control System"), variables::rcs);
				zgui::checkbox(XorStr("AntiAim"), variables::antiaim);

				const std::vector<zgui::multi_select_item> AACombo{ {XorStr("LBY"), &variables::lby},
				{ XorStr("Micromovements"), &variables::micro } };

				zgui::multi_combobox(XorStr("AA Additions"), AACombo);
			}

			zgui::next_column(195, 45);

			zgui::begin_groupbox(XorStr("Triggerbot"), { 190, 325 });
			{
				zgui::checkbox(XorStr("Triggerbot"), variables::triggerbot);
				zgui::key_bind(XorStr("Triggerbot Bind"), variables::triggerbot_bind);
				zgui::slider_float(XorStr("Delay"), 0.f, 300.f, variables::trigger_delay);
				zgui::slider_float(XorStr("Min Damage"), 0.f, 100.f, variables::trigger_min_damage);
				zgui::checkbox(XorStr("Magnet Trigger"), variables::trigger_magnet);
				if (variables::trigger_magnet) {
					zgui::slider_float(XorStr("FOV"), 0.f, 90.f, variables::trigger_fov);
					zgui::slider_float(XorStr("Smooth"), 0.f, 30.f, variables::trigger_smooth);
				}
				zgui::end_groupbox();
			}
			zgui::end_groupbox();
		}
		else if (MenuTab == 2)
		{
			if (VisualsTab == 0)
			{
				zgui::begin_groupbox(XorStr("Enemies"), { 190, 325 });
				{
					zgui::checkbox(XorStr("Chams"), variables::chams);
					zgui::colorpicker(XorStr("Chams Color"), variables::Chams_Color);
					zgui::checkbox(XorStr("Ignore Z"), variables::ignore_z);
					zgui::colorpicker(XorStr("Chams Z Color"), variables::Chams_Color_Z);
					zgui::combobox("Chams Material", { XorStr("Material"), XorStr("Flat"), XorStr("Bark"), XorStr("Glossy") }, variables::chams_material);
					zgui::slider_float(XorStr("Transparency"), 0.f, 100.f, variables::transparency);
					zgui::checkbox(XorStr("Model Glow"), variables::glow_over);
					zgui::colorpicker(XorStr("Model Glow Color"), variables::Chams_Color_G);
					zgui::checkbox(XorStr("Glow"), variables::glow);
					zgui::colorpicker(XorStr("Glow Color"), variables::Glow_Color);
					zgui::slider_float(XorStr("Glow Bloom"), 0.f, 100.f, variables::Glow_A);
					zgui::checkbox(XorStr("ESP"), variables::esp);
					zgui::checkbox(XorStr("Box ESP"), variables::box_esp);
					zgui::checkbox(XorStr("Name ESP"), variables::name_esp);
					zgui::checkbox(XorStr("Ammo ESP"), variables::ammo_esp);
					zgui::checkbox(XorStr("Health ESP"), variables::hp_esp);
					zgui::checkbox(XorStr("Weapon ESP"), variables::weapon_esp);
					zgui::checkbox(XorStr("Radar Reveal"), variables::radar);
					zgui::end_groupbox();
				}

				zgui::next_column(195, 45);

				zgui::begin_groupbox(XorStr("Localplayer"), { 191, 240.75 });
				{
					zgui::checkbox(XorStr("Thirdperson"), variables::thirdperson);
					zgui::key_bind(XorStr("Thirdperson Key"), variables::thirdperson_key);
					zgui::checkbox(XorStr("Arm Chams"), variables::chams_arms);
					zgui::slider_float(XorStr("Transparency#arm"), 0.f, 100.f, variables::transparency_arm);
					zgui::combobox(XorStr("Chams Material#arm"), { XorStr("Normal"), XorStr("Glass"), XorStr("Air"), XorStr("Crysal") }, variables::chams_arms_mat);
					zgui::colorpicker(XorStr("Arm Chams Color"), variables::Chams_Color_Arm);
					zgui::checkbox(XorStr("Model Glow#arm"), variables::glow_over_arm);
					zgui::colorpicker(XorStr("Model Glow Color#arm"), variables::Chams_Color_G_Arm);
					zgui::checkbox(XorStr("Wireframe"), variables::chams_arms_wire);
					zgui::checkbox(XorStr("Remove Scope"), variables::removescope);
					zgui::checkbox(XorStr("Recoil Crosshair"), variables::recoil_crosshair);
					zgui::end_groupbox();
				}

				zgui::begin_groupbox(XorStr("Misc"), { 191, 81.25 });
				{
					zgui::checkbox(XorStr("Backtrack"), variables::backtrack_chams);
					zgui::colorpicker(XorStr("Chams Color#bt"), variables::Chams_Color_BT);
					zgui::end_groupbox();
				}
			}
			else if (VisualsTab == 1)
			{
				zgui::begin_groupbox(XorStr("World"), { 190, 325 });
				{
					zgui::end_groupbox();
				}
				zgui::next_column(289, 0);

				zgui::begin_groupbox(XorStr("Misc"), { 191, 325 });
				{
					zgui::end_groupbox();
				}
			}
		}
		else if (MenuTab == 3)
		{
			zgui::begin_groupbox(XorStr("Misc"), { 190, 325 });
			{
				zgui::checkbox(XorStr("No Flash"), variables::noflash); // 175
				zgui::checkbox(XorStr("FPS Boost"), variables::fpsboost); // 175
				zgui::slider_float(XorStr("Nightmode Value"), 0.f, 100.f, variables::nightmode); // -101.f
				zgui::checkbox(XorStr("Grenade Prediction"), variables::nade); // 175
				zgui::checkbox(XorStr("Disable Post-Processing"), variables::post); // 175
				zgui::slider_float(XorStr("Viewmodel FOV"), 0.f, 30.f, variables::viewmodel_fov); // -101.f
				zgui::checkbox(XorStr("Watermark"), variables::watermark); // 175
				if (variables::watermark)
					zgui::colorpicker(XorStr("Watermark Color"), variables::Watermark_Color);
				zgui::checkbox(XorStr("Chatspam"), variables::chatspam); // 175
				zgui::checkbox(XorStr("Clantag Changer"), variables::clantag); // 175
				zgui::checkbox(XorStr("Spectator List"), variables::spectator_list); // 175
				zgui::checkbox(XorStr("Resolver"), variables::resolver); // 175
				zgui::end_groupbox();
			}
			zgui::next_column(195, 45);

			zgui::begin_groupbox("Movement", { 191, 325 });
			{
				zgui::checkbox("Infinite Duck", variables::bullrush); // 175
				zgui::checkbox("Bunnyhop", variables::bhop); // 175
				zgui::checkbox("Jump Bug", variables::jump_bug); // 175
				zgui::key_bind("Jump Bug Key", variables::jump_bug_key);
				zgui::checkbox("Edge Bug", variables::edge_bug); // 175
				zgui::key_bind("Edge Bug Key", variables::edge_bug_key);
				zgui::checkbox("Edge Jump", variables::edge_jump); // 175
				zgui::checkbox("Auto Strafer", variables::auto_strafe); // 175
				zgui::checkbox("Show Velocity & Toggles", variables::velocity_toggles); // 175
				zgui::end_groupbox();
			}
		}
		else if (MenuTab == 4)
		{
		
			if (SkinTab == 0) {
				zgui::begin_groupbox("Skinchanger Main", { 385,325 });
				{
					zgui::combobox("Race", { "Default", "Black", "Mixed", "White", "Tan" }, variables::race);
					zgui::checkbox("Enabled#skinchanger", variables::skinchanger);
					if (zgui::button("Force Update", { 120,14 }))
						interfaces::clientstate->full_update();
					zgui::combobox("Knife Model", { "Default", "Bayonet", "Flip", "Gut Knife", "Karambit", "M9 Bayonet", "Huntsman Knife", "Falchion Knife", "Bowie Knife", "Butterfly Knife", "Shadow Daggers", "Ursus Knife", "Navaja Knife", "Stiletto Knife", "Talon Knife", "Nomad Knife", "Skeleton Knife", "Survival Knife", "Paracord Knife", "Classic Knife" }, variables::knife_model);
					switch (variables::knife_model)
					{
					case 1:
						zgui::combobox("Bayonet Skin", { "Vanilla", "Lore", "Gamma Doppler", "Autotronic", "Black Laminate", "Freehand", "Bright Water", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 2:
						zgui::combobox("Flip Knife Skin", { "Vanilla", "Lore", "Gamma Doppler", "Autotronic", "Black Laminate", "Freehand", "Bright Water", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 3:
						zgui::combobox("Gut Knife Skin", { "Vanilla", "Lore", "Gamma Doppler", "Autotronic", "Black Laminate", "Freehand", "Bright Water", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 4:
						zgui::combobox("Karambit Skin", { "Vanilla", "Lore", "Gamma Doppler", "Autotronic", "Black Laminate", "Freehand", "Bright Water", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 5:
						zgui::combobox("M9 Bayonet Skin", { "Vanilla", "Lore", "Gamma Doppler", "Autotronic", "Black Laminate", "Freehand", "Bright Water", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 6:
						zgui::combobox("Huntsman Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 7:
						zgui::combobox("Falchion Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 8:
						zgui::combobox("Bowie Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 9:
						zgui::combobox("Butterfly Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 10:
						zgui::combobox("Shadow Daggers Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 11:
						zgui::combobox("Ursus Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 12:
						zgui::combobox("Navaja Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 13:
						zgui::combobox("Stiletto Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 14:
						zgui::combobox("Talon Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 15:
						zgui::combobox("Nomad Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 16:
						zgui::combobox("Skeleton Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 17:
						zgui::combobox("Survival Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 18:
						zgui::combobox("Paracord Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					case 19:
						zgui::combobox("Classic Knife Skin", { "Vanilla", "Marble Fade", "Tiger Tooth", "Doppler", "Ultraviolet", "Damascus Steel", "Rust Coat", "Fade", "Slaughter", "Case Hardened", "Crimson Web", "Blue Steel", "Night", "Safari Mesh", "Forest DDPAT", "Stained", "Scorched", "Boreal Forest", "Urban Masked" }, variables::knife_skin);
						break;
					}
					zgui::end_groupbox();
				}
			}
			else if (SkinTab == 1) {
				
			}
			
		}
		
		zgui::end_window();
	}
	
}

void zgui_line(int x, int y, int x2, int y2, zgui::color c) noexcept
{
	render::draw_line(x, y, x2, y2, color(c.r, c.g, c.b, c.a));
}

void zgui_outline(int x, int y, int w, int h, zgui::color c) noexcept
{
	render::draw_outline(x, y, w, h, color(c.r, c.g, c.b, c.a));
}

void zgui_filled_rect(int x, int y, int w, int h, zgui::color c) noexcept
{
	render::draw_filled_rect(x, y, w, h, color(c.r, c.g, c.b, c.a));
}

void zgui_text(int x, int y, zgui::color c, int font, bool center, const char* text) noexcept
{
	render::draw_text_string(x, y, font, text, center, color(c.r, c.g, c.b, c.a));
}

void zgui_get_text_size(unsigned long font, const char* text, int& width, int& height) noexcept
{
	render::get_text_size(font, text, width, height);
}

float zgui_get_frametime() noexcept
{
	return interfaces::globals->frame_time;
}

float zgui_get_curtime() noexcept
{
	return interfaces::globals->cur_time;
}

void menu::initialize()
{
	variables::menu::opened = false;
	
	zgui::functions.draw_line = zgui_line;
	zgui::functions.draw_rect = zgui_outline;
	zgui::functions.draw_filled_rect = zgui_filled_rect;
	zgui::functions.draw_text = zgui_text;
	zgui::functions.get_text_size = zgui_get_text_size;
	zgui::functions.get_frametime = zgui_get_frametime;
	zgui::functions.get_curtime = zgui_get_curtime;

	console::log("[setup] menu initialized!\n");
}