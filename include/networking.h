#ifndef NETWORKING_H
#define NETWORKING_H

#include <iostream>
#include <vector>
#include <cstring>
#include "enet/enet.h"

// avoid struct padding issues across different machines
#pragma pack(push, 1)

struct PlayerState {
    int id;
    float x;
    float y;
    int health;
    int maxHealth;
    int direction;
};

// client sends this to host when they shoot
struct BulletFire {
    float startx, starty;
    float targetx, targety;
};

// host sends this to everyone so all clients spawn the bullet visually
struct BulletSpawn {
    int shooterId;
    float startx, starty;
    float targetx, targety;
};

// host sends this when a bullet hits someone
struct PlayerHit {
    int victimId;
    int shooterId;
    int heartsLeft;
};

// host sends updated scores to everyone after a kill
struct ScoreUpdate {
    int scores[6];
};

#pragma pack(pop)

// used to identify what kind of packet we received
enum MessageType {
    MSG_PLAYER_STATE,
    MSG_CHAT,
    MSG_MAP_SYNC,
    MSG_BULLET_FIRE,
    MSG_BULLET_SPAWN,
    MSG_PLAYER_HIT,
    MSG_SCORE_UPDATE
};

// send map number to a newly joined client so they load the same map as host
inline void sendMapSync(ENetPeer* peer, int mapNumber) {
    size_t size = sizeof(MessageType) + sizeof(int);
    std::vector<char> buffer(size);

    MessageType type = MSG_MAP_SYNC;
    memcpy(buffer.data(), &type, sizeof(MessageType));
    memcpy(buffer.data() + sizeof(MessageType), &mapNumber, sizeof(int));

    // use reliable so client never misses the map info
    ENetPacket* packet = enet_packet_create(buffer.data(), size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 1, packet);
}

// holds everything we need to know about a player we got from network
struct RemotePlayer {
    int id;
    bool active = false;
    float prevX, prevY;        // where they were before last update
    float currentX, currentY; // where they are now
    float t;                   // 0 to 1, how far between prev and current we are
    int health = 3;
    int maxHealth = 3;
    int direction = 1;         // 1=up 2=down 3=left 4=right
};

// simple linear interpolation for smooth movement between network updates
inline float netlerp(float a, float b, float t) {
    return a + (b - a) * t;
}

// send our position, health and direction to a specific peer
inline void sendPlayerState(ENetPeer* peer, int id, float x, float y, int health, int maxHealth, int direction) {
    size_t size = sizeof(MessageType) + sizeof(PlayerState);
    std::vector<char> buffer(size);

    MessageType type = MSG_PLAYER_STATE;
    memcpy(buffer.data(), &type, sizeof(MessageType));

    PlayerState state;
    state.id = id;
    state.x = x;
    state.y = y;
    state.health = health;
    state.maxHealth = maxHealth;
    state.direction = direction;
    memcpy(buffer.data() + sizeof(MessageType), &state, sizeof(PlayerState));

    // unreliable is fine here, we send position 20 times per second anyway
    ENetPacket* packet = enet_packet_create(buffer.data(), size, 0);
    enet_peer_send(peer, 0, packet);
}

// host uses this to tell everyone about a player's updated position
inline void broadcastPlayerState(ENetHost* host, int id, float x, float y, int health, int maxHealth, int direction) {
    size_t size = sizeof(MessageType) + sizeof(PlayerState);
    std::vector<char> buffer(size);

    MessageType type = MSG_PLAYER_STATE;
    memcpy(buffer.data(), &type, sizeof(MessageType));

    PlayerState state;
    state.id = id;
    state.x = x;
    state.y = y;
    state.health = health;
    state.maxHealth = maxHealth;
    state.direction = direction;
    memcpy(buffer.data() + sizeof(MessageType), &state, sizeof(PlayerState));

    ENetPacket* packet = enet_packet_create(buffer.data(), size, 0);
    enet_host_broadcast(host, 0, packet);
}

// send any packet type to a single peer, reliable by default for important messages
template<typename T>
inline void sendMsg(ENetPeer* peer, MessageType type, const T& payload, bool reliable = true) {
    size_t size = sizeof(MessageType) + sizeof(T);
    std::vector<char> buf(size);

    memcpy(buf.data(), &type, sizeof(MessageType));
    memcpy(buf.data() + sizeof(MessageType), &payload, sizeof(T));

    ENetPacket* p = enet_packet_create(buf.data(), size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_peer_send(peer, reliable ? 1 : 0, p);
}

// broadcast any packet type to all peers, reliable by default
template<typename T>
inline void broadcastMsg(ENetHost* host, MessageType type, const T& payload, bool reliable = true) {
    size_t size = sizeof(MessageType) + sizeof(T);
    std::vector<char> buf(size);

    memcpy(buf.data(), &type, sizeof(MessageType));
    memcpy(buf.data() + sizeof(MessageType), &payload, sizeof(T));

    ENetPacket* p = enet_packet_create(buf.data(), size, reliable ? ENET_PACKET_FLAG_RELIABLE : 0);
    enet_host_broadcast(host, reliable ? 1 : 0, p);
}

#endif