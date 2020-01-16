#include "draw/kv6.h"

#include <cstdio>
#include <array>
#include <cstddef>

#include "glad/glad.h"
#include "glm/glm.hpp"
#include "fmt/format.h"

#include "common.h"
#include "util/except.h"

namespace ace { namespace draw {
    using namespace detail;

    namespace {
        // adapted from SLAB6.C by Ken Silverman (http://advsys.net/ken)
        constexpr float GOLDRAT_2PI = (1 - 1 / glm::golden_ratio<float>()) * glm::two_pi<float>();
        constexpr int TABLE_SIZE = 256;

        glm::vec3 equiind2vec(long i, float zmulk, float zaddk) {
            float z = i * zmulk + zaddk;
            float r = sqrtf(1.f - z * z);
            float v = i * GOLDRAT_2PI;
            float x = cos(v) * r;
            float y = sin(v) * r;
            return { x, z, y };
        }

        template<int N>
        std::array<glm::vec3, N> equimemset() {
            const float zmulk = 2.0f / N;
            const float zaddk = zmulk * 0.5f - 1.0f;

            std::array<glm::vec3, N> table;

            for (long i = N - 2; i >= 0; --i) {
                table[i] = equiind2vec(i, zmulk, zaddk);
            }
            table[N - 1] = glm::vec3{ 0, 0, 0 };
            return table;
        }

        const auto NORMAL_TABLE = equimemset<TABLE_SIZE>();

        void gen_faces(float x, float y, float z, glm::vec3 color, glm::vec3 kv6norm, uint8_t vis, gl::experimental::mesh<KV6Vertex> &mesh) {
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

    KV6Data::KV6Data(const std::string &file) {
        FILE *f = fopen(file.c_str(), "rb");
        if (!f)
            THROW_ERROR("COULD NOT OPEN KV6 FILE {}", file);

        char magic[5];
        fread(magic, 4, 1, f);
        magic[4] = '\0';

        if (strcmp(magic, "Kvxl") != 0)
            THROW_ERROR("INVALID KV6 FILE MAGIC {}", file);

        constexpr auto ssize = sizeof(decltype(this->size)::value_type);
        fread(glm::value_ptr(this->size), ssize, decltype(this->size)::length(), f);
        
        if (any(lessThan(this->size, {1, 1, 1})) || this->size.z > std::numeric_limits<uint16_t>::max())
            THROW_ERROR("INVALID DIMENSIONS FOR KV6 {}: {}", file, this->size);
        
        constexpr auto psize = sizeof(decltype(this->pivot)::value_type);
        fread(glm::value_ptr(this->pivot), psize, decltype(this->pivot)::length(), f);

        fread(&this->num_voxels, sizeof(this->num_voxels), 1, f);
        
        if (this->num_voxels < 0)
            THROW_ERROR("INVALID NUMBER OF VOXELS FOR KV6 {}: {}", file, this->num_voxels);

        this->blocks = std::make_unique<KV6Entry[]>(this->num_voxels);
        for (int i = 0; i < this->num_voxels; i++) {
            uint8_t b = fgetc(f);
            uint8_t g = fgetc(f);
            uint8_t r = fgetc(f);
            uint8_t a = fgetc(f); // dummy, always 128

            uint16_t z;
            fread(&z, sizeof(z), 1, f);

            uint8_t visflags = fgetc(f);
            uint8_t normal = fgetc(f);

            this->blocks[i] = {{r, g, b}, z, visflags, normal };
        }

        // skip x plane surface voxel length
        fseek(f, this->size.x * 4, SEEK_CUR);

        // number of voxels in column (x, y) = column_length[x * size.y + y]
        this->num_columns = size_t(this->size.x) * size_t(this->size.y);
        this->column_lengths = std::make_unique<uint16_t[]>(this->num_columns);
        fread(this->column_lengths.get(), sizeof(this->column_lengths[0]), this->num_columns, f);

        fclose(f);
    }

    KV6Mesh::KV6Mesh(const std::string &file) : data(file) {
        int p = 0;
        for (long x = 0; x < this->data.size.x; x++) {
            for (long y = 0; y < this->data.size.y; y++) {
                uint16_t siz = this->data.column_lengths[x * this->data.size.y + y];
                for (uint16_t i = 0; i < siz; i++) {
                    auto &b = this->data.blocks[p++];
                    gen_faces(x - this->data.pivot.x, -b.z_pos + this->data.pivot.z, y - this->data.pivot.y,
                              glm::vec3{ b.color } / 255.f, NORMAL_TABLE[b.normal_index], b.vis_flags, this->mesh);
                }
            }
        }

        this->mesh.upload();
    }

    bool KV6::sprhitscan(glm::vec3 ray_origin, glm::vec3 ray_direction, glm::vec3 *h) const {
        glm::vec3 piv = this->mesh->data.pivot;
        glm::vec3 siz = this->mesh->data.size;

        glm::vec3 min = vox2draw(-piv);
        glm::vec3 max = vox2draw(siz - piv);

        glm::mat4 mm = this->get_model();
        glm::mat4 inverse = glm::inverse(mm);

        glm::vec3 r_origin = inverse * glm::vec4(ace::vox2draw(ray_origin), 1.0);
        glm::vec3 r_direction = normalize(inverse * glm::vec4(vox2draw(ray_direction), 0.0));

        for (int i = 0; i < decltype(r_direction)::length(); i++) {
            if (r_direction[i] == 0.0f || std::isnan(r_direction[i])) {
                r_direction[i] = 0.0001f;
            }
        }
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

        assert(!glm::any(glm::isnan(r_origin)));
        assert(!glm::any(glm::isnan(r_direction)));

        *h = draw2vox(glm::vec3(mm * glm::vec4(r_origin + r_direction * tmin, 1.0)));
        return true;
    }
}}