#include "draw/billboard.h"

namespace ace { namespace draw {
    // at this point BillboardManager is now probably defunct, and now replaceable with
    // a generic vao/vbo pair.
    BillboardManager::BillboardManager() {
        this->vao.attrib_pointer("3f,3f,1f", this->vbo.handle);
    }

    void BillboardManager::draw(Billboard bb) {
        this->vbo->emplace_back(bb);
    }

    void BillboardManager::draw(gl::ShaderProgram &s) {
        this->vbo.upload();
        this->vao.draw(GL_POINTS, this->vbo.draw_count, this->vbo.draw_offset);
    }
}}
