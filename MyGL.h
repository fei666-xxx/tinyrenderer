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
protected:
    Vec3f verts[3];
    Vec3f tex_coords[3];
    Vec3f normals[3];
    float intensity[3];

    Vec3f lightPos;
    Vec3f LightDir;
    Vec3f CamereViewDir;

    Matrix* M = NULL;
    Matrix MIT;
    Matrix* V = NULL;
    Matrix* P = NULL;
    Matrix* S = NULL;
    Matrix* M2S = NULL;
    Model* model = NULL;

    float* shadowbuffer;

public:
    virtual Vec3f VertexShader(int iface, int nthvert);
    virtual TGAColor FragmentShader(Vec3f bary);

    void SetM(Matrix* M);
    void SetV(Matrix* V);
    void SetP(Matrix* P);
    void SetS(Matrix* S);
    void SetM2S(Matrix* M2S);
    void SetModel(Model* model);
    void SetLightPos(Vec3f p);
    void SetViewDir(Vec3f p);
    void SetLightDir(Vec3f p);
    void SetShadowBuffer(float* s);
};

class Renderer
{
private:
    Camera* camera=NULL;
    Light* light=NULL;

    float* zbuffer=NULL;
    float* shadowbuffer=NULL;
    TGAImage image;
    TGAImage shadow;

    int width,height;

public:
    Renderer(int width_, int height_, TGAImage::Format color_);
    void Render(Model* model, Shader* shader);
    void RenderShadow(Model* model, Shader* shadow_shader);
    void SetCamera(Camera* c);
    void SetLight(Light* l);
    Matrix ScreenTrans();
    void Save(const char* filename);
    ~Renderer();
};


class ShadowShader: public Shader
{
    virtual Vec3f VertexShader(int iface, int nthvert);
    virtual TGAColor FragmentShader(Vec3f bary);
};

#endif