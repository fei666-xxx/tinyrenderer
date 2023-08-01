#ifndef __MODEL_H__
#define __MODEL_H__

#include <vector>
#include "geometry.h"
#include "BaseObject.h"
#include "utility.h"
#include "tgaimage.h"

class Model: public BaseObject
{
private:
	std::vector<Vec3f> verts_;
	std::vector<std::vector<int> > faces_;

	std::vector<Vec3f> tex_coords_;
	std::vector<std::vector<int> > uv_;

	std::vector<Vec3f> normals_;
	std::vector<std::vector<int> > normal_idx_;

	TGAImage diffuseMap;
	TGAImage normalMap;
	TGAImage specularMap;


public:
	Model(const char *filename);
	~Model();
	int nverts();
	int nfaces();
	Vec3f vert(int i);
	Vec3f texcoords(int i);
	Vec3f normals(int i);
	std::vector<int> face(int idx);
	std::vector<int> uvs(int idx);
	std::vector<int> normal_group(int idx);

	void LoadDiffuseMap(const char* filename);
	TGAImage& GetDiffuseMap();
	void LoadNormalMap(const char* filename);
	TGAImage& GetNormalMap();
	void LoadSpecularMap(const char* filename);
	TGAImage& GetSpecularMap();
	Matrix ModelTrans();
};

#endif