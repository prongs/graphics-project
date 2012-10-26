#ifndef CAR_H
#define CAR_H
#include "utils.h"
#include <string.h>
using namespace std;
class Car
{
public:
	Car();
	~Car();
	void display();
	void render_node(Lib3dsNode*);
	void generate_display_lists();
private:
	Lib3dsFile* file;
};

Car::Car()
{
	file=lib3ds_file_open("models/lamborgini/lamborgini.3ds");
	generate_display_lists();
}

Car::~Car()
{
	//TODO: add relevant code
	lib3ds_file_free(file);
}
void Car::display()
{
	for(Lib3dsNode* node = file->nodes;node!=NULL;node=node->next)
	{
		render_node(node);
	}	
}

void Car::generate_display_lists()
{
	Lib3dsNode		*p;
	Lib3dsMesh		*mesh;
	Lib3dsFace		*face;
	Lib3dsMaterial  *mat;
	Lib3dsMeshInstanceNode *meshData;
	float	(*norm_verts)[3];
	float   (*norm_faces)[3];
	float		M[4][4];
	int i;
	for(Lib3dsNode* node = file->nodes;node!=NULL;node=node->next)
	{
		if(node->type==LIB3DS_NODE_MESH_INSTANCE)
		{
			mesh=lib3ds_file_mesh_for_node(file, node);
			node->user_id=glGenLists(1);
			glNewList(node->user_id, GL_COMPILE);
			norm_verts=(float(*)[3])malloc(3*sizeof(*norm_verts)*mesh->nfaces);
			norm_faces = (float(*)[3]) malloc(sizeof(*norm_faces)*mesh->nfaces);
			lib3ds_matrix_copy(M, mesh->matrix);
			lib3ds_matrix_inv(M);
			glMultMatrixf(&M[0][0]);
			lib3ds_mesh_calculate_face_normals(mesh, norm_faces);
			lib3ds_mesh_calculate_vertex_normals(mesh, norm_verts);
			for (int fi = 0; fi < mesh->nfaces; ++fi)
			{
				face=mesh->faces+fi;
				mat=NULL;
				if (face->material>=0 && face->material< file->nmaterials)
					mat=file->materials[face->material];
				if (mat)
				{
					float s = pow(2, 10.0*mat->shininess);
					if (s>128.0) s = 128.0f;
					glMaterialfv(GL_FRONT, GL_AMBIENT, mat->ambient);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, mat->diffuse);
					glMaterialfv(GL_FRONT, GL_SPECULAR, mat->specular);
					glMaterialf(GL_FRONT, GL_SHININESS, s);
				}
				else {
					float a[]={0.2, 0.2, 0.2, 1.0};
					float d[]={0.8, 0.8, 0.8, 1.0};
					float s[]={0.0, 0.0, 0.0, 1.0};
					glMaterialfv(GL_FRONT, GL_AMBIENT, a);
					glMaterialfv(GL_FRONT, GL_DIFFUSE, d);
					glMaterialfv(GL_FRONT, GL_SPECULAR, s);
				}
				// Draw tri-face
				glBegin(GL_TRIANGLES);
				glNormal3fv(norm_faces[fi]);	// face normal
				for (i=0; i<3; ++i) {
					glNormal3fv(norm_verts[3*fi+i]);	// vertex normal
					glVertex3fv(mesh->vertices[face->index[i]]);
				}
				glEnd();
			}
			free(norm_faces);
			free(norm_verts);
			glEndList();
		}
	}
}
void Car::render_node(Lib3dsNode* node)
{
	for(Lib3dsNode* p=node->childs;p!=NULL;p=p->next)
	{
		render_node(p);
	}
	if (node->type==LIB3DS_NODE_MESH_INSTANCE)
	{
		glPushMatrix();
		Lib3dsMeshInstanceNode* meshData= (Lib3dsMeshInstanceNode*)node;
		glMultMatrixf(&node->matrix[0][0]);
		glTranslatef(-meshData->pivot[0], -meshData->pivot[1], -meshData->pivot[2]);
		glCallList(node->user_id);
		glPopMatrix();
	}
}
#endif