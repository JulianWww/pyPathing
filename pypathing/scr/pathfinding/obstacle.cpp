#include "obstacle.h"
#include <vector>
#include "node.h"


obstacle::ray::ray()
{
	this->length = 0;
	this->heading = {};
	this->startPos = {};
}

obstacle::ray::ray(std::vector<int>&start, std::vector<int>&end) {
	this->set(start, end);
}

void obstacle::ray::set(std::vector<int>& start, std::vector<int>& end)
{
	this->startPos = jce::jvector<float>(jce::convert<float>(start));
	auto endPos = jce::jvector<float>(jce::convert<float>(start));
	this->heading = endPos - this->startPos;
	this->length = this->heading.abs();
	this->heading = this->heading / this->length;
}

obstacle::sphere::sphere(float r, std::vector<float> o) : Baise(o)
{
	this->r = r;
}

bool obstacle::sphere::intersects(ray const& r)
{
	auto u = r.heading;
	auto o = r.startPos;

	auto deltaO = this->origin - o;

	float a = (-2 * u.dot(deltaO));
	float b = sqrt(2 * pow(u.dot(deltaO), 2) - 4 * u.abs() * (deltaO.abs() - this->r*this->r));
	float c = 2 * u.abs();

	float d1 = (a + b) / c;
	float d2 = (a - b) / c;

	if (d1 >= 0 && d1 < r.length) {
		return true;
	}
	return (d2 >= 0 && d2 < r.length);
}

bool obstacle::sphere::isIn(PathNode* n)
{
	auto pos = jce::convert<float>(n->pos);
	float d = (this->origin - pos).abs();
	return (d < this->r);
}

std::vector<float> obstacle::Baise::getOrigin()
{
	return std::vector<float>(this->origin.begin(), this->origin.end());
}

obstacle::Baise::Baise(std::vector<float> o)
{
	this->origin = o;
}

bool obstacle::Baise::intersects(ray const& r)
{
	return false;
}

bool obstacle::Baise::isIn(PathNode*)
{
	return false;
}
