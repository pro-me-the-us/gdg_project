#include <iostream>
#include <enet/enet.h>

int main(){
    //initialising the enet
    //enet_init. return 0 for not error and return error to std::cerr if any error
    if(enet_initialize()!=0){
        std::fprintf(stderr, "An error has occured in enet initialization\n");
        return EXIT_FAILURE;
    };

    atexit(enet_deinitialize);

    ENetHost* Client;
    //enet_host_create (Address, PeerCount, ChannelLimit, IncomingBandwidth, OutgoingBandwidth)
    //Address = NULL -> Tells enet that I am a client
    //PeerCount -> tells how many other hosts this host is allowed to connect to, it is 1 since client only need to connect to the server
    Client = enet_host_create(NULL,1,1,0,0);
    if(Client==NULL){
        std::fprintf(stderr,"An error occured while creating enet client host\n");
        return EXIT_FAILURE;
    }

    //hold the ip and port of the server
    ENetAddress address;
    //holds the data recceived from the server
    ENetEvent event;
    //tells us what we are connecting to,i.e. the peers of our host, for client it is 1 -> server, for server it is many clients
    ENetPeer* peer;


    //packing the server IP and Port into the ENetAddress structure variable "address"
    enet_address_set_host(&address,"127.0.0.1");
    address.port = 7777;

    //establishing the connection, connecting the client to the defined IP and Port
    //1 represents Channel Count, 0 represent User data that we want to share, 0 for now
    peer = enet_host_connect(Client,&address,1,0);
    if(peer == NULL){
        std::fprintf(stderr,"No avaliable peer to connect to\n");
        return EXIT_FAILURE;
    }

    //used to listen to the server - clinet connection and check if we got any data from the server
    if(enet_host_service(Client,&event,5000)>0 && event.type == ENET_EVENT_TYPE_CONNECT){
        std::cout<<"Successfully connected to 127.0.0.1:7777"<<std::endl;
    }   
    else{
        enet_peer_reset(peer);
        std::cout<<"Failed connected to 127.0.0.1:7777"<<std::endl;
        return EXIT_SUCCESS;
    }

    while(enet_host_service(Client,&event,1000)>0){
        switch(event.type){
            case ENET_EVENT_TYPE_RECEIVE:
                std::cout<<"Packet Lenght - "<<event.packet->dataLength<<"\nPacket Data - "<<event.packet->data<<"\nhost - "<<event.peer->address.host<<"\nPort - "<<event.peer->address.port<<"\nChannelID - "<<event.channelID<<std::endl; 
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout<<"Disconnected Host - "<<event.peer->address.host<<"\nPort - "<<event.peer->address.port<<std::endl;
                break;
        }
    }  

    enet_peer_disconnect(peer,0);

    while(enet_host_service(Client,&event,3000)>0){
        switch(event.type){
            case ENET_EVENT_TYPE_RECEIVE:
                enet_packet_destroy(event.packet);
                break;
            case ENET_EVENT_TYPE_DISCONNECT:
                std::cout<<"Disconnection Succeeded";
                break;
        }
    }

    return EXIT_SUCCESS;
}