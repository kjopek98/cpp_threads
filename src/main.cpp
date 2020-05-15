#include <thread>

#include "Server.h"
#include "Player.h"
#include "GameSession.h"


int main() {
    Server server;

    Player alice("Alice", &server);
    Player bob("Bob", &server);
    Player carl("Carl", &server);

    GameSession game1(1, &server);
    GameSession game2(2, &server);

    std::thread aliceThread(std::ref(alice));
    std::thread bobThread(std::ref(bob));
    std::thread carlThread(std::ref(carl));

    std::thread game1Thread(std::ref(game1));
    std::thread game2Thread(std::ref(game2));

    aliceThread.join();
    bobThread.join();
    carlThread.join();
    game1Thread.join();
    game2Thread.join();

    return 0;
}
