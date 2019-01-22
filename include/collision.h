#pragma once

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

//Collision methods (eventually need to return overlap, but for now, bool)

bool is_colliding(Box_Collider box1, Box_Collider box2);
bool is_colliding(Sphere_Collider sphere1, Sphere_Collider sphere2);
bool is_colliding(Box_Collider box, Sphere_Collider sphere);
bool is_colliding(Sphere_Collider sphere, Box_Collider box);

