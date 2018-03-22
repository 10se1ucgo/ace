#include "net.h"

#include "zlib.h"

#include "scene/game.h"
#include "scene/loading.h"


namespace net {
    // inspired by Python std zlib.decompressobj()
//    struct InflateObject {
//        InflateObject() {
//            stream.zalloc = Z_NULL;
//            stream.zfree = Z_NULL;
//            stream.opaque = Z_NULL;
//            stream.avail_in = 0;
//            stream.next_in = Z_NULL;
//            inflateInit(&stream);
//        }
//    private:
//        z_stream stream;
//    };

    std::vector<uint8_t> inflate(uint8_t *data, size_t len, size_t initial_size) {
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
        enet_host_flush(this->host);
        enet_host_destroy(this->host);
        fmt::print("~BaseNetClient()\n");
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
        if (this->peer != nullptr) {
            enet_peer_disconnect(this->peer, 0);
        }

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

    void BaseNetClient::send(const void *data, size_t len, enet_uint32 flags) const {
        if (this->peer == nullptr) THROW_ERROR("SENDING PACKET ON INVALID PEER\n");

        fmt::MemoryWriter hex_data;
        for (size_t i = 0; i < len; i++) {
            hex_data.write("\\x{:02X}", reinterpret_cast<const char *>(data)[i]);
        }

        ENetPacket *packet = enet_packet_create(data, len, flags);
        if (packet == nullptr) THROW_ERROR("COULD NOT ALLOCATE ENET PACKET FOR DATA {}\n", hex_data.str());
        int err = enet_peer_send(this->peer, 0, packet);
        if(err < 0) {
            THROW_ERROR("COULD NOT SEND PACKET WITH ERR {} FOR DATA {}\n", err, hex_data.str());
        };

//        enet_packet_destroy(packet);
    }

    NetworkClient::NetworkClient(ace::GameClient &client) : BaseNetClient(), client(client), disconnect_reason(DISCONNECT::INVALID), state(NetState::UNCONNECTED) {
        enet_host_compress_with_range_coder(this->host);
    }

    NetworkClient::~NetworkClient() {
        if(this->peer != nullptr)
            enet_peer_disconnect(this->peer, 0);
        fmt::print("~NetworkClient()\n");
    }

    void NetworkClient::connect(const Server &server) {
        if(server.version != "0.75")
            THROW_ERROR("Ace of Spades {} unsupported!\n", server.version);
        
        BaseNetClient::connect(server.ip.c_str(), server.port, VERSION);
        state = NetState::CONNECTING;
    }

    void NetworkClient::on_connect(const ENetEvent &event) {
        state = NetState::CONNECTED;
    }

    void NetworkClient::on_disconnect(const ENetEvent &event) {
        state = NetState::DISCONNECTED;
        disconnect_reason = DISCONNECT(event.data);
        fmt::print("DISCONNECTED: {}\n", get_disconnect_reason(disconnect_reason));
//        client.set_scene<ace::scene::LoadingScene>();
    }

    void NetworkClient::on_receive(const ENetEvent &event) {
        ByteReader br(event.packet->data, event.packet->dataLength);
        const auto packet_id = PACKET(br.read<uint8_t>());
        
        switch (packet_id) {
        case PACKET::MapStart: {
            client.set_scene<ace::scene::LoadingScene>("aos://0:0");
            map_writer.clear();
            uint32_t siz = br.read<uint32_t>();
            map_writer.vec.reserve(siz);
            fmt::print("MAP START {}\n", siz);
            state = NetState::MAP_TRANSFER;
        } break;
        case PACKET::MapChunk: {
            if (state != NetState::MAP_TRANSFER)
                fmt::print(stderr, "Receiving map chunks before map start???");
            size_t len;
            uint8_t *data = br.get(&len);
            map_writer.write(data, len);
        } break;
//        case PACKET::StateData:
//            state = NetState::CONNECTED;
        default: {
            auto packet = get_loader(packet_id);
            if (packet == nullptr) {
                fmt::print("CRITICAL: UNHANDLED PACKET WITH ID {}\n", packet_id);
                break;
            };
            packet->read(br);
            client.scene->on_packet(packet_id, std::move(packet));
        } break;
        }
    }

    void NetworkClient::send_packet(PACKET id, const Loader &pkt, enet_uint32 flags) const {
        ByteWriter writer;
        writer.write(static_cast<uint8_t>(id));
        pkt.write(writer);
//        if(id != PACKET::PositionData && id != PACKET::OrientationData && id != PACKET::InputData && id != PACKET::WeaponInput)
//            fmt::print("SENDING PACKET WITH ID {}\n", id);
        this->send(writer.vec.data(), writer.vec.size(), flags);
    }
};