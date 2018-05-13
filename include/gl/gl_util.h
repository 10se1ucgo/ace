#pragma once
#include <algorithm>

#include "glad/glad.h"

#include "common.h"
#include <SDL.h>

namespace ace { namespace gl {
    typedef void (*TGENERATOR)(GLsizei n, GLuint *handles);
    typedef void (*TDELETER)(GLsizei n, const GLuint *handles);

    template<TGENERATOR TGenerator, TDELETER TDeleter, typename THandle=GLuint>
    struct GLObj {
        THandle handle;

        GLObj() {
            TGenerator(1, &handle);
        }

        ~GLObj() {
            this->release();
        }

        GLObj(GLObj &&that) noexcept : handle(that.handle) {
            that.handle = 0;
        }

        GLObj &operator=(GLObj &&that) noexcept {
            if (this != &that) {
                this->release();
                std::swap(this->handle, that.handle);
            }
            return *this;
        }

        operator THandle() const { return this->handle; }

        ACE_NO_COPY(GLObj)
    private:
        void release() {
            if (this->handle == 0) return; 
            // i dont think OpenAL even considers 0 is an invalid id, but OH WELL OpenAL Soft does not generate id 0
            // but im not sure what it does when it deletes id 0, so lets avoid that
            TDeleter(1, &this->handle);
            this->handle = 0;
        }
    };

    namespace detail {
        // beccaus i cant use the glad function ptr directly
        // because lambdas can't be constant expressions until C++17 :(
        inline auto gen_vertex_arrays(GLsizei n, GLuint *handles) { return glGenVertexArrays(n, handles); }
        inline auto delete_vertex_arrays(GLsizei n, const GLuint *handles) { return glDeleteVertexArrays(n, handles); }

        inline auto gen_buffers(GLsizei n, GLuint *handles) { return glGenBuffers(n, handles); }
        inline auto del_buffers(GLsizei n, const GLuint *handles) { return glDeleteBuffers(n, handles); }

        inline auto gen_textures(GLsizei n, GLuint *handles) { return glGenTextures(n, handles); }
        inline auto del_textures(GLsizei n, const GLuint *handles) { return glDeleteTextures(n, handles); }
    }

    using vao = GLObj<detail::gen_vertex_arrays, detail::delete_vertex_arrays>;
    using vbo = GLObj<detail::gen_buffers, detail::del_buffers>;
    using texture = GLObj<detail::gen_textures, detail::del_textures>;

    namespace experimental {
        struct vao {
            vao &attrib_pointer(const std::string &format, gl::vbo &buffer, int divisor = 0);

            void draw(GLenum mode, GLsizei count, GLint first = 0) const {
                this->bind();
                glDrawArrays(mode, first, count);
            }

            void draw_instanced(GLenum mode, GLsizei count, GLsizei instance_count, GLint first = 0) const {
                this->bind();
                glDrawArraysInstanced(mode, first, count, instance_count);
            }

            void bind() const {
                glBindVertexArray(this->handle);
            }

            gl::vao handle;
            unsigned num_attributes{ 0 };
        };


        template<typename T>
        struct vbo {
            explicit vbo(GLenum usage = GL_STATIC_DRAW) : usage(usage) {

            }

            void upload() {
                if (this->data.empty()) return;

                glBindBuffer(GL_ARRAY_BUFFER, this->handle);
                if(GLAD_GL_VERSION_4_3) {
                    glInvalidateBufferData(this->handle);
                }
                this->draw_count = this->data.size();
                this->vbo_size = this->data.capacity() * sizeof(T);
                glBufferData(GL_ARRAY_BUFFER, this->vbo_size, nullptr, this->usage);
                glBufferSubData(GL_ARRAY_BUFFER, 0, this->draw_count * sizeof(T), this->data.data());
                this->data.clear();
            }

            std::vector<T> *operator->() { return &this->data; }

            gl::vbo handle;
            std::vector<T> data;
            GLenum usage;
            size_t vbo_size{};
            GLsizei draw_count{};
        };

        template<typename T>
        struct streaming_vbo : vbo<T> {
            streaming_vbo() : vbo<T>(GL_STREAM_DRAW) {

            }

            void upload() {
                if (this->data.empty()) return;

                glBindBuffer(GL_ARRAY_BUFFER, this->handle);
                if (this->data.size() * sizeof(T) + this->current_offset > this->vbo_size) {
                    this->vbo_size = this->data.capacity() * sizeof(T) * 2; // steal the vector's geometric growth pattern B)
                    this->current_offset = 0;
                    glInvalidateBufferData(this->handle);
                    glBufferData(GL_ARRAY_BUFFER, this->vbo_size, nullptr, this->usage);
                }

                void *buffer = glMapBufferRange(GL_ARRAY_BUFFER, this->current_offset, this->data.size() * sizeof(T), GL_MAP_WRITE_BIT | GL_MAP_UNSYNCHRONIZED_BIT);
                memcpy(buffer, this->data.data(), this->data.size() * sizeof(T));
                glUnmapBuffer(GL_ARRAY_BUFFER);

                this->draw_offset = this->current_offset / sizeof(T);
                this->current_offset = this->current_offset == 0 ? this->vbo_size / 2 : 0;

                this->draw_count = this->data.size();
                this->data.clear();
            }

            size_t current_offset{}, draw_offset{};
        };

        // RGBA32 texture helper
        struct texture2d {
            static constexpr GLenum target = GL_TEXTURE_2D;
            static constexpr GLenum gl_format = GL_RGBA;
            static constexpr auto sdl_format = SDL_PIXELFORMAT_RGBA32;
            using pixel_type = uint32_t;

            texture2d(int width = 0, int height = 0, std::unique_ptr<pixel_type[]> pixels = nullptr);

            texture2d(SDL_Surface *surface) : texture2d(surface->w, surface->h, nullptr) {
                this->copy_from_surface(surface);
            }

            void copy_from_surface(SDL_Surface *surface);

            void resize(int width, int height, bool upload = true);

            void set_rgba(int x, int y, pixel_type pixel) {
                if(x < 0 || y < 0 || x >= this->width || y >= this->height) {
                    throw std::out_of_range(fmt::format("x: {}/y: {} out of range ({}, {})", x, y, this->width, this->height));
                }

                this->lower = glm::min(this->lower, { x, y });
                this->upper = glm::max(this->upper, { x + 1, y + 1 });

                this->_pixels[x * this->width + y] = pixel;
                this->dirty = true;
            }

            void set_pixel(int x, int y, glm::u8vec4 pixel) {
                this->set_rgba(x, y, pack_bytes(pixel.r, pixel.g, pixel.b, pixel.a));
            }

            pixel_type get_rgba(int x, int y) const {
                if (x < 0 || y < 0 || x >= this->width || y >= this->height) {
                    throw std::out_of_range(fmt::format("x: {}/y: {} out of range ({}, {})", x, y, this->width, this->height));
                }
                return this->_pixels[x * this->width + y];
            }

            glm::u8vec4 get_pixel(int x, int y) const {
                return unpack_rgba(this->get_rgba(x, y));
            }

            void set_wrap_mode(GLenum mode) {
                this->bind(false);
                glTexParameteri(texture2d::target, GL_TEXTURE_WRAP_S, mode);
                glTexParameteri(texture2d::target, GL_TEXTURE_WRAP_T, mode);

            }

            void set_filter_mode(GLenum mode) {
                this->bind(false);
                glTexParameteri(texture2d::target, GL_TEXTURE_MIN_FILTER, mode);
                glTexParameteri(texture2d::target, GL_TEXTURE_MAG_FILTER, mode);
            }

            void upload() {
                if(!this->dirty) {
                    return;
                }
                // TODO mipmap support, region updating
                this->bind(false);
                glTexSubImage2D(texture2d::target, 0, 0, 0, this->width, this->height, texture2d::gl_format, GL_UNSIGNED_BYTE, this->_pixels.get());
                this->dirty = false;
            }

            void full_upload() {
                this->bind(false);
                glTexImage2D(texture2d::target, 0, texture2d::gl_format, this->width, this->height, 0, texture2d::gl_format, GL_UNSIGNED_BYTE, this->_pixels.get());
                this->dirty = false;
            }

            void bind(bool update = true) {
                glBindTexture(texture2d::target, this->handle);
                if(update) this->upload();
            }

            int w() const { return this->width; }
            int h() const { return this->height; }

            // use .get() if you're gonna read often but not write because this marks the texture to be updated
            pixel_type &operator[](const size_t index) {
                this->dirty = true;
                return this->_pixels[index];
            }

            pixel_type operator[](const size_t index) const {
                return this->_pixels[index];
            }

            const pixel_type *get() const { return this->_pixels.get(); }

            gl::texture handle;
        private:
            int width, height;
            std::unique_ptr<pixel_type[]> _pixels;
            bool dirty = true;

            glm::ivec2 lower, upper;
        };
    }
}}
