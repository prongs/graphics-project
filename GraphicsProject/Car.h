#ifndef CAR_H
#define CAR_H
#include "utils.h"
using namespace std;
class Car
{
public:
	Car();
	~Car();
	void display();
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
void Car::display()
{
	for(int i=0;i<file->nmeshes;i++)
	{
		Lib3dsMesh* mesh=file->meshes[i];
		float* v=mesh->vertices[0];
		for (int j = 0; j < mesh->nfaces; j++)
		{
			Lib3dsFace* face= mesh->faces+i;
			glColor3f(1.0,0.0,0.0);
			//debug(mesh->vertices[0][face->index[0]],mesh->vertices[1][face->index[0]],mesh->vertices[2][face->index[0]]);
			glVertex3f(mesh->vertices[0][face->index[0]],mesh->vertices[1][face->index[0]],mesh->vertices[2][face->index[0]]);
		}
		glPopMatrix();
	}
}
#endif
