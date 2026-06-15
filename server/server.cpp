#include <iostream>
#include <enet/enet.h>

int main(){

    if(enet_initialize()!=0){
        std::fprintf(stderr,"ENet Initialization Failed");
        return EXIT_FAILURE;
    }
    
    atexit(enet_deinitialize);

    ENetHost* server;
    ENetEvent event;
    //since enet_host_create requres address we will create it first for server
    ENetAddress address;

    //tell that let host be anyone, place holder for 0
    address.host = ENET_HOST_ANY;
    address.port = 7777;

    server = enet_host_create(&address,6,1,0,0);
    if(server==NULL){
        std::fprintf(stderr,"An error occured while creating enet server host\n");
        return EXIT_FAILURE;
    }

    while(true){
        while(enet_host_service(server,&event,1000) > 0){
            switch(event.type){
                case ENET_EVENT_TYPE_CONNECT:
                    std::cout<<"A new client connected from host: "<<event.peer->address.host<<" and port: "<<event.peer->address.port<<std::endl;
                    break;
                case ENET_EVENT_TYPE_RECEIVE:
                    std::cout<<"Packet Lenght - "<<event.packet->dataLength<<"\nPacket Data - "<<event.packet->data<<"\nhost - "<<event.peer->address.host<<"\nPort - "<<event.peer->address.port<<"\nChannelID - "<<event.channelID<<std::endl; 
                    break;
                case ENET_EVENT_TYPE_DISCONNECT:
                    std::cout<<"Disconnected Host - "<<event.peer->address.host<<"\nPort - "<<event.peer->address.port<<std::endl;
                    break;
            }
        }
    }
}