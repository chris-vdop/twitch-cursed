#include <iostream>
#include <cstdlib>
#include <ncurses.h>
#include <menu.h>
#include "twitch.h"
#include <menu.h>
#include <cstdlib>
#include <vector>
#include "configFile.h"
#include "pluginBase.h"



pluginBase* activatedPlugin = NULL;

int x,y;


WINDOW* titleWin = NULL;
WINDOW* pluginWin = NULL;

MENU* moduleMenu;


char nameBuffer[100][10];
char titleBuffer[100][100];


void loadConfig(string _fileName);
void printTitle();


void emptyBoxWin(WINDOW* _win)
{
    werase(_win);
    box(_win,0,0);
    refresh();
    wrefresh(_win);
}

void listModules();
void loadModule(int _index);

void quit(void)
{
    if (activatedPlugin != NULL)
        activatedPlugin->cleanUpCurses();

    endwin();
}


int main(void)
{
    int ch;
    initscr();
    atexit(quit);
    start_color();
    clear();
    noecho();
    curs_set(0);
    cbreak();
    nl();
    keypad(stdscr, TRUE);

    //set stdscr red
    init_pair(1, COLOR_BLACK, COLOR_RED);
    bkgd(COLOR_PAIR(1));
//
    refresh();
    loadConfig("config.cfg");
    clear();
    getmaxyx(stdscr,y,x);

    box(stdscr,0,0);
    if (y > 30 && configFile::getInstance()->getContents("printLogo") != "false")
    {
        titleWin = newwin(7,x-2,1,1);
        pluginWin = newwin(y-2-7,x-2,8,1);
        box(titleWin,0,0);
        printTitle();
    }
    else
        pluginWin = stdscr;

    box(pluginWin,0,0);



    refresh();
    if (titleWin != NULL)
        wrefresh(titleWin);

    wrefresh(pluginWin);


    listModules();


    return (0);
}



void loadConfig(string _fileName)
{
    if (configFile::getInstance()->readFile(_fileName) != 0)
        exit(-1);
    if (configFile::getInstance()->parseFile() != 0)
        exit(-1);
}


void printTitle()
{
    std::string logo[]= {
        "  __         _ __      __   _____                    __",
        " / /__    __(_) /_____/ /  / ___/_ _________ ___ ___/ /",
        "/ __/ |/|/ / / __/ __/ _ \\/ /__/ // / __(_-</ -_) _  / ",
        "\\__/|__,__/_/\\__/\\__/_//_/\\___/\\_,_/_/ /___/\\__/\\_,_/  ",
        "                                                       "
    };
    int centerposx = (x - logo[0].length())/2;
    for (int i =0; i < 5; i++)
        mvwprintw(titleWin,i+1,centerposx,logo[i].c_str());


}


void listModules()
{
    int x,y;
    getmaxyx(pluginWin,y,x);

    vector<ITEM*> pluginEntries;
    pluginEntries.resize(pluginList::getInstance()->activated_classes.size()+2);

    for (int i = 0; i < pluginList::getInstance()->activated_classes.size(); i++)
    {
        pluginEntries.at(i) = new_item(pluginList::getInstance()->activated_classes.at(i)->getName().c_str(),pluginList::getInstance()->activated_classes.at(i)->getDescription().c_str());
    }
    pluginEntries.at(pluginList::getInstance()->activated_classes.size()) = new_item("Exit","Return to console");
    pluginEntries.at(pluginList::getInstance()->activated_classes.size()+1) = 0;


    moduleMenu = new_menu(&pluginEntries[0]);

    WINDOW* win;

    win = derwin(pluginWin,y-4, x-4, 2, 2);
    set_menu_win (moduleMenu, win);
    set_menu_sub (moduleMenu, derwin(win, y-8, x-8, 3, 2));
    set_menu_mark (moduleMenu,"-->");
    box(win, 0, 0);
    mvwaddstr(win, 1, (x-26)/2, "***** Choose Module! *****");
    post_menu(moduleMenu);

    refresh();
    wrefresh(win);

    int ch;
    int chosen = -1;
    while ((ch = getch()))
    {
        switch(ch)
        {
        case KEY_DOWN:
            menu_driver(moduleMenu, REQ_DOWN_ITEM);
            break;
        case KEY_UP:
            menu_driver(moduleMenu, REQ_UP_ITEM);
            break;
        case 0xA:
            if(item_index(current_item(moduleMenu)) == pluginEntries.size()-2)
                exit(0);
            else
                chosen = item_index(current_item(moduleMenu));
        }
        if (chosen != -1)
            break;
        wrefresh(win);
    }
    //we have chosen the plugin we want, clean up the pluginWindow and start it!
    unpost_menu(moduleMenu);
    free_menu(moduleMenu);
    for( int i=0; i<=pluginEntries.size()-2; i++)
    {
        free_item(pluginEntries[i]);
    }
    delwin(win);
    emptyBoxWin(pluginWin);

    loadModule(chosen);
}



void loadModule(int _index)
{
    int ch;
    activatedPlugin = pluginList::getInstance()->activated_classes.at(_index);

    if (activatedPlugin != NULL)
        activatedPlugin->runGui(pluginWin);


    while((ch=getch()) != 'q')
    {
        if(activatedPlugin!=NULL)
            activatedPlugin->handleChar(ch);
    }
    
    emptyBoxWin(pluginWin);


    listModules();

}


