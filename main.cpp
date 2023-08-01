#include <iostream>
#include <algorithm>
#include <cassert>

#include "tgaimage.h"
#include "model.h"
#include "light.h"
#include "camera.h"
#include "utility.h"
#include "MyGL.h"


const int width = 800;
const int height = 800;

Light* light = new Light(Vec3f(3,3,3));
Camera* camera = new PerspCamera(Vec3f(0,0,6),Vec3f(0,0,-1), 45, -0.1, -10, 1.0);
Model* model = NULL;

Renderer renderer(width,height,TGAImage::RGB);
Shader* shader = new Shader;

int main(int argc, char** argv)
{
    if (2==argc) model = new Model(argv[1]);
    else  model = new Model("E:\\mixed\\TinyRenderer\\obj\\african_head.obj"); 

    model->LoadDiffuseMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_diffuse.tga");
    model->LoadNormalMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_nm.tga");
    model->LoadSpecularMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_spec.tga");

    model->SetRotation(Vec3f(0,0,0));
    model->SetScale(Vec3f(2,2,2));

    renderer.SetCamera(camera);
    renderer.SetLight(light);

    renderer.Render(model,shader);
    renderer.Save("E:\\mixed\\TinyRenderer\\output.tga");
    
    delete model;
    return 0;
} 