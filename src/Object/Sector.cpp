#include "Sector.h"

void Sector::addObject(std::shared_ptr<GameObject> object)
{
	space_objects.push_back(object);
}

bool Sector::isInSector(double _x, double _y, double _z)
{
	return (x_Min <= _x) && (_x < x_Max) && (y_Min <= _y) && (_y < y_Max) && (z_Min <= _z) && (_z < z_Max);
}