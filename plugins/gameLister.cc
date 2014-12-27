/**
 * \author Christopher "VdoP" Regali
 * \file gameLister.cc
 * \brief contains myGameLister a simple game listing module
 * \date 2014
 */


#include "pluginBase.h"

#include "twitch.h"



class myGameLister: public pluginBase
{
public:
    myGameLister();
    ~myGameLister();

    void cleanUpCurses();
    void runGui(WINDOW* _pluginWindow);
    void handleChar(int _ch);

private:


    void makeMenu();
    void run_command(int _index);


//keeping the memory allocated for menu titles and descriptions
    char nameBuffer[100][10];
    char titleBuffer[100][100];

//interface to the twitch api
    twitchCPP myTwitch;


//curses stuff
    std::vector<ITEM*> it;
    MENU   *me;
    WINDOW *win;


};



static myGameLister* myGameL = new myGameLister();



myGameLister::~myGameLister()
{

}


void myGameLister::cleanUpCurses()
{
    unpost_menu(me);
    free_menu(me);
    for( int i=0; i<=it.size()-2; i++)
    {
        free_item(it[i]);
    }

    delwin(win);
    werase(pluginWindow);
    box(pluginWindow,0,0);
}

void myGameLister::handleChar(int _ch)
{

    switch(_ch)
    {
    case KEY_DOWN:
        menu_driver(me, REQ_DOWN_ITEM);
        break;
    case KEY_UP:
        menu_driver(me, REQ_UP_ITEM);
        break;
    case 0xA:
        if(item_index(current_item(me)) == it.size()-2)
            exit(0);
        else
            run_command(item_index(current_item(me)));
    }


    wrefresh(win);
}

void myGameLister::makeMenu()
{
    myTwitch.refreshGames();

    //we only display some games
    int entries = configFile::StrToInt(configFile::getInstance()->getContents("glist_count"));
    if (entries < 10)
        entries = 10;

    if (entries > myTwitch.getGameInfoCount())
        entries = myTwitch.getGameInfoCount();

    mvwprintw(pluginWindow,2,1,"Loaded %i games from twitch toplist!",entries);
    refresh();
    wrefresh(pluginWindow);

    it.resize(entries+2);
    if (it.size() > 100)
    {
        mvwprintw(win,0,0,"Cannot have more than 100 entries! Sorry! \n");
        getch();
        exit(-1);
    }


    for (int i=0; i < entries; i++)
    {
        sprintf(nameBuffer[i],"%i",myTwitch.getGameInfo(i).viewers);
        string streamName = myTwitch.getGameInfo(i).name;
        sprintf(titleBuffer[i],"viewers in %i channels -- %s",myTwitch.getGameInfo(i).channels,myTwitch.getGameInfo(i).name.c_str());

        it[i] = new_item(nameBuffer[i],titleBuffer[i]);
    }

    it[entries] = new_item("return", "return to console");
    it[entries+1] = 0;
}

myGameLister::myGameLister()
{
    name = "gameL";
    description = "Game-lister and Browser";
    pluginList::getInstance()->activated_classes.push_back(this);
}

void myGameLister::run_command(int _index)
{

}

void myGameLister::runGui(WINDOW* _pluginWindow)
{
    int x,y;
    getmaxyx(_pluginWindow,y,x);

    pluginWindow = _pluginWindow;
    char buffer[200];
    sprintf(buffer,"Plugin: %s -- %s",name.c_str(),description.c_str());
    mvwaddstr(pluginWindow,1,1,buffer);

    wrefresh(pluginWindow);


    makeMenu();



    me = new_menu(&it[0]);

    win = derwin(pluginWindow,y-5, x-4, 3, 2);
    set_menu_win (me, win);
    set_menu_sub (me, derwin(win, y-8, x-8, 3, 2));
    set_menu_mark (me,"-->");
    box(win, 0, 0);
    mvwaddstr(win, 1, (x-26)/2, "***** Games toplist! *****");
    post_menu(me);

    mvwaddstr(win,17, 3, "End this module via q or return to bash via menu");



    refresh();
//     wrefresh(pluginWindow);
    wrefresh(win);
}





