#include "MyGL.h"

void line(int x0, int y0, int x1, int y1, TGAImage& image, TGAColor color)
{
    bool steep=false;
    
    //先判断斜率，再判断横坐标，两者顺序不能颠倒
    if(std::abs(x1-x0)<std::abs(y1-y0))
    {
        steep = true;
        std::swap(x0,y0);
        std::swap(x1,y1);
    }
    if(x0>x1)
    {
        std::swap(x0,x1);
        std::swap(y0,y1);
    }

    int dx = x1-x0;
    int dy = y1-y0;
    int tagent = std::abs(dy) * 2;
    int error = 0;
    int y = y0;

    for(int x=x0; x<=x1; x++)
    {
        if(steep) image.set(y,x,color);
        else image.set(x,y,color);

        error += tagent;
        if(error>dx)
        {
            y += (y1>y0?1:-1);
            error -= 2*dx;
        }
    }
}

int getCrossPointX(int y, Vec2i v0, Vec2i v1)
{
    if(v0.y==v1.y) return v0.x;
    if(v0.x > v1.x) std::swap(v0,v1);
    return v0.x + (int)((float)(v1.x-v0.x)*(y-v0.y)/(float)(v1.y-v0.y));
}

void triangle(Vec2i v0, Vec2i v1, Vec2i v2, TGAImage& image, TGAColor color)
{
    //sort points
    if(v0.y>v1.y) std::swap(v0,v1);
    if(v0.y>v2.y) std::swap(v0,v2);
    if(v1.y>v2.y) std::swap(v1,v2);

    for(int y=v0.y; y<=v2.y; y++)
    {
        int x0,x1;
        x1 = getCrossPointX(y, v0, v2);
        if(y<v1.y)
        {
            x0 = getCrossPointX(y, v1, v0);
        }
        else
        {
            x0 = getCrossPointX(y, v1, v2);
        }
        if(x0>x1) std::swap(x0,x1);
        for(int x=x0;x<=x1;x++) image.set(x,y,color);
    }

}

bool InTriangle(Vec2i* points, Vec2i v)
{
    Vec3i c1 = Vec3i(points[0]-v,0) ^ Vec3i(points[1]-v,0);
    Vec3i c2 = Vec3i(points[1]-v,0) ^ Vec3i(points[2]-v,0);
    Vec3i c3 = Vec3i(points[2]-v,0) ^ Vec3i(points[0]-v,0);
    return  (c1.z>=0 && c2.z>=0 && c3.z>=0) || (c1.z<=0 && c2.z<=0 && c3.z<=0);
}

Vec3f BaryCentric(Vec3f* points, Vec3f v)
{
    Vec3f vec = Vec3f(points[1].x-points[0].x, points[2].x-points[0].x, points[0].x-v.x) ^
                Vec3f(points[1].y-points[0].y, points[2].y-points[0].y, points[0].y-v.y);
    if(std::abs(vec.z)<1) return Vec3f(-1.0,0.0,0.0);
    return Vec3f(1.0-vec.x/vec.z-vec.y/vec.z, vec.x/vec.z, vec.y/vec.z);
}

void triangle(Vec3f* points, float* zbuffer, TGAImage& image, Shader* shader, Model* model)
{
    Vec2i bbox_min = {image.width()-1,image.height()-1};
    Vec2i bbox_max = {0,0};
    Vec2i clamp = {image.width()-1,image.height()-1};

    for(int i=0;i<3;i++)
    {
        bbox_min.x = std::max(0,std::min(bbox_min.x, (int)points[i].x));
        bbox_min.y = std::max(0,std::min(bbox_min.y, (int)points[i].y));

        bbox_max.x = std::min(clamp.x,std::max(bbox_max.x, (int)points[i].x));
        bbox_max.y = std::min(clamp.y,std::max(bbox_max.y, (int)points[i].y));
    }

    Vec3f temp;
    for(temp.x=bbox_min.x; temp.x<=bbox_max.x; temp.x++)
    {
        for(temp.y=bbox_min.y; temp.y<=bbox_max.y; temp.y++)
        {
            //if(InTriangle(points,temp)) image.set(temp.x, temp.y, color);
            Vec3f bc_screen = BaryCentric(points,temp);
            if(bc_screen.x<-EPS || bc_screen.y<-EPS || bc_screen.z<-EPS) continue;
            temp.z = 0;
            for(int i=0;i<3;i++) temp.z += bc_screen[i] * points[i].z;
            
            if(zbuffer[int(temp.x+temp.y*image.width())]<temp.z)
            {
                zbuffer[int(temp.x+temp.y*image.width())]=temp.z;
                TGAColor color = shader->FragmentShader(bc_screen);
                image.set(temp.x, temp.y, color);
            }
        }
    }

}

Vec3f Shader::VertexShader(int iface, int nthvert)
{
    Vec3f vert = model->vert(model->face(iface)[nthvert]);
    Vec4f vert4f = (*S)*(((*PVM)*Vec4f(vert)).NormalByW());

    tex_coords[nthvert] = model->texcoords(model->uvs(iface)[nthvert]);
    // TGAColor c = model->GetNormalMap().get((tex_coords[nthvert].x)*model->GetNormalMap().width(),
    //                                        (1-tex_coords[nthvert].y)*model->GetNormalMap().height());
    // normals[nthvert] = Vec3f(c.bgra[0],c.bgra[1],c.bgra[2]).normalize();
    // //intensity[nthvert] = normals[nthvert] * (lightPos-Vec3f(model->ModelTrans()*Vec4f(vert))).normalize();
    // intensity[nthvert] = std::max<float>(0,normals[nthvert] * (Vec3f(3,3,0).normalize()));

    intensity[nthvert] = std::max<float>(0,model->normals(model->normal_group(iface)[nthvert]) * (Vec3f(3,3,0).normalize()));
    

    return Vec3f(std::round(vert4f.x),std::round(vert4f.y),vert4f.z);
}

TGAColor Shader::FragmentShader(Vec3f bary)
{
    Vec3f uv = tex_coords[0]*bary[0] + tex_coords[1]*bary[1] + tex_coords[2]*bary[2];
    Vec3f normal = normals[0]*bary[0] + normals[1]*bary[1] + normals[1]*bary[1];
    int w = model->GetDiffuseMap().width();
    int h = model->GetDiffuseMap().height();
    float inten = intensity[0]*bary[0] + intensity[1]*bary[1] + intensity[2]*bary[2];
    //return model->GetDiffuseMap().get(uv.x*w,h-uv.y*h);
    return TGAColor(inten*255, inten*255, inten*255,255);
}

void Shader::SetPVM(Matrix* PVM)
{
    this->PVM = PVM;
}
void Shader::SetS(Matrix* S)
{
    this->S = S;
}
void Shader::SetModel(Model* model)
{
    this->model = model;
}
void Shader::SetLightPos(Vec3f p)
{
    lightPos = p;
}
void Shader::SetCameraPos(Vec3f p)
{
    cameraPos = p;
}

Renderer::Renderer(int width_, int height_, TGAImage::Format color_):image(width_,height_,color_), width(width_), height(height_)
{
    zbuffer = new float[width_*height_];
    for(int i=0;i<width_*height_;i++) zbuffer[i] = std::numeric_limits<float>::lowest();
}

void Renderer::Render(Model* model, Shader* shader)
{
    Matrix PVM = PVTrans() * model->ModelTrans();
    Matrix S = ScreenTrans();

    shader->SetPVM(&PVM);
    shader->SetS(&S);
    shader->SetModel(model);
    shader->SetLightPos(light->transform.position);
    shader->SetCameraPos(camera->transform.position);

    for(int i=0;i<model->nfaces();i++)
    {
        Vec3f points[3];

        for(int j=0;j<3;j++)
        {
            points[j] = shader->VertexShader(i,j);
        }
        triangle(points,zbuffer,image,shader,model);
    }
}
void Renderer::SetCamera(Camera* c)
{
    this->camera = c;
}
void Renderer::SetLight(Light* l)
{
    this->light = l;
}

Matrix Renderer::ScreenTrans()
{
    Matrix screenTrans;
    screenTrans[0][0] = width/2;
    screenTrans[0][3] = width/2;
    screenTrans[1][1] = height/2;
    screenTrans[1][3] = height/2;
    screenTrans[2][2] = 1;
    screenTrans[3][3] = 1;
    return screenTrans;
}

Matrix Renderer::PVTrans()
{
    return camera->ProjectTrans() * camera->ViewTrans();
}

void Renderer::Save(const char* filename)
{
    image.flip_vertically();
    image.write_tga_file(filename);
}

Renderer::~Renderer()
{
    delete camera;
    delete light;
    delete[] zbuffer;
}