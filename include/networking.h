#ifndef NETWORKING_H
#define NETWORKING_H

#include <iostream>
#include <vector>
#include <cstring>
#include "enet/enet.h"

// packed to avoid padding differences between platforms
#pragma pack(push, 1)
struct PlayerState {
    int id;
    float x;
    float y;
    int health;
    int maxHealth;
    int direction;  // which way the player is facing
};
#pragma pack(pop)

// message types so receiver knows how to read incoming packets
enum MessageType {
    MSG_PLAYER_STATE,
    MSG_CHAT,
    MSG_MAP_SYNC
};

// send map number to client so both load the same map
inline void sendMapSync(ENetPeer* peer, int mapNumber){
    size_t size = sizeof(MessageType) + sizeof(int);
    std::vector<char> buffer(size);
    MessageType type = MSG_MAP_SYNC;
    memcpy(buffer.data(), &type, sizeof(MessageType));
    memcpy(buffer.data() + sizeof(MessageType), &mapNumber, sizeof(int));
    // reliable because missing this breaks the whole session
    ENetPacket* packet = enet_packet_create(buffer.data(), size, ENET_PACKET_FLAG_RELIABLE);
    enet_peer_send(peer, 1, packet);
}

// stores interpolation data for each remote player we receive from network
struct RemotePlayer {
    int id;
    bool active = false;        // is this slot in use
    float prevX, prevY;         // position before latest network update
    float currentX, currentY;  // latest received position
    float t;                    // interpolation progress 0 to 1
    int health = 3;
    int maxHealth = 3;
    int direction = 1;          // facing direction 1=up 2=down 3=left 4=right
};

// smoothly blend between two values using interpolation factor t
inline float netlerp(float a, float b, float t){
    return a + (b - a) * t;
}

// pack and send local player state to a specific peer
inline void sendPlayerState(ENetPeer* peer, int id, float x, float y, int health, int maxHealth, int direction){
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
    enet_peer_send(peer, 0, packet);
}

// broadcast local player state to all connected peers
inline void broadcastPlayerState(ENetHost* host, int id, float x, float y, int health, int maxHealth, int direction){
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

#endif