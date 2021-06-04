#pragma once
#include <cstdint>
#include "../../source-sdk/math/Vector3d.hpp"

namespace utilities {
	template< typename t >
	__forceinline static t call_virtual( void *name, int offset ) {
		int *table = *( int ** ) name;
		int address = table[ offset ];
		return ( t ) ( address );
	}

	std::uint8_t* pattern_scan(const char* module_name, const char* signature) noexcept;
	bool goes_through_smoke(vec3_t start, vec3_t end);
}
