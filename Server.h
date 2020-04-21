//
// Created by krzysztof on 16.04.2020.
//
#include <cstdlib>
#include <iostream>
#include <string>
#include <vector>
#include <ctime>
#include <numeric>
#include <cmath>
#include <sstream>
#include <thread>
#include <chrono>
#include <mutex>
#include <queue>
#include <condition_variable>
#include "Player.h"

#ifndef CPP_THREADS_SERVER_H
#define CPP_THREADS_SERVER_H

using namespace std;

class Server {

public:
    mutex mu;
    condition_variable ready_cv;

    Server( queue<Player>& playerQueue);
    void addPlayer(Player player);
    Player releasePlayer();

private:
    queue <Player> playerQueue;
    const unsigned int size_ = 10;

};


#endif //CPP_THREADS_SERVER_H
