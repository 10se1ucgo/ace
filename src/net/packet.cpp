#include "net/packet.h"

namespace ace { namespace net {
    ////////////////////////////////////////////////

    void PositionData::read(ByteReader &reader) {
        this->position = reader.read_vec3<float>();
    }

    void PositionData::write(ByteWriter &writer) const {
        writer.write(position);
    }

    ////////////////////////////////////////////////

    void OrientationData::read(ByteReader &reader) {
        this->orientation = reader.read_vec3<float>();
    }

    void OrientationData::write(ByteWriter &writer) const {
        writer.write(this->orientation);
    }

    ////////////////////////////////////////////////

    void WorldUpdate::read(ByteReader &reader) {
        this->items.clear();

        // sometimes all 32 aren't sent? not sure why.
        int num = int(reader.remaining_size()) / 24; // 4 bytes per float * 6 floats
        for (int i = 0; i < num; ++i) {
            glm::vec3 p = reader.read_vec3<float>();
            glm::vec3 o = reader.read_vec3<float>();
            this->items.emplace_back(p, o);
        }
    }

    void WorldUpdate::write(ByteWriter &writer) const {
        // TODO
    }

    /////////////////////////////////////////////////////
   
    void InputData::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        uint8_t flags = reader.read<uint8_t>();
        this->up     = flags & (1 << 0);
        this->down   = flags & (1 << 1);
        this->left   = flags & (1 << 2);
        this->right  = flags & (1 << 3);
        this->jump   = flags & (1 << 4);
        this->crouch = flags & (1 << 5);
        this->sneak  = flags & (1 << 6);
        this->sprint = flags & (1 << 7);
    }

    void InputData::write(ByteWriter &writer) const {
        writer.write(this->pid);
        uint8_t flags =
                this->up     << 0 |
                this->down   << 1 |
                this->left   << 2 |
                this->right  << 3 |
                this->jump   << 4 |
                this->crouch << 5 |
                this->sneak  << 6 |
                this->sprint << 7;
        writer.write(flags);
    }

    /////////////////////////////////////////////////////

    void WeaponInput::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        uint8_t flags = reader.read<uint8_t>();
        this->primary = flags & (1 << 0);
        this->secondary = flags & (1 << 1);
    }

    void WeaponInput::write(ByteWriter &writer) const {
        writer.write(this->pid);
        uint8_t flags = this->primary << 0 | this->secondary << 1;
        writer.write(flags);
    }

    /////////////////////////////////////////////////////

    void HitPacket::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->value = reader.read<HIT>();
    }

    void HitPacket::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->value);
    }

    /////////////////////////////////////////////////////

    void SetHP::read(ByteReader &reader) {
        this->hp = reader.read<uint8_t>();
        this->type = reader.read<DAMAGE>();
        this->source = reader.read_vec3<float>();
    }

    void SetHP::write(ByteWriter &writer) const {
        writer.write(this->hp);
        writer.write(this->type);
        writer.write(this->source);
    }

    /////////////////////////////////////////////////////

    void GrenadePacket::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->fuse = reader.read<float>();
        this->position = reader.read_vec3<float>();
        this->velocity = reader.read_vec3<float>();
    }

    void GrenadePacket::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->fuse);
        writer.write(this->position);
        writer.write(this->velocity);
    }

    /////////////////////////////////////////////////////

    void SetTool::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->tool = reader.read<TOOL>();
    }

    void SetTool::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->tool);
    }

    /////////////////////////////////////////////////////

    void SetColor::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->color = reader.read_color();
    }

    void SetColor::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write_color(this->color);
    }

    /////////////////////////////////////////////////////

    void ExistingPlayer::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->team = reader.read<TEAM>();
        this->weapon = reader.read<WEAPON>();
        this->tool = reader.read<TOOL>();
        this->kills = reader.read<uint32_t>();
        this->color = reader.read_color();
        this->name = reader.read_str();
    }

    void ExistingPlayer::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->team);
        writer.write(this->weapon);
        writer.write(this->tool);
        writer.write(this->kills);
        writer.write_color(this->color);
        writer.write(this->name);
    }

    /////////////////////////////////////////////////////

    void ShortPlayerData::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->team = reader.read<int8_t>();
        this->weapon = reader.read<uint8_t>();
    }

    void ShortPlayerData::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->team);
        writer.write(this->weapon);
    }

    /////////////////////////////////////////////////////

    void MoveObject::read(ByteReader &reader) {
        this->type = reader.read<OBJECT>();
        this->state = reader.read<TEAM>();
        this->position = reader.read_vec3<float>();
    }

    void MoveObject::write(ByteWriter &writer) const {
        writer.write(this->type);
        writer.write(this->state);
        writer.write(this->position);
    }

    /////////////////////////////////////////////////////

    void CreatePlayer::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->weapon = reader.read<WEAPON>();
        this->team = reader.read<TEAM>();
        this->position = reader.read_vec3<float>();
        this->name = reader.read_str();
    }

    void CreatePlayer::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->weapon);
        writer.write(this->team);
        writer.write(this->position);
        writer.write(this->name);
    }

    /////////////////////////////////////////////////////

    void BlockAction::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->value = reader.read<ACTION>();
        this->position = reader.read_vec3<uint32_t>();
    }

    void BlockAction::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->value);
        writer.write(this->position);
    }

    /////////////////////////////////////////////////////

    void BlockLine::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->start = reader.read_vec3<uint32_t>();
        this->end = reader.read_vec3<uint32_t>();
    }

    void BlockLine::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->start);
        writer.write(this->end);
    }

    /////////////////////////////////////////////////////

    void CTFState::read(ByteReader &r) {
        this->team1_score = r.read<uint8_t>();
        this->team2_score = r.read<uint8_t>();
        this->cap_limit = r.read<uint8_t>();
        uint8_t intel_flags = r.read<uint8_t>();
        this->team1_has_intel = intel_flags & (1 << 0);
        this->team2_has_intel = intel_flags & (1 << 1);

        if (this->team2_has_intel) {
            this->team1_carrier = r.read<uint8_t>();
            this->team1_flag = {-1, -1, -1};
            r.read(11);
        } else {
            this->team1_carrier = -1;
            this->team1_flag = r.read_vec3<float>();
        }

        if (this->team1_has_intel) {
            this->team2_carrier = r.read<uint8_t>();
            this->team2_flag = {-1, -1, -1};
            r.read(11);
        } else {
            this->team2_carrier = -1;
            this->team2_flag = r.read_vec3<float>();
        }

        this->team1_base = r.read_vec3<float>();
        this->team2_base = r.read_vec3<float>();
    }

    /////////////////////////////////////////////////////

    void StateData::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->fog_color = reader.read_color();
        this->team1_color = reader.read_color();
        this->team2_color = reader.read_color();
        this->team1_name = reader.read_str(10, true);
        this->team2_name = reader.read_str(10, true);

        this->mode = reader.read<uint8_t>();
        memset(&this->state, 0, sizeof(this->state));
        if (mode == 0) {
            this->state.ctf.read(reader);
        } else {
            THROW_ERROR("Territory Control gamemode not supported!");
            this->state = ModeState();
            // TODO: territory control state
        }
    }

    void StateData::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write_color(this->fog_color);
        writer.write_color(this->team1_color);
        writer.write_color(this->team2_color);
        writer.write(this->team1_name, 10);
        writer.write(this->team2_name, 10);
        writer.write(this->mode);
        // TODO: game mode state
    }

    /////////////////////////////////////////////////////

    void KillAction::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->killer = reader.read<uint8_t>();
        this->type = reader.read<KILL>();
        this->respawn_time = reader.read<uint8_t>();
    }

    void KillAction::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->killer);
        writer.write(this->type);
        writer.write(this->respawn_time);
    }

    /////////////////////////////////////////////////////

    void ChatMessage::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->type = reader.read<CHAT>();
        this->message = reader.read_str();
    }

    void ChatMessage::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->type);
        writer.write(this->message);
    }

    /////////////////////////////////////////////////////

    void PlayerLeft::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
    }

    void PlayerLeft::write(ByteWriter &writer) const {
        writer.write(this->pid);
    }

    /////////////////////////////////////////////////////

    void TerritoryCapture::read(ByteReader &reader) {
        this->object = reader.read<OBJECT>();
        this->winning = reader.read<TEAM>();
        this->state = reader.read<TEAM>();
    }

    void TerritoryCapture::write(ByteWriter &writer) const {
        writer.write(this->object);
        writer.write(this->winning);
        writer.write(this->state);
    }

    /////////////////////////////////////////////////////

    void ProgressBar::read(ByteReader &reader) {
        this->object = reader.read<uint8_t>();
        this->team = reader.read<TEAM>();
        this->rate = reader.read<int8_t>();
        this->progress = reader.read<float>();
    }

    void ProgressBar::write(ByteWriter &writer) const {
        writer.write(this->object);
        writer.write(this->team);
        writer.write(this->rate);
        writer.write(this->progress);
    }

    /////////////////////////////////////////////////////

    void IntelCapture::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->winning = reader.read<uint8_t>();
    }

    void IntelCapture::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->winning);
    }

    /////////////////////////////////////////////////////

    void IntelPickup::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
    }

    void IntelPickup::write(ByteWriter &writer) const {
        writer.write(this->pid);
    }

    /////////////////////////////////////////////////////

    void IntelDrop::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->pos = reader.read_vec3<float>();
    }

    void IntelDrop::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->pos);
    }

    /////////////////////////////////////////////////////

    void Restock::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
    }

    void Restock::write(ByteWriter &writer) const {
        writer.write(this->pid);
    }

    /////////////////////////////////////////////////////

    void FogColor::read(ByteReader &reader) {
        // read BGR into RGB, then read A to form RGBA
        glm::u8vec3 rgb(reader.read_color());
        uint8_t a(reader.read<uint8_t>());
        this->color = glm::u8vec4(rgb, a);
    }

    void FogColor::write(ByteWriter &writer) const {
        // write RGB as BGR, then write A to form BGRA.
        writer.write_color(glm::u8vec3(this->color)); 
        writer.write(this->color.a);
    }

    /////////////////////////////////////////////////////

    void WeaponReload::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->primary = reader.read<uint8_t>();
        this->secondary = reader.read<uint8_t>();
    }

    void WeaponReload::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->primary);
        writer.write(this->secondary);
    }

    /////////////////////////////////////////////////////

    void ChangeTeam::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->team = reader.read<TEAM>();
    }

    void ChangeTeam::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->team);
    }

    /////////////////////////////////////////////////////

    void ChangeWeapon::read(ByteReader &reader) {
        this->pid = reader.read<uint8_t>();
        this->weapon = reader.read<WEAPON>();
    }

    void ChangeWeapon::write(ByteWriter &writer) const {
        writer.write(this->pid);
        writer.write(this->weapon);
    }

    /////////////////////////////////////////////////////

    void VersionResponse::read(ByteReader &reader) {
        this->client = static_cast<char>(reader.read<uint8_t>());
        this->version = reader.read_vec3<uint8_t>();
        this->os_info = reader.read_str();
    }

    void VersionResponse::write(ByteWriter &writer) const {
        writer.write(static_cast<uint8_t>(this->client));
        writer.write(this->version);
        writer.write(this->os_info);
    }

    /////////////////////////////////////////////////////
}}