#ifndef CAR_H
#define CAR_H
#include "utils.h"


class Car
{
public:
	Car();
	~Car();

private:
	Lib3dsFile* file;
};

Car::Car()
{
	file=lib3ds_file_open("models/lamborgini/lamborgini.3ds");

}

Car::~Car()
{
	//TODO: add relevant code
}
#endif