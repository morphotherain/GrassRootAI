#pragma once
#include "GameObject.h"

class Sector {
public:
	Sector() = default;
	Sector(double _x, double _y, double _z) :x(_x), y(_y),z(_y) {};
	~Sector() = default;

	long long int SectorID;
	double x;
	double y;
	double z;
	double x_Max;
	double y_Max;
	double z_Max;
	double x_Min;
	double y_Min;
	double z_Min;
	double radius = 5000000.0f;


	void addObject(std::shared_ptr<GameObject> object);
	std::vector<std::shared_ptr<GameObject>> space_objects;
	std::shared_ptr<Sector> SuperSector;
	bool isInSector(double x, double y, double z);

};