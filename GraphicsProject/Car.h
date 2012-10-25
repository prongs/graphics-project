#ifndef CAR_H
#define CAR_H
#include "utils.h"
using namespace std;
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
	Lib3dsMesh** meshes=file->meshes;
	Lib3dsMesh* mesh;
	cout<<sizeof(meshes)<<endl;
	for(int i=0;i<file->nmeshes;i++)
	{
		mesh=meshes[i];
	}
}

Car::~Car()
{
	//TODO: add relevant code
}
#endif
