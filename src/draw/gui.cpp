#include "draw/gui.h"

namespace ace { namespace draw {
    inline bool point_in_rect(glm::vec2 position, glm::vec2 size, glm::vec2 pos) {
        // fmt::print("{} {} {} {}\n", position.x <= pos.x, pos.x <= position.x + size.x, position.y <= pos.y, pos.y <= position.y + size.y);
        return position.x <= pos.x && pos.x <= position.x + size.x &&
               position.y <= pos.y && pos.y <= position.y + size.y;
    }

    Button::Button(scene::Scene &scene, glm::vec2 position, glm::vec2 size):
        GUIWidget(scene),
        _pos(position),
        _size(size) {
    }

    void Button::on_mouse_motion(int x, int y, int dx, int dy) {
        bool was_hovering = this->hovering;
        this->hovering = point_in_rect(this->_pos, this->_size, { x, y });
        if(!was_hovering && this->hovering) {
            this->fire("hover_start");
        } else if(was_hovering && !this->hovering) {
            this->fire("hover_end");
        }
    }

    void Button::on_mouse_button(int button, bool pressed) {
        bool was_pressed = this->pressed;
        this->pressed = pressed && button == SDL_BUTTON_LEFT && this->hovering;
        if(was_pressed && !this->pressed) {
            this->fire("press_end");
        } else if(!was_pressed && this->pressed) {
            this->fire("press_start");
        }
    }

    inline draw::SpriteGroup *load_button_image(scene::Scene &scene, const std::string &button_name, const std::string &section, const std::string &type="") {
        return scene.client.sprites.get(fmt::format("ui/common_elements/buttons/{}{}_{}.png", button_name, type, section));
    }

    ButtonImages::ButtonImages(scene::Scene &s, const std::string &button_name) {
        this->left = load_button_image(s, button_name, "left");
        this->mid = load_button_image(s, button_name, "mid");
        this->right = load_button_image(s, button_name, "right");

        this->left_hover = load_button_image(s, button_name, "left", "_hover");
        this->mid_hover = load_button_image(s, button_name, "mid", "_hover");
        this->right_hover = load_button_image(s, button_name, "right", "_hover");

        this->left_press = load_button_image(s, button_name, "left", "_press");
        this->mid_press = load_button_image(s, button_name, "mid", "_press");
        this->right_press = load_button_image(s, button_name, "right", "_press");
    }

    ImageTextButton::ImageTextButton(scene::Scene &s, std::string label, glm::vec2 position, glm::vec2 size, const std::string &font, int font_size, std::string button_name) :
        Button(s, position, size),
        font(this->scene.client.fonts.get(font, font_size)),
        label(this->font, std::move(label), { 0, 0, 0 }, { 1, 1 }, Align::CENTER),
        images(s, button_name) {
        this->update_images();
    }

    void ImageTextButton::draw() {
        this->left.draw();
        this->mid.draw();
        this->right.draw();
        this->label.draw();
    }

    void ImageTextButton::set_position(glm::vec2 position) {
        Button::set_position(position);
        this->update_position();
    }

    void ImageTextButton::set_size(glm::vec2 size) {
        Button::set_size(size);
        this->update_position();
    }

    void ImageTextButton::update_images() {
        if (this->pressed) {
            this->left.group = this->images.left_press;
            this->mid.group = this->images.mid_press;
            this->right.group = this->images.right_press;
        } else if (this->hovering) {
            this->left.group = this->images.left_hover;
            this->mid.group = this->images.mid_hover;
            this->right.group = this->images.right_hover;
        } else {
            this->left.group = this->images.left;
            this->mid.group = this->images.mid;
            this->right.group = this->images.right;
        }

        this->update_position();
    }

    void ImageTextButton::update_position() {
        this->left.scale = glm::vec2((float(this->_size.y) / left.group->h));
        this->right.scale = glm::vec2((float(this->_size.y) / right.group->h));
        this->mid.scale.x = (this->_size.x - (left.w() + right.w())) / mid.group->w;
        this->mid.scale.y = left.scale.y;

        this->left.position = this->_pos;
        this->mid.position = { this->left.position.x + this->left.w(), this->_pos.y };
        this->right.position = { this->mid.position.x + this->mid.w(), this->_pos.y };

        this->label.position = this->mid.position + this->mid.size() / 2.f;
        if (!this->pressed) {
            this->label.position.y -= 5 * mid.scale.y;
        }
    }
}}
