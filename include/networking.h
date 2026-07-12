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
};
#pragma pack(pop)

// message types so receiver knows how to read incoming packets
enum MessageType {
    MSG_PLAYER_STATE,
    MSG_CHAT
};

// stores interpolation data for each remote player we receive from network
struct RemotePlayer {
    int id;
    bool active = false;        // is this slot in use
    float prevX, prevY;         // position before latest network update
    float currentX, currentY;  // latest received position
    float t;                    // interpolation progress 0 to 1
     int health = 3;    
    int maxHealth = 3;
};

// smoothly blend between two values using interpolation factor t
inline float netlerp(float a, float b, float t){
    return a + (b - a) * t;
}

// pack and send local player position to a specific peer
inline void sendPlayerState(ENetPeer* peer, int id, float x, float y){
    size_t size = sizeof(MessageType) + sizeof(PlayerState);
    std::vector<char> buffer(size);

    MessageType type = MSG_PLAYER_STATE;
    memcpy(buffer.data(), &type, sizeof(MessageType));

    PlayerState state;
    state.id = id;
    state.x = x;
    state.y = y;
    memcpy(buffer.data() + sizeof(MessageType), &state, sizeof(PlayerState));

    ENetPacket* packet = enet_packet_create(buffer.data(), size, 0);
    enet_peer_send(peer, 0, packet);
}

// broadcast local player position to all connected peers
inline void broadcastPlayerState(ENetHost* host, int id, float x, float y){
    size_t size = sizeof(MessageType) + sizeof(PlayerState);
    std::vector<char> buffer(size);

    MessageType type = MSG_PLAYER_STATE;
    memcpy(buffer.data(), &type, sizeof(MessageType));

    PlayerState state;
    state.id = id;
    state.x = x;
    state.y = y;
    memcpy(buffer.data() + sizeof(MessageType), &state, sizeof(PlayerState));

    ENetPacket* packet = enet_packet_create(buffer.data(), size, 0);
    enet_host_broadcast(host, 0, packet);
}

#endif