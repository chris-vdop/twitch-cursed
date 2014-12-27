/**
 * \author Christopher "VdoP" Regali
 * \file systemInfo.cc
 * \brief contains systemInfo a simple system information display
 * \date 2014
 */


#include "pluginBase.h"


//for unix/linux cpu info from stat we need C style headers
#include <cstdlib>
#include <cstdio>
#include <cstring>
#include <unistd.h>


//memory information from sys
#include "sys/types.h"
#include "sys/sysinfo.h"
#include "sys/utsname.h"



class systemInfo: public pluginBase
{
public:
    systemInfo();
    ~systemInfo();

    void cleanUpCurses();
    void runGui(WINDOW* _pluginWindow);
    void handleChar(int _ch);

private:


    //this refreshed the display window win and the system information
    void refresh();
    void getSystemLoad();
    //functions for cpuload from http://stackoverflow.com/questions/63166/how-to-determine-cpu-and-memory-consumption-from-inside-a-process
    void init();
    void getCurrentValue();

    //curses stuff
    WINDOW *win;
    int x,y;


    //variables for keeping the system information saved

    unsigned long ram,ramUsed,swap,swapUsed;
    unsigned long int lastTotalUser, lastTotalUserLow, lastTotalSys, lastTotalIdle;

    double cpuPercent;
    string hostname, kernelInfo;


};

static systemInfo* mySysInfo = new systemInfo();

systemInfo::~systemInfo()
{

}


systemInfo::systemInfo()
{
    name = "sysInfo";
    description = "System Information Monitor";
    pluginList::getInstance()->activated_classes.push_back(this);
}



void systemInfo::runGui(WINDOW* _pluginWindow)
{
    getmaxyx(_pluginWindow,y,x);
    pluginWindow = _pluginWindow;
    char buffer[200];
    sprintf(buffer,"Plugin: %s -- %s",name.c_str(),description.c_str());
    mvwaddstr(pluginWindow,1,1,buffer);
    wrefresh(pluginWindow);


    win = derwin(pluginWindow,y-5, x-4, 3, 2);
    nodelay(stdscr, TRUE);
    int ch;
    while( (ch = getch()) != 'q' )
    {
      refresh();
      sleep(1);
    }

    nodelay(stdscr, FALSE);
    


}





void systemInfo::cleanUpCurses()
{
    werase(pluginWindow);
    box(pluginWindow,0,0);
}


void systemInfo::handleChar(int _ch)
{

}



void systemInfo::refresh()
{
    //erase the window and make a new border
    werase(win);
    box(win, 0, 0);
    mvwaddstr(win, 1, (x-26)/2, "***** System Status! *****");

    //refresh the system variables
    getSystemLoad();
    
     mvwprintw(win,3,2,"--- General Info: ");
    mvwprintw(win,4,2,"Hostname: %s",hostname.c_str());
    mvwprintw(win,5,2,"Kernel: %s",kernelInfo.c_str());
    
    

    //now print it all:
    mvwprintw(win,7,2,"--- Memory Info");
    mvwprintw(win,8,2,"Ram total: %i MB \t used: %i MB \t %: %5.1f",ram/(1024*1024),ramUsed/(1024*1024),((float)ramUsed/ram)*100);
    mvwprintw(win,9,2,"Swap total: %i MB \t used: %i MB \t %: %5.1f",swap/(1024*1024),swapUsed/(1024*1024),((float)swapUsed/swap)*100);


    mvwprintw(win,11,2,"--- CPU load: %5.2f %",cpuPercent);

   
    
    mvwprintw(win,y-8,2,"Press 'q' twice to exit!");
    

    wrefresh(win);
}



void systemInfo::getSystemLoad()
{

    //refresh memory
    struct sysinfo memory;
    sysinfo(&memory);
    ram = memory.totalram;
    ramUsed = ram - memory.freeram;
    swap = memory.totalswap;
    swapUsed = swap - memory.freeswap;

    ram *= memory.mem_unit ;
    ramUsed *= memory.mem_unit;
    swap *= memory.mem_unit;
    swapUsed *= memory.mem_unit;

    //do refresh for cpu percentage
    getCurrentValue();


    char buffer[200];

    //get hostname and kernel version
    gethostname(buffer,200);
    hostname = string( buffer);



    struct utsname utsname;
    uname(&utsname);


    sprintf(buffer,"%s-%s -- %s",utsname.sysname, utsname.release, utsname.version);
    kernelInfo =string(buffer) ;





}


void systemInfo::init() {
    FILE* file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %Ld %Ld %Ld %Ld", &lastTotalUser, &lastTotalUserLow,
           &lastTotalSys, &lastTotalIdle);
    fclose(file);
}


void systemInfo::getCurrentValue() {
    double percent;
    FILE* file;
    unsigned long long totalUser, totalUserLow, totalSys, totalIdle, total;


    file = fopen("/proc/stat", "r");
    fscanf(file, "cpu %Ld %Ld %Ld %Ld", &totalUser, &totalUserLow,
           &totalSys, &totalIdle);
    fclose(file);


    if (totalUser < lastTotalUser || totalUserLow < lastTotalUserLow ||
            totalSys < lastTotalSys || totalIdle < lastTotalIdle) {
        //Overflow detection. Just skip this value.
        percent = -1.0;
    }
    else {
        total = (totalUser - lastTotalUser) + (totalUserLow - lastTotalUserLow) +
                (totalSys - lastTotalSys);
        percent = total;
        total += (totalIdle - lastTotalIdle);
        percent /= total;
        percent *= 100;
    }


    lastTotalUser = totalUser;
    lastTotalUserLow = totalUserLow;
    lastTotalSys = totalSys;
    lastTotalIdle = totalIdle;

    cpuPercent = percent;

}





