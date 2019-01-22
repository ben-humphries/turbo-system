#include "collision.h"

#include <stdio.h>

void Box_Collider::init(Math::vec3 position, Math::vec3 size)
{
	this->position = position;
	this->size = size;
}

void Sphere_Collider::init(Math::vec3 position, float radius)
{
	this->position = position;
	this->radius = radius;
}

bool is_colliding(Box_Collider box1, Box_Collider box2)
{
	Math::vec3 min1 = box1.position;
	Math::vec3 min2 = box2.position;
	Math::vec3 max1 = box1.position + box1.size;
	Math::vec3 max2 = box2.position + box2.size;

	return
		(min1.x <= max2.x && max1.x >= min2.x) &&
		(min1.y <= max2.y && max1.y >= min2.y) &&
		(min1.z <= max2.z && max1.z >= min2.z);
}
bool is_colliding(Sphere_Collider sphere1, Sphere_Collider sphere2)
{
	float distance_squared =
		(sphere1.position.x - sphere2.position.x) * (sphere1.position.x - sphere2.position.x) +
		(sphere1.position.y - sphere2.position.y) * (sphere1.position.y - sphere2.position.y) +
		(sphere1.position.z - sphere2.position.z) * (sphere1.position.z - sphere2.position.z);

	return distance_squared < (sphere1.radius + sphere2.radius) * (sphere1.radius + sphere2.radius); //square this value so we dont have to sqrt the distance
}
bool is_colliding(Box_Collider box, Sphere_Collider sphere)
{
	//closest point of box to sphere
	float x = Math::max(box.position.x, Math::min(sphere.position.x, box.position.x + box.size.x));
	float y = Math::max(box.position.y, Math::min(sphere.position.y, box.position.y + box.size.y));
	float z = Math::max(box.position.z, Math::min(sphere.position.z, box.position.z + box.size.z));

	//distance from point to sphere
	float distance_squared =
		(x - sphere.position.x) * (x - sphere.position.x) +
		(y - sphere.position.y) * (y - sphere.position.y) +
		(z - sphere.position.z) * (z - sphere.position.z);

	return distance_squared < sphere.radius * sphere.radius;
}
bool is_colliding(Sphere_Collider sphere, Box_Collider box)
{
	return is_colliding(box, sphere);
}

//TEST

void test_collisions()
{
	printf("Running collision tests\n");

	Box_Collider a;
	a.init(Math::vec3(-2,-2,-2), Math::vec3(3,3,3));
	Box_Collider b;
	b.init(Math::vec3(0,0,0), Math::vec3(1,1,1));

	Sphere_Collider sa;
	sa.init(Math::vec3(0,0,0), 2);

	Sphere_Collider sb;
	sb.init(Math::vec3(3,0,0), 2);

	printf("colliding: %d\n", is_colliding(a, b));
	printf("colliding: %d\n", is_colliding(a, sa));
	printf("colliding: %d\n", is_colliding(sb, b));
	printf("colliding: %d\n", is_colliding(sb, sa));
		
}