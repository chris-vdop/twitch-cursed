/**
 * \author Christopher "VdoP" Regali
 * \file favourites.cc
 * \brief contains myFavourites, a simple favourite stream manager
 * \date 2014
 */


#include "pluginBase.h"

#include "twitch.h"

class myFavourites: public pluginBase
{
public:
    myFavourites();
    ~myFavourites();

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


static myFavourites* myFav = new myFavourites();



myFavourites::myFavourites()
{
    name="fav";
    description = "simple favourite stream manager";

    //register this plugin at the master_list
    pluginList::getInstance()->activated_classes.push_back(this);
}


myFavourites::~myFavourites()
{

}



void myFavourites::handleChar(int _ch)
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


void myFavourites::runGui(WINDOW* _pluginWindow)
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

    win = derwin(pluginWindow,y-4, x-4, 2, 2);
    set_menu_win (me, win);
    set_menu_sub (me, derwin(win, y-8, x-8, 3, 2));
    set_menu_mark (me,"-->");
    box(win, 0, 0);
    mvwaddstr(win, 1, (x-26)/2, "***** Choose stream! *****");
    post_menu(me);

    mvwaddstr(win,17, 3, "End this module via q or return to bash via menu");



    refresh();
//     wrefresh(pluginWindow);
    wrefresh(win);
}





void myFavourites::cleanUpCurses()
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




void myFavourites::makeMenu()
{
    int entries = configFile::StrToInt(configFile::getInstance()->getContents("menu_entries"));
    it.resize(entries+2);
    if (it.size() > 100)
    {
        mvwprintw(win,0,0,"Cannot have more than 100 entries! Sorry! \n");
        getch();
        exit(-1);
    }

    vector<string> streamList;
    for (int i=0; i < entries; i++) {
        streamList.push_back(configFile::explodeStringCustomDelim(configFile::getInstance()->getContents("entry_"+configFile::IntToStr(i)),"/").back());
        streamList.at(i).erase( std::remove_if(  streamList.at(i).begin(),  streamList.at(i).end(), ::isspace ),  streamList.at(i).end() );
    }
    //load all the streams at once, so we dont have to wait SOOO long
    myTwitch.loadManyStreams(streamList);

    for (int i=0; i < entries; i++)
    {

        sprintf(nameBuffer[i],"S%i",i);
        string streamName = configFile::getInstance()->getContents("entry_"+configFile::IntToStr(i));
        int index = myTwitch.getIndexByName(streamList.at(i));

        if (myTwitch.getStreamInfo(index).online)
        {
            sprintf(nameBuffer[i],"ON_%i",myTwitch.getStreamInfo(index).viewers);
            sprintf(titleBuffer[i],"%s -- %s",myTwitch.getStreamInfo(index).name.c_str(), myTwitch.getStreamInfo(index).game.c_str());
        }
        else
        {
            sprintf(nameBuffer[i],"OFF",i);
            sprintf(titleBuffer[i],"%s",configFile::getInstance()->getContents("entry_"+configFile::IntToStr(i)).c_str());
        }

        it[i] = new_item(nameBuffer[i],titleBuffer[i]);
    }

    it[entries] = new_item("return", "return to console");
    it[entries+1] = 0;
}



void myFavourites::run_command(int _index)
{
    string cmd = configFile::getInstance()->getContents("entry_"+configFile::IntToStr(_index));
    //   mvprintw(1,1,"Item with index %i selected, gives command: %s",_index,cmd.c_str());
    char buffer[400];
    sprintf(buffer, configFile::getInstance()->getContents("run_command").c_str(),cmd.c_str() ,configFile::getInstance()->getContents("entry_"+configFile::IntToStr(_index)+"_quality").c_str());
    char buffer2[450];
    sprintf(buffer2,"running command: %s",buffer);
    mvprintw(1,1,buffer2);
    system(buffer);
}
