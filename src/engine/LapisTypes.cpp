#include "LapisTypes.h"


namespace Lapis
{

	////////////////////// Vec2

	Vec2 Lapis::Vec2::operator-() const
	{
		Vec2 res;

		res.x = -this->x;
		res.y = -this->y;

		return res;
	}
	Vec2 Vec2::operator+(const Vec2& other) const
	{
		Vec2 res;
		
		res.x = this->x + other.x;
		res.y = this->y + other.y;

		return res;
	}
	Vec2 Vec2::operator-(const Vec2& other) const
	{
		Vec2 res;

		res.x = this->x - other.x;
		res.y = this->y - other.y;

		return res;
	}
	Vec2 Vec2::operator*(const float& mul) const
	{
		Vec2 res;

		res.x = x * mul;
		res.y = y * mul;
		
		return res;
	}

	////////////////////// Vec3

	Vec3 Lapis::Vec3::operator-() const
	{
		Vec3 res;

		res.x = -this->x;
		res.y = -this->y;
		res.z = -this->z;

		return res;
	}

	////////////////////// Vec4

	Vec4 Lapis::Vec4::operator+(const Vec4& other) const
	{
		Vec4 res;

		res.x = this->x + other.x;
		res.y = this->y + other.y;
		res.z = this->z + other.z;
		res.w = this->w + other.w;

		return res;
	}

}