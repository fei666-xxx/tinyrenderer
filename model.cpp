#include <iostream>
#include <string>
#include <fstream>
#include <sstream>
#include <vector>
#include "model.h"

Model::Model(const char *filename) : verts_(), faces_() 
{
    std::ifstream in;
    in.open (filename, std::ifstream::in);
    if (in.fail())
    {
        std::cerr << filename << " file open failed\n";
        return;
    } 
    std::string line;
    while (!in.eof()) 
    {
        std::getline(in, line);
        std::istringstream iss(line.c_str());
        char trash;
        if (!line.compare(0, 2, "v ")) 
        {
            iss >> trash;
            Vec3f v;
            for (int i=0;i<3;i++) iss >> v.raw[i];
            verts_.push_back(v);
        }
        else if (!line.compare(0, 2, "vt"))
        {
            iss >> trash >> trash;
            Vec3f tex;
            for (int i=0;i<3;i++)
            {
                iss >> tex.raw[i];
                //std::cout << tex.raw[i] << "\n";
            }
            tex_coords_.push_back(tex);
        }
        else if (!line.compare(0, 2, "vn"))
        {
            iss >> trash >> trash;
            Vec3f n;
            for (int i=0;i<3;i++)
            {
                iss >> n.raw[i];
                //std::cout << tex.raw[i] << "\n";
            }
            normals_.push_back(n);
        }
        else if (!line.compare(0, 2, "f ")) 
        {
            std::vector<int> f;
            std::vector<int> t;
            std::vector<int> n;
            int itrash, idx, idx_t, idx_n;
            iss >> trash;
            while (iss >> idx >> trash >> idx_t >> trash >> idx_n) 
            {
                idx--; // in wavefront obj all indices start at 1, not zero
                f.push_back(idx);

                idx_t--;
                t.push_back(idx_t);

                idx_n--;
                n.push_back(idx_n);
            }
            faces_.push_back(f);
            uv_.push_back(t);
            normal_idx_.push_back(n);
        }
    }
    std::cerr << "# v# " << verts_.size() << " f# "  << faces_.size() << std::endl;
}

Model::~Model() {}

int Model::nverts() 
{
    return (int)verts_.size();
}

int Model::nfaces() 
{
    return (int)faces_.size();
}

std::vector<int> Model::face(int idx) 
{
    return faces_[idx];
}

std::vector<int> Model::uvs(int idx) 
{
    return uv_[idx];
}

Vec3f Model::vert(int i) 
{
    return verts_[i];
}

Vec3f Model::texcoords(int i) 
{
    return tex_coords_[i];
}

Vec3f Model::normals(int i)
{
    return normals_[i];
}

std::vector<int> Model::normal_group(int idx)
{
    return normal_idx_[idx];
}

void Model::LoadDiffuseMap(const char* filename)
{
    diffuseMap.read_tga_file(filename);
}

TGAImage& Model::GetDiffuseMap()
{
    return this->diffuseMap;
}

void Model::LoadNormalMap(const char* filename)
{
    normalMap.read_tga_file(filename);
}

TGAImage& Model::GetNormalMap()
{
    return this->normalMap;
}

void Model::LoadSpecularMap(const char* filename)
{
    specularMap.read_tga_file(filename);
}

TGAImage& Model::GetSpecularMap()
{
    return this->specularMap;
}

Matrix Model::ModelTrans()
{
    return TransToM(transform.position) * 
           RotateXToM(transform.rotation[0]) * 
           RotateYToM(transform.rotation[1]) * 
           RotateZToM(transform.rotation[2]) * 
           ScaleToM(transform.scale);
}

TGAColor Model::GetTexel(Vec3f u_v)
{
    return diffuseMap.get(u_v.x*diffuseMap.width(),u_v.y*diffuseMap.height());
}
Vec3f Model::GetNormal(int iface, int nthvert)
{
    return normals_[normal_idx_[iface][nthvert]];
}
Vec3f Model::GetNormal(Vec3f u_v)
{
    Vec2i uvn(u_v.x*normalMap.width(),u_v.y*normalMap.height());
    TGAColor c = normalMap.get(uvn[0],uvn[1]);
    Vec3f res;
    for(int i=0;i<3;i++) res.raw[2-i] = (float)c[i]/255.f*2.f - 1.f;
    return res.normalize();
}
Vec3f Model::GetUV(int iface, int nthvert)
{
    Vec3f u_v = this->tex_coords_[uv_[iface][nthvert]];
    u_v.x = 1-u_v.x;
    u_v.y = 1-u_v.y;
    return u_v;
}
Vec3f Model::GetVert(int iface, int nthvert)
{
    return verts_[faces_[iface][nthvert]];
}
float Model::GetSpecular(Vec3f u_v)
{
    return specularMap.get(u_v.x*specularMap.width(),u_v.y*specularMap.height()).bgra[0]*1.0f;
}