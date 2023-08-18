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
    Vec3f vert = model->GetVert(iface, nthvert);
    Vec4f vert4f = (*S)*((((*P)*(*V)*(*M))*Vec4f(vert)).NormalByW());
    //std::cout << vert4f;

    verts[nthvert] = (*M)*Vec4f(vert);
    tex_coords[nthvert] = model->GetUV(iface, nthvert);
    normals[nthvert] = MIT * Vec4f(model->GetNormal(iface,nthvert),0);

    return Vec3f(vert4f);
}

TGAColor Shader::FragmentShader(Vec3f bary)
{
    Vec3f vt = verts[0]*bary[0] + verts[1]*bary[1] + verts[2]*bary[2];
    Vec3f uv = tex_coords[0]*bary[0] + tex_coords[1]*bary[1] + tex_coords[2]*bary[2];
    //Vec3f normal = MIT * model->GetNormal(uv);
    Vec3f bn = (normals[0]*bary[0] + normals[1]*bary[1] + normals[2]*bary[2]).normalize();

    Matrix A(3,3);
    A.SetRow(0, verts[1]-verts[0]);
    A.SetRow(1, verts[2]-verts[0]);
    A.SetRow(2,bn);

    Matrix AI = A.inverse();

    Vec3f T = AI * Vec3f(tex_coords[1][0]-tex_coords[0][0], tex_coords[2][0]-tex_coords[0][0], 0);
    Vec3f B = AI * Vec3f(tex_coords[1][1]-tex_coords[0][1], tex_coords[2][1]-tex_coords[0][1], 0);

    Matrix C(3,3);
    C.SetCol(0,T.normalize());
    C.SetCol(1,B.normalize());
    C.SetCol(2,bn);

    Vec3f normal = (C * model->GetNormal(uv)).normalize();

    float inten = std::max<float>(0,normal * LightDir);
    TGAColor diffuse = model->GetTexel(uv)*inten;

    TGAColor ambient(100,100,100,255);

    Vec3f r = (normal * (normal * LightDir * 2) - LightDir).normalize();
    TGAColor specular = TGAColor(255,255,255,255) * pow(std::max<float>(0,(CamereViewDir * r)), model->GetSpecular(uv));

    //完成shadow mapping，计算当前像素是否在阴影中
    Vec4f vt_shadow = (*S) * (((*M2S) * Vec4f(vt)).NormalByW());
    int idx = int(vt_shadow.x) + int(vt_shadow.y)*800;
    float dd = shadowbuffer[idx];
    //std::cout << dd << " " << vt_shadow.z << "\n";
    float shadow = 0.3 + 0.7*(shadowbuffer[idx]<vt_shadow.z+0.02);
    //float shadow=1;
    // if(shadow>0.8) return TGAColor(255,255,255,255);
    // return TGAColor(255,0,0,255);
    return ambient*0.00 + (diffuse*1.2 + specular*0.2)*shadow;
    //return diffuse;
    //return TGAColor(255,255,255,255)*inten;
}

void Shader::SetM(Matrix* M)
{
    this->M = M;
    this->MIT = (*M).inverse().transpose();
}
void Shader::SetV(Matrix* V)
{
    this->V = V;
}
void Shader::SetP(Matrix* P)
{
    this->P = P;
}
void Shader::SetS(Matrix* S)
{
    this->S = S;
}
void Shader::SetM2S(Matrix* M2S)
{
    this->M2S = M2S;
}
void Shader::SetModel(Model* model)
{
    this->model = model;
}
void Shader::SetLightPos(Vec3f p)
{
    lightPos = p;
}
void Shader::SetViewDir(Vec3f p)
{
    CamereViewDir = p;
}
void Shader::SetLightDir(Vec3f p)
{
    LightDir = p;
}
void Shader::SetShadowBuffer(float* s)
{
    shadowbuffer = s;
}

Renderer::Renderer(int width_, int height_, TGAImage::Format color_):image(width_,height_,color_), 
                                                                    shadow(width_,height_,color_),
                                                                    width(width_), height(height_)
{
    zbuffer = new float[width_*height_];
    for(int i=0;i<width_*height_;i++) zbuffer[i] = std::numeric_limits<float>::lowest();
    shadowbuffer = new float[width_*height_];
    for(int i=0;i<width_*height_;i++) shadowbuffer[i] = std::numeric_limits<float>::lowest();
}

void Renderer::RenderShadow(Model* model, Shader* shadow_shader)
{
    //shadow pass
    Matrix M = model->ModelTrans();
    Matrix V = ViewTrans(light->transform.position, light->LightDir*-1);
    Matrix P = camera->ProjectTrans();
    Matrix S = ScreenTrans();

    shadow_shader->SetM(&M);
    shadow_shader->SetV(&V);
    shadow_shader->SetP(&P);
    shadow_shader->SetS(&S);
    shadow_shader->SetModel(model);
    shadow_shader->SetLightPos(light->transform.position);
    shadow_shader->SetLightDir(light->LightDir.normalize());
    shadow_shader->SetViewDir(light->LightDir.normalize());

    for(int i=0;i<model->nfaces();i++)
    {
        Vec3f points[3];

        for(int j=0;j<3;j++)
        {
            points[j] = shadow_shader->VertexShader(i,j);
        }
        triangle(points,shadowbuffer,shadow,shadow_shader,model);
    }
}

void Renderer::Render(Model* model, Shader* shader)
{
    Matrix M = model->ModelTrans();
    Matrix V = camera->ViewTrans();
    Matrix P = camera->ProjectTrans();
    Matrix S = ScreenTrans();
    Matrix m2s = P*ViewTrans(light->transform.position, light->LightDir*-1);
    Vec3f viewdir = camera->transform.position;


    shader->SetM(&M);
    shader->SetV(&V);
    shader->SetP(&P);
    shader->SetS(&S);
    shader->SetModel(model);
    shader->SetLightPos(light->transform.position);
    shader->SetLightDir(light->LightDir.normalize());
    shader->SetViewDir(viewdir.normalize());
    shader->SetM2S(&m2s);
    shader->SetShadowBuffer(shadowbuffer);


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

void Renderer::Save(const char* filename)
{
    //image.flip_vertically();
    image.write_tga_file(filename);
    shadow.write_tga_file("E:\\mixed\\TinyRenderer\\shadow.tga");
    // for(int i=0;i<800*800;i++)
    // {
    //     if(shadowbuffer[i]>-3)std::cout << shadowbuffer[i] << "\n";
    // }
}

Renderer::~Renderer()
{
    delete camera;
    delete light;
    delete[] zbuffer;
    delete[] shadowbuffer;
}

Vec3f ShadowShader::VertexShader(int iface, int nthvert)
{
    Vec3f vert = model->GetVert(iface, nthvert);
    Vec4f vert4f = (*S)*((((*P)*(*V)*(*M))*Vec4f(vert)).NormalByW());

    verts[nthvert] = vert4f;

    return Vec3f(vert4f);
}

TGAColor ShadowShader::FragmentShader(Vec3f bary)
{
    Vec3f vt = verts[0]*bary[0] + verts[1]*bary[1] + verts[2]*bary[2];
    return TGAColor(255,255,255,255) * ((vt.z+1.0)*0.5);
}