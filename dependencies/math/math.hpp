#pragma once
#include <windows.h>
#include <iostream>
#include <cstdint>
#include <memory>
#include <vector>
#include <thread>
#include <chrono>
#include <array>
#include <fstream>
#include <istream>
#include <unordered_map>
#include <intrin.h>
#include <filesystem>
#include <assert.h>

#include "../utilities/singleton.hpp"
#include "../utilities/fnv.hpp"
#include "../utilities/utilities.hpp"
#include "../../dependencies/minhook/minhook.h"
#include "../interfaces/interfaces.hpp"
#include "../../source-sdk/sdk.hpp"
#include "../../core/hooks/hooks.hpp"
#include "../../dependencies/math/math.hpp"
#include "../../dependencies/utilities/renderer/renderer.hpp"
#include "../../dependencies/utilities/console/console.hpp"
#include "../utilities/csgo.hpp"

namespace math {
	void correct_movement(vec3_t old_angles, c_usercmd* cmd, float old_forwardmove, float old_sidemove);
	void CorrectMovement(c_usercmd* cmd, vec3_t wish_angle, vec3_t old_angles);
	vec3_t calculate_angle(vec3_t& a, vec3_t& b);
	void sin_cos(float r, float* s, float* c);
	vec3_t angle_vector(vec3_t angle);
	void transform_vector(vec3_t&, matrix_t&, vec3_t&);
	void vector_angles(vec3_t&, vec3_t&);
	void angle_vectors(vec3_t&, vec3_t*, vec3_t*, vec3_t*);
	float NormalizeAngleFloat(float flAng);
	vec3_t vector_add(vec3_t&, vec3_t&);
	vec3_t vector_subtract(vec3_t&, vec3_t&);
	vec3_t vector_multiply(vec3_t&, vec3_t&);
	vec3_t vector_divide(vec3_t&, vec3_t&);
	bool screen_transform(const vec3_t& point, vec3_t& screen);
	bool world_to_screen(const vec3_t& origin, vec2_t& screen);

	/* Pasted Math Functions Yeah Yeah */

	#define XM_CONST constexpr
	XM_CONST float XM_2PI = 6.283185307f;
	XM_CONST float XM_PIDIV2 = 1.570796327f;
	XM_CONST float XM_1DIV2PI = 0.159154943f;

	inline void XMScalarSinCos
	(
		float* pSin,
		float* pCos,
		float  Value
	)
	{
		assert(pSin);
		assert(pCos);

		// Map Value to y in [-pi,pi], x = 2*pi*quotient + remainder.
		float quotient = XM_1DIV2PI * Value;
		if (Value >= 0.0f)
		{
			quotient = static_cast<float>(static_cast<int>(quotient + 0.5f));
		}
		else
		{
			quotient = static_cast<float>(static_cast<int>(quotient - 0.5f));
		}
		float y = Value - XM_2PI * quotient;

		// Map y to [-pi/2,pi/2] with sin(y) = sin(Value).
		float sign;
		if (y > XM_PIDIV2)
		{
			y = M_PI - y;
			sign = -1.0f;
		}
		else if (y < -XM_PIDIV2)
		{
			y = -M_PI - y;
			sign = -1.0f;
		}
		else
		{
			sign = +1.0f;
		}

		float y2 = y * y;

		// 11-degree minimax approximation
		*pSin = (((((-2.3889859e-08f * y2 + 2.7525562e-06f) * y2 - 0.00019840874f) * y2 + 0.0083333310f) * y2 - 0.16666667f) * y2 + 1.0f) * y;

		// 10-degree minimax approximation
		float p = ((((-2.6051615e-07f * y2 + 2.4760495e-05f) * y2 - 0.0013888378f) * y2 + 0.041666638f) * y2 - 0.5f) * y2 + 1.0f;
		*pCos = sign * p;
	}

	inline void angle_vectors(const vec3_t& angles, vec3_t& forward) {
		float	sp, sy, cp, cy;

		XMScalarSinCos(&sp, &cp, DEG2RAD(angles.x));
		XMScalarSinCos(&sy, &cy, DEG2RAD(angles.y));

		forward.x = cp * cy;
		forward.y = cp * sy;
		forward.z = -sp;
	}

	inline void vec3_t_angles(const vec3_t& forward, vec3_t& angles) {
		float	tmp, y, pitch;

		if (forward[1] == 0 && forward[0] == 0) {
			y = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			y = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (y < 0)
				y += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = y;
		angles[2] = 0;
	}

	inline void normalize_angle(vec3_t& angles) {
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}
	inline void clamp_angle(vec3_t& angles) {
		angles.x = std::clamp(angles.x, -89.0f, 89.0f);
		angles.y = std::clamp(angles.y, -180.0f, 180.0f);
		angles.z = 0.0f;
	}

	inline bool sanitize_angle(vec3_t& angles)
	{
		vec3_t temp = angles;
		normalize_angle(temp);
		clamp_angle(temp);

		if (!isfinite(temp.x) ||
			!isfinite(temp.y) ||
			!isfinite(temp.z))
			return false;

		angles = temp;

		return true;
	}

	inline vec3_t calculate_angle(const vec3_t& src, const vec3_t& dst) {

		vec3_t angle;
		vec3_t_angles(dst - src, angle);

		return angle;
	}

	inline float get_fov(const vec3_t& view_angle, const vec3_t& aim_angle)
	{
		vec3_t ang, aim;

		angle_vectors(view_angle, aim);
		angle_vectors(aim_angle, ang);

		return RAD2DEG(acos(aim.dot(ang) / aim.length_sqr()));
	}

	inline float distance_point_to_line(vec3_t Point, vec3_t LineOrigin, vec3_t Dir) {
		auto PointDir = Point - LineOrigin;

		auto TempOffset = PointDir.dot(Dir) / (Dir.x * Dir.x + Dir.y * Dir.y + Dir.z * Dir.z);
		if (TempOffset < 0.000001f)
			return FLT_MAX;

		auto PerpendicularPoint = LineOrigin + (Dir * TempOffset);

		return (Point - PerpendicularPoint).length();
	}

	inline void normalize_angles(vec3_t& angles) {
		for (auto i = 0; i < 3; i++) {
			while (angles[i] < -180.0f) angles[i] += 360.0f;
			while (angles[i] > 180.0f) angles[i] -= 360.0f;
		}
	}

	inline void Clamp(vec3_t& angles) {
		if (angles.y > 180.0f)
			angles.y = 180.0f;
		else if (angles.y < -180.0f)
			angles.y = -180.0f;

		if (angles.x > 89.0f)
			angles.x = 89.0f;
		else if (angles.x < -89.0f)
			angles.x = -89.0f;

		angles.z = 0;
	}

	inline bool sanitize(vec3_t& angles)
	{
		vec3_t temp = angles;
		normalize_angles(temp);
		Clamp(temp);

		if (!isfinite(temp.x) ||
			!isfinite(temp.y) ||
			!isfinite(temp.z))
			return false;

		angles = temp;

		return true;
	}

	inline void VectorAngles(const vec3_t& forward, vec3_t& angles)
	{
		float	tmp, yaw, pitch;

		if (forward[1] == 0 && forward[0] == 0) {
			yaw = 0;
			if (forward[2] > 0)
				pitch = 270;
			else
				pitch = 90;
		}
		else {
			yaw = (atan2(forward[1], forward[0]) * 180 / M_PI);
			if (yaw < 0)
				yaw += 360;

			tmp = sqrt(forward[0] * forward[0] + forward[1] * forward[1]);
			pitch = (atan2(-forward[2], tmp) * 180 / M_PI);
			if (pitch < 0)
				pitch += 360;
		}

		angles[0] = pitch;
		angles[1] = yaw;
		angles[2] = 0;
	}
};
