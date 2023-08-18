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

Light* light = new Light(Vec3f(4,0,4));
//Camera* camera = new PerspCamera(Vec3f(3,4,3),Vec3f(-3,-4,-3), 60.0, -2, -20, 1.0);
Camera* camera = new OrthoCamera(Vec3f(0,0,3),Vec3f(0,0,-3), 5.0, -2, -20, 1.0);
Model* model = NULL;

Model* model2 = NULL;

Renderer renderer(width,height,TGAImage::RGB);
Shader* shader = new Shader;
Shader* shadow_shader= new ShadowShader; 

int main(int argc, char** argv)
{
    if (2==argc) model = new Model(argv[1]);
    else  model = new Model("E:\\mixed\\TinyRenderer\\obj\\african_head.obj"); 

    model->LoadDiffuseMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_diffuse.tga");
    //model->LoadNormalMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_nm.tga");
    model->LoadNormalMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_nm_tangent.tga");
    model->LoadSpecularMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_spec.tga");

    //model->SetPosition(Vec3f(1,0,0));
    //model->SetRotation(Vec3f(0,-30,0));
    //model->SetScale(Vec3f(0.5,0.5,0.5));


    model2 = new Model("E:\\mixed\\TinyRenderer\\obj\\african_head.obj"); 
    model2->LoadDiffuseMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_diffuse.tga");
    model2->LoadNormalMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_nm_tangent.tga");
    model2->LoadSpecularMap("E:\\mixed\\TinyRenderer\\Texture\\african_head_spec.tga");
    model2->SetPosition(Vec3f(0.75,0,0.75));
    //model2->SetScale(Vec3f(0.5,0.5,0.5));



    light->SetDir(light->transform.position);

    renderer.SetCamera(camera);
    renderer.SetLight(light);

    renderer.RenderShadow(model,shadow_shader);
    renderer.RenderShadow(model2,shadow_shader);
    renderer.Render(model,shader);
    renderer.Render(model2,shader);
    renderer.Save("E:\\mixed\\TinyRenderer\\output.tga");
    


    // Vec4f p(9,4,18.072,1);
    // std::cout << p;
    // Camera* camera = new PerspCamera(Vec3f(0,10,-10),Vec3f(0,-1,std::sqrt(3)), 60.0, -5, -40, 1.333);
    // p = camera->ViewTrans() * p;
    // std::cout << p;
    // p = camera->ProjectTrans() * p;
    // std::cout << p;
    // std::cout << p.NormalByW();
    // p = renderer.ScreenTrans() * p;
    // std::cout << p;


    delete model;
    delete shader;

    return 0;
} 