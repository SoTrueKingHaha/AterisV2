#pragma once

#define _USE_MATH_DEFINES
#include <cmath>
#include <algorithm>
#include <vector>
#include "../../sdk/game/vector.h"
#include "../../sdk/game/vmatrix.h"
#include "../draw/draw.h"

#define DIST_EPSILON (0.03125f)

class __math
{
public:
	typedef float VMatrixRaw_t[ 4 ];
	void sin_cos( float radians, float *sine, float *cosine );
	double __fastcall fast_sqrt( double n );
	void vector_transform( const vector &input, const matrix3x4_t &matrix, vector &output );
	void vector_copy( const vector &src, vector &dst );
	float normalize_radians( float a ) noexcept;
	float normalize_angle( float ang ) noexcept;
	float angle_diff_radians( float a1, float a2 ) noexcept; // Makes the difference between two angles wrap between -pi and pi
	void angle_vectors( const vector &angles, vector *forward, vector *right = nullptr, vector *up = nullptr ); // Converts a QAngle into either one or three normalised Vectors.
	QAngle look_at( vector Start, vector target );
	vector calc_angle( const vector &src, const vector &dst ); // Get the angle from one position in 3D space to be facing directly towards the other direction in 3D space
	float calc_fov( const vector &src, const vector &dst );
	float remap_val_clamped( float val, float a1, float a2, float b1, float b2 );
	float get_magnitude( const vector &a );
	vector velocity_to_angles( const vector &dir );
	vector get_rotated_pos( vector a, const float rotation, const float dist );
	float normalize_yaw( float f );
	void vector_angles( const vector &forward, QAngle &angles ); // Converts a single Vector into a QAngle.
	float get_clockwise_angle( const vector &viewangle, const vector &aimangle );
	float map_float( float val, float a1, float a2, float b1, float b2 );
	float vector_normalize( vector &a );
	color rgba_to_clr( float r, float g, float b, float a );
	float lerp_float( float a1, float a2, float t );
	float lerp_float_other( float a, float b, float t );
	color lerp_color( const color &a1, const color &a2, float t );
	float angle_mod( float a );
	float approach_angle( float target, float value, float speed );
	float turn_epsilon( vector originalvector );
	float direction_turning( vector vel, vector lastvel );
	void matrix_set_column( const vector& in, int column, matrix3x4_t& out );
	void angle_matrix( const QAngle& angles, matrix3x4_t& matrix );
	void MatrixMultiply( const matrix3x4_t& src1, const matrix3x4_t& src2, matrix3x4_t& dst );
	void MatrixCopy( const matrix3x4_t& src, matrix3x4_t& dst );
	void VectorRotate( const vector& in1, const matrix3x4_t& in2, vector& out );
	void VectorRotate( const vector& in1, const QAngle& in2, vector& out );
	int random_int( int a1, int a2 );
	float random_float( float a1, float a2 );
	void random_seed( unsigned seed );
	bool compare_float( float a, float b, float epsilon = FLT_EPSILON );
};
inline __math math;