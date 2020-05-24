#ifndef CPP_THREADS_STATES_H
#define CPP_THREADS_STATES_H

class States {

public:
    enum class ServerState {
        Idle,
        RequestGameSession,
        CancelRequestGameSession,
        RequestPlayers,
        CancelRequestPlayers
    };
};


#endif //CPP_THREADS_STATES_H
