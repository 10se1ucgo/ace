#include "kv6.h"

#include <cstdio>
#include <array>
#include <cstddef>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "fmt/format.h"

#include "common.h"
#include "util/except.h"


using namespace detail;

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

    const auto NORMAL_TABLE = equimemset<TABLE_SIZE>();

    struct KV6Vox {
        uint8_t r, g, b, vis, normal;
        uint16_t height;
    };

    void gen_faces(float x, float y, float z, glm::vec3 color, glm::vec3 kv6norm, uint8_t vis, ace::gl::experimental::mesh<KV6Vertex> &mesh) {
        const float x0 = x - 0.5f, x1 = x + 0.5f;
        const float y0 = y - 0.5f, y1 = y + 0.5f;
        const float z0 = z - 0.5f, z1 = z + 0.5f;

        // vis = 0b11111111;

        if (vis & 1 << 0) {
            // LEFT
            mesh.add_quad(
                { { x0, y0, z0 }, color, { -1, 0, 0 }, kv6norm },
                { { x0, y0, z1 }, color, { -1, 0, 0 }, kv6norm },
                { { x0, y1, z1 }, color, { -1, 0, 0 }, kv6norm },
                { { x0, y1, z0 }, color, { -1, 0, 0 }, kv6norm }
            );
        }
        if (vis & 1 << 1) {
            // RIGHT
            mesh.add_quad(
                { { x1, y0, z0 }, color, { 1, 0, 0 }, kv6norm },
                { { x1, y1, z0 }, color, { 1, 0, 0 }, kv6norm },
                { { x1, y1, z1 }, color, { 1, 0, 0 }, kv6norm },
                { { x1, y0, z1 }, color, { 1, 0, 0 }, kv6norm }
            );
        }
        if (vis & 1 << 2) {
            // BACK
            mesh.add_quad(
                { { x0, y0, z0 }, color, { 0, 0, -1 }, kv6norm },
                { { x0, y1, z0 }, color, { 0, 0, -1 }, kv6norm },
                { { x1, y1, z0 }, color, { 0, 0, -1 }, kv6norm },
                { { x1, y0, z0 }, color, { 0, 0, -1 }, kv6norm }
            );
        }
        if (vis & 1 << 3) {
            // FRONT
            mesh.add_quad(
                { { x0, y0, z1 }, color, { 0, 0, 1 }, kv6norm },
                { { x1, y0, z1 }, color, { 0, 0, 1 }, kv6norm },
                { { x1, y1, z1 }, color, { 0, 0, 1 }, kv6norm },
                { { x0, y1, z1 }, color, { 0, 0, 1 }, kv6norm }
            );
        }
        if (vis & 1 << 4) {
            // TOP
            mesh.add_quad(
                { { x0, y1, z0 }, color, { 0, 1, 0 }, kv6norm },
                { { x0, y1, z1 }, color, { 0, 1, 0 }, kv6norm },
                { { x1, y1, z1 }, color, { 0, 1, 0 }, kv6norm },
                { { x1, y1, z0 }, color, { 0, 1, 0 }, kv6norm }

            );
        }
        if (vis & 1 << 5) {
            // BOTTOM
            mesh.add_quad(
                { { x0, y0, z0 }, color, { 0, -1, 0 }, kv6norm },
                { { x1, y0, z0 }, color, { 0, -1, 0 }, kv6norm },
                { { x1, y0, z1 }, color, { 0, -1, 0 }, kv6norm },
                { { x0, y0, z1 }, color, { 0, -1, 0 }, kv6norm }
            );
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

    fread(&this->xsiz, sizeof(this->xsiz), 1, f); fread(&this->ysiz, sizeof(this->ysiz), 1, f); fread(&this->zsiz, sizeof(this->zsiz), 1, f);
    fread(&this->xpiv, sizeof(this->xpiv), 1, f); fread(&this->ypiv, sizeof(this->ypiv), 1, f); fread(&this->zpiv, sizeof(this->zpiv), 1, f);
    fread(&this->num_voxels, sizeof(this->num_voxels), 1, f);

    

//    KV6Vox *blocks = new KV6Vox[num_voxels];
    std::unique_ptr<KV6Vox[]> blocks(std::make_unique<KV6Vox[]>(this->num_voxels));


    for (long i = 0; i < this->num_voxels; i++) {
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

//    uint16_t *xyoffset = new uint16_t[xsiz * ysiz];
    std::unique_ptr<uint16_t[]> xyoffset(std::make_unique<uint16_t[]>(xsiz * ysiz));
//    fread(&xyoffset, sizeof(uint16_t), xsiz * ysiz, f);
    for (int i = 0; i < xsiz * ysiz; ++i) {
        fread(&xyoffset[i], sizeof(decltype(xyoffset)::element_type), 1, f);
    }
    fclose(f);
    

    int p = 0;
    for(long x = 0; x < this->xsiz; x++) {
        for(long y = 0; y < this->ysiz; y++) {
            uint16_t siz = xyoffset[x * this->ysiz + y];
            for (uint16_t i = 0; i < siz; i++) {
                KV6Vox &b = blocks[p];
                gen_faces(x - this->xpiv, -b.height + this->zpiv, y - this->ypiv, glm::vec3{ b.r, b.g, b.b } / 255.f, NORMAL_TABLE[b.normal], b.vis, this->mesh);
                p++;
            }
        }
    }

    this->mesh.upload();
}

// ray_origin -> position of ray
// ray_direction -> direction of ray
// *h -> set to the (world) position of the hit (unchanged if not hit)
// Adapted from https://gamedev.stackexchange.com/questions/18436/most-efficient-aabb-vs-ray-collision-algorithms/18459#18459
bool KV6::sprhitscan(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 *h) {
    auto piv = glm::vec3{ this->mesh->xpiv, this->mesh->ypiv, this->mesh->zpiv };
    auto siz = glm::vec3{ this->mesh->xsiz, this->mesh->ysiz, this->mesh->zsiz };

    glm::vec3 min = ace::vox2draw(-piv);
    glm::vec3 max = ace::vox2draw(siz - piv);

    glm::mat4 mm = this->get_model();
    glm::mat4 inverse = glm::inverse(mm);

    glm::vec3 r_origin = inverse * glm::vec4(ace::vox2draw(ray_origin), 1.0);
    glm::vec3 r_direction = glm::normalize(inverse * glm::vec4(ace::vox2draw(ray_direction), 0.0));

    glm::vec3 dirfrac = 1.0f / r_direction;
    float t1 = (min.x - r_origin.x) * dirfrac.x;
    float t2 = (max.x - r_origin.x) * dirfrac.x;
    float t3 = (min.y - r_origin.y) * dirfrac.y;
    float t4 = (max.y - r_origin.y) * dirfrac.y;
    float t5 = (min.z - r_origin.z) * dirfrac.z;
    float t6 = (max.z - r_origin.z) * dirfrac.z;

    float tmin = std::max(std::max(std::min(t1, t2), std::min(t3, t4)), std::min(t5, t6));
    float tmax = std::min(std::min(std::max(t1, t2), std::max(t3, t4)), std::max(t5, t6));

    if (tmax < 0 || tmin > tmax) {
        return false;
    }

    *h = ace::draw2vox(glm::vec3(mm * glm::vec4(r_origin + r_direction * tmin, 1.0)));
    return true;
}