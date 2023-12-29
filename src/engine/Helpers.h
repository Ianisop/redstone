#pragma once
#include <DirectXMath.h>
#define _USE_MATH_DEFINES
#include <math.h>
#include "LapisTypes.h"

		
#define safe_release(x) if(x) {x->Release();}

namespace Lapis
{
	namespace Helpers
	{

#define DEG2RAD (M_PI/180)

#define RAD2DEG (180/M_PI)

		constexpr Lapis::Vec3 AxisY = Lapis::Vec3(0.0, 1.0, 0.0);

		inline DirectX::XMMATRIX XMMatrixScaling(Lapis::Vec3 scale) {
			return DirectX::XMMatrixScaling(scale.x, scale.y, scale.z);
		}

		inline DirectX::XMMATRIX XMMatrixRotationRollPitchYaw(Lapis::Vec3 rot) {
			return DirectX::XMMatrixRotationRollPitchYaw(rot.z, rot.x, rot.y);
		}

		inline DirectX::XMMATRIX XMMatrixRotationAxis(Lapis::Vec3 axis, float angle) {
			return DirectX::XMMatrixRotationAxis({axis.x, axis.y, axis.z}, angle);
		}

		inline DirectX::XMMATRIX XMMatrixTranslation(Lapis::Vec3 pos) {
			return DirectX::XMMatrixTranslation(pos.x, pos.y, pos.z);
		}

		inline DirectX::XMVECTOR XMVectorSet(Lapis::Vec4 vec)
		{
			return DirectX::XMVectorSet(vec.x, vec.y, vec.z, vec.w);
		}
	}
}