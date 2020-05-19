#include <ncurses.h>
#include <thread>

#include "Server.h"
#include "Player.h"
#include "GameSession.h"

#define RED     1
#define YELLOW  2
#define GREEN   3
#define WHITE   4

WINDOW * win;

int height, width, start_y, start_x;


void generateWindow(Player &alice, Player &bob, Player &carl, GameSession &game1, GameSession &game2)
{

    mvwprintw(win, 1, 1, "SO2 Project - Matchmaker ");
    mvwprintw(win, 2, 1, "Krzysztof Jopek 241406, Jakub Plona 241284 ");
    mvwprintw(win, height-6, width-60, "Player %s - obecny stan: ", alice.getName().c_str());
    mvwprintw(win, height-5, width-60, "Player %s - obecny stan: ", bob.getName().c_str() );
    mvwprintw(win, height-4, width-60, "Player %s - obecny stan: %s", carl.getName().c_str() );
    mvwprintw(win, height-3, width-60, "Gra nr %d - obecny stan: %s", game1.getId());
    mvwprintw(win, height-2, width-60, "Gra nr %d - obecny stan: %s", game2.getId());

    refresh();
    wrefresh(win);

    wgetch(win);

    //endwin();
    // exit(EXIT_SUCCESS);
}


int main() {

    initscr();
    noecho();
    cbreak();

    int yMax, xMax;
    getmaxyx(stdscr, yMax,xMax);

     height=15, width=xMax - 12, start_y=yMax - 20, start_x=5;

     win = newwin(height, width, start_y, start_x);

    if(!has_colors()){
        printw("Terminal does not support color");
        getch();
        return -1;
    }

    box(win,0,0);
    refresh();
    wrefresh(win);

    Server server;

    Player alice("Alice", &server);
    Player bob("Bob", &server);
    Player carl("Carl", &server);

    GameSession game1(1, &server);
    GameSession game2(2, &server);

    std::thread GUIThread(&generateWindow, std::ref(alice),std::ref(bob),std::ref(carl),std::ref(game1), std::ref(game2));

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



    return 0;
}

