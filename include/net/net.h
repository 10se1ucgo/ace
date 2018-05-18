#pragma once
#include "util/except.h"
#include "enet/enet.h"

#include "net/packet.h"
#include "common.h"

namespace ace { class GameClient; }

namespace ace { namespace net {
    std::vector<uint8_t> inflate(uint8_t *data, size_t len, size_t initial_size = 2 << 16);

    enum class NetState {
        UNCONNECTED,
        DISCONNECTED,
        CONNECTING,
        CONNECTED,
        MAP_TRANSFER,
        // PACK_TRANSFER,
    };

    struct Server {
        std::string ip;
        int port;
        std::string version;

        Server(std::string ip, int port, std::string version) :
            ip(std::move(ip)),
            port(port),
            version(std::move(version)) {
        }

        Server(const std::string &identifier): port(32887), version("0.75") {
            auto proto_pos = identifier.find(':', 0);

            if (identifier.substr(0, proto_pos) == "aos") {
                auto ip_pos = proto_pos + 3; // `:\\` 
                auto port_pos = identifier.find(':', ip_pos);
                auto version_pos = identifier.find(':', port_pos + 1);
                if (version_pos != std::string::npos && port_pos != std::string::npos) {
                    this->version = identifier.substr(version_pos + 1);
                }

                uint32_t ip = std::stoul(identifier.substr(ip_pos, port_pos - ip_pos));
                this->ip = fmt::format("{}.{}.{}.{}", ip >> 0 & 0xFF, ip >> 8 & 0xFF, ip >> 16 & 0xFF, ip >> 24 & 0xFF);
                if(port_pos != std::string::npos)
                    this->port = std::stoi(identifier.substr(port_pos + 1, version_pos - (port_pos + 1)));
            } else {
                this->ip = identifier.substr(0, proto_pos);
                if(proto_pos != std::string::npos)
                    this->port = std::stoi(identifier.substr(proto_pos + 1));
            }

            fmt::print("Server: {}:{}:{}\n", this->ip, this->port, this->version);
        }
    };

    struct BaseNetClient {
        BaseNetClient();
        virtual ~BaseNetClient();
        ACE_NO_COPY_MOVE(BaseNetClient)

        void update(double dt);
        void connect(const char *host, int port, uint32_t data=0);
        void disconnect();
        void send(const void *data, size_t len, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) const;

        virtual void on_connect(const ENetEvent &event) = 0;
        virtual void on_disconnect(const ENetEvent &event) = 0;
        virtual void on_receive(const ENetEvent &event) = 0;

        ENetHost *host;
        ENetPeer *peer;

        static struct enet_initer {
            enet_initer() { enet_initialize(); }
            ~enet_initer() { enet_deinitialize(); }
        } initer;
    };

    struct NetworkClient : BaseNetClient {
        NetworkClient(ace::GameClient &client);
        ACE_NO_COPY_MOVE(NetworkClient)

//        using BaseNetClient::connect;
        void connect(const Server &server);

        void on_connect(const ENetEvent& event) final;
        void on_disconnect(const ENetEvent& event) final;
        void on_receive(const ENetEvent& event) final;

        void send_packet(PACKET id, const Loader &pkt, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) const;
        void send_packet(const Loader &pkt, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) const {
            this->send_packet(pkt.packet_id(), pkt, flags);
        }
        // template<typename TLoader, typename = std::enable_if<!std::is_same_v<TLoader, Loader>>>
        // void send_packet(const TLoader &pkt, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) const {
        //     ByteWriter writer;
        //     writer.write(static_cast<uint8_t>(TLoader::packet_id2));
        //     pkt.write(writer);
        //     this->send(writer.vec.data(), writer.vec.size(), flags);
        // }
        
        ByteWriter map_writer;
        std::vector<net::ExistingPlayer> players;

        ace::GameClient &client;

//        bool connected;
        DISCONNECT disconnect_reason;
        NetState state;
    private:
        void set_state(NetState state);
    };

    inline const char *get_disconnect_reason(DISCONNECT reason) {
        switch(reason) {
        case DISCONNECT::BANNED: return "You have been banned from this server.";
        case DISCONNECT::KICKED: return "You have been kicked from this server.";
        case DISCONNECT::WRONG_VERSION: return "Server/client version mismatch!";
        case DISCONNECT::FULL: return "Server is full!";
        default: return "Unknown reason.";
        }
    }
}}
