#pragma once

#include "common.h"

//Box Collider
struct Box_Collider {
	Math::vec3 position;
	Math::vec3 size;
};
//Sphere Collider
struct Sphere_Collider {
	Math::vec3 position;
	float radius;
};

//Capsule Collider (?)

//Collision methods
//TODO(ben-humphries): eventually these should return overlap

bool is_colliding(Box_Collider box1, Box_Collider box2);
bool is_colliding(Sphere_Collider sphere1, Sphere_Collider sphere2);
bool is_colliding(Box_Collider box, Sphere_Collider sphere);
bool is_colliding(Sphere_Collider sphere, Box_Collider box);

//TODO(ben-humphries): Make collision methods for vec3 and box, vec3 and sphere?

void test_collisions();
