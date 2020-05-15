#ifndef CPP_THREADS_REQUESTDATA_H
#define CPP_THREADS_REQUESTDATA_H

#include <vector>

#include "Player.h"
#include "GameSession.h"

class RequestData {

public:
    enum class Type {
        RequestGame,
        CancelRequestGame,
        RequestPlayers,
        CancelRequestPlayers
    };
    Type type;

    Player *player;

    GameSession *gameSession;
    int nPlayers;
    std::vector<Player *> *acquiredPlayers;

};

#endif //CPP_THREADS_REQUESTDATA_H
