#include "gl/gl_util.h"
#include "util/except.h"

namespace ace { namespace gl {
    namespace {
        std::vector<std::string> split(const std::string &str, char delim) {
            std::vector<std::string> ret;
            ret.emplace_back("");
            for(const char c : str) {
                if(c == delim) {
                    if(!ret.back().empty())
                        ret.emplace_back("");
                } else {
                    ret.back().push_back(c);
                }
            }
            return ret;
        }

        std::pair<GLenum, size_t> get_attrib_type(char c) {
            switch (c) {
            case 'f': return { GL_FLOAT, sizeof(GLfloat) };
            case 'd': return { GL_DOUBLE, sizeof(GLdouble) };
            case 'b': return { GL_BYTE, sizeof(GLbyte) };
            case 'B': return { GL_UNSIGNED_BYTE, sizeof(GLubyte) };
            case 's': return { GL_SHORT, sizeof(GLshort) }; // prob should be h/H but theres no string attrib type so its ok B)
            case 'S': return { GL_UNSIGNED_SHORT, sizeof(GLushort) };
            case 'i': return { GL_INT, sizeof(GLint) };
            case 'I': return { GL_UNSIGNED_INT, sizeof(GLuint) };
            default: return { 0, 0 };
            }
        }

        struct VertexAttribute {
            size_t index;
            int components;
            GLenum type;
            size_t size;
            GLboolean normalized;
        };
    }

    namespace experimental {
        /*
        format string: comma seperated list of generic vertex attributes
        "<components><type>[n],..."

        Number of components MUST be between [1, 4]
        Components can also be <n>x<m> denoting a matrix with n colums and m rows.
        (Matrices use n vertex attribute indexes)

        Valid types: f = GLfloat, d = GLdouble, b = GLbyte, s = GLshort, i = GLint.
        Capital denotes unsigned (only for integer types)
        'n' after the type denotes a normalized integer, mapped to a float in the range [0.0, 1.0] (unsigned) or [-1.0, 1.0] (signed)
        Ignored for float type.

        e.g.:
        "2f" == one vec2
        "3i,4f,3x4f" == one ivec3, two vec4, one mat3x4
        "1Bn" == one unsigned byte normalized to glsl float between [0.0, 1.0]

        LIMITATION: VERTEX BUFFER *MUST* BE TIGHTLY PACKED (TODO: fix)
        */
        vao &vao::attrib_pointer(const std::string &format, gl::vbo &buffer, int divisor) {
            size_t stride = 0;

            this->bind();
            glBindBuffer(GL_ARRAY_BUFFER, buffer);

            std::vector<VertexAttribute> attribs;
            for (auto &fmt : split(format, ',')) {

                int num, offset;
                if (fmt.at(1) == 'x') {
                    num = fmt.at(0) - '0';
                    offset = 2;
                } else {
                    num = 1;
                    offset = 0;
                }

                int components = fmt.at(0 + offset) - '0';
                std::pair<GLenum, size_t> attrib(get_attrib_type(fmt.at(1 + offset)));
                GLenum type = attrib.first;
                size_t size = attrib.second;
                bool normalized = false;
                if(fmt.length() > 2u + offset && fmt.at(2 + offset) != 'n' && type != GL_FLOAT) {
                    normalized = true;
                }

                if (size == 0) {
                    THROW_ERROR("INVALID VAO FORMAT STRING {}: UNRECOGNIZED TYPE IN '{}'\n", format, fmt);
                }
                if (components < 1 || components > 4 || num < 1 || num > 4) {
                    THROW_ERROR("INVALID VAO FORMAT STRING {}: TOO MANY COMPONENTS IN '{}'\n", format, fmt);
                }

                for (int i = 0; i < num; i++) {
                    stride += size * components;
                    attribs.push_back({ this->num_attributes++, components, type, size, normalized });
                    // need to calculate total stride before we can call glVertexAttribPointer
                }
            }

            size_t pointer = 0;
            for (auto &attr : attribs) {
                if (attr.type == GL_FLOAT || attr.normalized) {
                    glVertexAttribPointer(attr.index, attr.components, attr.type, attr.normalized, stride, reinterpret_cast<void *>(pointer));
                } else {
                    glVertexAttribIPointer(attr.index, attr.components, attr.type, stride, reinterpret_cast<void *>(pointer));
                }
                glEnableVertexAttribArray(attr.index);
                glVertexAttribDivisor(attr.index, divisor);

                pointer += attr.size * attr.components;
            }
            return *this;
        }

        texture2d::texture2d(int width, int height, std::unique_ptr<pixel_type[]> pixels):
            width(width),
            height(height),
            _pixels(std::move(pixels)) {

            if (this->_pixels == nullptr) {
                this->_pixels = std::make_unique<pixel_type[]>(this->width * this->height);
            } else {
                this->full_upload();
            }

            this->set_filter_mode(GL_LINEAR);
            this->set_wrap_mode(GL_CLAMP_TO_EDGE);
        }

        void texture2d::copy_from_surface(SDL_Surface *surface) {
            if (this->width != surface->w && this->height != surface->h) {
                this->resize(surface->w, surface->h, false);
            }

            bool needs_convert = surface->format->format != texture2d::sdl_format;

            if (needs_convert) {
                surface = SDL_ConvertSurfaceFormat(surface, texture2d::sdl_format, 0);
            }

            assert(surface->pitch * surface->h == this->width * this->height * int(sizeof(decltype(this->_pixels)::
                element_type)));
            std::memcpy(this->_pixels.get(), surface->pixels, surface->pitch * surface->h);

            if (needs_convert) {
                SDL_FreeSurface(surface);
            }

            this->full_upload();
        }

        void texture2d::resize(int width, int height, bool upload) {
            this->width = width;
            this->height = height;
            this->_pixels = std::make_unique<pixel_type[]>(this->width * this->height);

            if (upload) this->full_upload();
        }
    }
}}
