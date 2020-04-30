#include <thread>

#include "Server.h"
#include "Player.h"
#include "GameSession.h"

int main() {
    Server server;

    std::thread aliceThread(Player("Alice", &server));
    std::thread bobThread(Player("Bob", &server));
    std::thread cThread(Player("C", &server));

    std::thread game1Thread(GameSession(1, &server));
    std::thread game2Thread(GameSession(2, &server));

    aliceThread.join();
    bobThread.join();
    cThread.join();
    game1Thread.join();
    game2Thread.join();

    return 0;
}
