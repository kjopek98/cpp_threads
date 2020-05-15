#include "GameSession.h"

#include <thread>
#include <chrono>

GameSession::GameSession(unsigned int id, Server *server) : id(id), currentGameSession(GameSessionState::Idle),
                                                            server(server) {
}

void GameSession::onPlayersCollected(const std::vector<Player *> &collectedPlayers) {
    // TODO simulate the game being played
    for (const auto &player : collectedPlayers) {
        player->onGameFound(this);
    }
    std::this_thread::sleep_for(std::chrono::seconds(10));
}

void GameSession::operator()() {
    // TODO implement
}

unsigned int GameSession::getId() const {
    return id;
}

GameSession::GameSessionState GameSession::getState() const {
    return currentGameSession;
}

int GameSession::getCurrentSessionDuration() const {
    return currentSessionDuration;
}
