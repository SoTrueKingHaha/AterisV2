#include "math.h"

void __math::sin_cos( float radians, float *sine, float *cosine )
{
	_asm
	{
		fld		DWORD PTR[ radians ]
		fsincos

		mov edx, DWORD PTR[ cosine ]
		mov eax, DWORD PTR[ sine ]

		fstp DWORD PTR[ edx ]
		fstp DWORD PTR[ eax ]
	}
}

double __fastcall __math::fast_sqrt( double n )
{
	_asm fld qword ptr[ esp + 4 ]
		_asm fsqrt
	_asm ret 8
}

void __math::vector_transform( const vector &input, const matrix3x4_t &matrix, vector &output )
{
	for ( auto i = 0; i < 3; i++ )
	{
		output[ i ] = input.dot( ( vector & )matrix.matrix3x4[ i ] ) + matrix[ i ][ 3 ];
	}
}

void __math::vector_copy( const vector &src, vector &dst )
{
	dst.x = src.x;
	dst.y = src.y;
	dst.z = src.z;
}

float __math::normalize_radians( float a ) noexcept
{
	return std::isfinite( a ) ? static_cast< float >( std::remainder( a, PI * 2 ) ) : 0.0f;
}

float __math::normalize_angle( float ang ) noexcept
{
	if ( !std::isfinite( ang ) )
		return 0.0f;

	return std::remainder( ang, 360.0f );
}

float __math::angle_diff_radians( float a1, float a2 ) noexcept
{
	float delta;

	delta = normalize_radians( a1 - a2 );

	if ( a1 > a2 )
	{
		if ( delta >= PI )
		{
			delta -= static_cast< float >( PI * 2 );
		}
	}
	else
	{
		if ( delta <= -PI )
		{
			delta += static_cast< float >( PI * 2 );
		}
	}

	return delta;
}

void __math::angle_vectors( const vector &angles, vector *forward, vector *right, vector *up )
{
	float sinRoll, sinPitch, sinYaw, cosRoll, cosPitch, cosYaw;
	sin_cos( DEG2RAD( angles.x ), &sinPitch, &cosPitch );
	sin_cos( DEG2RAD( angles.y ), &sinYaw, &cosYaw );
	sin_cos( DEG2RAD( angles.z ), &sinRoll, &cosRoll );

	if ( forward )
	{
		forward->x = cosPitch * cosYaw;
		forward->y = cosPitch * sinYaw;
		forward->z = -sinPitch;
	}

	if ( right )
	{
		right->x = ( -1 * sinRoll * sinPitch * cosYaw + -1 * cosRoll * -sinYaw );
		right->y = ( -1 * sinRoll * sinPitch * sinYaw + -1 * cosRoll * cosYaw );
		right->z = -1 * sinRoll * cosPitch;
	}

	if ( up )
	{
		up->x = ( cosRoll * sinPitch * cosYaw + -sinRoll * -sinYaw );
		up->y = ( cosRoll * sinPitch * sinYaw + -sinRoll * cosYaw );
		up->z = cosRoll * cosPitch;
	}
}

QAngle __math::look_at( vector Start, vector target )
{
	target -= Start;
	QAngle angles;
	if ( target.y == 0.0f && target.x == 0.0f )
	{
		angles.x = ( target.z > 0.0f ) ? 270.0f : 90.0f;
		angles.y = 0.0f;
	}
	else
	{
		angles.x = static_cast< float >( atan2( -target.z, target.length_2d( ) ) * 180.f / PI );
		angles.y = static_cast< float >( atan2( target.y, target.x ) * 180.f / PI );
	}

	angles.z = 0.0f;
	return angles;
}

// Fast arctangent with single precission
_declspec( naked ) float _vectorcall arctg( float x )
{
	// When |x|<=1 following formula is used:
	//
	//                       a0 + a0*a1*x^2 + a0*a2*x^4 + a0*a3*x^6
	// arctg x = x * ------------------------------------------------------- = x*P(x)/Q(x)
	//                a0 + a0*(a1+b0)*x^2 + a0*(a2+b1)*x^4 + a0*(a3+b2)*x^6
	//
	// The a0 constant is reduced, but it is needed to less the error
	// and prevent overflow at large |x|.
	// P(x) and Q(x) are 3th degree polinomials of x^2.
	// When 1<|x|<62919776 (approx.) used formula is
	//
	//                                     pi/2*|x|*x^6*Q(1/x)-x^6*P(1/x)
	// arctg x = pi/2*sgn(x)-arctg(1/x) = --------------------------------
	//                                             x*x^6*Q(1/x)
	//
	// Here x^6*P(1/x) and x^6*Q(1/x) are 3th degree polinomials of x^2 too.
	// When |x|>=62919776 used formula is arctg x = pi/2*sgn(x)
	//
	static const float ct[ 14 ] =   // Constants table
	{
	  6.28740248E-17f,            // a0*(a1+b0)=a0*c1
	  4.86816205E-17f,            // a0*a1
	  2.24874633E-18f,            // a0*(a3+b2)=a0*c3
	  4.02179944E-19f,            // a0*a3
	  4.25772129E-17f,            // a0
	  4.25772129E-17f,            // a0
	  2.50182216E-17f,            // a0*(a2+b1)=a0*c2
	  1.25756219E-17f,            // a0*a2
	  0.0707963258f,              // (pi-3)/2
	  1.5f,                       // 3/2
	  1.0f,                       // 1
	  -0.0707963258f,             // -(pi-3)/2
	  -1.5f,                      // -3/2
	  3.95889818E15f              // Threshold of x^2 when arctg(x)=pi/2*sgn(x)
	};
	_asm
	{
		vshufps xmm1, xmm0, xmm0, 0    // xmm1 = x # x : x # x
		mov edx, offset ct           // edx contains the address of constants table
		vmulps xmm2, xmm1, xmm1       // xmm2 = x^2 # x^2 : x^2 # x^2
		vmovups xmm3, [ edx + 16 ]       // xmm3 = a0*a2 # a0*c2 : a0 # a0
		vmulps xmm4, xmm2, xmm2       // xmm4 = y^2 # y^2 : y^2 # y^2
		vucomiss xmm2, [ edx + 40 ]      // Compare y=x^2 to 1
		ja arctg_big                // Jump if |x|>1
		vfmadd231ps xmm3, xmm2, [ edx ] // xmm3 ~ a3*y+a2 # c3*y+c2 : a1*y+1 # c1*y+1
		vmovhlps xmm1, xmm1, xmm3     // xmm1 ~ a3*y+a2 # c3*y+c2
		vfmadd231ps xmm3, xmm4, xmm1  // xmm3 ~ a3*y^3+a2*y^2+a1*y+1 # c3*y^3+c2*y^2+c1*y+1
		vmovshdup xmm2, xmm3         // xmm2 = P; xmm3 = Q
		vdivss xmm2, xmm2, xmm3       // xmm2 = P/Q
		vmulss xmm0, xmm0, xmm2       // xmm0 = x*P/Q = arctg(x)
		ret                         // Return
		arctg_big :                // When |x|>1 use formula pi/2*sgn(x)-arctg(1/x)
		vfmadd213ps xmm3, xmm2, [ edx ] // xmm3 ~ a2*y+a3 # c2*y+c3 : y+a1 # y+c1
			vmovmskpd eax, xmm1          // eax=3, if x<0, otherwise eax=0
			vmovhlps xmm0, xmm0, xmm3     // xmm0 ~ a2*y+a3 # c2*y+c3
			vfmadd213ps xmm3, xmm4, xmm0  // xmm3 ~ y^3+a1*y^2+a2*y+a3 # y^3+c1*y^2+c2*y+c3
			vmovss xmm0, [ edx + 4 * eax + 32 ]  // xmm0 = (pi-3)/2*sgn(x)
			vmovshdup xmm4, xmm3         // xmm4 = P; xmm3 = Q
			vmulss xmm1, xmm1, xmm3       // xmm1 = x*Q
			vucomiss xmm2, [ edx + 52 ]      // Compare y=x^2 to threshold value
			jnb arctg_end               // The data is already in xmm0, if |x|>=62919776
			vfmsub132ss xmm0, xmm4, xmm1  // xmm0 = (pi-3)/2*|x|*Q-P
			vdivss xmm0, xmm0, xmm1       // xmm0 = (pi-3)/2*sgn(x)-P/(x*Q)
			arctg_end :                // Add to result 3/2*sgn(x)
		vaddss xmm0, xmm0, [ edx + 4 * eax + 36 ] // xmm0 = pi/2*sgn(x)-P/(x*Q)
			ret                         // Return
	}
}

vector __math::calc_angle( const vector &src, const vector &dst )
{
	vector angles = vector( 0.0f, 0.0f, 0.0f );
	vector deltaPosition = ( src - dst );
	float flHypotenuse = static_cast< float >( fast_sqrt( ( deltaPosition.x * deltaPosition.x ) + ( deltaPosition.y * deltaPosition.y ) ) );

	angles.x = ( arctg( deltaPosition.z / flHypotenuse ) * static_cast< float >( M_RADPI ) );
	angles.y = ( arctg( deltaPosition.y / deltaPosition.x ) * static_cast< float >( M_RADPI ) );
	angles.z = 0;

	if ( deltaPosition.x >= 0.0f )
	{
		angles.y += 180.f;
	}

	return angles;
}

float __math::calc_fov( const vector &src, const vector &dst )
{
	vector sourceAngle = vector( 0.0f, 0.0f, 0.0f );
	angle_vectors( src, &sourceAngle );

	vector destAngle = vector( 0.0f, 0.0f, 0.0f );
	angle_vectors( dst, &destAngle );

	float result = RAD2DEG( acos( destAngle.dot( sourceAngle ) / destAngle.length_sqr( ) ) );

	if ( !isfinite( result ) || isinf( result ) || isnan( result ) )
	{
		result = 0.0f;
	}

	return result;
}

float __math::remap_val_clamped( float val, float a1, float a2, float b1, float b2 )
{
	if ( a1 == a2 )
	{
		return val >= a2 ? b2 : b1;
	}

	float mult = ( val - a1 ) / ( a2 - a1 );

	mult = std::clamp( mult, 0.0f, 1.0f );

	return b1 + ( b2 - b1 ) * mult;
}

float __math::get_magnitude( const vector &a )
{
	return sqrtf( a.dot( a ) );
}

vector __math::velocity_to_angles( const vector &dir )
{
	float yaw, pitch;

	if ( dir.y == 0.0f && dir.x == 0.0f )
	{
		yaw = 0.0f;

		if ( dir.z > 0.0f )
		{
			pitch = 270.0f;
		}
		else
		{
			pitch = 90.0f;
		}
	}
	else
	{
		yaw = RAD2DEG( atan2f( dir.y, dir.x ) );
		pitch = RAD2DEG( atan2f( -dir.z, get_magnitude( vector( dir ) ) ) );

		if ( yaw < 0.0f )
		{
			yaw += 360.0f;
		}

		if ( pitch < 0.0f )
		{
			pitch += 360.0f;
		}
	}

	return vector{ pitch, yaw, 0.0f };
}


vector __math::get_rotated_pos( vector a, const float rotation, const float dist )
{
	const auto rad = DEG2RAD( rotation );
	a.x += cosf( rad ) * dist;
	a.y += sinf( rad ) * dist;

	return a;
}

float __math::normalize_yaw( float f )
{
	if ( std::isnan( f ) || std::isinf( f ) )
	{
		f = 0.0f;
	}

	if ( f > 9999999.0f )
	{
		f = 0.0f;
	}

	if ( f < -9999999.0f )
	{
		f = 0.0f;
	}

	while ( f < -180.0f )
	{
		f += 360.0f;
	}

	while ( f > 180.0f )
	{
		f -= 360.0f;
	}

	return f;
}

void __math::vector_angles( const vector &forward, QAngle &angles )
{
	float temp, yaw, pitch;

	if ( forward.y == 0 && forward.x == 0 )
	{
		yaw = 0;

		if ( forward.z > 0 )
		{
			pitch = 270;
		}
		else
		{
			pitch = 90;
		}
	}

	else
	{
		yaw = RAD2DEG( atan2f( forward.y, forward.x ) );

		if ( yaw < 0 )
		{
			yaw += 360;
		}

		temp = forward.length_2d( );
		pitch = RAD2DEG( atan2f( -forward.z, temp ) );

		if ( pitch < 0 )
		{
			pitch += 360;
		}
	}

	angles.x = pitch;
	angles.y = yaw;
	angles.z = 0;
}

float __math::get_clockwise_angle( const vector &viewangle, const vector &aimangle )
{
	vector vecAngle = vector( );
	angle_vectors( viewangle, &vecAngle );

	vector vecAim = vector( );
	angle_vectors( aimangle, &vecAim );

	return -atan2( vecAngle.x * vecAim.y - vecAngle.y * vecAim.x, vecAngle.x * vecAim.x + vecAngle.y * vecAim.y );
}

float __math::map_float( float val, float a1, float a2, float b1, float b2 )
{
	return ( val - a1 ) * ( b2 - b1 ) / ( a1 - a2 ) + b1;
}

float __math::vector_normalize( vector &a )
{
	float length = a.length( );

	if ( !length )
	{
		a.set( );
	}
	else
	{
		length /= length;
	}

	return length;
}

color __math::rgba_to_clr( float r, float g, float b, float a )
{
	return color(
		( unsigned char )std::clamp( r + .5f, 0.0f, 255.0f ),
		( unsigned char )std::clamp( g + .5f, 0.0f, 255.0f ),
		( unsigned char )std::clamp( b + .5f, 0.0f, 255.0f ),
		( unsigned char )std::clamp( a + .5f, 0.0f, 255.0f )
	);
}

float __math::lerp_float( float a1, float a2, float t )
{
	return a1 + ( a2 - a1 ) * t;
}

float __math::lerp_float_other( float a, float b, float t )
{
	return ( 1 - t ) * a + t * b;
}

color __math::lerp_color( const color &a1, const color &a2, float t )
{
	if ( t <= 0.0f ) return a1;
	if ( t >= 1.0f ) return a2;
	return rgba_to_clr(
		lerp_float( static_cast< float >( a1.r ), static_cast< float >( a2.r ), t ),
		lerp_float( static_cast< float >( a1.g ), static_cast< float >( a2.g ), t ),
		lerp_float( static_cast< float >( a1.b ), static_cast< float >( a2.b ), t ),
		lerp_float( static_cast< float >( a1.a ), static_cast< float >( a2.a ), t )
	);
}

float __math::angle_mod( float a )
{
	a = ( 360.f / 65536 ) * ( ( int )( a * ( 65536.f / 360.0f ) ) & 65535 );
	return a;
}

float __math::approach_angle( float target, float value, float speed )
{
	float delta = target - value;

	// Speed is assumed to be positive
	if ( speed < 0 )
	{
		speed = -speed;
	}

	if ( delta < -180 )
	{
		delta += 360;
	}
	else if ( delta > 180 )
	{
		delta -= 360;
	}

	if ( delta > speed )
	{
		value += speed;
	}
	else if ( delta < -speed )
	{
		value -= speed;
	}
	else
	{
		value = target;
	}

	return value;
}

float __math::turn_epsilon( vector originalvector )
{
	float max = 0.f;
	for ( auto i = -1; i < 3; i++ )
	{
		for ( auto k = -1; k < 3; k++ )
		{
			vector vel = originalvector;
			vel.x += i * DIST_EPSILON;
			vel.y += k * DIST_EPSILON;
			float turn = RAD2DEG( std::atan2( vel.y, vel.x ) - std::atan2( originalvector.y, originalvector.x ) );
			if ( turn > max )
			{
				max = turn;
			}
		}
	}
	return max;
}

float __math::direction_turning( vector vel, vector lastvel )
{
	if ( vel.length( ) < 1.f )
	{
		return 0;
	}
	if ( lastvel.length( ) < 1.f )
	{
		return 0;
	}

	float dir = RAD2DEG( std::atan2( vel.y, vel.x ) - std::atan2( lastvel.y, lastvel.x ) );
	while ( dir > 180.f )
	{
		dir -= 360.f;
	}
	while ( dir < -180.f )
	{
		dir += 360.f;
	}
	if ( fabsf( dir ) > 165.f || fabsf( dir ) < 1.f )
	{
		return 0;
	}
	if ( fabsf( dir ) <= fabsf( turn_epsilon( vel ) ) )
	{
		return 0;
	}
	return dir;
}

void __math::matrix_set_column( const vector& in, int column, matrix3x4_t& out )
{
	out[ 0 ][ column ] = in.x;
	out[ 1 ][ column ] = in.y;
	out[ 2 ][ column ] = in.z;
}

void __math::angle_matrix( const QAngle& angles, matrix3x4_t& matrix )
{
	float sr, sp, sy, cr, cp, cy;

	sin_cos( DEG2RAD( angles.y ), &sy, &cy );
	sin_cos( DEG2RAD( angles.x ), &sp, &cp );
	sin_cos( DEG2RAD( angles.z ), &sr, &cr );

	// matrix = (YAW * PITCH) * ROLL
	matrix[ 0 ][ 0 ] = cp * cy;
	matrix[ 1 ][ 0 ] = cp * sy;
	matrix[ 2 ][ 0 ] = -sp;

	float crcy = cr * cy;
	float crsy = cr * sy;
	float srcy = sr * cy;
	float srsy = sr * sy;
	matrix[ 0 ][ 1 ] = sp * srcy - crsy;
	matrix[ 1 ][ 1 ] = sp * srsy + crcy;
	matrix[ 2 ][ 1 ] = sr * cp;

	matrix[ 0 ][ 2 ] = ( sp * crcy + srsy );
	matrix[ 1 ][ 2 ] = ( sp * crsy - srcy );
	matrix[ 2 ][ 2 ] = cr * cp;

	matrix[ 0 ][ 3 ] = 0.0f;
	matrix[ 1 ][ 3 ] = 0.0f;
	matrix[ 2 ][ 3 ] = 0.0f;
}

void __math::MatrixCopy( const matrix3x4_t& src, matrix3x4_t& dst )
{
	if ( &src != &dst )
	{
		memcpy( dst.matrix3x4, src.matrix3x4, 12 * sizeof( float ) );
	}
}

void __math::MatrixMultiply( const matrix3x4_t& src1, const matrix3x4_t& src2, matrix3x4_t& dst )
{
	// Make sure it works if src1 == dst or src2 == dst
	matrix3x4_t tmp1, tmp2;
	const VMatrixRaw_t* s1 = ( &src1 == &dst ) ? tmp1.matrix3x4 : src1.matrix3x4;
	const VMatrixRaw_t* s2 = ( &src2 == &dst ) ? tmp2.matrix3x4 : src2.matrix3x4;

	if ( &src1 == &dst )
	{
		MatrixCopy( src1, tmp1 );
	}
	if ( &src2 == &dst )
	{
		MatrixCopy( src2, tmp2 );
	}

	dst[ 0 ][ 0 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 0 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 0 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 0 ];
	dst[ 0 ][ 1 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 1 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 1 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 1 ];
	dst[ 0 ][ 2 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 2 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 2 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 2 ];
	dst[ 0 ][ 3 ] = s1[ 0 ][ 0 ] * s2[ 0 ][ 3 ] + s1[ 0 ][ 1 ] * s2[ 1 ][ 3 ] + s1[ 0 ][ 2 ] * s2[ 2 ][ 3 ];

	dst[ 1 ][ 0 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 0 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 0 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 0 ];
	dst[ 1 ][ 1 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 1 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 1 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 1 ];
	dst[ 1 ][ 2 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 2 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 2 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 2 ];
	dst[ 1 ][ 3 ] = s1[ 1 ][ 0 ] * s2[ 0 ][ 3 ] + s1[ 1 ][ 1 ] * s2[ 1 ][ 3 ] + s1[ 1 ][ 2 ] * s2[ 2 ][ 3 ];

	dst[ 2 ][ 0 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 0 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 0 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 0 ];
	dst[ 2 ][ 1 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 1 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 1 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 1 ];
	dst[ 2 ][ 2 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 2 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 2 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 2 ];
	dst[ 2 ][ 3 ] = s1[ 2 ][ 0 ] * s2[ 0 ][ 3 ] + s1[ 2 ][ 1 ] * s2[ 1 ][ 3 ] + s1[ 2 ][ 2 ] * s2[ 2 ][ 3 ];
}

void __math::VectorRotate( const vector& in1, const matrix3x4_t& in2, vector& out )
{
	for ( int i = 0; i < 3; i++ )
	{
		out[ i ] = in1.dot( ( vector& )in2.matrix3x4[ i ] );
	}
}

void __math::VectorRotate( const vector& in1, const QAngle& in2, vector& out )
{
	matrix3x4_t matRotate;
	angle_matrix( in2, matRotate );
	VectorRotate( in1, matRotate, out );
}

int __math::random_int( int a1, int a2 )
{
	static const auto random_int = reinterpret_cast< int( _cdecl* )( int, int ) >( GetProcAddress( GetModuleHandleW( L"vstdlib.dll" ), "RandomInt" ) );
	return random_int( a1, a2 );
}

float __math::random_float( float a1, float a2 )
{
	static const auto random_float = reinterpret_cast< float( _cdecl* )( float, float ) >( GetProcAddress( GetModuleHandleW( L"vstdlib.dll" ), "RandomFloat" ) );
	return random_float( a1, a2 );
}

void __math::random_seed( unsigned seed )
{
	static const auto random_seed = reinterpret_cast< void( _cdecl* )( unsigned ) >( GetProcAddress( GetModuleHandleW( L"vstdlib.dll" ), "RandomSeed" ) );
	random_seed( seed );
}

#undef max
bool __math::compare_float( float a, float b, float epsilon )
{
	return ( fabs( a - b ) <= epsilon * std::max( fabs( a ), fabs( b ) ) );
}