#ifndef __USERLOGIN_H__
#define __USERLOGIN_H__

#include <bits/stdc++.h>
using namespace std;


class userLogin{
    private:
    string path ="../../trello_application/TextDB/";

    public:
    userLogin() {

    }

    bool doesExist(string username, string password) {
        ifstream fileParser;
        fileParser.open(path + username + password + ".txt");
        if (fileParser) {
            return true;
        } 
        return false;
    }

    






};




#endif //
