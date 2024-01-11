#include "Game.h"
#include "Lapis/LapisTypes.h"
#include "Lapis/Engine.h"
#include <iostream>


using namespace Lapis;

namespace Game
{
	//Transforms for instantiated obj
	Lapis::Transform cube1;
	Lapis::Transform cube2;
	bool flag = false;


	void SpawnStuff()
	{
		if (flag == false)
		{
			cube1.pos.x = 1;
			cube2.pos.x = 5;
			flag = true;
		}

		Draw::D3::Cube(Transform(cube1), Color(255, 0, 0, 1));
		Draw::D3::Cube(Transform(cube2), Color(255, 0, 0, 1));

	}


	void Run()
	{
		std::cout << Vec3::Distance(cube1.pos, cube2.pos) << "\n";
		SpawnStuff();
		Move();
	}


	/// <summary>
	///  3D box collision
	/// </summary>
	/// <param name="a"></param>
	/// <param name="b"></param>
	/// <returns>True</returns>
	bool IsBoxColliding(Vec3 a, Vec3 b)
	{
		if(Vec3::Distance(a,b) <= 0)
		{
			// Collide
			return true;
		}
		return false;
	}


	//test collision
	void Move()
	{
		if (!IsBoxColliding(cube1.pos, cube2.pos))
		{
			if (GetAsyncKeyState('F'))
			{
				cube1.pos.x += 0.1;
				cube2.pos.x -= 0.1;
			}


		}
		else
		{
			std::cout << "Colliding" << "\n";
		}
	}


}
