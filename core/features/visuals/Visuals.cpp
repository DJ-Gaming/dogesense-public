#include "../features.hpp"
#include "../../menu/menu.hpp"
#include "../../../dependencies/xor.h"

bool visuals::get_player_box(player_t* ent, box& in) {

	vec3_t origin, min, max, flb, brt, blb, frt, frb, brb, blt, flt;
	int left, top, right, bottom;

	origin = ent->abs_origin();
	min = ent->collideable()->mins() + origin;
	max = ent->collideable()->maxs() + origin;

	vec3_t points[] = {
		vec3_t(min.x, min.y, min.z),
		vec3_t(min.x, max.y, min.z),
		vec3_t(max.x, max.y, min.z),
		vec3_t(max.x, min.y, min.z),
		vec3_t(max.x, max.y, max.z),
		vec3_t(min.x, max.y, max.z),
		vec3_t(min.x, min.y, max.z),
		vec3_t(max.x, min.y, max.z)
	};
	if (!interfaces::debug_overlay->world_to_screen(points[3], flb) || !interfaces::debug_overlay->world_to_screen(points[5], brt)
		|| !interfaces::debug_overlay->world_to_screen(points[0], blb) || !interfaces::debug_overlay->world_to_screen(points[4], frt)
		|| !interfaces::debug_overlay->world_to_screen(points[2], frb) || !interfaces::debug_overlay->world_to_screen(points[1], brb)
		|| !interfaces::debug_overlay->world_to_screen(points[6], blt) || !interfaces::debug_overlay->world_to_screen(points[7], flt))
		return false;

	vec3_t arr[] = { flb, brt, blb, frt, frb, brb, blt, flt };

	left = flb.x;
	top = flb.y;
	right = flb.x;
	bottom = flb.y;

	for (int i = 1; i < 8; i++) {
		if (left > arr[i].x)
			left = arr[i].x;
		if (bottom < arr[i].y)
			bottom = arr[i].y;
		if (right < arr[i].x)
			right = arr[i].x;
		if (top > arr[i].y)
			top = arr[i].y;
	}
	in.x = (int)left;
	in.y = (int)top;
	in.w = int(right - left);
	in.h = int(bottom - top);


	return true;
}

void visuals::draw_box(player_t* ent, box bbox)
{
	if (!(variables::box_esp)) return;

	render::draw_rect(bbox.x, bbox.y, bbox.w, bbox.h, color(255, 255, 255, 255)); // For Corner Boxes: render::draw_rect(bbox.x, bbox.y, bbox.w * 0.25, bbox.h * 0.25, color(255, 255, 255, 255));
	render::draw_rect(bbox.x - 1, bbox.y - 1, bbox.w + 2, bbox.h + 2, color(0, 0, 0, 170)); // For Corner Boxes: render::draw_rect(bbox.x - 1, bbox.y - 1, (bbox.w + 2) * 0.25, (bbox.h + 2) * 0.25, color(0, 0, 0, 170)); 
	render::draw_rect(bbox.x + 1, bbox.y + 1, bbox.w - 2, bbox.h - 2, color(0, 0, 0, 170)); // For Corner Boxes: render::draw_rect(bbox.x + 1, bbox.y + 1, (bbox.w - 2) * 0.25, (bbox.h - 2) * 0.25, color(0, 0, 0, 170));
}

void visuals::draw_name(player_t* entity, box bbox)
{
	if (!(variables::name_esp)) return;

	player_info_t info; //player info

	interfaces::engine->get_player_info(entity->index(), &info);

	if (info.name == "raz") return;

	if (info.fakeplayer)
		render::text(bbox.x + (bbox.w / 2), bbox.y - 15, render::fonts::esp_font, "bot - " + std::string(info.name), true, color(255, 255, 255, 255));
	else
		render::text(bbox.x + (bbox.w / 2), bbox.y - 15, render::fonts::esp_font, info.name, true, color(255, 255, 255, 255));
}

void visuals::draw_gun(player_t* entity, box bbox)
{
	if (!(variables::weapon_esp)) return;


	weapon_t* weapon_in_hand = entity->active_weapon();
	std::string weapon = entity->active_weapon()->get_weapon_data()->weapon_name;
	weapon.erase(0, 7);

	if (!weapon_in_hand) return;

	render::text(bbox.x + (bbox.w / 2), (bbox.y + 5) + (bbox.h), render::fonts::esp_font, weapon, true, color(255, 255, 255, 255));
}

void visuals::draw_ammo(player_t* entity, box bbox)
{
	if (!(variables::ammo_esp)) return;

	weapon_t* weapon = entity->active_weapon();

	if (!weapon) return;

	auto gun_ammo = weapon->clip1_count();

	auto math_for_the_ammo_bar = gun_ammo * bbox.w / weapon->get_weapon_data()->weapon_max_clip;

	int visible_ammo = weapon->get_weapon_data()->weapon_max_clip / 4;

	if (gun_ammo < 0) return;

	render::draw_filled_rect(bbox.x - 1, bbox.y + bbox.h + 3, bbox.w + 2, 4, color(0, 0, 0, 170));
	render::draw_filled_rect(bbox.x, bbox.y + bbox.h + 4, math_for_the_ammo_bar, 2, color(0, 50, 255));
	if (gun_ammo <= visible_ammo) {
		render::text((bbox.x + 10) + math_for_the_ammo_bar - 10, bbox.y + bbox.h - 2, render::fonts::esp_font, std::to_string(gun_ammo), true, color(255, 255, 255, 255));
	}
}

constexpr float SPEED_FREQ = 255 / 1.0f;

void HealthGreenRedGradient(int32_t Health, color& Color) {
	Health = max(0, min(Health, 100));

	Color.r = min((510 * (100 - Health)) / 100, 255);
	Color.g = min((510 * Health) / 100, 255);
	Color.b = 0;
	Color.a = 255;
}

void visuals::draw_hp(player_t* entity, box bbox)
{
	if (!(variables::hp_esp)) return;

	int hp = entity->health();

	static float prev_player_hp[65];

	if (prev_player_hp[entity->index()] > hp)
		prev_player_hp[entity->index()] -= SPEED_FREQ * interfaces::globals->frame_time;
	else
		prev_player_hp[entity->index()] = hp;

	int bar = bbox.h * (prev_player_hp[entity->index()] / 100.f);

	color hp_color = color(0, 0, 0, 255);

	HealthGreenRedGradient(hp, hp_color);

	render::draw_filled_rect(bbox.x - 6, bbox.y - 1, 4, bbox.h + 2, color(0, 0, 0, 170));
	render::draw_filled_rect(bbox.x - 5, bbox.y + bbox.h - bar, 2, bar, hp_color);

	if (hp < 100)
		render::text(bbox.x - 9, bbox.y + bbox.h - bar - 10, render::fonts::esp_font_small, std::to_string(entity->health()), false, color(255, 255, 255));
}

void visuals::spectator_list() {
	int kapi = 0;
	int screen_x, screen_y;

	interfaces::engine->get_screen_size(screen_x, screen_y);

	for (int i = 1; i <= 64; i++) {
		player_t* player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));
		if (player) {
			auto handle = player->observer_target();
			if (handle != 0xFFFFFFFF)
			{
				player_t* target = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity_handle(handle));
				if (target && target == csgo::local_player)
				{
					player_info_t spectator_info;
					interfaces::engine->get_player_info(player->index(), &spectator_info);
					std::string spectator_name = spectator_info.name;
					if (spectator_info.fakeplayer) spectator_name += " (BOT)";

					render::draw_text_string(4, screen_y / 2 - 80 + 15 * kapi, render::fonts::esp_font, spectator_name, false, color::white(220));
					kapi++;
				}
			}
		}
	}

	render::draw_text_string(4, screen_y / 2 - 80 - 15, render::fonts::esp_font, "Spectators:", false, color::white(220));
}

void visuals::loop_entities()
{
	if (variables::spectator_list)
		spectator_list();
	for (int i = 1; i < interfaces::globals->max_clients; i++)
	{
		player_t* entity = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(i));

		if (!(variables::esp)) return;

		if (!entity
			|| !csgo::local_player
			|| !entity->is_alive()
			|| entity == csgo::local_player
			|| entity->team() == csgo::local_player->team()
			|| entity->dormant())
			continue;

		if (variables::radar)
			entity->spotted() = true;

		box bbox;

		if (!visuals::get_player_box(entity, bbox)) continue;


		draw_box(entity, bbox);
		draw_name(entity, bbox);
		draw_hp(entity, bbox);
		draw_ammo(entity, bbox);
		draw_gun(entity, bbox);
	}
}

void visuals::nightmode() {
	static convar* draw_specific_static_prop = interfaces::console->get_convar("r_drawspecificstaticprop");
	if (draw_specific_static_prop->get_int() != 0)
		draw_specific_static_prop->set_value(0);

	for (mat_handle_t i = interfaces::material_system->first_material(); i != interfaces::material_system->invalid_material_handle(); i = interfaces::material_system->next_material(i))
	{
		i_material* material = interfaces::material_system->get_material(i);
		if (!material)
			continue;

		if (strstr(material->get_texture_group_name(), "World") || strstr(material->get_texture_group_name(), "StaticProp"))
		{
			float nightmode_value = (100.f - float(variables::nightmode)) / 100.f;
			float props_value = std::clamp(nightmode_value + 0.3f, 0.00f, 1.00f);

			if (variables::nightmode > 0) {
				if (strstr(material->get_texture_group_name(), "StaticProp"))
					material->color_modulate(props_value, props_value, props_value);
				else
					material->color_modulate(nightmode_value, nightmode_value, nightmode_value);
			}
			else
				material->color_modulate(1.0f, 1.0f, 1.0f);
		}
	}
}

void visuals::watermark() {
	int screen_x, screen_y;
	interfaces::engine->get_screen_size(screen_x, screen_y);
	// Constants for padding, etc...
	const auto margin = 10; // Padding between screen edges and watermark
	const auto padding = 4; // Padding between watermark elements

	// Constants for colors
	const auto col_background = color(0, 0, 0, 255);
	const auto col_accent = color(variables::Watermark_Color[0] * 255.f, variables::Watermark_Color[1] * 255.f, variables::Watermark_Color[2] * 255.f);
	const auto col_text = color(255, 255, 255); // Watermark text color

	// Setup time
	auto t = std::time(nullptr);
	std::ostringstream time;
	// Format: 12:59:09
	time << std::put_time(std::localtime(&t), XorStr("%H:%M:%S"));

	// Cheat variables
	std::string logo = XorStr("DogeSense");
	const std::string user = XorStr("beta");

	auto delay = (int)(0);
	auto tick_rate = (int)(1.f / interfaces::globals->interval_per_tick); // m_interval_per_tick

	auto text = logo + XorStr(" | ") + user + XorStr(" | ") + std::to_string(tick_rate) + XorStr("tick | ") + time.str().data();

	auto text_size = render::get_text_size(render::fonts::watermark_font, text);
	auto text_pos = vec2_t(screen_x - margin - padding - text_size.x, // Right align + margin + padding + text_size
		margin + padding); // Top align

	auto bg_size = vec2_t(padding + text_size.x + padding, // Width
		padding + text_size.y + padding); // Height
	auto bg_pos = vec2_t(screen_x - margin - padding - text_size.x - padding, // Right align + margin
		margin); // Top align

	// render::draw_rect(bg_pos.x, bg_pos.y, bg_size.x, bg_size.y, col_background); // Background
	render::draw_rect(bg_pos.x, bg_pos.y, bg_size.x, bg_size.y, col_background); // Background
	render::draw_rect(bg_pos.x, bg_pos.y, bg_size.x, 2, col_accent); // Accent line
	render::text(text_pos.x, text_pos.y, render::fonts::watermark_font, text, false, col_text); // Text
}