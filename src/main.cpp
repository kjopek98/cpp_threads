#include <ncurses.h>
#include <thread>
#include <vector>
#include "Server.h"
#include "Player.h"
#include "GameSession.h"

WINDOW * win;

int height, width, start_y, start_x;
std::vector< Player*> players;
std::vector< GameSession*> gameSessions;

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

void generateWindow( std::vector<Player*> players, std::vector<GameSession*> gameSessions )
{

    while(1) {
        int i = 4;
        mvwprintw(win, 1, 1, "SO2 Project - Matchmaker ");
        mvwprintw(win, 2, 1, "Krzysztof Jopek 241406, Jakub Plona 241284 ");
        for (auto &player : players) {
            mvwprintw(win, i, 1, "Player %s - obecny stan: %s", player->getName().c_str(),
                      getPlayerState(player).c_str());
            wrefresh(win);
            i++;
        }
        for (auto &gameSession : gameSessions) {
            mvwprintw(win, i , 1, "Gra nr %d - obecny stan: %s", gameSession->getId(),
                    getGameState(gameSession).c_str());
            wrefresh(win);
            i++;
        }
        wrefresh(win);
    }

}


int main() {

    initscr();
    noecho();
    cbreak();

    int yMax, xMax;
    getmaxyx(stdscr, yMax,xMax);

    height=15, width=xMax - 12, start_y=yMax - 20, start_x=5;
    win = newwin(height, width, start_y, start_x);


    box(win,0,0);
    refresh();
    wrefresh(win);

    Server server;

    Player alice("Alice", &server);
    players.push_back(&alice);
    Player bob("Bob", &server);
    players.push_back(&bob);
    Player carl("Carl", &server);
    players.push_back(&carl);

    GameSession game1(1, &server);
    gameSessions.push_back(&game1);
    GameSession game2(2, &server);
    gameSessions.push_back(&game2);

    std::thread GUIThread(&generateWindow, std::ref(players), std::ref(gameSessions));

    std::thread aliceThread(std::ref(alice));
    std::thread bobThread(std::ref(bob));
    std::thread carlThread(std::ref(carl));

    std::thread game1Thread(std::ref(game1));
    std::thread game2Thread(std::ref(game2));

    GUIThread.join();
    aliceThread.join();
    bobThread.join();
    carlThread.join();
    game1Thread.join();
    game2Thread.join();

    endwin();
    return 0;
}

