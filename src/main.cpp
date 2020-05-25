#include <ncurses.h>
#include <thread>
#include <vector>
#include <chrono>
#include <iostream>
#include "Server.h"
#include "Player.h"
#include "GameSession.h"

WINDOW * win;



static std::string getPlayerState(Player *player){

    std::string answer;
    switch(player->getState()) {
        case Player::PlayerState::Idle:
            answer = "Idle                  ";
            break;
        case Player::PlayerState::WaitingForGame:
            answer = "Waiting for game      ";
            break;
        case Player::PlayerState::CancelingGameRequest:
            answer = "Canceling game request";
            break;
        case Player::PlayerState::Playing:
            answer = "Playing                ";
            break;
    }
     return answer;
}
static std::string getGameState(GameSession *gameSession){

    std::string answer;
    switch(gameSession->getState()) {
        case GameSession::GameSessionState::Idle:
            answer = "Idle               ";
            break;
        case GameSession::GameSessionState::WaitingForPlayers:
            answer = "Waiting for players";
            break;
        case GameSession::GameSessionState::CancelingSession:
            answer = "Canceling session  ";
            break;
        case GameSession::GameSessionState::Running:
            answer = "Running            ";
            break;
    }
    return answer;
}

static std::string getWorkerState(States::ServerState state){

    std::string answer;
    switch(state) {
        case States::ServerState::Idle:
            answer = "Idle                    ";
            break;
        case States::ServerState::RequestGameSession:
            answer = "RequestGameSession      ";
            break;
        case States::ServerState::CancelRequestGameSession:
            answer = "CancelRequestGameSession";
            break;
        case States::ServerState::RequestPlayers:
            answer = "RequestPlayers          ";
            break;
        case States::ServerState::CancelRequestPlayers:
            answer = "RequestPlayers          ";
            break;
    }
    return answer;
}

void generateWindow( std::vector<Player*> players, std::vector<GameSession*> gameSessions, Server &server )
{

    while(1) {
        int i = 4;
        mvwprintw(win, 1, 1, "SO2 Project - Matchmaker ");
        mvwprintw(win, 2, 1, "Krzysztof Jopek 241406, Jakub Plona 241284 ");
        for (auto &player : players) {
            mvwprintw(win, i, 1, "Player %s - obecny stan: %s", player->getName().c_str(),
                      getPlayerState(player).c_str());
            i++;
        }
        i++;
        for (auto &gameSession : gameSessions) {
            mvwprintw(win, i, 1, "Gra nr %d - obecny stan: %s", gameSession->getId(),
                      getGameState(gameSession).c_str());
            std::vector currentPlayers = gameSession->getCurrentPlayers();
            i++;
            mvwprintw(win, i, 1, "W grze:             ");
            int y = 10;
            for (auto &player : currentPlayers) {
                mvwprintw(win, i, y, "%s  ", player->getName().c_str());
                y = y + 10;
            }
            i++;
        }
        i++;
        std::vector waitingPlayers = server.getWaitingPlayerQueue();
        mvwprintw(win, i, 1, "Kolejka oczekujacych:                                                       ");
        int z = 28;
        for (auto &player : waitingPlayers) {
            mvwprintw(win, i, z, "%s ", player->getName().c_str());
            z = z + 3;
        }
        i= i+2;
        std::vector workersStates = server.getWorkerStates();
        int worker_no = 1;
        for (auto &state : workersStates) {
            mvwprintw(win, i, 1, "Stan workera serwera nr %d: %s", worker_no, getWorkerState(state).c_str());
            worker_no++;
            i++;
        }
        refresh();
        wrefresh(win);
    }

}


int main() {

    initscr();
    noecho();
    cbreak();

    int height, width, start_y, start_x;
    std::vector< Player*> players;
    std::vector< GameSession*> gameSessions;
    std::vector<std::thread> playerThreads;
    std::vector<std::thread> gameThreads;

    const int numberOfPlayers = 11;
    const int numberOfGames = 10;

    int yMax, xMax;
    getmaxyx(stdscr, yMax, xMax);

    height = 50, width = xMax - 12, start_y = yMax - 50, start_x = 5;
    win = newwin(height, width, start_y, start_x);

    box(win, 0, 0);
    refresh();
    wrefresh(win);

    Server server;

    std::string name;

    for (int i = 0; i < numberOfPlayers; i++) {
        name = std::to_string(i);
        players.push_back(new Player( name , &server));
    }

    for (int i = 0; i < numberOfGames; i++) {
        gameSessions.push_back(new GameSession( i, &server));
    }


    std::thread GUIThread(&generateWindow, std::ref(players), std::ref(gameSessions), std::ref(server));


    for (auto &player : players) {
      playerThreads.emplace_back(std::ref(*player));
    }

    for (auto &game : gameSessions) {
        gameThreads.emplace_back(std::ref(*game));
    }

    GUIThread.join();

    for (auto &player : playerThreads) {
        player.join();
    }

    for (auto &game : gameThreads) {
        game.join();
    }


    endwin();
    return 0;
}

