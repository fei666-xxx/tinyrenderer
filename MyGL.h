#ifndef __MYGL_H__
#define __MYGL_H__

#include <algorithm>
#include <vector>

#include "tgaimage.h"
#include "geometry.h"
#include "utility.h"
#include "camera.h"
#include "model.h"
#include "light.h"

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color);

int getCrossPointX(int y, Vec2i v0, Vec2i v1);

void triangle(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, TGAColor color);

bool InTriangle(Vec2i* points, Vec2i v);

Vec3f BaryCentric(Vec3f* points, Vec3f v);

void triangle(Vec3f* points, float* zbuffer, TGAImage& image, TGAImage& texture, Vec3f* texcoords);


class Shader
{
private:
    Vec3f tex_coords[3];
    Vec3f normals[3];
    float intensity[3];

    Vec3f lightPos;
    Vec3f cameraPos;

    Matrix* PVM = NULL;
    Matrix* S = NULL;
    Model* model = NULL;

public:
    Vec3f VertexShader(int iface, int nthvert);
    TGAColor FragmentShader(Vec3f bary);

    void SetPVM( Matrix* PVM);
    void SetS(Matrix* S);
    void SetModel(Model* model);
    void SetLightPos(Vec3f p);
    void SetCameraPos(Vec3f p);
};

class Renderer
{
private:
    Camera* camera=NULL;
    Light* light=NULL;

    float* zbuffer=NULL;
    TGAImage image;

    int width,height;

public:
    Renderer(int width_, int height_, TGAImage::Format color_);
    void Render(Model* model, Shader* shader);
    void SetCamera(Camera* c);
    void SetLight(Light* l);
    Matrix ScreenTrans();
    Matrix PVTrans();
    void Save(const char* filename);
    ~Renderer();
};

#endif