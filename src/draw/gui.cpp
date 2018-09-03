#include "draw/gui.h"

#include "game_client.h"

namespace ace { namespace draw {
    inline bool point_in_rect(glm::vec2 position, glm::vec2 size, glm::vec2 point) {
        return position.x <= point.x && point.x <= position.x + size.x &&
               position.y <= point.y && point.y <= position.y + size.y;
    }

    void GUIWidget::fire(const std::string &event) {
        
        const auto handler(this->handlers.find(event));
        if (handler != this->handlers.end()) {
            handler->second();
        }
    }

    void BaseButton::on_mouse_motion(int x, int y, int dx, int dy) {
        if(!this->enabled()) {
            this->hovering = this->pressed = false;
            return;
        }

        bool was_hovering = this->hovering;
        this->hovering = this->hit_test({ x, y });
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

    bool BaseButton::hit_test(glm::vec2 mouse_position) {
        return point_in_rect(this->_pos, this->_size, mouse_position);
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

    IconButton::IconButton(scene::Scene &s, glm::vec2 position, glm::vec2 size,
                           const std::string &image, glm::vec2 img_scale, const std::string &button) :
        BaseButton(s, position, size),
        normal(load_button_image(s, button, "")),
        hover(load_button_image(s, button, "", "_hover")),
        press(load_button_image(s, button, "", "_press")),
        button(this->normal),
        image(!image.empty() ? s.client.sprites.get(image) : nullptr) {

        this->image.alignment = Align::CENTER;
        if(this->image.group)
            this->image.scale = img_scale;

        this->update_position();
    }

    void IconButton::draw() {
        this->button.draw();
        if(this->image.group)
            this->image.draw();
    }

    void IconButton::update_images() {
        if (this->pressed) {
            this->button.group = this->press;
        } else if (this->hovering) {
            this->button.group = this->hover;
        } else {
            this->button.group = this->normal;
        }

        this->update_position();
    }

    void IconButton::update_position() {
        this->button.tint = this->enabled() ? glm::vec4(1.0f) : glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        this->button.scale = this->_size / glm::vec2{ this->button.group->w(), this->button.group->h() };
        this->button.position = this->_pos;
        this->button.group->order = -1;

        if (this->image.group) {
            this->image.position = this->_pos + this->_size / 2.f;
            if (this->pressed) {
                this->image.position.y += 5 * button.scale.y;
            }
        }
    }

    TextButton::TextButton(scene::Scene &s, glm::vec2 position, glm::vec2 size,
                           std::string text, const std::string &icon, draw::Align alignment,
                           int font_size, const std::string &font):
        BaseButton(s, position, size),
        icon(s.client.sprites.get(icon)),
        text(s.client.fonts.get(font, font_size), std::move(text), glm::vec3(1), glm::vec2(1), Align::BOTTOM_LEFT),
        alignment(alignment) {

        this->icon.alignment = Align::BOTTOM_LEFT;
        
        TextButton::layout();
    }

    void TextButton::draw() {
        this->icon.draw();
        this->text.draw();
    }

    void TextButton::layout() {
        BaseButton::layout();

        this->icon.scale = this->_size / glm::vec2{ this->icon.group->w(), this->icon.group->h() };

        auto text_size(this->text.size());
        auto icon_size(this->icon.size());
        this->hitbox_size.x = text_size.x + icon_size.x;
        this->hitbox_size.y = std::max(text_size.y, icon_size.y);

        this->hitbox_pos = this->_pos;
        switch (this->alignment) {
//        case Align::BOTTOM_LEFT:
//            break;
        case Align::BOTTOM_CENTER:
            this->hitbox_pos.x -= this->hitbox_size.x / 2.f;
            break;
        case Align::BOTTOM_RIGHT:
            this->hitbox_pos.x -= this->hitbox_size.x;
            break;
        default:
            break;
        }


        this->text.position = this->icon.position = this->hitbox_pos;

        this->hitbox_pos.y -= this->hitbox_size.y / 2.f;

        this->text.position += glm::vec2(icon_size.x, text_size.y / 2.f);
        this->icon.position.y += icon_size.y / 2.f;

        this->icon.tint = !this->hovering ? glm::vec4(1.0f) : glm::vec4(0.7f, 0.7f, 0.7f, 1.0f);
        this->text.set_color(glm::vec3(this->icon.tint) * (glm::vec3(189, 169, 74) / 255.f));
    }

    bool TextButton::hit_test(glm::vec2 mouse_position) {
        return point_in_rect(this->hitbox_pos, this->hitbox_size, mouse_position);
    }

    Button::Button(scene::Scene &s, std::string label, glm::vec2 position, glm::vec2 size, int font_size, const std::string &font, const std::string &button_name) :
        BaseButton(s, position, size),
        label(this->scene.client.fonts.get(font, font_size), std::move(label), { 0, 0, 0 }, { 1, 1 }, Align::CENTER),
        images(s, button_name) {
        this->update_images();
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
        this->left.scale = glm::vec2(this->_size.y / left.group->h());
        this->right.scale = glm::vec2(this->_size.y / right.group->h());
        this->mid.scale.x = (this->_size.x - (left.w() + right.w())) / mid.group->w();
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

        this->scale = this->size().y / this->bar->h();
    }

    void ProgressBar::draw() {
        float space_between_bullets = (this->scale * this->bar->w() + this->padding);
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
        this->_image.scale = glm::vec2(size / this->_image.group->h());
        this->_image.position = position;
        this->_image.alignment = draw::Align::CENTER;

        auto p = this->_image.get_position(draw::Align::TOP_LEFT);
        this->_title.position = p + title_offset * this->_image.scale;
    }

    ScrollBar::ScrollBar(scene::Scene &scene, glm::vec2 position, glm::vec2 size) : GUIWidget(scene, position, size), 
        up(scene, position, glm::vec2{ size.x }, "ui/common_elements/scroll_bar/scroll_bar_arrow_up.png", glm::vec2(0.6)),
        down(scene, position + glm::vec2(0, size.y - size.x), glm::vec2{ size.x }, "ui/common_elements/scroll_bar/scroll_bar_arrow_down.png", glm::vec2(0.6)),
        bottom(scene.client.sprites.get("ui/common_elements/scroll_bar/scroll_bar_bottom.png")),
        mid(scene.client.sprites.get("ui/common_elements/scroll_bar/scroll_bar_mid.png")), 
        top(scene.client.sprites.get("ui/common_elements/scroll_bar/scroll_bar_top.png")) {

        this->up.on("press_end", [this]() {
            this->set_value(this->value() - 1);
        });
        this->down.on("press_end", [this]() {
            this->set_value(this->value() + 1);
        });
        
        ScrollBar::layout();
    }

    void ScrollBar::draw() {
        this->up.draw();
        this->down.draw();
        this->top.draw();
        if(this->mid.scale.x > 0 && this->mid.scale.y > 0)
            this->mid.draw();
        this->bottom.draw();
    }

    void ScrollBar::update(double dt) {
        this->up.update(dt);
        this->down.update(dt);
    }

    void ScrollBar::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
        this->up.on_key(scancode, modifiers, pressed);
        this->down.on_key(scancode, modifiers, pressed);
//
//        this->_thumb_position += pressed;
//        this->layout();
    }

    void ScrollBar::on_mouse_button(int button, bool pressed) {
        this->up.on_mouse_button(button, pressed);
        this->down.on_mouse_button(button, pressed);
    }

    void ScrollBar::on_mouse_motion(int x, int y, int dx, int dy) {
        this->up.on_mouse_motion(x, y, dx, dy);
        this->down.on_mouse_motion(x, y, dx, dy);
    }

    void ScrollBar::layout() {
        auto bar_size = this->_size;
        bar_size.y = (this->_size.y - (this->up.size().y + this->down.size().y)) / this->_thumb_range;
        auto bar_pos = this->_pos;
        bar_pos.y += this->up.size().y + (float(this->_thumb_position) / this->_thumb_range * (bar_size.y * this->_thumb_range));
        this->top.scale = glm::vec2(bar_size.x / this->top.group->w());
        this->bottom.scale = glm::vec2(bar_size.x / this->bottom.group->w());
        this->mid.scale.y = std::max(0.f, (bar_size.y - (this->top.h() + this->bottom.h())) / mid.group->h());
        this->mid.scale.x = this->top.scale.x;

        this->top.position = bar_pos;
        this->mid.position = { bar_pos.x, this->top.position.y + this->top.h() };
        this->bottom.position = { bar_pos.x, this->mid.position.y + this->mid.h() };
    }

    void ScrollBar::update_thumb() {
        this->_thumb_position = glm::clamp(this->_thumb_position, 0, this->_thumb_range);
        this->layout();
        this->fire("bar_change");
    }

    bool ListCtrl::Column::pressed(glm::vec2 mpos) const {
        auto pos(this->header.position);
        auto siz(this->header.size());
        pos.y -= siz.y;
        return point_in_rect(pos, siz, mpos);
    }

    ListCtrl::ListCtrl(scene::Scene &scene, glm::vec2 position, glm::vec2 size, glm::vec2 scale, ScrollBar *sb):
        GUIWidget(scene, position, size),
        sb(sb),
        column_font(scene.client.fonts.get("edo.ttf", 18 * scale.y)),
        entry_font(scene.client.fonts.get("Vera.ttf", 18 * scale.y)),
        scale(scale),
        col_height(30 * scale.y),
        name{ column_font, "Name", 5 * scale.x },
        players{ column_font, "Players", 250 * scale.x },
        map{ column_font, "Map",  350 * scale.x },
        mode{ column_font, "Mode", 495 * scale.x },
        ping{ column_font, "Ping", 575 * scale.x } {
 
        ListCtrl::layout();

        if(this->sb) {
            this->sb->on("bar_change", [this]() {
                this->set_offset(this->sb->value());
            });
        }
    }

    void ListCtrl::draw() {
        this->name.header.draw();
        this->players.header.draw();
        this->map.header.draw();
        this->mode.header.draw();
        this->ping.header.draw();

        
        int num_entries = this->_size.y / (this->col_height);
        glm::vec2 pen(this->_pos + glm::vec2{ 0, this->col_height * 2 });
        for(int i = offset; i < std::min(int(entries.size()), num_entries + offset); i++) {
            auto &e = entries.at(i);
            auto color =  i == this->_selected ? (glm::vec3(189, 169, 74) / 255.f) : glm::vec3(1);
            this->entry_font->draw_truncated(this->players.x_pos - this->name.x_pos - 25, e.name, pen + glm::vec2{ name.x_pos, 0 }, color);
            this->entry_font->draw(std::to_string(e.players) + "/" + std::to_string(e.max_players), pen + glm::vec2{ players.x_pos, 0 }, color);
            this->entry_font->draw(e.map, pen + glm::vec2{ map.x_pos, 0 }, color);
            this->entry_font->draw(e.mode, pen + glm::vec2{ mode.x_pos, 0 }, color);
            this->entry_font->draw(std::to_string(e.ping), pen + glm::vec2{ ping.x_pos, 0 }, color);
            pen.y += this->col_height;
        }
    }

    void ListCtrl::layout() {
        auto pos(this->_pos);
        pos.y += this->col_height;
        this->name.set_position(pos);
        this->players.set_position(pos);
        this->map.set_position(pos);
        this->mode.set_position(pos);
        this->ping.set_position(pos);
    }

    // wow this is so bad
#define LIST_PREDICATE(VAR) [ascending](const ServerEntry &a, const ServerEntry &b) {\
        return ascending ? a.VAR > b.VAR : a.VAR < b.VAR;\
    }

    void ListCtrl::sort() {
        // wow this is worse
        bool ascending = this->sort_ascending;
        switch(this->sort_order) {
        case SortOrder::NAME:
            std::sort(this->entries.begin(), this->entries.end(), LIST_PREDICATE(name));
            break;
        case SortOrder::PLAYERS:
            std::sort(this->entries.begin(), this->entries.end(), LIST_PREDICATE(players));
            break;
        case SortOrder::MAP:
            std::sort(this->entries.begin(), this->entries.end(), LIST_PREDICATE(map));
            break;
        case SortOrder::MODE:            
            std::sort(this->entries.begin(), this->entries.end(), LIST_PREDICATE(mode));
            break;
        case SortOrder::PING:
            std::sort(this->entries.begin(), this->entries.end(), LIST_PREDICATE(ping));
            break;
        default:
            break;
        }
    }

    void ListCtrl::set_offset(int offset) {
        if (offset == this->offset) return;
        this->offset = glm::clamp(offset, 0, int(entries.size() - 1));
        this->update_sb();
    }

    void ListCtrl::update_sb() const {
        if (!this->sb) return;
        this->sb->set_thumb(this->offset, this->entries.size());
    }

#undef LIST_PREDICATE

    void ListCtrl::on_mouse_button(int button, bool pressed) {
        if (!pressed || button != SDL_BUTTON_LEFT) return;

        int num_entries = this->_size.y / (this->col_height);
        glm::vec2 mpos(this->scene.client.mouse.x , this->scene.client.mouse.y);
        glm::vec2 pen(this->_pos + glm::vec2{ 0, this->col_height });
        for (int i = offset; i < std::min(int(entries.size()), num_entries + offset); i++) {
            if (point_in_rect(pen, { ping.x_pos, this->col_height }, mpos)) {
                this->_selected = i;
                this->fire("on_item_selected");
                break;
            }
            pen.y += this->col_height;
        }

        SortOrder last_sort_order = this->sort_order;
        if(this->name.pressed(mpos)) {
            this->sort_order = SortOrder::NAME;
        } else if(this->players.pressed(mpos)) {
            this->sort_order = SortOrder::PLAYERS;
        } else if (this->map.pressed(mpos)) {
            this->sort_order = SortOrder::MAP;
        } else if (this->mode.pressed(mpos)) {
            this->sort_order = SortOrder::MODE;
        } else if (this->ping.pressed(mpos)) {
            this->sort_order = SortOrder::PING;
        } else {
            return;
        }

        if(last_sort_order == this->sort_order) {
            this->sort_ascending = !this->sort_ascending;
        }

        this->sort();
    }

    void ListCtrl::on_key(SDL_Scancode scancode, int modifiers, bool pressed) {
//        if()
        if (!pressed) return;
        if(scancode == SDL_SCANCODE_LEFT || scancode == SDL_SCANCODE_UP)
            this->set_offset(this->offset - 1);
        else if(scancode == SDL_SCANCODE_RIGHT || scancode == SDL_SCANCODE_DOWN)
            this->set_offset(this->offset + 1);
    }

    const ListCtrl::ServerEntry *ListCtrl::selected() const {
        if (size_t(this->_selected) >= this->entries.size()) return nullptr;
        return &this->entries.at(this->_selected);
    }
}}
