#include "kv6.h"

#include <cstdio>
#include <array>
#include <cstddef>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "fmt/format.h"

#include "common.h"
#include "util/except.h"

namespace {
    // adapted from SLAB6.C by Ken Silverman (http://advsys.net/ken)
    constexpr float GOLDRAT = 0.3819660112501052f;
    constexpr float PI = 3.141592653589793f;
    constexpr int TABLE_SIZE = 256;

    glm::vec3 equiind2vec(long i, float zmulk, float zaddk) {
        float z = i * zmulk + zaddk;
        float r = sqrtf(1.f - z * z);
        float v = i * (GOLDRAT * PI * 2);
        float x = cos(v) * r;
        float y = sin(v) * r;
        return { -x, z, -y };
    }

    template<int N>
    std::array<glm::vec3, N> equimemset() {
        const float zmulk = 2.0f / N;
        const float zaddk = zmulk * 0.5f - 1.0f;

        std::array<glm::vec3, N> table{};

//        table[table.size() - 1] = { 0, 0, 0 };
        for (long i = N - 2; i >= 0; --i) {
            table[i] = equiind2vec(i, zmulk, zaddk);
        }
        return table;
    }

    const auto TABLE = equimemset<TABLE_SIZE>();

    struct KV6Vox {
        uint8_t r, g, b, vis, normal;
        uint16_t height;
    };

    struct Vertex {
        glm::vec3 vertex;
        glm::vec3 color;
        glm::vec3 normal; // face normal
        glm::vec3 kv6norm; // kv6 normal
//        uint8_t face; // extra face shading
    };

    void gen_faces(float x, float y, float z, glm::vec3 color, glm::vec3 kv6norm, uint8_t vis, std::vector<Vertex> &v) {
        const float x0 = x - 0.5f, x1 = x + 0.5f;
        const float y0 = y - 0.5f, y1 = y + 0.5f;
        const float z0 = z - 0.5f, z1 = z + 0.5f;

        // vis = 0b11111111;


        if (vis & 1 << 0) {
            //LEFT
            v.push_back({ { x0, y0, z0 }, color,{ -1, 0, 0 }, kv6norm/*, 0*/ });
            v.push_back({ { x0, y1, z0 }, color,{ -1, 0, 0 }, kv6norm/*, 0*/ });
            v.push_back({ { x0, y0, z1 }, color,{ -1, 0, 0 }, kv6norm/*, 0*/ });
            v.push_back({ { x0, y0, z1 }, color,{ -1, 0, 0 }, kv6norm/*, 0*/ });
            v.push_back({ { x0, y1, z0 }, color,{ -1, 0, 0 }, kv6norm/*, 0*/ });
            v.push_back({ { x0, y1, z1 }, color,{ -1, 0, 0 }, kv6norm/*, 0*/ });
        }
        if (vis & 1 << 1) {
            //RIGHT
            v.push_back({ { x1, y0, z0 }, color,{ 1, 0, 0 }, kv6norm,/* 1 */});
            v.push_back({ { x1, y0, z1 }, color,{ 1, 0, 0 }, kv6norm,/* 1 */});
            v.push_back({ { x1, y1, z0 }, color,{ 1, 0, 0 }, kv6norm,/* 1 */});
            v.push_back({ { x1, y1, z0 }, color,{ 1, 0, 0 }, kv6norm,/* 1 */});
            v.push_back({ { x1, y0, z1 }, color,{ 1, 0, 0 }, kv6norm,/* 1 */});
            v.push_back({ { x1, y1, z1 }, color,{ 1, 0, 0 }, kv6norm,/* 1 */});
        }
        if (vis & 1 << 2) {
            //BACK
            v.push_back({ { x0, y0, z0 }, color,{ 0, 0, -1 }, kv6norm/*, 2*/ });
            v.push_back({ { x1, y0, z0 }, color,{ 0, 0, -1 }, kv6norm/*, 2*/ });
            v.push_back({ { x0, y1, z0 }, color,{ 0, 0, -1 }, kv6norm/*, 2*/ });
            v.push_back({ { x0, y1, z0 }, color,{ 0, 0, -1 }, kv6norm/*, 2*/ });
            v.push_back({ { x1, y0, z0 }, color,{ 0, 0, -1 }, kv6norm/*, 2*/ });
            v.push_back({ { x1, y1, z0 }, color,{ 0, 0, -1 }, kv6norm/*, 2*/ });
        }
        if (vis & 1 << 3) {
            //FRONT
            v.push_back({ { x0, y0, z1 }, color,{ 0, 0, 1 }, kv6norm,/* 3 */});
            v.push_back({ { x0, y1, z1 }, color,{ 0, 0, 1 }, kv6norm,/* 3 */});
            v.push_back({ { x1, y0, z1 }, color,{ 0, 0, 1 }, kv6norm,/* 3 */});
            v.push_back({ { x1, y0, z1 }, color,{ 0, 0, 1 }, kv6norm,/* 3 */});
            v.push_back({ { x0, y1, z1 }, color,{ 0, 0, 1 }, kv6norm,/* 3 */});
            v.push_back({ { x1, y1, z1 }, color,{ 0, 0, 1 }, kv6norm,/* 3 */});
        }
        if (vis & 1 << 4) {
            //TOP
            v.push_back({ { x0, y1, z0 }, color,{ 0, 1, 0 }, kv6norm,/* 4 */});
            v.push_back({ { x1, y1, z0 }, color,{ 0, 1, 0 }, kv6norm,/* 4 */});
            v.push_back({ { x0, y1, z1 }, color,{ 0, 1, 0 }, kv6norm,/* 4 */});
            v.push_back({ { x0, y1, z1 }, color,{ 0, 1, 0 }, kv6norm,/* 4 */});
            v.push_back({ { x1, y1, z0 }, color,{ 0, 1, 0 }, kv6norm,/* 4 */});
            v.push_back({ { x1, y1, z1 }, color,{ 0, 1, 0 }, kv6norm,/* 4 */});
        }
        if (vis & 1 << 5) {
            //BOTTOM
            v.push_back({ { x0, y0, z0 }, color,{ 0, -1, 0 }, kv6norm/*, 5*/ });
            v.push_back({ { x0, y0, z1 }, color,{ 0, -1, 0 }, kv6norm/*, 5*/ });
            v.push_back({ { x1, y0, z0 }, color,{ 0, -1, 0 }, kv6norm/*, 5*/ });
            v.push_back({ { x1, y0, z0 }, color,{ 0, -1, 0 }, kv6norm/*, 5*/ });
            v.push_back({ { x0, y0, z1 }, color,{ 0, -1, 0 }, kv6norm/*, 5*/ });
            v.push_back({ { x1, y0, z1 }, color,{ 0, -1, 0 }, kv6norm/*, 5*/ });
        }
    }
}


KV6Mesh::KV6Mesh(const std::string &name) {
    FILE *f = fopen(name.c_str(), "rb");
    if (!f) THROW_ERROR("COULDN'T OPEN KV6Mesh FILE {}", name);

    char magic[5];
    fread(magic, 4, 1, f);
    magic[4] = '\0';
    if (strcmp(magic, "Kvxl") != 0) THROW_ERROR("INVALID KV6Mesh FILE MAGIC {}", name);

    fread(&xsiz, sizeof(xsiz), 1, f); fread(&ysiz, sizeof(ysiz), 1, f); fread(&zsiz, sizeof(zsiz), 1, f);
    fread(&xpiv, sizeof(xpiv), 1, f); fread(&ypiv, sizeof(ypiv), 1, f); fread(&zpiv, sizeof(zpiv), 1, f);
    fread(&num_voxels, sizeof(num_voxels), 1, f);

    

    KV6Vox *blocks = new KV6Vox[num_voxels];
    for (long i = 0; i < num_voxels; i++) {
        uint8_t b = fgetc(f);
        uint8_t g = fgetc(f);
        uint8_t r = fgetc(f);
        uint8_t a = fgetc(f);
        uint16_t height;
        fread(&height, sizeof(height), 1, f);
        uint8_t visibility = fgetc(f);
        uint8_t normalindex = fgetc(f);
        blocks[i] = { r, g, b, visibility, normalindex, height };
    }
    fseek(f, xsiz * 4, SEEK_CUR);

    uint16_t *xyoffset = new uint16_t[xsiz * ysiz];
//    fread(&xyoffset, sizeof(uint16_t), xsiz * ysiz, f);
    for (int i = 0; i < xsiz * ysiz; ++i) {
        fread(xyoffset + i, sizeof(*xyoffset), 1, f);
    }
    fclose(f);
    
    std::vector<Vertex> v;
    v.reserve(24 * num_voxels);

    int p = 0;
    for(long x = 0; x < xsiz; x++) {
        for(long y = 0; y < ysiz; y++) {
            uint16_t siz = xyoffset[x * ysiz + y];
            for (uint16_t i = 0; i < siz; i++) {
                KV6Vox b = blocks[p];
                gen_faces(x - xpiv, -b.height + zpiv, y - ypiv, glm::vec3{ b.r, b.g, b.b } / 255.f, TABLE[b.normal], b.vis, v);
                p++;
            }
        }
    }


    glBindVertexArray(vao);

    glBindBuffer(GL_ARRAY_BUFFER, vbo);
    glBufferData(GL_ARRAY_BUFFER, v.size() * sizeof(Vertex), v.data(), GL_STATIC_DRAW);

    glVertexAttribPointer(0, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, vertex)));
    glEnableVertexAttribArray(0);
    glVertexAttribPointer(1, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, color)));
    glEnableVertexAttribArray(1);                                               
    glVertexAttribPointer(2, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, normal)));
    glEnableVertexAttribArray(2);                                             
    glVertexAttribPointer(3, 3, GL_FLOAT, GL_FALSE, sizeof(Vertex), reinterpret_cast<void*>(offsetof(Vertex, kv6norm)));
    glEnableVertexAttribArray(3);
//    glVertexAttribIPointer(4, 1, GL_UNSIGNED_BYTE, sizeof(Vertex),  reinterpret_cast<void*>(offsetof(Vertex, face)));
//    glEnableVertexAttribArray(4);

    this->vertices = v.size();

    delete[] blocks;
    delete[] xyoffset;
}

// p0 -> position of ray
// v0 -> direction of ray
// *h -> set to the (world) position of the hit (unchanged if not hit)
// Adapted from voxlap5.c by Ken Silverman
bool KV6::sprhitscan(glm::vec3 p0, glm::vec3 v0, glm::vec3 *h) {
    // todo this isn't accurate
    glm::mat4 mm = this->get_model();
    glm::vec3 ss = -ace::draw2vox(glm::vec3{ mm[0] });
    glm::vec3 sh = -ace::draw2vox(glm::vec3{ mm[1] });
    glm::vec3 sf = ace::draw2vox(glm::vec3{ mm[2] });
    glm::vec3 v(dot(v0, ss), dot(v0, sh), dot(v0, sf));

    glm::vec3 sprp(ace::draw2vox(this->position));
    glm::vec3 t(p0 - sprp);
    glm::vec3 u(dot(t, ss), dot(t, sh), dot(t, sf));
    u /= glm::vec3{ dot(ss, ss), dot(sh, sh), dot(sf, sf) };
    u += glm::vec3{ this->mesh->xpiv, this->mesh->ypiv, this->mesh->zpiv };

    int ix0 = 0;
    int ix1 = this->mesh->xsiz;
    float g = ix0;
    t.x = float(ix1);               (*(long *)&t.x)--;
    t.y = float(this->mesh->ysiz);  (*(long *)&t.y)--;
    t.z = float(this->mesh->zsiz);  (*(long *)&t.z)--;
    float f;
    if (u.x <   g) { if (v.x <= 0) return false; f = (g - u.x) / v.x; u.x = g; u.y += v.y*f; u.z += v.z*f; }
    else if (u.x > t.x) { if (v.x >= 0) return false; f = (t.x - u.x) / v.x; u.x = t.x; u.y += v.y*f; u.z += v.z*f; }
    if (u.y <   0) { if (v.y <= 0) return false; f = (0 - u.y) / v.y; u.y = 0; u.x += v.x*f; u.z += v.z*f; }
    else if (u.y > t.y) { if (v.y >= 0) return false; f = (t.y - u.y) / v.y; u.y = t.y; u.x += v.x*f; u.z += v.z*f; }
    if (u.z <   0) { if (v.z <= 0) return false; f = (0 - u.z) / v.z; u.z = 0; u.x += v.x*f; u.y += v.y*f; }
    else if (u.z > t.z) { if (v.z >= 0) return false; f = (t.z - u.z) / v.z; u.z = t.z; u.x += v.x*f; u.y += v.y*f; }

    glm::ivec3 a(u.x - 0.5, u.y - 0.5, u.z - 0.5);
    if ((unsigned long)(a.x - ix0) >= ix1) return false;
    if ((unsigned long)a.y >= this->mesh->ysiz) return false;
    if ((unsigned long)a.z >= this->mesh->zsiz) return false;

    u -= glm::vec3{ this->mesh->xpiv, this->mesh->ypiv, this->mesh->zpiv };
    *h = glm::vec3{ dot(v, {ss.x, sh.x, sf.x}), dot(v, {ss.y, sh.y, sf.y}), dot(v, {ss.z, sh.z, sf.z}) } + sprp;
    return true;
}