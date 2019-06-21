#include "net/net.h"

#define ZLIB_CONST
#include "zlib.h"

#include "game_client.h"
#include "scene/game.h"
#include "scene/loading.h"
#include "scene/menu.h"

namespace ace { namespace net {
    std::vector<uint8_t> inflate(const uint8_t *data, size_t len, size_t initial_size) {
        z_stream stream;

        stream.zalloc = Z_NULL;
        stream.zfree = Z_NULL;
        stream.opaque = Z_NULL;

        std::vector<uint8_t> buf(initial_size);

        stream.next_in = data;
        stream.avail_in = len;
        stream.next_out = buf.data();
        stream.avail_out = buf.size();
        
        int status = inflateInit(&stream);
        do {
            if (status != Z_OK && status != Z_BUF_ERROR) {
                inflateEnd(&stream);
                THROW_ERROR("ERROR INFLATING DATA (size {}): {}", len, zError(status));
            }

            status = inflate(&stream, Z_FINISH);

            size_t position = stream.next_out - buf.data();
            if (position >= buf.size()) {
                buf.resize(position * 2);
            }
            stream.avail_out = buf.size() - position;
            stream.next_out = buf.data() + position;
        } while (status != Z_STREAM_END);
        inflateEnd(&stream);

        return buf;
    }

    constexpr int VERSION = 3;

    // lmao awful design, or GENIUS?
    BaseNetClient::enet_initer BaseNetClient::initer;

    BaseNetClient::~BaseNetClient() {
        if (this->peer != nullptr)
            enet_peer_disconnect_now(this->peer, 0);
        enet_host_destroy(this->host);
    };

    BaseNetClient::BaseNetClient() : host(enet_host_create(nullptr, 1, 1, 0, 0)), peer(nullptr) {
        if (this->host == nullptr) {
            THROW_ERROR("COULD NOT ALLOCATE ENET HOST");
        }
    }

    void BaseNetClient::update(double dt) {
        ENetEvent event;
        while (enet_host_service(this->host, &event, 0) > 0) {
            switch (event.type) {
            case ENET_EVENT_TYPE_CONNECT:
                this->on_connect(event);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                this->peer = nullptr;
                this->already_requested_disconnect = false;
                this->on_disconnect(event);
                break;
            case ENET_EVENT_TYPE_RECEIVE:
                this->on_receive(event);
                enet_packet_destroy(event.packet);
                break;
            default:
                break;
            }
        }
    }

    void BaseNetClient::connect(const char *host, int port, uint32_t data) {
        // if (this->peer != nullptr) {
        //     enet_peer_disconnect(this->peer, 0);
        //     this->peer = nullptr;
        // }
        this->disconnect();

        fmt::print("CONNECTING TO {}:{}\n", host, port);
        ENetAddress address;
        if (enet_address_set_host(&address, host) < 0) {
            THROW_ERROR("RESOLUTION FAILIURE\n");
        }
        address.port = port;

        this->peer = enet_host_connect(this->host, &address, 1, data);

        if (this->peer == nullptr) {
            THROW_ERROR("FAILED TO ALLOCATE PEER\n");
        }
    }

    void BaseNetClient::disconnect() {
        if (this->peer == nullptr) return;
            
        if (this->already_requested_disconnect) {
            enet_peer_disconnect_now(this->peer, 0);
            this->peer = nullptr;
            this->already_requested_disconnect = false;
            ENetEvent event;
            event.data = 0;
            this->on_disconnect(event);
        } else {
            enet_peer_disconnect_later(this->peer, 0);
            this->already_requested_disconnect = true;
        }
    }

    void BaseNetClient::send(const void *data, size_t len, enet_uint32 flags) const {
        if (this->peer == nullptr) return fmt::print(stderr, "SENDING PACKET ON INVALID PEER\n");

        fmt::MemoryWriter hex_data;
        for (size_t i = 0; i < len; i++) {
            hex_data.write("\\x{:02X}", reinterpret_cast<const char *>(data)[i]);
        }

        ENetPacket *packet = enet_packet_create(data, len, flags);
        if (packet == nullptr) THROW_ERROR("COULD NOT ALLOCATE ENET PACKET FOR DATA {}\n", hex_data.str());
        int err = enet_peer_send(this->peer, 0, packet);
        if(err < 0) {
            fmt::print(stderr, "COULD NOT SEND PACKET WITH ERR {} FOR DATA {}\n", err, hex_data.str());
        }
    }

    NetworkClient::NetworkClient(ace::GameClient &client) : BaseNetClient(), client(client), disconnect_reason(DISCONNECT::INVALID), state(NetState::UNCONNECTED) {
        enet_host_compress_with_range_coder(this->host);
    }

    void NetworkClient::connect(const Server &server) {
        if(server.version != "0.75")
            THROW_ERROR("Ace of Spades {} unsupported!\n", server.version);
        
        BaseNetClient::connect(server.ip.c_str(), server.port, VERSION);
        this->set_state(NetState::CONNECTING);
    }

    void NetworkClient::on_connect(const ENetEvent &event) {
        this->set_state(NetState::CONNECTED);
    }

    void NetworkClient::on_disconnect(const ENetEvent &event) {
        this->disconnect_reason = DISCONNECT(event.data);
        this->set_state(NetState::DISCONNECTED);
        fmt::print("DISCONNECTED: {}\n", get_disconnect_reason(this->disconnect_reason));
        this->client.set_scene<ace::scene::MainMenuScene>();
    }

    void NetworkClient::on_receive(const ENetEvent &event) {
        ByteReader br(event.packet->data, event.packet->dataLength);
        const auto packet_id = PACKET(br.read<uint8_t>());
        
        switch (packet_id) {
        case PACKET::MapStart: {
            this->map_writer.clear();
            this->map_writer.reserve(br.read<uint32_t>());
            this->set_state(NetState::MAP_TRANSFER);
            break;
        } 
        case PACKET::MapChunk: {
            if (this->state != NetState::MAP_TRANSFER)
                fmt::print(stderr, "Receiving map chunks before map start???");
            this->map_writer.write(br.remaining_data(), br.remaining_size());
            break;
        } 
        case PACKET::HandShakeInit: {
            auto data = br.read<uint32_t>();
            ByteWriter bw;
            bw.write(static_cast<uint8_t>(PACKET::HandShakeReturn));
            bw.write(data);
            this->send_packet(bw);
            break;
        } 
        case PACKET::VersionRequest: {
            // TODO clean this up and define these constants elsewhere.
            net::VersionResponse response;
            response.client = 'a';
            response.version = { 0, 2, 3 }; // MAJOR.MINOR.PATCH
            response.os_info = SDL_GetPlatform();
            this->send_packet(response);
            break;
        } 
        default: {
            auto packet = get_loader(packet_id);
            if (packet == nullptr) {
                fmt::print("CRITICAL: UNKNOWN PACKET WITH ID {}\n", packet_id);
                break;
            }
            packet->read(br);
            this->client.scene->on_packet(packet_id, std::move(packet));
            break;
        }
        }
    }

    void NetworkClient::send_packet(const ByteWriter &writer, enet_uint32 flags) const {
//        if(id != PACKET::PositionData && id != PACKET::OrientationData && id != PACKET::InputData && id != PACKET::WeaponInput)
//            fmt::print("SENDING PACKET WITH ID {}\n", id);
        this->send(writer.data(), writer.size(), flags);
    }

    void NetworkClient::set_state(NetState state) {
        this->state = state;
        this->client.scene->on_net_event(state);
    }
}};