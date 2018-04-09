#include "draw/gui.h"

#include "game_client.h"

namespace ace { namespace draw {
    inline bool point_in_rect(glm::vec2 position, glm::vec2 size, glm::vec2 pos) {
        // fmt::print("{} {} {} {}\n", position.x <= pos.x, pos.x <= position.x + size.x, position.y <= pos.y, pos.y <= position.y + size.y);
        return position.x <= pos.x && pos.x <= position.x + size.x &&
               position.y <= pos.y && pos.y <= position.y + size.y;
    }

    void BaseButton::on_mouse_motion(int x, int y, int dx, int dy) {
        if(!this->enabled()) {
            this->hovering = this->pressed = false;
            return;
        }

        bool was_hovering = this->hovering;
        this->hovering = point_in_rect(this->_pos, this->_size, { x, y });
        if(!was_hovering && this->hovering) {
            this->fire("hover_start");
        } else if(was_hovering && !this->hovering) {
            this->fire("hover_end");
        }
    }

    void BaseButton::on_mouse_button(int button, bool pressed) {
        if (!this->enabled()) {
            this->hovering = this->pressed = false;
            return;
        }

        bool was_pressed = this->pressed;
        this->pressed = pressed && button == SDL_BUTTON_LEFT && this->hovering;
        if(was_pressed && !this->pressed) {
            this->fire("press_end");
        } else if(!was_pressed && this->pressed) {
            this->fire("press_start");
        }
    }

    inline draw::SpriteGroup *load_button_image(scene::Scene &scene, const std::string &button_name, const std::string &section, const std::string &type="", int order = 0) {
        auto button(scene.client.sprites.get(fmt::format("ui/common_elements/buttons/{}{}{}.png", button_name, type, section)));
        button->order = order;
        return button;
    }

    Button::ButtonImages::ButtonImages(scene::Scene &s, const std::string &button_name) {
        this->left = load_button_image(s, button_name, "_left", "", -1);
        this->mid = load_button_image(s, button_name, "_mid", "", -2);
        this->right = load_button_image(s, button_name, "_right", "", -1);

        this->left_hover = load_button_image(s, button_name, "_left", "_hover", -1);
        this->mid_hover = load_button_image(s, button_name, "_mid", "_hover", -2);
        this->right_hover = load_button_image(s, button_name, "_right", "_hover", -1);

        this->left_press = load_button_image(s, button_name, "_left", "_press", -1);
        this->mid_press = load_button_image(s, button_name, "_mid", "_press", -2);
        this->right_press = load_button_image(s, button_name, "_right", "_press", -1);
    }

    BitmapButton::BitmapButton(scene::Scene &s, glm::vec2 position, glm::vec2 size,
                               const std::string &image, const std::string &button) :
        BaseButton(s, position, size),
        normal(load_button_image(s, button, "")),
        hover(load_button_image(s, button, "", "_hover")),
        press(load_button_image(s, button, "", "_press")),
        button(this->normal),
        image(!image.empty() ? s.client.sprites.get(image) : nullptr) {

        this->image.alignment = Align::CENTER;

        this->update_position();
    }

    void BitmapButton::draw() {
        this->button.draw();
        if(this->image.group)
            this->image.draw();
    }

    void BitmapButton::update_images() {
        if (this->pressed) {
            this->button.group = this->press;
        } else if (this->hovering) {
            this->button.group = this->hover;
        } else {
            this->button.group = this->normal;
        }

        this->update_position();
    }

    void BitmapButton::update_position() {
        this->button.tint = this->enabled() ? glm::vec4(1.0f) : glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        this->button.scale = this->_size / glm::vec2{ this->button.group->w, this->button.group->h };
        this->button.position = this->_pos;
        this->button.group->order = -1;

        if (this->image.group) {
            this->image.position = this->_pos + this->_size / 2.f;
            if (this->pressed) {
                this->image.position.y += 5 * button.scale.y;
            }
        }
    }

    Button::Button(scene::Scene &s, std::string label, glm::vec2 position, glm::vec2 size, const std::string &font, int font_size, const std::string &button_name) :
        BaseButton(s, position, size),
        label(this->scene.client.fonts.get(font, font_size), std::move(label), { 0, 0, 0 }, { 1, 1 }, Align::CENTER),
        images(s, button_name) {
        this->update_images();

        // fmt::print("TXT: {} SIZE: {} BUTTON SIZE: {}\n", this->label.str(), this->label.size(), this->size());
    }

    void Button::draw() {
        this->left.draw();
        this->mid.draw();
        this->right.draw();
        this->label.draw();
    }

    void Button::update_images() {
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

    void Button::update_position() {
        this->left.scale = glm::vec2(this->_size.y / left.group->h);
        this->right.scale = glm::vec2(this->_size.y / right.group->h);
        this->mid.scale.x = (this->_size.x - (left.w() + right.w())) / mid.group->w;
        this->mid.scale.y = left.scale.y;

        this->left.position = this->_pos;
        this->mid.position = { this->left.position.x + this->left.w(), this->_pos.y };
        this->right.position = { this->mid.position.x + this->mid.w(), this->_pos.y };

        this->label.position = this->mid.position + this->mid.size() / 2.f;
        if (!this->pressed) {
            this->label.position.y -= 5 * mid.scale.y;
        }

        auto color = this->enabled() ? glm::vec4(1.0f) : glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        this->left.tint = this->right.tint = this->mid.tint = color;
    }

    ProgressBar::ProgressBar(scene::Scene &scene, glm::vec2 position, glm::vec2 size, const std::string &image) :
        GUIWidget(scene, position, size),
        bar(scene.client.sprites.get(image)) {

        this->scale = this->size().y / this->bar->h;
    }

    void ProgressBar::draw() {
        float space_between_bullets = (this->scale * this->bar->w + this->padding);
        int total_bullets = this->size().x / space_between_bullets;
        int filled_bullets = total_bullets * (this->value / float(this->range));
        glm::vec2 draw_pos(this->position());
        for(int x = 0; x < filled_bullets; x++) {
            this->bar->draw(glm::vec4{ 1.0f }, draw_pos, 0.0f, glm::vec2(this->scale));
            draw_pos.x += space_between_bullets;
        }
        for (int x = 0; x < total_bullets - filled_bullets; x++) {
            this->bar->draw(glm::vec4{ 0.8f, 0.8f, 0.8f, 1.0f }, draw_pos, 0.0f, glm::vec2(this->scale));
            draw_pos.x += space_between_bullets;
        }
    }

    Frame::Frame(scene::Scene &scene, const std::string &image, std::string title, glm::vec2 position,
                 glm::vec2 title_offset, float size):
        _image(scene.client.sprites.get(image)),
        _title(scene.client.fonts.get("AldotheApache.ttf", 48), std::move(title), glm::vec3(1), glm::vec2(1),
               draw::Align::CENTER) {

        this->_image.group->order = Layer::FRAME;
        this->_image.scale = glm::vec2(size / this->_image.group->h);
        this->_image.position = position;
        this->_image.alignment = draw::Align::CENTER;

        auto p = this->_image.get_position(draw::Align::TOP_LEFT);
        this->_title.position = p + title_offset * this->_image.scale;
    }
}}
