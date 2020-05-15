#include <thread>

#include "Server.h"
#include "Player.h"
#include "GameSession.h"


int main() {
    Server server;
    Player alice("Alice", &server);
    Player bob("Bob", &server);
    Player carl("Carl", &server);

    std::thread aliceThread(std::ref(alice));
    std::thread bobThread(std::ref(bob));
    std::thread carlThread(std::ref(carl));

    std::thread game1Thread(GameSession(1, &server));
    std::thread game2Thread(GameSession(2, &server));

    aliceThread.join();
    bobThread.join();
    carlThread.join();
    game1Thread.join();
    game2Thread.join();

    return 0;
}
