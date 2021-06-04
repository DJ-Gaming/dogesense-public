#include "../utilities/csgo.hpp"

#define NOMINMAX
#define XM_CONST constexpr
XM_CONST float XM_2PI = 6.283185307f;
XM_CONST float XM_PIDIV2 = 1.570796327f;
XM_CONST float XM_1DIV2PI = 0.159154943f;

/* Backtrack */

void MScalarSinCos
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

//aimtux
void math::correct_movement(vec3_t old_angles, c_usercmd* cmd, float old_forwardmove, float old_sidemove) {
	float delta_view;
	float f1;
	float f2;

	if (old_angles.y < 0.f)
		f1 = 360.0f + old_angles.y;
	else
		f1 = old_angles.y;

	if (cmd->viewangles.y < 0.0f)
		f2 = 360.0f + cmd->viewangles.y;
	else
		f2 = cmd->viewangles.y;

	if (f2 < f1)
		delta_view = abs(f2 - f1);
	else
		delta_view = 360.0f - abs(f1 - f2);

	delta_view = 360.0f - delta_view;

	cmd->forwardmove = cos(DEG2RAD(delta_view)) * old_forwardmove + cos(DEG2RAD(delta_view + 90.f)) * old_sidemove;
	cmd->sidemove = sin(DEG2RAD(delta_view)) * old_forwardmove + sin(DEG2RAD(delta_view + 90.f)) * old_sidemove;
}

void AngleVectors(const vec3_t& angles, vec3_t& forward, vec3_t& right, vec3_t& up)
{
	float sr, sp, sy, cr, cp, cy;

	MScalarSinCos(&sp, &cp, DEG2RAD(angles[0]));
	MScalarSinCos(&sy, &cy, DEG2RAD(angles[1]));
	MScalarSinCos(&sr, &cr, DEG2RAD(angles[2]));

	forward.x = (cp * cy);
	forward.y = (cp * sy);
	forward.z = (-sp);
	right.x = (-1 * sr * sp * cy + -1 * cr * -sy);
	right.y = (-1 * sr * sp * sy + -1 * cr * cy);
	right.z = (-1 * sr * cp);
	up.x = (cr * sp * cy + -sr * -sy);
	up.y = (cr * sp * sy + -sr * cy);
	up.z = (cr * cp);
}

void math::CorrectMovement(c_usercmd* cmd, vec3_t wish_angle, vec3_t old_angles)
{
	if (old_angles.x != wish_angle.x || old_angles.y != wish_angle.y || old_angles.z != wish_angle.z)
	{
		vec3_t wish_forward, wish_right, wish_up, cmd_forward, cmd_right, cmd_up;

		auto viewangles = old_angles;
		auto movedata = vec3_t(cmd->forwardmove, cmd->sidemove, cmd->upmove);
		viewangles.normalize();

		if (!(csgo::local_player->flags() & fl_onground) && viewangles.z != 0.f)
			movedata.y = 0.f;

		AngleVectors(wish_angle, wish_forward, wish_right, wish_up);
		AngleVectors(viewangles, cmd_forward, cmd_right, cmd_up);

		auto v8 = sqrt(wish_forward.x * wish_forward.x + wish_forward.y * wish_forward.y), v10 = sqrt(wish_right.x * wish_right.x + wish_right.y * wish_right.y), v12 = sqrt(wish_up.z * wish_up.z);

		vec3_t wish_forward_norm(1.0f / v8 * wish_forward.x, 1.0f / v8 * wish_forward.y, 0.f),
			wish_right_norm(1.0f / v10 * wish_right.x, 1.0f / v10 * wish_right.y, 0.f),
			wish_up_norm(0.f, 0.f, 1.0f / v12 * wish_up.z);

		auto v14 = sqrt(cmd_forward.x * cmd_forward.x + cmd_forward.y * cmd_forward.y), v16 = sqrt(cmd_right.x * cmd_right.x + cmd_right.y * cmd_right.y), v18 = sqrt(cmd_up.z * cmd_up.z);

		vec3_t cmd_forward_norm(1.0f / v14 * cmd_forward.x, 1.0f / v14 * cmd_forward.y, 1.0f / v14 * 0.0f),
			cmd_right_norm(1.0f / v16 * cmd_right.x, 1.0f / v16 * cmd_right.y, 1.0f / v16 * 0.0f),
			cmd_up_norm(0.f, 0.f, 1.0f / v18 * cmd_up.z);

		auto v22 = wish_forward_norm.x * movedata.x, v26 = wish_forward_norm.y * movedata.x, v28 = wish_forward_norm.z * movedata.x, v24 = wish_right_norm.x * movedata.y, v23 = wish_right_norm.y * movedata.y, v25 = wish_right_norm.z * movedata.y, v30 = wish_up_norm.x * movedata.z, v27 = wish_up_norm.z * movedata.z, v29 = wish_up_norm.y * movedata.z;

		vec3_t correct_movement;
		correct_movement.x = cmd_forward_norm.x * v24 + cmd_forward_norm.y * v23 + cmd_forward_norm.z * v25 + (cmd_forward_norm.x * v22 + cmd_forward_norm.y * v26 + cmd_forward_norm.z * v28) + (cmd_forward_norm.y * v30 + cmd_forward_norm.x * v29 + cmd_forward_norm.z * v27);
		correct_movement.y = cmd_right_norm.x * v24 + cmd_right_norm.y * v23 + cmd_right_norm.z * v25 + (cmd_right_norm.x * v22 + cmd_right_norm.y * v26 + cmd_right_norm.z * v28) + (cmd_right_norm.x * v29 + cmd_right_norm.y * v30 + cmd_right_norm.z * v27);
		correct_movement.z = cmd_up_norm.x * v23 + cmd_up_norm.y * v24 + cmd_up_norm.z * v25 + (cmd_up_norm.x * v26 + cmd_up_norm.y * v22 + cmd_up_norm.z * v28) + (cmd_up_norm.x * v30 + cmd_up_norm.y * v29 + cmd_up_norm.z * v27);

		correct_movement.x = std::clamp(correct_movement.x, -450.f, 450.f);
		correct_movement.y = std::clamp(correct_movement.y, -450.f, 450.f);
		correct_movement.z = std::clamp(correct_movement.z, -320.f, 320.f);

		cmd->forwardmove = correct_movement.x;
		cmd->sidemove = correct_movement.y;
		cmd->upmove = correct_movement.z;

		cmd->buttons &= ~(in_moveright | in_moveleft | in_back | in_forward);
		if (cmd->sidemove != 0.0) {
			if (cmd->sidemove <= 0.0)
				cmd->buttons |= in_moveleft;
			else
				cmd->buttons |= in_moveright;
		}

		if (cmd->forwardmove != 0.0) {
			if (cmd->forwardmove <= 0.0)
				cmd->buttons |= in_back;
			else
				cmd->buttons |= in_forward;
		}
	}
}
vec3_t math::calculate_angle(vec3_t& a, vec3_t& b) {
	vec3_t angles;
	vec3_t delta;
	delta.x = (a.x - b.x);
	delta.y = (a.y - b.y);
	delta.z = (a.z - b.z);

	double hyp = sqrt(delta.x * delta.x + delta.y * delta.y);
	angles.x = (float)(atanf(delta.z / hyp) * 57.295779513082f);
	angles.y = (float)(atanf(delta.y / delta.x) * 57.295779513082f);

	angles.z = 0.0f;
	if (delta.x >= 0.0) { angles.y += 180.0f; }
	return angles;
}

void math::sin_cos(float r, float* s, float* c) {
	*s = sin(r);
	*c = cos(r);
}

vec3_t math::angle_vector(vec3_t angle) {
	auto sy = sin(angle.y / 180.f * static_cast<float>(M_PI));
	auto cy = cos(angle.y / 180.f * static_cast<float>(M_PI));

	auto sp = sin(angle.x / 180.f * static_cast<float>(M_PI));
	auto cp = cos(angle.x / 180.f * static_cast<float>(M_PI));

	return vec3_t(cp * cy, cp * sy, -sp);
}

void math::transform_vector(vec3_t & a, matrix_t & b, vec3_t & out) {
	out.x = a.dot(b.mat_val[0]) + b.mat_val[0][3];
	out.y = a.dot(b.mat_val[1]) + b.mat_val[1][3];
	out.z = a.dot(b.mat_val[2]) + b.mat_val[2][3];
}

void math::vector_angles(vec3_t & forward, vec3_t & angles) {
	if (forward.y == 0.0f && forward.x == 0.0f) {
		angles.x = (forward.z > 0.0f) ? 270.0f : 90.0f;
		angles.y = 0.0f;
	}
	else {
		angles.x = atan2(-forward.z, vec2_t(forward).length()) * -180 / static_cast<float>(M_PI);
		angles.y = atan2(forward.y, forward.x) * 180 / static_cast<float>(M_PI);

		if (angles.y > 90)
			angles.y -= 180;
		else if (angles.y < 90)
			angles.y += 180;
		else if (angles.y == 90)
			angles.y = 0;
	}

	angles.z = 0.0f;
}

float math::NormalizeAngleFloat(float flAng)
{
	// rotate left by 180 degrees
	flAng = flAng + 180.0f;

	// normalize to -360..360 degrees
	flAng = fmod(flAng, 360.0f);
	if (flAng != flAng) // http://stackoverflow.com/a/570694
	{
		// edge case error handling
		flAng = 0.0f;
	}

	// rotate negative angles left by 360 degrees to get the positive equivalent
	if (flAng < 0.0f)
	{
		flAng += 360.0f;
	}

	// rotate right by 180 degrees
	flAng -= 180.0f;

	return flAng;
}

void math::angle_vectors(vec3_t & angles, vec3_t * forward, vec3_t * right, vec3_t * up) {
	float sp, sy, sr, cp, cy, cr;

	sin_cos(DEG2RAD(angles.x), &sp, &cp);
	sin_cos(DEG2RAD(angles.y), &sy, &cy);
	sin_cos(DEG2RAD(angles.z), &sr, &cr);

	if (forward) {
		forward->x = cp * cy;
		forward->y = cp * sy;
		forward->z = -sp;
	}

	if (right) {
		right->x = -1 * sr * sp * cy + -1 * cr * -sy;
		right->y = -1 * sr * sp * sy + -1 * cr * cy;
		right->z = -1 * sr * cp;
	}

	if (up) {
		up->x = cr * sp * cy + -sr * -sy;
		up->y = cr * sp * sy + -sr * cy;
		up->z = cr * cp;
	}
}

vec3_t math::vector_add(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x + b.x,
		a.y + b.y,
		a.z + b.z);
}

vec3_t math::vector_subtract(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x - b.x,
		a.y - b.y,
		a.z - b.z);
}

vec3_t math::vector_multiply(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x * b.x,
		a.y * b.y,
		a.z * b.z);
}

vec3_t math::vector_divide(vec3_t & a, vec3_t & b) {
	return vec3_t(a.x / b.x,
		a.y / b.y,
		a.z / b.z);
}

bool math::screen_transform(const vec3_t & point, vec3_t & screen) {
	auto matrix = interfaces::engine->world_to_screen_matrix();

	float w = matrix[3][0] * point.x + matrix[3][1] * point.y + matrix[3][2] * point.z + matrix[3][3];
	screen.x = matrix[0][0] * point.x + matrix[0][1] * point.y + matrix[0][2] * point.z + matrix[0][3];
	screen.y = matrix[1][0] * point.x + matrix[1][1] * point.y + matrix[1][2] * point.z + matrix[1][3];
	screen.z = 0.0f;

	int inverse_width = static_cast<int>((w < 0.001f) ? -1.0f / w :
		1.0f / w);

	screen.x *= inverse_width;
	screen.y *= inverse_width;
	return (w < 0.001f);
}

bool math::world_to_screen(const vec3_t & origin, vec2_t & screen) {
	static std::uintptr_t view_matrix;
	if ( !view_matrix )
		view_matrix = *reinterpret_cast< std::uintptr_t* >( reinterpret_cast< std::uintptr_t >( utilities::pattern_scan( "client.dll", "0F 10 05 ? ? ? ? 8D 85 ? ? ? ? B9" ) ) + 3 ) + 176;

	const auto& matrix = *reinterpret_cast< view_matrix_t* >( view_matrix );

	const auto w = matrix.m[ 3 ][ 0 ] * origin.x + matrix.m[ 3 ][ 1 ] * origin.y + matrix.m[ 3 ][ 2 ] * origin.z + matrix.m[ 3 ][ 3 ];
	if ( w < 0.001f )
		return false;

	int x, y;
	interfaces::engine->get_screen_size( x, y );
	
	screen.x = static_cast<float>(x) / 2.0f;
	screen.y = static_cast<float>(y) / 2.0f;

	screen.x *= 1.0f + ( matrix.m[ 0 ][ 0 ] * origin.x + matrix.m[ 0 ][ 1 ] * origin.y + matrix.m[ 0 ][ 2 ] * origin.z + matrix.m[ 0 ][ 3 ] ) / w;
	screen.y *= 1.0f - ( matrix.m[ 1 ][ 0 ] * origin.x + matrix.m[ 1 ][ 1 ] * origin.y + matrix.m[ 1 ][ 2 ] * origin.z + matrix.m[ 1 ][ 3 ] ) / w;

	return true;
}