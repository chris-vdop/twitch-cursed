#include "twitch.h"


int twitchCPP::refreshStreams()
{
    vector<string> allNames;
    for (int i =0; i < gottenStreamInfo.size(); i++)
        allNames.push_back(gottenStreamInfo.at(i).name);

    return loadManyStreams(allNames,false);
}


int twitchCPP::loadWholeGame(string _gameName, bool insertStructs)
{
    gottenStreamInfo.clear();
    json_t* root;
    json_error_t error;
    string twitchGet = "https://api.twitch.tv/kraken/streams?game=" + twitchCPP::urlencode(_gameName);

    string jsonResp = DownloadJSON(twitchGet);

    //evaluate the query
    root = json_loads(jsonResp.c_str(), 0, &error);

    if (!root)
        return -1;

    json_t* streamsArray = json_object_get(root,"streams");

    if (!json_is_array(streamsArray))
    {
        printf("Problem loading array!\n");
        json_decref(root);
        return -1;
    }
    
    parseStreamsArray(streamsArray);
    json_decref(root);
    return 0;
}


int twitchCPP::parseStreamsArray(json_t* _streamsArray)
{

    //loop over all found streams
    for(int i = 0; i < json_array_size(_streamsArray); i++)
    {
        json_t* streamData = json_array_get(_streamsArray,i);
        json_t* viewers = json_object_get(streamData,"viewers");
        json_t* channel = json_object_get(streamData,"channel");
        json_t* name = json_object_get(channel,"display_name");
        json_t* cname = json_object_get(channel,"name");
        json_t* game = json_object_get(streamData,"game");

        string nameStr;
        if (json_is_string(cname))
            nameStr = json_string_value(cname);

        //if we cant find the struct, we make it
        if (getIndexByName(nameStr) == -1)
        {
            twitchStreamInfo newStream;
            newStream.name = nameStr;
            newStream.online = true;
            gottenStreamInfo.push_back(newStream);
        }



        twitchStreamInfo& modifyStruct = getStreamInfo(getIndexByName(nameStr));
        //if we get here, its online
        modifyStruct.online = true;
        if (json_is_string(name))
            modifyStruct.display_name = json_string_value(name);
        if (json_is_integer(viewers))
            modifyStruct.viewers = json_integer_value(viewers);
        if (json_is_string(game))
            modifyStruct.game = json_string_value(game);

    }
}





std::string twitchCPP::char2hex( char dec )
{
    char dig1 = (dec&0xF0)>>4;
    char dig2 = (dec&0x0F);
    if ( 0<= dig1 && dig1<= 9) dig1+=48;    //0,48 in ascii
    if (10<= dig1 && dig1<=15) dig1+=65-10; //A,65 in ascii
    if ( 0<= dig2 && dig2<= 9) dig2+=48;
    if (10<= dig2 && dig2<=15) dig2+=65-10;

    std::string r;
    r.append( &dig1, 1);
    r.append( &dig2, 1);
    return r;
}

std::string twitchCPP::urlencode( const std::string &c )
{

    std::string escaped;
    int max = c.length();
    for(int i=0; i<max; i++)
    {
        if ( (48 <= c[i] && c[i] <= 57) ||//0-9
                (65 <= c[i] && c[i] <= 90) ||//ABC...XYZ
                (97 <= c[i] && c[i] <= 122) || //abc...xyz
                (c[i]=='~' || c[i]=='-' || c[i]=='_' || c[i]=='.')
           )
        {
            escaped.append( &c[i], 1);
        }
        else if (c[i] == ' ')
            escaped.append("+"); //this is a html form special
        else
        {
            escaped.append("%");
            escaped.append( twitchCPP::char2hex(c[i]) );//converts char 255 to string "FF"
        }
    }
    return escaped;
}




int twitchCPP::refreshGames()
{
    gamesPlayed.clear();

    json_t* root;
    json_error_t error;
    string jsonResp = DownloadJSON("https://api.twitch.tv/kraken/games/top");


    //evaluate the query
    root = json_loads(jsonResp.c_str(), 0, &error);

    if (!root)
        return -1;

    json_t* top = json_object_get(root,"top");

    if (!json_is_array(top))
    {
        printf("Problem loading array!\n");
        json_decref(root);
        return -1;
    }

    for(int i = 0; i < json_array_size(top); i++)
    {
        json_t* entry = json_array_get(top,i);

        json_t* viewers = json_object_get(entry,"viewers");
        json_t* channels = json_object_get(entry,"channels");
        json_t* game = json_object_get(entry,"game");

        twitchGameInfo newGame;


        if (game != json_null())
        {
            json_t* name = json_object_get(game,"name");



            if (json_is_string(name))
                newGame.name = json_string_value(name);
            if (json_is_integer(viewers))
                newGame.viewers = json_integer_value(viewers);
            if (json_is_integer(channels))
                newGame.channels = json_integer_value(channels);

            gamesPlayed.push_back(newGame);

        }
    }
    json_decref(root);
    return 0;


}




int twitchCPP::loadManyStreams(vector< string > _nameList,bool insertStructs)
{
    for(int i =0; i < _nameList.size(); i++)
        _nameList.at(i).erase( std::remove_if(  _nameList.at(i).begin(),  _nameList.at(i).end(), ::isspace ),  _nameList.at(i).end() );

    string streamList = configFile::implodeString(_nameList,",");
    streamList.erase(streamList.size()-1,streamList.size());
    streamList.erase( std::remove_if( streamList.begin(), streamList.end(), ::isspace ), streamList.end() );

    json_t *root;
    json_error_t error;
    string jsonResp = DownloadJSON("https://api.twitch.tv/kraken/streams?channel="+streamList);


    if (insertStructs)
    {
        for (int i =0; i < _nameList.size(); i++)
        {
            twitchStreamInfo myStream;
            //set sane defaults
            myStream.name = _nameList.at(i);
            myStream.online = false;
            //insert if missing
            if (getIndexByName(_nameList.at(i)) == -1)
            {
                gottenStreamInfo.push_back(myStream);
            }
        }
    }
    //set all queried streams to offline
    for (int i = 0;  i < _nameList.size(); i ++)
        getStreamInfo(getIndexByName(_nameList.at(i))).online = false;


    //evaluate the query
    root = json_loads(jsonResp.c_str(), 0, &error);

    if (!root)
        return -1;

    json_t* streamsArray = json_object_get(root,"streams");

    if (!json_is_array(streamsArray))
    {
        printf("Problem loading array!\n");
        json_decref(root);
        return -1;
    }
    
    parseStreamsArray(streamsArray);
    json_decref(root);
    return 0;
}




int twitchCPP::getIndexByName(string _name)
{
    for (int i = 0; i < gottenStreamInfo.size(); i++)
    {
        if (gottenStreamInfo.at(i).name == _name)
            return i;
    }
    return -1;
}


int twitchCPP::loadStreamInfo(string _strName)
{

    json_t *root;
    json_error_t error;

    string jsonResp = DownloadJSON("https://api.twitch.tv/kraken/streams/"+_strName);



    root = json_loads(jsonResp.c_str(), 0, &error);

    if (!root)
        return -1;

    twitchStreamInfo newStream;
    newStream.name = _strName;

    json_t *streamData = json_object_get(root,"stream");
    if ( streamData == json_null())
        newStream.online = false;
    else
        newStream.online = true;

    if (newStream.online)
    {
        json_t* viewers = json_object_get(streamData,"viewers");
        json_t* channel = json_object_get(streamData,"channel");
        json_t* name = json_object_get(channel,"display_name");
        json_t* game = json_object_get(channel,"game");



        if (json_is_string(name))
            newStream.display_name = json_string_value(name);
        if (json_is_integer(viewers))
            newStream.viewers = json_integer_value(viewers);
        if (json_is_string(game))
            newStream.game = json_string_value(game);
    }
    json_decref(root);
    gottenStreamInfo.push_back(newStream);
    return gottenStreamInfo.size()-1;

}



std::string twitchCPP::DownloadJSON(std::string URL)
{
    CURL *curl;
    CURLcode res;
    struct curl_slist *headers=NULL; // init to NULL is important
    std::ostringstream oss;
    curl_slist_append(headers, "Accept: application/vnd.twitchtv.v3+json");
//     curl_slist_append( headers, "Content-Type: application/json");
    curl_slist_append( headers, "charsets: utf-8");
    curl = curl_easy_init();

    if (curl)
    {
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl, CURLOPT_URL, URL.c_str());
        curl_easy_setopt(curl, CURLOPT_HTTPGET,1);
        curl_easy_setopt(curl, CURLOPT_HTTPHEADER, headers);
        curl_easy_setopt(curl,CURLOPT_WRITEFUNCTION, twitchCPP::write_data);
        curl_easy_setopt(curl,CURLOPT_WRITEDATA, &stream);

        res = curl_easy_perform(curl);

        if (CURLE_OK == res)
        {
            char *ct;
            res = curl_easy_getinfo(curl, CURLINFO_CONTENT_TYPE, &ct);
            if((CURLE_OK == res) && ct)
            {
                string returnValue = stream.str();
                stream.clear();
                stream.str("");
                return returnValue;
            }
        }
    }

}



size_t twitchCPP::write_data(char *ptr, size_t size, size_t nmemb, void *userdata) {
    std::ostringstream *stream = (std::ostringstream*)userdata;
    size_t count = size * nmemb;
    stream->write(ptr, count);
    return count;
}


