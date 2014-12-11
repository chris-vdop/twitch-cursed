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

    void addFav(int _index);
    void tuneIn(int _index);
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
    WINDOW *chanWin;
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

    delwin(chanWin);
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

    case 'a':
        addFav(item_index(current_item(me)));
        break;
    case 't':
        tuneIn(item_index(current_item(me)));
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
    string gameName = myTwitch.getGameInfo(_index).name;
    myTwitch.loadWholeGame(gameName);
    //clear the window for the channellist
    werase(chanWin);
    box(chanWin,0,0);

    int x,y;
    getmaxyx(chanWin,y,x);

    int max = y-4;
    if (myTwitch.getStreamInfoCount() < max)
        max = myTwitch.getStreamInfoCount();


    mvwaddstr(chanWin, 1, (x-26)/2, "***** Channels toplist! *****");


    for (int i =0; i < max; i++)
    {
        mvwprintw(chanWin,i+2,4,"%i. %s %iV",i,myTwitch.getStreamInfo(i).display_name.c_str(),myTwitch.getStreamInfo(i).viewers);
    }

    mvwprintw(chanWin,y-3,8,">> press 't' to tune in to a stream directly");
    mvwprintw(chanWin,y-2,8,">> press 'a' to add one of these to your favourites!");

    refresh();
    wrefresh(chanWin);
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

    win = derwin(pluginWindow,y-5, (x-4)/2, 3, 2);
    set_menu_win (me, win);
    set_menu_sub (me, derwin(win, y-8, (x-8)/2, 3, 2));
    set_menu_mark (me,"-->");
    box(win, 0, 0);
    int wx,wy;
    getmaxyx(win,wy,wx);
    mvwaddstr(win, 1, (wx-26)/2, "***** Games toplist! *****");
    post_menu(me);

    mvwaddstr(win,wy-3, 3, ">> End this module via 'q' or return to bash via menu");


    chanWin = derwin(pluginWindow,y-5,(x-4)/2, 3, (x-4)/2 + 2);


    box(chanWin,0,0);

    refresh();
    wrefresh(win);
    wrefresh(chanWin);
}


void myGameLister::addFav(int _index)
{
    char input[10];
    int x,y;
    getmaxyx(chanWin,y,x);
    mvwprintw(chanWin,y-3,8,"!!Enter the stream number, then press Enter!");
    wrefresh(chanWin);
    int ret =  wgetnstr(chanWin,input,4);
    stringstream s(input);
    int out = -1;
    s >> out;
    if (out >= 0 && out < myTwitch.getStreamInfoCount())
    {
        string indexStr = configFile::getInstance()->getContents("menu_entries");
        int index = configFile::StrToInt(indexStr);
        string key_menu =   "entry_"+configFile::IntToStr(index);
        string value_menu = "twitch.tv/"+myTwitch.getStreamInfo(out).name;
        configFile::getInstance()->getDataMap()[key_menu] = value_menu;
        string src = "source";
        string key = "entry_"+configFile::IntToStr(index)+"_quality";
        configFile::getInstance()->getDataMap()[key] = src;
        index++;
        configFile::getInstance()->getDataMap()["menu_entries"] = configFile::IntToStr(index);
        configFile::getInstance()->writeFile("config.cfg");
        mvwprintw(chanWin,y-3,8,"!!Added %s to favourites and wrote config.cfg",value_menu.c_str());
        wrefresh(chanWin);
    }
}


void myGameLister::tuneIn(int _index)
{
    char input[10];
    int x,y;
    getmaxyx(chanWin,y,x);
    mvwprintw(chanWin,y-3,8,"!!Enter the stream number, then press Enter!");
    wrefresh(chanWin);
    int ret =  wgetnstr(chanWin,input,4);
    stringstream s(input);
    int out = -1;
    s >> out;
    if (out >= 0 && out < myTwitch.getStreamInfoCount())
    {
        string command = configFile::getInstance()->getContents("run_command");
        char buffer[200];
        string value_url = "twitch.tv/"+myTwitch.getStreamInfo(out).name;
        string quality = "source";
        sprintf(buffer,command.c_str(),value_url.c_str(),quality.c_str());
        system(buffer);

        mvwprintw(chanWin,y-3,8,"!!Starting stream %s!",value_url.c_str());
        wrefresh(chanWin);
    }

}




