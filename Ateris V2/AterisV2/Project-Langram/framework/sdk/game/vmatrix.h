#pragma once

// TODO: fill this out.
class view_matrix
{
public:
	inline const matrix3x4_t &As3x4( ) const {
		return *( ( const matrix3x4_t * )this );
	}

	__forceinline float* operator[](int i)
	{
		return m[i];
	}
	__forceinline const float* operator[](int i) const
	{
		return m[i];
	}

	__forceinline float* Base()
	{
		return &m[0][0];
	}
	__forceinline const float* Base() const
	{
		return &m[0][0];
	}

	inline view_matrix operator-()
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m[i][j] = -m[i][j];
			}
		}
	}

	inline view_matrix operator*(const view_matrix& x)
	{
		for (int i = 0; i < 4; i++)
		{
			for (int j = 0; j < 4; j++)
			{
				m[i][j] *= x[i][j];
			}
		}
	}

public:
	float m[4][4];
};