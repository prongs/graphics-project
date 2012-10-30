#ifndef Model_H
#define Model_H
#include "utils.h"
#include <string.h>
#include <string>
#include <sstream>
#include "BitmapTexture.h"
using namespace std;

class Model
{
public:
	Model(const char* filename);
	~Model();
	void display();
	void render_node(Lib3dsNode*);
	void generate_display_lists();
	void load_Model_camera(bool);
	void process_node(Lib3dsNode* node);
	void process_materials();
private:
	Lib3dsFile* file;
	bool viewFromCamera;
	float angle;
};

Model::Model(const char* filename)
{
	file=lib3ds_file_open(filename);
	process_materials();
	generate_display_lists();
	viewFromCamera=false;
	angle=-90.0;
}

Model::~Model()
{
	//TODO: add relevant code
	lib3ds_file_free(file);
}
void Model::display()
{
	angle+=0.0;
	if(viewFromCamera)
	{
		float M[4][4];
		int c = lib3ds_file_camera_by_name(file, "Camera001");
		Lib3dsCamera* camera = file->cameras[c];
		Lib3dsCameraNode* camNode = lib3ds_node_new_camera(camera);
		Lib3dsTargetNode* tgtNode = lib3ds_node_new_camera_target(camera);
		glMatrixMode(GL_PROJECTION);
		glLoadIdentity();
		gluPerspective( camera->fov, 128/72, 0.1f, 6000.0);
		glMatrixMode(GL_MODELVIEW);
		glLoadIdentity();
		glRotatef(angle, 1.0, 0,0);
		//glRotatef(-10, 0.0, 0.0,1.0);
		lib3ds_matrix_camera(M, camera->position, tgtNode->pos, camera->roll);
		glMultMatrixf(&M[0][0]);
	}
	for(Lib3dsNode* node = file->nodes;node!=NULL;node=node->next)
	{
		render_node(node);
	}
}
void Model::load_Model_camera(bool b)
{
	viewFromCamera=b;
}
void Model::generate_display_lists()
{
	for(Lib3dsNode* node = file->nodes;node!=NULL;node=node->next)
	{
		process_node(node);
		for(Lib3dsNode* p=node->childs;p!=NULL;p=p->next)
		{
			process_node(p);
		}
	}
}

void Model::process_node(Lib3dsNode* node)
{
	Lib3dsMesh		*mesh;
	Lib3dsFace		*face;
	Lib3dsMaterial  *mat;
	Lib3dsMeshInstanceNode *meshData;
	float	(*norm_verts)[3];
	float   (*norm_faces)[3];
	float		M[4][4];
	if(node->type==LIB3DS_NODE_MESH_INSTANCE && strcmp(node->name, "$$$DUMMY") != 0)
	{
		mesh=lib3ds_file_mesh_for_node(file, node);
		node->user_id=glGenLists(1);
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
      bool map_textures = mat && mat->texture1_map.name[0];
      GLenum error;
      if (map_textures) 
      {
        glBindTexture(GL_TEXTURE_2D, mat->texture1_map.user_id);
      }
      error = glGetError();
			glBegin(GL_TRIANGLES);
			glNormal3fv(norm_faces[fi]);	// face normal
			for (int i=0; i<3; ++i) {
				glNormal3fv(norm_verts[3*fi+i]);	// vertex normal
				if(map_textures) 
        {
					glTexCoord2fv(mesh->texcos[face->index[i]]);
        }
				glVertex3fv(mesh->vertices[face->index[i]]);
			}
			glEnd();
		}
		free(norm_faces);
		free(norm_verts);
		glEndList();
	}
}

void Model::render_node(Lib3dsNode* node)
{
	for(Lib3dsNode* p=node->childs;p!=NULL;p=p->next)
	{
		render_node(p);
	}
	if(node->type==LIB3DS_NODE_MESH_INSTANCE && strcmp(node->name, "$$$DUMMY") != 0)
	{
		glPushMatrix();
		Lib3dsMeshInstanceNode* meshData= (Lib3dsMeshInstanceNode*)node;
		glMultMatrixf(&node->matrix[0][0]);
		glTranslatef(-meshData->pivot[0], -meshData->pivot[1], -meshData->pivot[2]);
		glCallList(node->user_id);
		glPopMatrix();
	}
}

void Model::process_materials()
{
	for (int i = 0; i < file->nmaterials; i++)
	{
		Lib3dsMaterial* mat = file->materials[i];
		Lib3dsTextureMap* tex = &mat->texture1_map;
		if (tex->name[0])
		{
			stringstream name;
			char fullname[13+64];
			strcpy(fullname, "models/track/");
			strcpy(fullname+13, tex->name);
			LoadGLTextures(fullname, &tex->user_id);
		}
	}
}


#endif
