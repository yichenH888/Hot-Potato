#include "helper.hpp"

class singlePlayer {
   public:
    int socket_fd;
    int id;
    int numPlayer;
    string ip;  // For master, ip is master's ip; For player, ip is player's neighbour's ip
    singlePlayer() : socket_fd(0), id(0), numPlayer(0) {}
};

class PlayerFromMaster : public singlePlayer {
   public:
    int port;
    PlayerFromMaster() {
        port = 0;
    }
};

class PlayerFromPlayer : public singlePlayer {
   public:
    int rightNei_socket;
    int leftNei_socket;
    PlayerFromPlayer() : rightNei_socket(0), leftNei_socket(0) {}
};