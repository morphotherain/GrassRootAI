#pragma once
#include "GameObject.h"

class Sector {
public:
	Sector() = default;
	Sector(double _x, double _y, double _z) :x(_x), y(_y),z(_y) {};
	~Sector() = default;

	long long int SectorID = 0;
	double x;
	double y;
	double z;
	double x_Max = 0;
	double y_Max = 0;
	double z_Max = 0;
	double x_Min = 0;
	double y_Min = 0;
	double z_Min = 0;
	double radius = 5000000.0f;


	void addObject(std::shared_ptr<GameObject> object);
	std::vector<std::shared_ptr<GameObject>> space_objects;
	std::shared_ptr<Sector> SuperSector;
	bool isInSector(double x, double y, double z);

};