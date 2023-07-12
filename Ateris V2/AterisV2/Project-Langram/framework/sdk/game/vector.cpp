#include "vector.h"

#include "../../utils/math/math.h"

vector vector::angle_vector() const
{
	vector forward;
	float sp, sy, cp, cy;

	math.sin_cos(DEG2RAD(this->x), &sp, &cp);
	math.sin_cos(DEG2RAD(this->y), &sy, &cy);

	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	return forward;
}
