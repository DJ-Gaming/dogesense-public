#include "../features.hpp"

void glow::run() {

	player_t* local = reinterpret_cast<player_t*>(interfaces::entity_list->get_client_entity(interfaces::engine->get_local_player()));

	if (local) {

		for (int i = 0; i < interfaces::glow_manager->size; ++i) {
			glow_object_definition_t& object = interfaces::glow_manager->objects[i];

			if (object.unused())
				continue;

			entity_t* ent = (entity_t*)object.entity;

			if (!ent || ent->dormant())
				continue;

			if (ent == local)
				continue;

			if (ent->team() == local->team())
				continue;

			float Glow_A = variables::Glow_A;
			
			if (ent->is_player()) {
			
				object.color[0] = variables::Glow_Color[0];
				object.color[1] = variables::Glow_Color[1];
				object.color[2] = variables::Glow_Color[2];
				object.alpha = 1;
				object.bloom_amount = Glow_A / 100.f;
				object.full_bloom_render = false;
				object.render_when_occluded = true;
				object.render_when_unoccluded = false;
			}
		}
	}

}