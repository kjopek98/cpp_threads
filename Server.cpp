//
// Created by krzysztof on 16.04.2020.
//

#include "Server.h"

using namespace  std;

Server::Server(queue<Player>& queue) {
    playerQueue = queue;
}

void Server::addPlayer(Player player){
    unique_lock<std::mutex> locker(mu);
    ready_cv.wait(locker, [=] { return playerQueue.size() < size_ ; });
    playerQueue.push(player);
    locker.unlock();
    ready_cv.notify_all();

}

//?Zwracamy  cały obiekt lub jedynie id playera
Player Server::releasePlayer(){
    unique_lock<std::mutex> locker(mu);
    Player releasedPlayer = playerQueue.front();
    playerQueue.pop();
    locker.unlock();
    ready_cv.notify_all();

    return releasedPlayer;
}


