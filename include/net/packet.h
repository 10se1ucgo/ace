#pragma once
#include <vector>
#include "fmt/format.h"
#include "glm/glm.hpp"
#include "common.h"
#include "util/except.h"

namespace ace { namespace net {
    struct ByteReader {
        ByteReader(uint8_t *data, size_t size) : start(data), pos(start), end(start + size) {
        }

        uint8_t *read(size_t num) {
            if (this->pos + num > this->end) {
                THROW_ERROR("NO DATA LEFT READING PKT {}\n", start);
            }

            uint8_t *pos = this->pos;
            this->pos += num;
            return pos;
        }

        uint8_t *get(size_t *len) const {
            *len = this->end - this->pos;
            return this->pos;
        }

        template<typename T>
        T read() {
            return *reinterpret_cast<T *>(this->read(sizeof(T)));
        }

        std::string read_bytes(size_t length = 0) {
            uint8_t *str;
            if (length == 0) {
                length = strnlen(reinterpret_cast<char *>(this->pos), this->end - this->pos);
                str = this->read(length + 1); // read past null terminator
            }
            else {
                str = this->read(length);
            }
            return std::string(str, str + length);
        }

        template<typename T>
        glm::tvec2<T> read_vec2() {
            return { this->read<T>(), this->read<T>(), this->read<T>() };
        }

        template<typename T>
        glm::tvec3<T> read_vec3() {
            return { this->read<T>(), this->read<T>(), this->read<T>() };
        }

        template<typename T>
        glm::tvec4<T> read_vec4() {
            return { this->read<T>(), this->read<T>(), this->read<T>(), this->read<T>() };
        }

        glm::u8vec3 read_color() {
            const auto col = this->read_vec3<uint8_t>();
            return { col.b, col.g, col.r };
        }

        uint8_t *start, *pos, *end;
    };

    struct ByteWriter {
        std::vector<uint8_t> vec;

        void write(const uint8_t *buf, size_t size) {
            this->vec.insert(this->vec.end(), buf, buf + size);
        }

        template<typename T>
        void write(T value) {
            // fmt::print("{} -> ", value);
            uint8_t *data = reinterpret_cast<uint8_t *>(&value);
//            for(int i = 0; i < sizeof(T); i++) {
//                fmt::print("\\x{:02x}", data[i]);
//            }
//            fmt::print("\n");
            this->write(data, sizeof(T));
        }

        void write(const std::string &value, size_t len = 0) {
            // fmt::print("{} -> ", value);
            if (len == 0) {
                len = value.length();
            }

            auto *data = reinterpret_cast<const uint8_t *>(value.c_str());
//            for (size_t i = 0; i < len + 1; i++) {
//                fmt::print("\\x{:02x}", data[i]);
//            }
//            fmt::print("\n");

            this->write(data, len + 1);
        }

        template<typename T>
        void write(glm::tvec2<T> value) {
            this->write(value.x);
            this->write(value.y);
        }

        template<typename T>
        void write(glm::tvec3<T> value) {
            this->write(value.x);
            this->write(value.y);
            this->write(value.z);
        }

        template<typename T>
        void write(glm::tvec4<T> value) {
            this->write(value.x);
            this->write(value.y);
            this->write(value.z);
            this->write(value.w);
        }

        void write_color(glm::u8vec3 value) {
            this->write(value.b);
            this->write(value.g);
            this->write(value.r);
        }

        void clear() {
            this->vec.clear();
        }
    };

    enum class PACKET : uint8_t {
        PositionData,
        OrientationData,
        WorldUpdate,
        InputData,
        WeaponInput,
        HitPacket, // C->S
        SetHP = HitPacket, // S->C
        GrenadePacket,
        SetTool,
        SetColor,
        ExistingPlayer,
        ShortPlayerData,
        MoveObject,
        CreatePlayer,
        BlockAction,
        BlockLine,
        StateData,
        KillAction,
        ChatMessage,
        MapStart,
        MapChunk,
        PlayerLeft,
        TerritoryCapture,
        ProgressBar,
        IntelCapture,
        IntelPickup,
        IntelDrop,
        Restock,
        FogColor,
        WeaponReload,
        ChangeTeam,
        ChangeWeapon,
    };

    enum class WEAPON : uint8_t {
        INVALID = 255,
        SEMI = 1,
        SMG,
        SHOTGUN,
    };

    enum class HIT : uint8_t {
        INVALID = 255,
        TORSO = 1,
        HEAD,
        ARMS,
        LEGS,
        MELEE,
    };

    enum class TOOL : uint8_t {
        INVALID = 255,
        SPADE = 1,
        BLOCK,
        WEAPON,
        GRENADE,
    };

    enum class ACTION : uint8_t {
        INVALID = 255,
        BUILD = 1,
        DESTROY,
        SPADE,
        GRENADE,
    };

    enum class CHAT : uint8_t {
        INVALID = 255,
        ALL = 1,
        TEAM,
        SYSTEM,
//        BIG
    };

    enum class OBJECT : uint8_t {
        INVALID = 255,
        BLUE_FLAG = 1,
        GREEN_FLAG,
        BLUE_BASE,
        GREEN_BASE,
    };

    enum class KILL : uint8_t {
        INVALID = 255,
        WEAPON = 1,
        HEADSHOT,
        MELEE,
        GRENADE,
        FALL,
        TEAM_CHANGE,
        CLASS_CHANGE,
    };

    enum class DISCONNECT : uint8_t {
        INVALID = 255,
        UNDEFINED = 1,
        BANNED,
        KICKED,
        WRONG_VERSION,
        FULL,
    };

    enum class DAMAGE : uint8_t {
        INVALID = 255,
        FALL = 1,
        OTHER,
    };

    enum class TEAM : uint8_t {
        SPECTATOR = 255,
        TEAM1 = 1,
        TEAM2,
        NEUTRAL,
    };


    struct Loader {
        virtual ~Loader() = default;
        virtual void read(ByteReader &reader) = 0;
        virtual void write(ByteWriter &writer) const = 0;
        virtual PACKET packet_id() const = 0;
    };

// BADMACROBADMACROBADMACRO
#define _PACKET_ID(PacketName) \
    PACKET packet_id() const final { return PACKET::PacketName; } \
//    static constexpr PACKET packet_id2 = PACKET::PacketName;
// BADMACROBADMACROBADMACRO

    struct PositionData final : Loader  {
        glm::vec3 position;

        void read(ByteReader &reader) final {
            this->position = reader.read_vec3<float>();
        }
        void write(ByteWriter &writer) const final {
            writer.write(position);
        }

        _PACKET_ID(PositionData)
    };

    struct OrientationData : Loader {
        glm::vec3 orientation;

        void read(ByteReader &reader) override {
            this->orientation = reader.read_vec3<float>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->orientation);
        }

        _PACKET_ID(OrientationData)
    };

    struct WorldUpdate : Loader {
        std::vector<std::pair<glm::vec3, glm::vec3>> items;

        void read(ByteReader &reader) override {
            items.clear();
            for (int i = 0; i < 32; ++i) {
                glm::vec3 p = reader.read_vec3<float>();
                glm::vec3 o = reader.read_vec3<float>();
                items.emplace_back(p, o);
            }
        }
        void write(ByteWriter &writer) const override {
            
        }

        _PACKET_ID(WorldUpdate)
    };

    struct InputData : Loader {
        uint8_t pid;
        bool up : 1, down : 1, left : 1, right : 1, jump : 1, crouch : 1, sneak : 1, sprint : 1;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            uint8_t flags = reader.read<uint8_t>();
            this->up = flags & (1 << 0);
            this->down = flags & (1 << 1);
            this->left = flags & (1 << 2);
            this->right = flags & (1 << 3);
            this->jump = flags & (1 << 4);
            this->crouch = flags & (1 << 5);
            this->sneak = flags & (1 << 6);
            this->sprint = flags & (1 << 7);
        }

        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            uint8_t flags =
                this->up << 0 |
                this->down << 1 |
                this->left << 2 |
                this->right << 3 |
                this->jump << 4 |
                this->crouch << 5 |
                this->sneak << 6 |
                this->sprint << 7;
            writer.write(flags);
        }

        _PACKET_ID(InputData)
    };

    struct WeaponInput : Loader {
        uint8_t pid;
        bool primary : 1, secondary : 1;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            uint8_t flags = reader.read<uint8_t>();
            this->primary = flags & (1 << 0);
            this->secondary = flags & (1 << 1);
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            uint8_t flags = this->primary << 0 | this->secondary << 1;
            writer.write(flags);
        }

        _PACKET_ID(WeaponInput)
    };

    struct HitPacket : Loader {
        uint8_t pid;
        HIT value;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->value = reader.read<HIT>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->value);
        }

        _PACKET_ID(HitPacket)
    };

    struct SetHP : Loader {
        uint8_t hp;
        DAMAGE type;
        glm::vec3 source;

        void read(ByteReader &reader) override {
            this->hp = reader.read<uint8_t>();
            this->type = reader.read<DAMAGE>();
            this->source = reader.read_vec3<float>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->hp);
            writer.write(this->type);
            writer.write(this->source);
        }

        _PACKET_ID(SetHP)
    };

    struct GrenadePacket : Loader {
        uint8_t pid;
        float fuse;
        glm::vec3 position, velocity;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->fuse = reader.read<float>();
            this->position = reader.read_vec3<float>();
            this->velocity = reader.read_vec3<float>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->fuse);
            writer.write(this->position);
            writer.write(this->velocity);
        }

        _PACKET_ID(GrenadePacket)
    };

    struct SetTool : Loader {
        uint8_t pid;
        TOOL tool;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->tool = reader.read<TOOL>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->tool);
        }

        _PACKET_ID(SetTool)
    };

    struct SetColor : Loader {
        uint8_t pid;
        glm::u8vec3 color;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->color = reader.read_color();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write_color(this->color);
        }

        _PACKET_ID(SetColor)
    };

    struct ExistingPlayer : Loader {
        uint8_t pid;
        TEAM team;
        WEAPON weapon;
        TOOL tool;
        uint32_t kills;
        glm::u8vec3 color;
        std::string name;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->team = reader.read<TEAM>();
            this->weapon = reader.read<WEAPON>();
            this->tool = reader.read<TOOL>();
            this->kills = reader.read<uint32_t>();
            this->color = reader.read_color();
            this->name = reader.read_bytes();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->team);
            writer.write(this->weapon);
            writer.write(this->tool);
            writer.write(this->kills);
            writer.write_color(this->color);
            writer.write(this->name);
        }

        _PACKET_ID(ExistingPlayer)
    };

    struct ShortPlayerData : Loader {
        uint8_t pid, weapon;
        int8_t team;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->team = reader.read<int8_t>();
            this->weapon = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->team);
            writer.write(this->weapon);
        }

        _PACKET_ID(ShortPlayerData)
    };

    struct MoveObject : Loader {
        OBJECT type;
        TEAM state;
        glm::vec3 position;

        void read(ByteReader &reader) override {
            this->type = reader.read<OBJECT>();
            this->state = reader.read<TEAM>();
            this->position = reader.read_vec3<float>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->type);
            writer.write(this->state);
            writer.write(this->position);
        }

        _PACKET_ID(MoveObject)
    };

    struct CreatePlayer : Loader {
        uint8_t pid;
        WEAPON weapon;
        TEAM team;
        glm::vec3 position;
        std::string name;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->weapon = reader.read<WEAPON>();
            this->team = reader.read<TEAM>();
            this->position = reader.read_vec3<float>();
            this->name = reader.read_bytes();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->weapon);
            writer.write(this->team);
            writer.write(this->position);
            writer.write(this->name);
        }

        _PACKET_ID(CreatePlayer)
    };

    struct BlockAction : Loader {
        uint8_t pid;
        ACTION value;
        glm::i32vec3 position;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->value = reader.read<ACTION>();
            this->position = reader.read_vec3<uint32_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->value);
            writer.write(this->position);
        }

        _PACKET_ID(BlockAction)
    };

    struct BlockLine : Loader {
        uint8_t pid;
        glm::i32vec3 start, end;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->start = reader.read_vec3<uint32_t>();
            this->end = reader.read_vec3<uint32_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->start);
            writer.write(this->end);
        }

        _PACKET_ID(BlockLine)
    };

    struct CTFState {
        uint8_t team1_score, team2_score, cap_limit;
        bool team1_has_intel : 1, team2_has_intel : 1;
        uint8_t team1_carrier, team2_carrier;
        glm::vec3 team1_flag, team2_flag, team1_base, team2_base;

        void read(ByteReader &r) {
            this->team1_score = r.read<uint8_t>();
            this->team2_score = r.read<uint8_t>();
            this->cap_limit = r.read<uint8_t>();
            uint8_t intel_flags = r.read<uint8_t>();
            this->team1_has_intel = intel_flags & (1 << 0);
            this->team2_has_intel = intel_flags & (1 << 1);

            if(this->team2_has_intel) {
                this->team1_carrier = r.read<uint8_t>();
                this->team1_flag = { -1, -1, -1 };
                r.read(11);
                
            } else {
                this->team1_carrier = 255;
                this->team1_flag = r.read_vec3<float>();
            }

            if (this->team1_has_intel) {
                this->team2_carrier = r.read<uint8_t>();
                this->team2_flag = { -1, -1, -1 };
                r.read(11);
            }
            else {
                this->team2_carrier = 255;
                this->team2_flag = r.read_vec3<float>();
            }

            this->team1_base = r.read_vec3<float>();
            this->team2_base = r.read_vec3<float>();
        }
    };

    struct TCState {
        uint8_t territory_count;
        struct Territory {
            glm::vec3 position;
            uint8_t team;
        } territories[16]; // 16 MAX
    };

    struct StateData : Loader {
        uint8_t pid{0};
        glm::u8vec3 fog_color, team1_color, team2_color;
        std::string team1_name, team2_name;
        uint8_t mode{0};
        union ModeState {
            CTFState ctf; TCState tc;
            ModeState() : ctf() { }
        } state;

        void read(ByteReader &reader) override {
            
            this->pid = reader.read<uint8_t>();
            this->fog_color = reader.read_color();
            this->team1_color = reader.read_color();
            this->team2_color = reader.read_color();
            this->team1_name = reader.read_bytes(10);
            this->team2_name = reader.read_bytes(10);
            this->team1_name = this->team1_name.substr(0, strnlen(this->team1_name.c_str(), this->team1_name.size()));
            this->team2_name = this->team2_name.substr(0, strnlen(this->team2_name.c_str(), this->team2_name.size()));

            this->mode = reader.read<uint8_t>();
            memset(&this->state, 0, sizeof(this->state));
            if(mode == 0) {
                this->state.ctf.read(reader);
            } else {
                this->state = ModeState();
                // TODO: territory control state
            }
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write_color(this->fog_color);
            writer.write_color(this->team1_color);
            writer.write_color(this->team2_color);
            writer.write(this->team1_name, 10);
            writer.write(this->team2_name, 10);
            writer.write(this->mode);
            // TODO: game mode state
        }

        _PACKET_ID(StateData)
    };

    struct KillAction : Loader {
        uint8_t pid, killer;
        KILL type;
        uint8_t respawn_time;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->killer = reader.read<uint8_t>();
            this->type = reader.read<KILL>();
            this->respawn_time = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->killer);
            writer.write(this->type);
            writer.write(this->respawn_time);
        }

        _PACKET_ID(KillAction)
    };

    struct ChatMessage : Loader {
        uint8_t pid;
        CHAT type;
        std::string message;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->type = reader.read<CHAT>();
            this->message = reader.read_bytes();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->type);
            writer.write(this->message);
        }

        _PACKET_ID(ChatMessage)
    };

    struct PlayerLeft : Loader {
        uint8_t pid;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
        }

        _PACKET_ID(PlayerLeft)
    };

    struct TerritoryCapture : Loader {
        OBJECT object;
        TEAM winning;
        TEAM state;

        void read(ByteReader &reader) override {
            this->object = reader.read<OBJECT>();
            this->winning = reader.read<TEAM>();
            this->state = reader.read<TEAM>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->object);
            writer.write(this->winning);
            writer.write(this->state);
        }

        _PACKET_ID(TerritoryCapture)
    };

    struct ProgressBar : Loader {
        uint8_t object;
        TEAM team;
        int8_t rate;
        float progress;

        void read(ByteReader &reader) override {
            this->object = reader.read<uint8_t>();
            this->team = reader.read<TEAM>();
            this->rate = reader.read<int8_t>();
            this->progress = reader.read<float>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->object);
            writer.write(this->team);
            writer.write(this->rate);
            writer.write(this->progress);
        }

        _PACKET_ID(ProgressBar)
    };

    struct IntelCapture : Loader {
        uint8_t pid, winning;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->winning = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->winning);
        }

        _PACKET_ID(IntelCapture)
    };

    struct IntelPickup : Loader {
        uint8_t pid;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
        }

        _PACKET_ID(IntelPickup)
    };

    struct IntelDrop : Loader {
        uint8_t pid;
        glm::vec3 pos;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->pos = reader.read_vec3<float>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->pos);
        }

        _PACKET_ID(IntelDrop)
    };

    struct Restock : Loader {
        uint8_t pid;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
        }

        _PACKET_ID(Restock)
    };

    struct FogColor : Loader {
        glm::u8vec4 color;

        void read(ByteReader &reader) override {
            this->color = glm::u8vec4(reader.read_color(), reader.read<uint8_t>()); // read BGR into RGB, then read A to form RGBA
        }
        void write(ByteWriter &writer) const override {
            writer.write(glm::u8vec3(this->color)); // write RGB as BGR, then write A to form BGRA.
            writer.write(this->color.a);
        }

        _PACKET_ID(FogColor)
    };

    struct WeaponReload : Loader {
        uint8_t pid, primary, secondary;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->primary = reader.read<uint8_t>();
            this->secondary = reader.read<uint8_t>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->primary);
            writer.write(this->secondary);
        }

        _PACKET_ID(WeaponReload)
    };

    struct ChangeTeam : Loader {
        uint8_t pid;
        TEAM team;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->team = reader.read<TEAM>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->team);
        }

        _PACKET_ID(ChangeTeam)
    };

    struct ChangeWeapon : Loader {
        uint8_t pid;
        WEAPON weapon;

        void read(ByteReader &reader) override {
            this->pid = reader.read<uint8_t>();
            this->weapon = reader.read<WEAPON>();
        }
        void write(ByteWriter &writer) const override {
            writer.write(this->pid);
            writer.write(this->weapon);
        }

        _PACKET_ID(ChangeWeapon)
    };

#undef _PACKET_ID

    inline std::unique_ptr<Loader> get_loader(PACKET id) {
        switch(id) {
            case PACKET::PositionData: return std::make_unique<PositionData>();
            case PACKET::OrientationData: return std::make_unique< OrientationData>();
            case PACKET::WorldUpdate: return std::make_unique< WorldUpdate>();
            case PACKET::InputData: return std::make_unique<InputData>();
            case PACKET::WeaponInput: return std::make_unique<WeaponInput>();
            case PACKET::SetHP: return std::make_unique<SetHP>();
            case PACKET::GrenadePacket: return std::make_unique<GrenadePacket>();
            case PACKET::SetTool: return std::make_unique<SetTool>();
            case PACKET::SetColor: return std::make_unique<SetColor>();
            case PACKET::ExistingPlayer: return std::make_unique<ExistingPlayer>();
            case PACKET::ShortPlayerData: return std::make_unique<ShortPlayerData>();
            case PACKET::MoveObject: return std::make_unique<MoveObject>();
            case PACKET::CreatePlayer: return std::make_unique<CreatePlayer>();
            case PACKET::BlockAction: return std::make_unique<BlockAction>();
            case PACKET::BlockLine: return std::make_unique<BlockLine>();
            case PACKET::StateData: return std::make_unique<StateData>();
            case PACKET::KillAction: return std::make_unique<KillAction>();
            case PACKET::ChatMessage: return std::make_unique<ChatMessage>();
//            case PACKET::MapStart: break;
//            case PACKET::MapChunk: break;
            case PACKET::PlayerLeft: return std::make_unique<PlayerLeft>();
            case PACKET::TerritoryCapture: return std::make_unique<TerritoryCapture>();
            case PACKET::ProgressBar: return std::make_unique<ProgressBar>();
            case PACKET::IntelCapture: return std::make_unique<IntelCapture>();
            case PACKET::IntelPickup: return std::make_unique<IntelPickup>();
            case PACKET::IntelDrop: return std::make_unique<IntelDrop>();
            case PACKET::Restock: return std::make_unique<Restock>();
            case PACKET::FogColor: return std::make_unique<FogColor>();
            case PACKET::WeaponReload: return std::make_unique<WeaponReload>();
            case PACKET::ChangeTeam: std::make_unique<ChangeTeam>();
            case PACKET::ChangeWeapon: std::make_unique<ChangeWeapon>();
            default: return nullptr;
        }
    }

//    auto x = sizeof(Packet);
}}

namespace fmt {
#define ENUM_FORMAT_ARG(T) \
    template <typename ArgFormatter> \
    void format_arg(fmt::BasicFormatter<char, ArgFormatter> &f, const char *&format_str, T e) { \
        f.writer() << static_cast<std::underlying_type<T>::type>(e); \
    }

    ENUM_FORMAT_ARG(ace::net::ACTION)
    ENUM_FORMAT_ARG(ace::net::CHAT)
    ENUM_FORMAT_ARG(ace::net::DAMAGE)
    ENUM_FORMAT_ARG(ace::net::DISCONNECT)
    ENUM_FORMAT_ARG(ace::net::HIT)
    ENUM_FORMAT_ARG(ace::net::KILL)
    ENUM_FORMAT_ARG(ace::net::OBJECT)
    ENUM_FORMAT_ARG(ace::net::PACKET)
    ENUM_FORMAT_ARG(ace::net::TEAM)
    ENUM_FORMAT_ARG(ace::net::TOOL)
    ENUM_FORMAT_ARG(ace::net::WEAPON)
#undef ENUM_FORMAT_ARG
}
