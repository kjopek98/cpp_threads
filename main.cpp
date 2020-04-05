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
//#include <ncurses.h>

using namespace std;


struct GameSession{
    GameSession(){};
    bool isFull = false;
    mutex mtx;
    vector <unsigned int> players;
};

class Player{
    int id;

public:
    Player();


    void searchingGame(){};


};


int main() {
    return 0;
}

