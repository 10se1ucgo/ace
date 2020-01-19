#pragma once
#include <vector>
#include "fmt/format.h"
#include "glm/glm.hpp"
#include "common.h"
#include "util/except.h"

namespace ace { namespace net {
    struct ByteReader {
        ByteReader(const uint8_t *data, size_t size) : start(data), pos(start), end(start + size) {
        }

        const uint8_t *read(size_t num) {
            if (this->pos + num > this->end) {
                THROW_ERROR("NO DATA LEFT READING PKT {}\n", this->to_hex_str());
            }

            const uint8_t *pos = this->pos;
            this->pos += num;
            return pos;
        }

        const uint8_t *remaining_data() const {
            return this->pos;
        }

        size_t remaining_size() const {
            return this->end - this->pos;
        }

        const uint8_t *total_data() const {
            return this->start;
        }

        size_t total_size() const {
            return this->end - this->start;
        }

        std::string to_hex_str(int start = 0) const {
            fmt::MemoryWriter hex_data;
            for (auto i = start; i < this->end - this->start; i++) {
                hex_data.write("\\x{:02X}", reinterpret_cast<const char *>(this->start)[i]);
            }
            return hex_data.str();
        }

        template<typename T>
        T read() {
            return *reinterpret_cast<const T *>(this->read(sizeof(T)));
        }

        std::string read_str(size_t length = 0, bool strip_null = false) {
            const uint8_t *str;
            if (length == 0) {
                length = strnlen(reinterpret_cast<const char *>(this->pos), this->end - this->pos);
                str = this->read(length + 1); // read past null terminator
            } else {
                str = this->read(length);
                if (strip_null) length = strnlen(reinterpret_cast<const char *>(str), length);
            }
            return std::string(str, str + length);
        }

        template<typename T>
        glm::tvec2<T> read_vec2() {
            glm::tvec2<T> res;
            res.x = this->read<T>();
            res.y = this->read<T>();
            return res;
        }

        template<typename T>
        glm::tvec3<T> read_vec3() {
            glm::tvec3<T> res;
            res.x = this->read<T>();
            res.y = this->read<T>();
            res.z = this->read<T>();
            return res;
        }

        template<typename T>
        glm::tvec4<T> read_vec4() {
            glm::tvec4<T> res;
            res.x = this->read<T>();
            res.y = this->read<T>();
            res.z = this->read<T>();
            res.w = this->read<T>();
            return res;
        }

        glm::u8vec3 read_color() {
            const auto col = this->read_vec3<uint8_t>();
            return { col.b, col.g, col.r };
        }

    private:
        const uint8_t *start, *pos, *end;
    };

    struct ByteWriter {
        void write(const uint8_t *buf, size_t size) {
            this->vec.insert(this->vec.end(), buf, buf + size);
        }

        const uint8_t *data() const {
            return this->vec.data();
        }

        size_t size() const {
            return this->vec.size();
        }

        size_t capacity() const {
            return this->vec.capacity();
        }

        void reserve(size_t capacity) {
            this->vec.reserve(capacity);
        }

        void clear() {
            this->vec.clear();
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

    private:
        std::vector<uint8_t> vec;
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
        HandShakeInit,
        HandShakeReturn,
        VersionRequest,
        VersionResponse
    };

    enum class WEAPON : uint8_t {
        SEMI,
        SMG,
        SHOTGUN,
        INVALID = 255,
    };

    enum class HIT : uint8_t {
        TORSO,
        HEAD,
        ARMS,
        LEGS,
        MELEE,
        INVALID = 255,
    };

    enum class TOOL : uint8_t {
        SPADE,
        BLOCK,
        WEAPON,
        GRENADE,
        INVALID = 255,
    };

    enum class ACTION : uint8_t {
        BUILD,
        DESTROY,
        SPADE,
        GRENADE,
        INVALID = 255,
    };

    enum class CHAT : uint8_t {
        ALL,
        TEAM,
        SYSTEM,
        // BIG
        INVALID = 255,
    };

    enum class OBJECT : uint8_t {
        BLUE_FLAG,
        GREEN_FLAG,
        BLUE_BASE,
        GREEN_BASE,
        INVALID = 255,
    };

    enum class KILL : uint8_t {
        WEAPON,
        HEADSHOT,
        MELEE,
        GRENADE,
        FALL,
        TEAM_CHANGE,
        CLASS_CHANGE,
        INVALID = 255,
    };

    enum class DISCONNECT : uint8_t {
        UNDEFINED,
        BANNED,
        KICKED,
        WRONG_VERSION,
        FULL,
        INVALID = 255,
    };

    enum class DAMAGE : uint8_t {
        FALL,
        OTHER,
        INVALID = 255,
    };

    enum class TEAM : int8_t {
        SPECTATOR = -1,
        TEAM1,
        TEAM2,
        NEUTRAL,
    };

    struct PacketHandler;

    struct Loader {
        virtual ~Loader() = default;
        virtual void read(ByteReader &reader) = 0;
        virtual void write(ByteWriter &writer) const = 0;
        virtual PACKET packet_id() const = 0;
        virtual void dispatch(PacketHandler &handler) { }
    };

// BADMACROBADMACROBADMACRO
#define _PACKET_ID(PacketName) PACKET packet_id() const override { return PACKET::PacketName; }
#define _DECL_DISPATCH_FUNC void dispatch(PacketHandler &handler) override;
//    static constexpr PACKET packet_id2 = PACKET::PacketName;
// BADMACROBADMACROBADMACRO

    struct PositionData final : Loader  {
        glm::vec3 position;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(PositionData);
        _DECL_DISPATCH_FUNC;
    };

    struct OrientationData final : Loader {
        glm::vec3 orientation;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(OrientationData);
        _DECL_DISPATCH_FUNC;
    };

    struct WorldUpdate final : Loader {
        std::vector<std::pair<glm::vec3, glm::vec3>> items;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(WorldUpdate);
        _DECL_DISPATCH_FUNC;
    };

    struct InputData final : Loader {
        uint8_t pid;
        bool up : 1, down : 1, left : 1, right : 1, jump : 1, crouch : 1, sneak : 1, sprint : 1;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(InputData);
        _DECL_DISPATCH_FUNC;
    };

    struct WeaponInput final : Loader {
        uint8_t pid;
        bool primary : 1, secondary : 1;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(WeaponInput);
        _DECL_DISPATCH_FUNC;
    };

    struct HitPacket final : Loader {
        uint8_t pid;
        HIT value;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(HitPacket);
        _DECL_DISPATCH_FUNC;
    };

    struct SetHP final : Loader {
        uint8_t hp;
        DAMAGE type;
        glm::vec3 source;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(SetHP);
        _DECL_DISPATCH_FUNC;
    };

    struct GrenadePacket final : Loader {
        uint8_t pid;
        float fuse;
        glm::vec3 position, velocity;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(GrenadePacket);
        _DECL_DISPATCH_FUNC;
    };

    struct SetTool final : Loader {
        uint8_t pid;
        TOOL tool;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(SetTool);
        _DECL_DISPATCH_FUNC;
    };

    struct SetColor final : Loader {
        uint8_t pid;
        glm::u8vec3 color;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(SetColor);
        _DECL_DISPATCH_FUNC;
    };

    struct ExistingPlayer final : Loader {
        uint8_t pid;
        TEAM team;
        WEAPON weapon;
        TOOL tool;
        uint32_t kills;
        glm::u8vec3 color;
        std::string name;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(ExistingPlayer);
        _DECL_DISPATCH_FUNC;
    };

    struct ShortPlayerData final : Loader {
        uint8_t pid, weapon;
        int8_t team;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(ShortPlayerData);
        _DECL_DISPATCH_FUNC;
    };

    struct MoveObject final : Loader {
        OBJECT type;
        TEAM state;
        glm::vec3 position;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(MoveObject);
        _DECL_DISPATCH_FUNC;
    };

    struct CreatePlayer final : Loader {
        uint8_t pid;
        WEAPON weapon;
        TEAM team;
        glm::vec3 position;
        std::string name;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(CreatePlayer);
        _DECL_DISPATCH_FUNC;
    };

    struct BlockAction final : Loader {
        uint8_t pid;
        ACTION value;
        glm::i32vec3 position;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(BlockAction);
        _DECL_DISPATCH_FUNC;
    };

    struct BlockLine final : Loader {
        uint8_t pid;
        glm::i32vec3 start, end;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(BlockLine);
        _DECL_DISPATCH_FUNC;
    };

    struct CTFState {
        uint8_t team1_score, team2_score, cap_limit;
        bool team1_has_intel : 1, team2_has_intel : 1;
        uint8_t team1_carrier, team2_carrier;
        glm::vec3 team1_flag, team2_flag, team1_base, team2_base;

        void read(ByteReader &r);
    };

    struct TCState {
        uint8_t territory_count;
        struct Territory {
            glm::vec3 position;
            uint8_t team;
        } territories[16]; // 16 MAX
    };

    struct StateData final : Loader {
        uint8_t pid;
        glm::u8vec3 fog_color;

        glm::u8vec3 team1_color, team2_color;
        std::string team1_name, team2_name;

        uint8_t mode;
        union ModeState {
            CTFState ctf; TCState tc;
            ModeState() : ctf() { }
        } state;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(StateData);
        _DECL_DISPATCH_FUNC;
    };

    struct KillAction final : Loader {
        uint8_t pid, killer;
        KILL type;
        uint8_t respawn_time;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(KillAction);
        _DECL_DISPATCH_FUNC;
    };

    struct ChatMessage final : Loader {
        uint8_t pid;
        CHAT type;
        std::string message;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(ChatMessage);
        _DECL_DISPATCH_FUNC;
    };

    struct PlayerLeft final : Loader {
        uint8_t pid;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(PlayerLeft);
        _DECL_DISPATCH_FUNC;
    };

    struct TerritoryCapture final : Loader {
        OBJECT object;
        TEAM winning;
        TEAM state;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(TerritoryCapture);
        _DECL_DISPATCH_FUNC;
    };

    struct ProgressBar final : Loader {
        uint8_t object;
        TEAM team;
        int8_t rate;
        float progress;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(ProgressBar);
        _DECL_DISPATCH_FUNC;
    };

    struct IntelCapture final : Loader {
        uint8_t pid, winning;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(IntelCapture);
        _DECL_DISPATCH_FUNC;
    };

    struct IntelPickup final : Loader {
        uint8_t pid;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(IntelPickup);
        _DECL_DISPATCH_FUNC;
    };

    struct IntelDrop final : Loader {
        uint8_t pid;
        glm::vec3 pos;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(IntelDrop);
        _DECL_DISPATCH_FUNC;
    };

    struct Restock final : Loader {
        uint8_t pid;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(Restock);
        _DECL_DISPATCH_FUNC;
    };

    struct FogColor final : Loader {
        glm::u8vec4 color;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(FogColor);
        _DECL_DISPATCH_FUNC;
    };

    struct WeaponReload final : Loader {
        uint8_t pid, primary, secondary;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(WeaponReload);
        _DECL_DISPATCH_FUNC;
    };

    struct ChangeTeam final : Loader {
        uint8_t pid;
        TEAM team;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(ChangeTeam);
        _DECL_DISPATCH_FUNC;
    };

    struct ChangeWeapon final : Loader {
        uint8_t pid;
        WEAPON weapon;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(ChangeWeapon);
        _DECL_DISPATCH_FUNC;
    };

    struct VersionResponse final : Loader {
        char client;
        // MAJOR.MINOR.PATCH
        glm::u8vec3 version;
        std::string os_info;

        void read(ByteReader &reader) override;
        void write(ByteWriter &writer) const override;
        _PACKET_ID(VersionResponse);
        _DECL_DISPATCH_FUNC;
    };

#undef _PACKET_ID
#undef _DECL_DISPATCH_FUNC

#define _DECL_HANDLE_FUNC(PacketType) virtual void handle(PacketType &packet) { }
    struct PacketHandler {
        virtual ~BasePacketHandler() = default;
        _DECL_HANDLE_FUNC(PositionData)
        _DECL_HANDLE_FUNC(OrientationData)
        _DECL_HANDLE_FUNC(WorldUpdate)
        _DECL_HANDLE_FUNC(InputData)
        _DECL_HANDLE_FUNC(WeaponInput)
        _DECL_HANDLE_FUNC(HitPacket)
        _DECL_HANDLE_FUNC(SetHP)
        _DECL_HANDLE_FUNC(GrenadePacket)
        _DECL_HANDLE_FUNC(SetTool)
        _DECL_HANDLE_FUNC(SetColor)
        _DECL_HANDLE_FUNC(ExistingPlayer)
        _DECL_HANDLE_FUNC(ShortPlayerData)
        _DECL_HANDLE_FUNC(MoveObject)
        _DECL_HANDLE_FUNC(CreatePlayer)
        _DECL_HANDLE_FUNC(BlockAction)
        _DECL_HANDLE_FUNC(BlockLine)
        _DECL_HANDLE_FUNC(StateData)
        _DECL_HANDLE_FUNC(KillAction)
        _DECL_HANDLE_FUNC(ChatMessage)
        _DECL_HANDLE_FUNC(PlayerLeft)
        _DECL_HANDLE_FUNC(TerritoryCapture)
        _DECL_HANDLE_FUNC(ProgressBar)
        _DECL_HANDLE_FUNC(IntelCapture)
        _DECL_HANDLE_FUNC(IntelPickup)
        _DECL_HANDLE_FUNC(IntelDrop)
        _DECL_HANDLE_FUNC(Restock)
        _DECL_HANDLE_FUNC(FogColor)
        _DECL_HANDLE_FUNC(WeaponReload)
        _DECL_HANDLE_FUNC(ChangeTeam)
        _DECL_HANDLE_FUNC(ChangeWeapon)
        _DECL_HANDLE_FUNC(VersionResponse)
    };
#undef _DECL_HANDLE_FUNC

#define _IMPL_DISPATCH_FUNC(PacketType) inline void PacketType::dispatch(BasePacketHandler &handler) { handler.handle(*this); };
    _IMPL_DISPATCH_FUNC(PositionData);
    _IMPL_DISPATCH_FUNC(OrientationData);
    _IMPL_DISPATCH_FUNC(WorldUpdate);
    _IMPL_DISPATCH_FUNC(InputData);
    _IMPL_DISPATCH_FUNC(WeaponInput);
    _IMPL_DISPATCH_FUNC(HitPacket);
    _IMPL_DISPATCH_FUNC(SetHP);
    _IMPL_DISPATCH_FUNC(GrenadePacket);
    _IMPL_DISPATCH_FUNC(SetTool);
    _IMPL_DISPATCH_FUNC(SetColor);
    _IMPL_DISPATCH_FUNC(ExistingPlayer);
    _IMPL_DISPATCH_FUNC(ShortPlayerData);
    _IMPL_DISPATCH_FUNC(MoveObject);
    _IMPL_DISPATCH_FUNC(CreatePlayer);
    _IMPL_DISPATCH_FUNC(BlockAction);
    _IMPL_DISPATCH_FUNC(BlockLine);
    _IMPL_DISPATCH_FUNC(StateData);
    _IMPL_DISPATCH_FUNC(KillAction);
    _IMPL_DISPATCH_FUNC(ChatMessage);
    _IMPL_DISPATCH_FUNC(PlayerLeft);
    _IMPL_DISPATCH_FUNC(TerritoryCapture);
    _IMPL_DISPATCH_FUNC(ProgressBar);
    _IMPL_DISPATCH_FUNC(IntelCapture);
    _IMPL_DISPATCH_FUNC(IntelPickup);
    _IMPL_DISPATCH_FUNC(IntelDrop);
    _IMPL_DISPATCH_FUNC(Restock);
    _IMPL_DISPATCH_FUNC(FogColor);
    _IMPL_DISPATCH_FUNC(WeaponReload);
    _IMPL_DISPATCH_FUNC(ChangeTeam);
    _IMPL_DISPATCH_FUNC(ChangeWeapon);
    _IMPL_DISPATCH_FUNC(VersionResponse);
#undef _IMPL_DISPATCH_FUNC

    inline std::unique_ptr<Loader> get_loader(PACKET id) {
        switch(id) {
            case PACKET::PositionData:  return std::make_unique<PositionData>();
            case PACKET::OrientationData: return std::make_unique<OrientationData>();
            case PACKET::WorldUpdate: return std::make_unique<WorldUpdate>();
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
            case PACKET::ChangeTeam: return std::make_unique<ChangeTeam>();
            case PACKET::ChangeWeapon: return std::make_unique<ChangeWeapon>();
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
