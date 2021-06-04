#include "../features.hpp"

void chams::init() {
	std::ofstream("csgo/materials/glowOverlay.vmt") << R"#("VertexLitGeneric" {
 
	"$additive" "1"
	"$envmap" "models/effects/cube_white"
	"$envmaptint" "[1 1 1]"
	"$envmapfresnel" "1"
	"$envmapfresnelminmaxexp" "[0 1 2]"
	"$alpha" "1"
})#";

	std::ofstream("csgo/materials/glow_Arm.vmt") << R"#("VertexLitGeneric" {
 
	"$additive" "1"
	"$envmap" "models/effects/cube_white"
	"$envmaptint" "[1 1 1]"
	"$envmapfresnel" "1"
	"$envmapfresnelminmaxexp" "[0 1 2]"
	"$alpha" "1"
})#";

	std::ofstream("csgo/materials/simple_regular_reflective.vmt") << R"#("VertexLitGeneric"
	{

	 "$basetexture" "vgui/white_additive"
	 "$ignorez"      "0"
	 "$envmap"       "env_cubemap"
	 "$normalmapalphaenvmapmask"  "1"
	 "$envmapcontrast"             "1"
	 "$nofog"        "1"
	 "$model"        "1"
	 "$nocull"       "0"
	 "$selfillum"    "1"
	 "$halflambert"  "1"
	 "$znearer"      "0"
	 "$flat"         "1"
})#";

	std::ofstream("csgo\\materials\\matReg.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
	std::ofstream("csgo\\materials\\matXQZ.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "1"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";

	std::ofstream("csgo\\materials\\matBT.vmt") << R"#("VertexLitGeneric"
{
  "$basetexture" "vgui/white_additive"
  "$ignorez"      "0"
  "$envmap"       ""
  "$nofog"        "1"
  "$model"        "1"
  "$nocull"       "0"
  "$selfillum"    "1"
  "$halflambert"  "1"
  "$znearer"      "0"
  "$flat"         "1"
}
)#";
}

void chams::shutdown() {
	std::remove("csgo\\materials\\glowOverlay.vmt");
	std::remove("csgo\\materials\\glow_Arm.vmt");
	std::remove("csgo\\materials\\simple_regular_reflective.vmt");
	std::remove("csgo\\materials\\matReg.vmt");
	std::remove("csgo\\materials\\matXQZ.vmt");
	std::remove("csgo\\materials\\matBT.vmt");
}
void modulate_shit_clr(i_material* mat, float* clr) {
	bool b_found = false;
	auto p_var = mat->find_var("$envmaptint", &b_found);
	if (b_found)
		(*(void(__thiscall**)(int, float, float, float))(*(DWORD*)p_var + 44))((uintptr_t)p_var, clr[0], clr[1], clr[2]);
}
void modulate_shit_alpha(i_material* mat, float alpha) {
	bool b_found = false;
	auto p_var = mat->find_var("$alpha", &b_found);
	if (b_found)
		(*(void(__thiscall**)(int, float))(*(DWORD*)p_var + 44))((uintptr_t)p_var, alpha);
}
void chams::run(hooks::draw_model_execute::fn ofunc, i_mat_render_context* ctx, const draw_model_state_t& state, const model_render_info_t& info, matrix_t* matrix) {

	static i_material* mat_textured = interfaces::material_system->find_material("matReg", nullptr);
	static i_material* mat_xqz = interfaces::material_system->find_material("matXQZ", nullptr);
	static i_material* mat_bt = interfaces::material_system->find_material("matBT", nullptr);
	static i_material* mat_flat = interfaces::material_system->find_material("debug/debugdrawflat", TEXTURE_GROUP_MODEL);
	static i_material* mat_eso_glow = interfaces::material_system->find_material("glowOverlay", nullptr); // "dev/glow_armsrace", TEXTURE_GROUP_MODEL
	static i_material* mat_eso_glow_xqz = interfaces::material_system->find_material("glowOverlay_XQZ", nullptr); // "dev/glow_armsrace", TEXTURE_GROUP_MODEL
	static i_material* mat_glow = interfaces::material_system->find_material("dev/glow_armsrace", TEXTURE_GROUP_MODEL);
	static i_material* mat_bark = interfaces::material_system->find_material("models/props/de_dust/hr_dust/foliage/palm_bark_01", TEXTURE_GROUP_MODEL);
	static i_material* mat_gloss = interfaces::material_system->find_material("models/player/ct_fbi/ct_fbi_glass", TEXTURE_GROUP_MODEL);
	static i_material* mat_desync = interfaces::material_system->find_material("models/inventory_items/dogtags/dogtags_outline", TEXTURE_GROUP_MODEL);
    static i_material* mat_plastic = interfaces::material_system->find_material("simple_regular_reflective", TEXTURE_GROUP_MODEL);

	/* Arm Shit */

	static i_material* mat_eso_glow_arm = interfaces::material_system->find_material("glow_Arm", nullptr); // "dev/glow_armsrace", TEXTURE_GROUP_MODEL
	static i_material* mat_air = interfaces::material_system->find_material("models/inventory_items/trophy_majors/gloss", TEXTURE_GROUP_MODEL); // air
	static i_material* mat_glass = interfaces::material_system->find_material("models/inventory_items/cologne_prediction/cologne_prediction_glass", TEXTURE_GROUP_MODEL); // glass
	static i_material* mat_crystal = interfaces::material_system->find_material("models/inventory_items/trophy_majors/crystal_clear", TEXTURE_GROUP_MODEL); // crystal
	static i_material* mat_normal = interfaces::material_system->find_material("simple_regular_reflective", nullptr); // normal

	i_material* material = mat_textured;
	i_material* material_arms = mat_normal;

	switch (variables::chams_arms_mat) {
	case 0:
		material_arms = mat_normal;
		break;
	case 1:
		material_arms = mat_glass;
		break;
	case 2:
		material_arms = mat_air;
		break;
	case 3:
		material_arms = mat_crystal;
		break;
	}

	if (variables::chams_material == 0) {
		material = mat_textured;
	}
	else if (variables::chams_material == 1) {
		material = mat_flat;
	}
	else if (variables::chams_material == 2) {
		material = mat_bark;
	}
	else if (variables::chams_material == 3) {
		material = mat_gloss;
	}

	if (interfaces::engine->is_in_game() && interfaces::engine->is_connected() && csgo::local_player) {
		if (info.model) {
			std::string model_name = interfaces::model_info->get_model_name(info.model);

			if (model_name.find("models/player/") != std::string::npos && variables::chams) {
				auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
				if (csgo::local_player && player && player->is_player() && !player->dormant() && player->is_alive()) {
					if (player->team() != csgo::local_player->team()) {

						if (variables::backtrack && variables::backtrack_chams && backtracking::records[player->index()].size() > 0 && backtracking::records[player->index()].back().bone_matrix && backtracking::records[player->index()].back().bone_matrix_built) {
							interfaces::render_view->modulate_color(variables::Chams_Color_BT);
							interfaces::render_view->set_blend(0.5f);
							interfaces::model_render->override_material(mat_bt);
							ofunc(interfaces::model_render, ctx, state, info, backtracking::records[player->index()].back().bone_matrix);
						}

						if (variables::ignore_z) {
							interfaces::render_view->modulate_color(variables::Chams_Color_Z);
							if (variables::chams_material == 3)
								modulate_shit_clr(mat_gloss, variables::Chams_Color_Z);
							interfaces::render_view->set_blend(float(100 - variables::transparency) / 100.f);
							if (variables::chams_material == 0) {
								interfaces::model_render->override_material(mat_xqz);
							}
							else {
								material->set_material_var_flag(material_var_flags_t::material_var_ignorez, true);
								interfaces::model_render->override_material(material);
							}
							ofunc(interfaces::model_render, ctx, state, info, matrix);
						}
						interfaces::render_view->modulate_color(variables::Chams_Color);
						if (variables::chams_material == 3)
							modulate_shit_clr(mat_gloss, variables::Chams_Color);
						interfaces::render_view->set_blend(float(100 - variables::transparency) / 100.f);
						if (variables::chams_material != 0)
							material->set_material_var_flag(material_var_flags_t::material_var_ignorez, false);
						interfaces::model_render->override_material(material);
						ofunc(interfaces::model_render, ctx, state, info, matrix);

						if (variables::glow_over) {

							interfaces::render_view->modulate_color(variables::Chams_Color_G);
							modulate_shit_clr(mat_eso_glow, variables::Chams_Color_G);
							interfaces::render_view->set_blend(float(100));
							modulate_shit_alpha(mat_eso_glow, float(100));
							mat_eso_glow->set_material_var_flag(material_var_flags_t::material_var_ignorez, variables::ignore_z);
							interfaces::model_render->override_material(mat_eso_glow);

							ofunc(interfaces::model_render, ctx, state, info, matrix);
						}
					}
					/*
					else if (player == csgo::local_player) {
						if (variables::antiaim)
						{
							if (fake_state.init_fake_anim && fake_state.m_got_fake_matrix)
							{
								for (auto& i : fake_state.m_fake_matrix)
								{
									i[0][3] += info.origin.x;
									i[1][3] += info.origin.y;
									i[2][3] += info.origin.z;
								}

								interfaces::render_view->modulate_color(variables::Chams_Color_G);
								modulate_shit_clr(mat_eso_glow, variables::Chams_Color_G);
								interfaces::render_view->set_blend(1.f);
								modulate_shit_alpha(mat_eso_glow, 1.f);
								mat_eso_glow->set_material_var_flag(material_var_flags_t::material_var_ignorez, false);
								interfaces::model_render->override_material(mat_eso_glow);
								ofunc(interfaces::model_render, ctx, state, info, fake_state.m_fake_matrix);

								for (auto& i : fake_state.m_fake_matrix)
								{
									i[0][3] -= info.origin.x;
									i[1][3] -= info.origin.y;
									i[2][3] -= info.origin.z;
								}
							}
							static const float white[3] = { 1.f,1.f,1.f };
							interfaces::render_view->modulate_color(white);
							interfaces::render_view->set_blend(1.f);
							interfaces::model_render->override_material(nullptr);
							//ofunc(interfaces::model_render, ctx, state, info, matrix);
						}
					}
					*/
				}
			}

			if ((model_name.find("arms") != std::string::npos || model_name.find("v_models") != std::string::npos) && !(model_name.find("uid") != std::string::npos || model_name.find("stattrack") != std::string::npos) && variables::chams_arms) {
				auto player = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(info.entity_index));
				if (csgo::local_player && csgo::local_player->is_alive()) {
					interfaces::render_view->modulate_color( variables::Chams_Color_Arm );
					modulate_shit_clr(material_arms, variables::Chams_Color_Arm);
					interfaces::render_view->set_blend(float(100 - variables::transparency_arm) / 100.f);
					material_arms->set_material_var_flag(material_var_flags_t::material_var_ignorez, false); // change later
					variables::chams_arms_wire ? material_arms->set_material_var_flag(material_var_wireframe, true) : material_arms->set_material_var_flag(material_var_wireframe, false);
					interfaces::model_render->override_material(material_arms);
					ofunc(interfaces::model_render, ctx, state, info, matrix);

					if (variables::glow_over_arm) {
						ofunc(interfaces::model_render, ctx, state, info, matrix);
						interfaces::render_view->modulate_color(variables::Chams_Color_G_Arm);
						modulate_shit_clr(mat_eso_glow_arm, variables::Chams_Color_G_Arm);
						interfaces::render_view->set_blend(float(100));
						modulate_shit_alpha(mat_eso_glow_arm, float(100));
						mat_eso_glow_arm->set_material_var_flag(material_var_flags_t::material_var_ignorez, false);
						interfaces::model_render->override_material(mat_eso_glow_arm);
					}
				}
			}

		}
	}
	ofunc(interfaces::model_render, ctx, state, info, matrix);
	interfaces::model_render->override_material(nullptr);
}