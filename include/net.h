#pragma once
#include "util/except.h"
#include "enet/enet.h"

#include "net/packet.h"
#include "common.h"

namespace ace { class GameClient; }

namespace net {
    struct BaseNetClient {
        BaseNetClient();
        virtual ~BaseNetClient();
        ACE_NO_COPY_MOVE(BaseNetClient)

        void update(double dt);
        void send(const void *data, size_t len, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) const;

        virtual void on_connect(const ENetEvent &event) = 0;
        virtual void on_disconnect(const ENetEvent &event) = 0;
        virtual void on_receive(const ENetEvent &event) = 0;

        ENetHost *host;
        ENetPeer *peer;
    };

    struct NetworkClient : BaseNetClient {
        NetworkClient(ace::GameClient &client);
        ~NetworkClient();
        ACE_NO_COPY_MOVE(NetworkClient)

        ByteWriter map_writer;
        std::vector<net::ExistingPlayer> players;
        ace::GameClient &client;

        bool connected;
        DISCONNECT disconnect_reason;

        void on_connect(const ENetEvent& event) final;
        void on_disconnect(const ENetEvent& event) final;
        void on_receive(const ENetEvent& event) final;

        void send_packet(PACKET id, Loader &pkt, enet_uint32 flags = ENET_PACKET_FLAG_RELIABLE) const;
        void connect(const char *host, int port);
    };

    inline const char *get_disconnect_reason(DISCONNECT reason) {
        switch(reason) {
        case DISCONNECT::BANNED: return "You have been banned from this server.";
        case DISCONNECT::KICKED: return "You have been kicked from this server.";
        case DISCONNECT::WRONG_VERSION: return "Mismatch between server/client version.";
        case DISCONNECT::FULL: return "Server is full!";
        default: return "Unknown reason.";
        }
    }
}
