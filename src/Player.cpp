#include "Player.h"

#include <utility>

Player::Player(std::string name, Server *server) : playerName(std::move(name)), currentPlayerState(PlayerState::Idle),
                                                   server(server) {
}

void Player::operator()() {
    // TODO implement
}

void Player::onGameFound(const GameSession *gameSession) {
    // TODO implement
}

Player::PlayerState Player::getState() const {
    return currentPlayerState;
}

std::string Player::getName() const {
    return playerName;
}
