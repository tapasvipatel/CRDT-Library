#include <bits/stdc++.h>
#include <iostream>
#include <vector>
#include <stdio.h>
#include <dirent.h>
#include <chrono>
#include <thread>
#include <unistd.h>
// #include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "userLogin.hpp"
#include "../src/state_based/LWWMultiSetSB.hpp"
#include "../src/state_based/MultiSetSB.hpp"
#include "../src/state_based/VectorSB.hpp"
#include "../src/state_based/PNCounterSB.hpp"
#include "../src/state_based/GMapSB.hpp"
#include "../src/state_based/GSetSB.hpp"
#include "../src/state_based/ORSetSB.hpp"
#include "../src/state_based/TwoPSetSB.hpp"
#include <filesystem>
namespace fs = std::filesystem;

using namespace std;
using std::filesystem::directory_iterator;
tgui::Label::Ptr usersOnline;

crdt::state::GMapMetadata<int32_t, string> priorityList;
crdt::state::GMapSBString<int32_t, string> priorityListServer;

//taps
//void automatic_merge(tgui::GuiBase &gui);
//std::thread automatic_merge_thread;

string filePath = "/home/vishcapstone/Documents/CRDT-Library/trello_application/json/";
//string filePath = "/home/tapasvi/workspace/CRDT-Library/trello_application/json/";
crdt::state::MultiSetMetadata<string> backlogList;
crdt::state::MultiSetSB<string> backlogServer;
crdt::state::MultiSetMetadata<string> inprogressList;
crdt::state::MultiSetSB<string> inprogressServer;
crdt::state::MultiSetMetadata<string> readytotestList;
crdt::state::MultiSetSB<string> readytotestServer;
crdt::state::MultiSetMetadata<string> completeList;
crdt::state::MultiSetSB<string> completeServer;
crdt::state::MultiSetMetadata<string> notaddedList;
crdt::state::MultiSetSB<string> notaddedServer;

// Counters
crdt::state::PNCounterMetadata<uint32_t> numTasksBacklog;
crdt::state::PNCounterSB<uint32_t> numTasksBacklogServer;

crdt::state::PNCounterMetadata<uint32_t> numTasksInprogress;
crdt::state::PNCounterSB<uint32_t> numTasksInprogressServer;

crdt::state::PNCounterMetadata<uint32_t> numTasksReadytotest;
crdt::state::PNCounterSB<uint32_t> numTasksReadytotestServer;

crdt::state::PNCounterMetadata<uint32_t> numTasksComplete;
crdt::state::PNCounterSB<uint32_t> numTasksCompleteServer;

crdt::state::PNCounterMetadata<uint32_t> numTasksNotadded;
crdt::state::PNCounterSB<uint32_t> numTasksNotaddedServer;

int32_t globalTime = 0;

class userInfo {
private:
    string passWord;
    crdt::state::VectorMetadata<string> replicaUserOnline;
    string path ="../../trello_application/TextDB/";

public:
    uint32_t uniqueID;
    int id;
    string userName;
    userInfo() {
        userName = "";
        passWord = "";
        id = 0;
    }

    int getUserStatus(string username, string password)
    {
        ifstream fileParser;
        fileParser.open(path + username + password + ".txt");
        auto line = readNthLineS(fileParser, 4); //get the unique ID which is the third line
        stringstream ss(line);
        vector<string> breakText;
        while(ss.good()) {
            string temp = "";
            getline(ss,temp,':');
            breakText.push_back(temp);
        }
        string status = breakText.back();
        status.erase(status.begin());
        return status == "0" ? 0 : 1;
    }

    void mergeVectorCRDT()
    {
        vector<string> arr;
        replicaUserOnline.clear();
        for (const auto & file : directory_iterator(path)) 
        {
            ifstream filein(file.path());
            string line = "";
            int lineNumber = 0;
            while (getline(filein,line)) {
                 if (lineNumber == 0) {
                    arr.push_back(line);
                }
                if (lineNumber == 3) {
                    stringstream ss(line);
                    vector<string> breakText;
                    while(ss.good()) {
                        string temp = "";
                        getline(ss,temp,':');
                        breakText.push_back(temp);
                    }
                    string status = breakText.back();
                    status.erase(status.begin());
                    if (status == "0") {
                        arr.pop_back();
                    }
                }
                lineNumber++;
            }
        }
        for (string curr: arr) {
            replicaUserOnline.push_back(curr);
        }
    }

    void setUserStatus(int statusCode) {
        int lineNumber = 0;
        int line_to_replace = 3;
        ifstream filein(path + userName + passWord + ".txt");
        ofstream fileout(path + userName + passWord + "output.txt");
        string line = "";
        string _oldName = path + userName + passWord + ".txt";
        string _newName = path + userName + passWord + "output.txt";
        char *oldName =  const_cast<char*>(_oldName.c_str());
        char *newName =  const_cast<char*>(_newName.c_str());
        while (getline(filein,line))
        {
            if (lineNumber == line_to_replace)
            {
                if (statusCode == 0) {
                    fileout << "User status: 0" << endl;
                } else {
                    fileout << "User status: 1" << endl;
                }
            } else {
                fileout << line << endl;
            }
            lineNumber++;
        }
        filein.close();
        fileout.close();
        remove(oldName);
        rename(newName, oldName);
    }
    
 
    string readNthLineS(istream& in, int n) {
        int lineNumber = 0;
        string temp = "";
        while(getline(in,temp)) {
            if (lineNumber == n - 1) {
                return temp;
            }
            lineNumber++;
        }
        return "";
    }

    void readNthLine(istream& in, int n) {
        for (int i = 0; i < n-1; ++i) {
            in.ignore(numeric_limits<streamsize>::max(), '\n');
        }
        in >> uniqueID;
    }


    void parseFile(string username, string password) 
    {
        passWord = password;
        ifstream fileParser;
        fileParser.open(path + username + password + ".txt");
        readNthLine(fileParser, 3); //get the unique ID which is the third line
    }

    void setupCRDTHandler()
    {
        crdt::state::VectorSB<uint32_t> handlerVector(this->uniqueID);
        crdt::state::GMapSB<uint32_t, uint32_t> handlerMap(this->uniqueID);
        crdt::state::LWWMultiSetSB<uint32_t> handlerLWWMS(this->uniqueID);
        crdt::state::PNCounterSB<uint32_t> handlerPNCounter(this->uniqueID); 
    }

    void setUserName(string userName) {
        this->userName = userName;
    }

    string getUserName() {
        return this->userName;
    }

    uint32_t getHash() {
        return this->uniqueID;
    }

    vector<string> getAllUsersOnline()
    {
        return this->replicaUserOnline.queryPayload();
    }

};

//Fuction Definations
void loadWidgets(tgui::GuiBase &gui, tgui::Label::Ptr &message);
void loadWidgets2(tgui::GuiBase &gui);
void loadWidgets3(tgui::GuiBase &gui);
void deleteBoard(tgui::GuiBase &gui, int boardType, string task);
void updateTableMaster(tgui::GuiBase &gui);

//Class Definations
userInfo endUser;

// -------------------------------- Window Screen 2 ----------------------------------------------------------//

int32_t getPriority(string task) {

    vector<char> priority;
    for (int i = 0; i < task.length(); i++) {

        if (task[i] == '.') {
            break;
        }
        priority.push_back(task[i]);

    }

    string stringPriority(priority.begin(), priority.end());
    int32_t key = stoi(stringPriority);
    return key;
}

void deleteBoard(tgui::GuiBase &gui, int boardType, string task) {

    //auto timeValue = std::time(nullptr);
    switch (boardType) {
        case 1:
            loadWidgets3(std::ref(gui));
            backlogList.remove(task);
            backlogList.serializeFileApp(filePath + "backlog/" + endUser.userName + "_backlog.json");
            backlogServer.addExternalReplica({backlogList});
            //backlogServer.updateMetaData(backlogList.queryId(), backlogList);
            updateTableMaster(std::ref(gui));
            break;
        case 2:
            loadWidgets3(std::ref(gui));
            inprogressList.remove(task);
            inprogressList.serializeFileApp(filePath + "inprogress/" + endUser.userName + "_inprogress.json");
            //inprogressServer.updateMetaData(inprogressList.queryId(), inprogressList);
            inprogressServer.addExternalReplica({inprogressList});
            updateTableMaster(std::ref(gui));
            break;
        case 3:
            loadWidgets3(std::ref(gui));
            readytotestList.remove(task);
            readytotestList.serializeFileApp(filePath + "readytotest/" + endUser.userName + "_readytotest.json");
            //readytotestServer.updateMetaData(readytotestList.queryId(), readytotestList);
            readytotestServer.addExternalReplica({readytotestList});
            updateTableMaster(std::ref(gui));
            break;
        case 4:
            loadWidgets3(std::ref(gui));
            completeList.remove(task);
            completeList.serializeFileApp(filePath + "complete/" + endUser.userName + "_complete.json");
            //completeServer.updateMetaData(completeList.queryId(), completeList);
            completeServer.addExternalReplica({completeList});
            updateTableMaster(std::ref(gui));
            break;
        case 5:
            loadWidgets3(std::ref(gui));
            //auto temp = std::chrono::system_clock::now();
            notaddedList.remove(task);
            notaddedList.serializeFileApp(filePath + "notadded/" + endUser.userName + "_notadded.json");
            //notaddedServer.updateMetaData(notaddedList.queryId(), notaddedList);
            notaddedServer.addExternalReplica({notaddedList});
            updateTableMaster(std::ref(gui));
            break;
    }
}

// taps
void updateTableMaster(tgui::GuiBase &gui)
{
    priorityList.clear();
    priorityListServer.clear();

    numTasksBacklog.clear();
    numTasksBacklogServer.clear();

    numTasksInprogress.clear();
    numTasksInprogressServer.clear();

    numTasksReadytotest.clear();
    numTasksReadytotestServer.clear();

    numTasksComplete.clear();
    numTasksCompleteServer.clear();

    numTasksNotadded.clear();
    numTasksNotaddedServer.clear();

    // backlog
    multiset<string> backlogPayload = backlogServer.queryMultiset();
    int count = 0;
    int deleteCount = 0;
    for(auto element : backlogPayload)
    {
        priorityList.insert(getPriority(element), element);
        priorityList.serializeFile_StringValue(filePath + "prioritylist/" + endUser.userName + "_prioritylist.json");
        priorityListServer.addExternalReplica({priorityList});

        numTasksBacklog.increasePayload(1);
        numTasksBacklog.serializeFile(filePath + "numtasksbacklog/" + endUser.userName + "_numtasksbacklog.json");
        numTasksBacklogServer.addExternalReplica({numTasksBacklog});
        
        auto backlog = tgui::Button::create(element);
        backlog->setSize({"12%", "12%"});
        int y = count + 308;
        count += 150;
        string y_position = to_string(y);
        backlog->setPosition(55, y);
        backlog->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
        backlog->getRenderer()->setTextColor(tgui::Color::Black);
        gui.add(backlog);

        auto backlogDelete = tgui::Button::create("-");
        backlogDelete->setSize(35, 35);
        int deleteY = deleteCount + 374;
        deleteCount += 150;
        backlogDelete->setPosition(242, deleteY);
        backlogDelete->getRenderer()->setBackgroundColor(sf::Color(240, 0, 0));
        backlogDelete->getRenderer()->setTextColor(tgui::Color::Black);
        backlogDelete->getRenderer()->setTextStyle(tgui::Bold);
        gui.add(backlogDelete);
        backlogDelete->onPress(&deleteBoard, std::ref(gui), 1, element);
    }

    // inprogress
    multiset<string> inprogressPayload = inprogressServer.queryMultiset();
    count = 0;
    deleteCount = 0;
    for(auto element : inprogressPayload)
    {
        priorityList.insert(getPriority(element), element);
        priorityList.serializeFile_StringValue(filePath + "prioritylist/" + endUser.userName + "_prioritylist.json");
        priorityListServer.addExternalReplica({priorityList});

        numTasksInprogress.increasePayload(1);
        numTasksInprogress.serializeFile(filePath + "numtasksinprogress/" + endUser.userName + "_numtasksinprogress.json");
        numTasksInprogressServer.addExternalReplica({numTasksInprogress});

        auto inprogress = tgui::Button::create(element);
        inprogress->setSize({"12%", "12%"});
        int y = count + 308;
        count += 150;
        string y_position = to_string(y);
        inprogress->setPosition(371, y);
        inprogress->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
        inprogress->getRenderer()->setTextColor(tgui::Color::Black);
        gui.add(inprogress);

        auto inprogressDelete = tgui::Button::create("-");
        inprogressDelete->setSize(35, 35);
        int deleteY = deleteCount + 374;
        deleteCount += 150;
        inprogressDelete->setPosition(558, deleteY);
        inprogressDelete->getRenderer()->setBackgroundColor(sf::Color(240, 0, 0));
        inprogressDelete->getRenderer()->setTextColor(tgui::Color::Black);
        inprogressDelete->getRenderer()->setTextStyle(tgui::Bold);
        gui.add(inprogressDelete);
        inprogressDelete->onPress(&deleteBoard, std::ref(gui), 2, element);
    }

    // readytotest
    multiset<string> readytotestPayload = readytotestServer.queryMultiset();
    count = 0;
    deleteCount = 0;
    for(auto element : readytotestPayload)
    {
        priorityList.insert(getPriority(element), element);
        priorityList.serializeFile_StringValue(filePath + "prioritylist/" + endUser.userName + "_prioritylist.json");
        priorityListServer.addExternalReplica({priorityList});

        numTasksReadytotest.increasePayload(1);
        numTasksReadytotest.serializeFile(filePath + "numtasksreadytotest/" + endUser.userName + "numtasksreadytotest.json");
        numTasksReadytotestServer.addExternalReplica({numTasksReadytotest});

        auto readytotest = tgui::Button::create(element);
        readytotest->setSize({"12%", "12%"});
        int y = count + 308;
        count += 150;
        string y_position = to_string(y);
        readytotest->setPosition(685, y);
        readytotest->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
        readytotest->getRenderer()->setTextColor(tgui::Color::Black);
        gui.add(readytotest);

        auto readytotestDelete = tgui::Button::create("-");
        readytotestDelete->setSize(35, 35);
        int deleteY = deleteCount + 374;
        deleteCount += 150;
        readytotestDelete->setPosition(872, deleteY);
        readytotestDelete->getRenderer()->setBackgroundColor(sf::Color(240, 0, 0));
        readytotestDelete->getRenderer()->setTextColor(tgui::Color::Black);
        readytotestDelete->getRenderer()->setTextStyle(tgui::Bold);
        gui.add(readytotestDelete);
        readytotestDelete->onPress(&deleteBoard, std::ref(gui), 3, element);
    }

    // complete
    multiset<string> completePayload = completeServer.queryMultiset();
    count = 0;
    deleteCount = 0;
    for(auto element : completePayload)
    {
        priorityList.insert(getPriority(element), element);
        priorityList.serializeFile_StringValue(filePath + "prioritylist/" + endUser.userName + "_prioritylist.json");
        priorityListServer.addExternalReplica({priorityList});

        numTasksComplete.increasePayload(1);
        numTasksComplete.serializeFile(filePath + "numtaskscomplete/" + endUser.userName + "_numtaskscomplete.json");
        numTasksCompleteServer.addExternalReplica({numTasksComplete});

        auto complete = tgui::Button::create(element);
        complete->setSize({"12%", "12%"});
        int y = count + 308;
        count += 150;
        string y_position = to_string(y);
        complete->setPosition(999, y);
        complete->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
        complete->getRenderer()->setTextColor(tgui::Color::Black);
        gui.add(complete);

        auto completeDelete = tgui::Button::create("-");
        completeDelete->setSize(35, 35);
        int deleteY = deleteCount + 374;
        deleteCount += 150;
        completeDelete->setPosition(1186, deleteY);
        completeDelete->getRenderer()->setBackgroundColor(sf::Color(240, 0, 0));
        completeDelete->getRenderer()->setTextColor(tgui::Color::Black);
        completeDelete->getRenderer()->setTextStyle(tgui::Bold);
        gui.add(completeDelete);
        completeDelete->onPress(&deleteBoard, std::ref(gui), 4, element);
    }

    // notadded
    multiset<string> notaddedPayload = notaddedServer.queryMultiset();
    count = 0;
    deleteCount = 0;
    for(auto element : notaddedPayload)
    {
        priorityList.insert(getPriority(element), element);
        priorityList.serializeFile_StringValue(filePath + "prioritylist/" + endUser.userName + "_prioritylist.json");
        priorityListServer.addExternalReplica({priorityList});

        numTasksNotadded.increasePayload(1);
        numTasksNotadded.serializeFile(filePath + "numtasksnotadded/" + endUser.userName + "numtasksnotadded.json");
        numTasksNotaddedServer.addExternalReplica({numTasksNotadded});

        auto notadded = tgui::Button::create(element);
        notadded->setSize({"12%", "12%"});
        int y = count + 308;
        count += 150;
        string y_position = to_string(y);
        notadded->setPosition(1313, y);
        notadded->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
        notadded->getRenderer()->setTextColor(tgui::Color::Black);
        gui.add(notadded);

        auto notaddedDelete = tgui::Button::create("-");
        notaddedDelete->setSize(35, 35);
        int deleteY = deleteCount + 374;
        deleteCount += 150;
        notaddedDelete->setPosition(1500, deleteY);
        notaddedDelete->getRenderer()->setBackgroundColor(sf::Color(240, 0, 0));
        notaddedDelete->getRenderer()->setTextColor(tgui::Color::Black);
        notaddedDelete->getRenderer()->setTextStyle(tgui::Bold);
        gui.add(notaddedDelete);
        notaddedDelete->onPress(&deleteBoard, std::ref(gui), 5, element);
    }

    // priority list
    map<int32_t, string> prioritylistPayload = priorityListServer.getTotalPayload();
    count = 0;
    int iteration = 0;
    
    for (auto i = prioritylistPayload.rbegin(); i != prioritylistPayload.rend(); i++)
    {
        if (iteration == 5)
        {
            break;
        }

        auto priorityListButton = tgui::Button::create(i->second);
        priorityListButton->setSize({"10%, 10%"});
        int y = count + 258;
        count += 100;
        string y_position = to_string(y);
        priorityListButton->setPosition(1610, y);
        priorityListButton->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
        priorityListButton->getRenderer()->setTextColor(tgui::Color::Black);
        gui.add(priorityListButton);
        iteration++;
    }

    // update counters on the screen
    string valueone = "Num: " + to_string(numTasksBacklogServer.queryPayload());
    auto numTasksBacklogLabel = tgui::Button::create(valueone);
    numTasksBacklogLabel->setSize({"10%", "10%"});
    numTasksBacklogLabel->setPosition({"4.5%", "90%"});
    numTasksBacklogLabel->getRenderer()->setBackgroundColor(sf::Color(219, 153, 247));
    numTasksBacklogLabel->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(numTasksBacklogLabel);

    string valuethree = "Num: " + to_string(numTasksInprogressServer.queryPayload());
    auto numTasksInprogressLabel = tgui::Button::create(valuethree);
    numTasksInprogressLabel->setSize({"10%", "10%"});
    numTasksInprogressLabel->setPosition({"22.5%", "90%"});
    numTasksInprogressLabel->getRenderer()->setBackgroundColor(sf::Color(219, 153, 247));
    numTasksInprogressLabel->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(numTasksInprogressLabel);

    string valuefive = "Num: " + to_string(numTasksReadytotestServer.queryPayload());
    auto numTasksReadytotestLabel = tgui::Button::create(valuefive);
    numTasksReadytotestLabel->setSize({"10%", "10%"});
    numTasksReadytotestLabel->setPosition({"38.5%", "90%"});
    numTasksReadytotestLabel->getRenderer()->setBackgroundColor(sf::Color(219, 153, 247));
    numTasksReadytotestLabel->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(numTasksReadytotestLabel);

    string valuetwo = "Num: " + to_string(numTasksCompleteServer.queryPayload());
    auto numTasksCompleteLabel = tgui::Button::create(valuetwo);
    numTasksCompleteLabel->setSize({"10%", "10%"});
    numTasksCompleteLabel->setPosition({"55.5%", "90%"});
    numTasksCompleteLabel->getRenderer()->setBackgroundColor(sf::Color(219, 153, 247));
    numTasksCompleteLabel->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(numTasksCompleteLabel);

    string valuefour = "Num: " + to_string(numTasksNotaddedServer.queryPayload());
    auto numTasksNotaddedLabel = tgui::Button::create(valuefour);
    numTasksNotaddedLabel->setSize({"10%", "10%"});
    numTasksNotaddedLabel->setPosition({"72.5%", "90%"});
    numTasksNotaddedLabel->getRenderer()->setBackgroundColor(sf::Color(219, 153, 247));
    numTasksNotaddedLabel->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(numTasksNotaddedLabel);
}

void convergeBoard(tgui::GuiBase &gui, int statusCode)
{
    endUser.mergeVectorCRDT();
    gui.remove(usersOnline);
    string getAllUsers = "Users Online: ";
    for (auto i: endUser.getAllUsersOnline()) {
        getAllUsers+=i + ", ";
        //cout << i << endl;
    }
    while(getAllUsers .back() == ',' || getAllUsers.back() == ' ') getAllUsers.pop_back();
    for (int i = 2 ; i < getAllUsers.size(); i++) {
        if (getAllUsers[i] == ',' && getAllUsers[i-2] == getAllUsers[i]) {
            getAllUsers.erase(getAllUsers.begin() + i);
            i--;
        }
    }
    usersOnline->setSize({"100.0%", "100.0%"});
    usersOnline->setPosition({"20%", "2.0%"});
    usersOnline->setText(getAllUsers);
    gui.add(usersOnline);
    if (statusCode == 1) {
        loadWidgets2(gui);
    }

    // tapshere
    // Read all crdts and update in local copy
    string rootFolder = filePath;
    string backlogFolder = rootFolder + "backlog";
    string completeFolder = rootFolder + "complete";
    string inprogressFolder = rootFolder + "inprogress";
    string readytotestFolder = rootFolder + "readytotest";
    string notaddedFolder = rootFolder + "notadded";
    string prioritylistFolder = rootFolder + "prioritylist";

    // Get backlog updates
    vector<crdt::state::MultiSetMetadata<string>> backlogMetadataList;
    for(auto & file : fs::directory_iterator(backlogFolder))
    {
        crdt::state::MultiSetMetadata<string> replica;
        replica.deserializeFileApp(file.path());
        backlogMetadataList.push_back(replica);
    }

    backlogServer.addExternalReplica(backlogMetadataList);

    // Get complete updates
    vector<crdt::state::MultiSetMetadata<string>> completeMetadataList;
    for(auto & file : fs::directory_iterator(completeFolder))
    {
        crdt::state::MultiSetMetadata<string> replica;
        replica.deserializeFileApp(file.path());
        completeMetadataList.push_back(replica);
    }

    completeServer.addExternalReplica(completeMetadataList);

    // Get inprogress updates
    vector<crdt::state::MultiSetMetadata<string>> inprogressMetadataList;
    for(auto & file : fs::directory_iterator(inprogressFolder))
    {
        crdt::state::MultiSetMetadata<string> replica;
        replica.deserializeFileApp(file.path());
        inprogressMetadataList.push_back(replica);
    }

    inprogressServer.addExternalReplica(inprogressMetadataList);

    // Get readytotest updates
    vector<crdt::state::MultiSetMetadata<string>> readytotestMetadataList;
    for(auto & file : fs::directory_iterator(readytotestFolder))
    {
        crdt::state::MultiSetMetadata<string> replica;
        replica.deserializeFileApp(file.path());
        readytotestMetadataList.push_back(replica);
    }

    readytotestServer.addExternalReplica(readytotestMetadataList);

    // Get notadded updates
    vector<crdt::state::MultiSetMetadata<string>> notaddedMetadataList;
    for(auto & file : fs::directory_iterator(notaddedFolder))
    {
        crdt::state::MultiSetMetadata<string> replica;
        replica.deserializeFileApp(file.path());
        notaddedMetadataList.push_back(replica);
    }

    notaddedServer.addExternalReplica(notaddedMetadataList);

    /*
    // Get priority list updates
    vector<crdt::state::GMapMetadata<int32_t, string>> priorityMetadataList;
    for(auto & file : fs::directory_iterator(prioritylistFolder))
    {
        crdt::state::GMapMetadata<int32_t, string> replica;
        replica.deserializeFile_StringValue(file.path());
        priorityMetadataList.push_back(replica);
    }

    priorityListServer.addExternalReplica(priorityMetadataList);
    */

    // Get counter updates
    string numTasksBacklogFolder = rootFolder + "numtasksbacklog";
    string numTasksCompleteFolder = rootFolder + "numtaskscomplete";
    string numTasksInprogressFolder = rootFolder + "numtasksinprogress";
    string numTasksNotaddedFolder = rootFolder + "numtasksnotadded";
    string numTasksReadytotestFolder = rootFolder + "numtasksreadytotest";

    /*
    vector<crdt::state::PNCounterMetadata<uint32_t>> numTasksBacklogMetadataList;
    for(auto & file : fs::directory_iterator(numTasksBacklogFolder))
    {
        crdt::state::PNCounterMetadata<uint32_t> replica;
        replica.deserializeFile(file.path());
        numTasksBacklogMetadataList.push_back(replica);
    }

    numTasksBacklogServer.addExternalReplica(numTasksBacklogMetadataList);

    vector<crdt::state::PNCounterMetadata<uint32_t>> numTasksCompleteMetadataList;
    for(auto & file : fs::directory_iterator(numTasksCompleteFolder))
    {
        crdt::state::PNCounterMetadata<uint32_t> replica;
        replica.deserializeFile(file.path());
        numTasksCompleteMetadataList.push_back(replica);
    }

    numTasksCompleteServer.addExternalReplica(numTasksCompleteMetadataList);

    vector<crdt::state::PNCounterMetadata<uint32_t>> numTasksInprogressMetadataList;
    for(auto & file : fs::directory_iterator(numTasksInprogressFolder))
    {
        crdt::state::PNCounterMetadata<uint32_t> replica;
        replica.deserializeFile(file.path());
        numTasksInprogressMetadataList.push_back(replica);
    }

    numTasksInprogressServer.addExternalReplica(numTasksInprogressMetadataList);

    vector<crdt::state::PNCounterMetadata<uint32_t>> numTasksNotaddedMetadataList;
    for(auto & file : fs::directory_iterator(numTasksNotaddedFolder))
    {
        crdt::state::PNCounterMetadata<uint32_t> replica;
        replica.deserializeFile(file.path());
        numTasksNotaddedMetadataList.push_back(replica);
    }

    numTasksNotaddedServer.addExternalReplica(numTasksNotaddedMetadataList);

    vector<crdt::state::PNCounterMetadata<uint32_t>> numTasksReadytotestMetadataList;
    for(auto & file : fs::directory_iterator(numTasksReadytotestFolder))
    {
        crdt::state::PNCounterMetadata<uint32_t> replica;
        replica.deserializeFile(file.path());
        numTasksReadytotestMetadataList.push_back(replica);
    }

    numTasksReadytotestServer.addExternalReplica(numTasksReadytotestMetadataList);
    */

    updateTableMaster(std::ref(gui));
}

void logout(tgui::GuiBase &gui)
{
    endUser.setUserStatus(0);
    tgui::Label::Ptr message = tgui::Label::create();
    gui.removeAllWidgets();
    loadWidgets(gui, message); //Send back to login Screen
}

void updateTextSize(tgui::GuiBase &gui)
{
    // Update the text size of all widgets in the gui, based on the current window height
    const float windowHeight = gui.getView().getRect().height;
    gui.setTextSize(static_cast<unsigned int>(0.07f * windowHeight)); // 7% of height
}

void createBoard(tgui::EditBox::Ptr assignee, tgui::EditBox::Ptr task, tgui::EditBox::Ptr urgency, tgui::GuiBase &childGui, int boardType, tgui::GuiBase &gui) {

    string _assignee = (string)assignee->getText();
    string _task = (string)task->getText();
    string _urgency = (string)urgency->getText();
    string bType;

    tgui::Label::Ptr msg = tgui::Label::create();

    if (_assignee.empty() || _task.empty() || _urgency.empty())
    {
        msg->setSize({"80.0%", "10.0%"});
        msg->setPosition({"32%", "70.0%"});
        msg->setText("Please fill in all fields");
        msg->getRenderer()->setTextColor(sf::Color::Red);
        childGui.add(msg);
        return;
    }
    else {

        switch (boardType) {
            case 1:
                bType = "Backlog";
                break;
            case 2:
                bType = "In Progress";
                break;
            case 3:
                bType = "Ready for Test";
                break;
            case 4:
                bType = "Complete";
                break;
            case 5:
                bType = "Not Added";
                break;
        }

        // taps
        cout << "Creating board of type: " << bType << endl;
        cout << "Assignee: " << _assignee << endl;
        cout << "Task: " << _task << endl;
        cout << "Urgency: " << _urgency << endl;

        string _data = _urgency + ". " + _task;

        int32_t key = stoi(_urgency);

        priorityList.insert(key, _task);
        priorityList.serializeFile_StringValue(filePath + "prioritylist/" + endUser.userName + "_prioritylist.json");
        priorityListServer.addExternalReplica({priorityList});

        auto timeValue = std::time(nullptr);
        switch (boardType) {
            case 1:
                backlogList.insert(_data);
                backlogList.serializeFileApp(filePath + "backlog/" + endUser.userName + "_backlog.json");
                backlogServer.addExternalReplica({backlogList});
                updateTableMaster(std::ref(gui));
                break;
            case 2:
                inprogressList.insert(_data);
                inprogressList.serializeFileApp(filePath + "inprogress/" + endUser.userName + "_inprogress.json");
                inprogressServer.addExternalReplica({inprogressList});
                updateTableMaster(std::ref(gui));
                break;
            case 3:
                readytotestList.insert(_data);
                readytotestList.serializeFileApp(filePath + "readytotest/" + endUser.userName + "_readytotest.json");
                readytotestServer.addExternalReplica({readytotestList});
                updateTableMaster(std::ref(gui));
                break;
            case 4:
                completeList.insert(_data);
                completeList.serializeFileApp(filePath + "complete/" + endUser.userName + "_complete.json");
                completeServer.addExternalReplica({completeList});
                updateTableMaster(std::ref(gui));
                break;
            case 5:
                //auto temp = std::chrono::system_clock::now();
                notaddedList.insert(_data);
                notaddedList.serializeFileApp(filePath + "notadded/" + endUser.userName + "_notadded.json");
                notaddedServer.addExternalReplica({notaddedList});
                updateTableMaster(std::ref(gui));
                break;
        }

        globalTime += 1;
    }
}

void addBoard(tgui::GuiBase &gui, int boardType) {

    sf::RenderWindow addWindow(sf::VideoMode(720, 480), "Add Board");
    tgui::Gui childGui(addWindow);

    childGui.setFont("../../blackjack.otf");

    auto assignee = tgui::EditBox::create();
    assignee->setSize({"66.67%", "12.5%"});
    assignee->setPosition({"16.67%", "22.0%"});
    assignee->setDefaultText("Assignee");
    childGui.add(assignee);

    auto task = tgui::EditBox::copy(assignee);
    task->setPosition({"16.67%", "39.0%"});
    task->setDefaultText("Task");
    childGui.add(task);

    auto urgency = tgui::EditBox::copy(assignee);
    urgency->setPosition({"16.67%", "56.0%"});
    urgency->setDefaultText("Urgency");
    childGui.add(urgency);

    auto createBoardButton = tgui::Button::create("Create Board");
    createBoardButton->setSize({"35%", "16.67%"});
    createBoardButton->setPosition({"35%", "80%"});
    childGui.add(createBoardButton);

    updateTextSize(childGui);

    createBoardButton->onPress(&createBoard, assignee, task, urgency, std::ref(childGui), boardType, std::ref(gui));

    while (addWindow.isOpen())
    {
        sf::Event event;
        while (addWindow.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                endUser.setUserStatus(1);
                addWindow.close();
            }
                
            childGui.handleEvent(event);
        }
        addWindow.clear(sf::Color(238, 238, 228));
        childGui.draw();
        addWindow.display();
    }
}

void loadWidgets3(tgui::GuiBase &gui)
{
    gui.removeAllWidgets();
    const float windowHeight = gui.getView().getRect().height;
    gui.setTextSize(static_cast<unsigned int>(0.04f * windowHeight)); 
    tgui::Label::Ptr welcomeMessage = tgui::Label::create();
    welcomeMessage->setSize({"100.0%", "100.0%"});
    welcomeMessage->setPosition({"0%", "2.0%"});
    welcomeMessage->setText("Welcome: " + endUser.getUserName());
    //cout << endUser.getHash() << endl;
    //message->getRenderer()->setTextColor(sf::Color(0, 200, 0));
    gui.add(welcomeMessage);
    gui.add(usersOnline);

    // Create the merge button
    auto mergeBoard = tgui::Button::create("Merge Board");
    mergeBoard ->setSize({"10%", "10%"});
    mergeBoard ->setPosition({"87%", "90%"});
    mergeBoard->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    mergeBoard ->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(mergeBoard);

    //Log Out Button
    auto logOut = tgui::Button::create("Logout");
    logOut ->setSize({"10%", "10%"});
    logOut ->setPosition({"87%", "2.0%"});
    logOut ->getRenderer()->setBackgroundColor(tgui::Color::Red);
    logOut ->getRenderer()->setTextColor(tgui::Color::White);
    gui.add(logOut); 

    // Priority List Button
    auto priorityListHeader = tgui::Button::create("Priority");
    priorityListHeader ->setSize({"10%", "10%"});
    priorityListHeader ->setPosition({"87%", "15.0%"});
    priorityListHeader ->getRenderer()->setBackgroundColor(tgui::Color::White);
    priorityListHeader ->getRenderer()->setTextColor(tgui::Color::Red);
    gui.add(priorityListHeader); 

    //Backlog button
    auto backlog = tgui::Button::create("Backlog");
    backlog->setSize({"12%", "12%"});
    backlog->setPosition({"3%", "15%"});
    backlog->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    backlog->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(backlog);

    //Add a backlog board button
    auto backlogAdd = tgui::Button::create("+");
    backlogAdd->setSize(35, 35);
    backlogAdd->setPosition(242, 192);
    backlogAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    backlogAdd->getRenderer()->setTextColor(tgui::Color::Black);
    backlogAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(backlogAdd);

    //In progress button
    auto ipr = tgui::Button::create("In Progress");
    ipr->setSize({"12%", "12%"});
    ipr->setPosition({"20%", "15%"});
    ipr->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    ipr->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(ipr);  

    //Add an in progress board button
    auto iprAdd = tgui::Button::create("+");
    iprAdd->setSize(35, 35);
    iprAdd->setPosition(557, 192);
    iprAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    iprAdd->getRenderer()->setTextColor(tgui::Color::Black);
    iprAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(iprAdd);

    //Ready to test button
    auto ready = tgui::Button::create("Ready to Test");
    ready->setSize({"12%", "12%"});
    ready->setPosition({"37%", "15%"});
    ready->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    ready->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(ready);

    //Add a ready board button
    auto readyAdd = tgui::Button::create("+");
    readyAdd->setSize(35, 35);
    readyAdd->setPosition(871, 192);
    readyAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    readyAdd->getRenderer()->setTextColor(tgui::Color::Black);
    readyAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(readyAdd);

    //Complete button
    auto complete = tgui::Button::create("Complete");
    complete->setSize({"12%", "12%"});
    complete->setPosition({"54%", "15%"});
    complete->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    complete->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(complete);    

    //Add a complete board button
    auto completeAdd = tgui::Button::create("+");
    completeAdd->setSize(35, 35);
    completeAdd->setPosition(1185, 192);
    completeAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    completeAdd->getRenderer()->setTextColor(tgui::Color::Black);
    completeAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(completeAdd);

    //Not added button
    auto notAdded = tgui::Button::create("Not Added");
    notAdded->setSize({"12%", "12%"});
    notAdded->setPosition({"71%", "15%"});
    notAdded->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    notAdded->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(notAdded);   

    //Add a not added board button
    auto notAddedAdd = tgui::Button::create("+");
    notAddedAdd->setSize(35, 35);
    notAddedAdd->setPosition(1499, 192);
    notAddedAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    notAddedAdd->getRenderer()->setTextColor(tgui::Color::Black);
    notAddedAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(notAddedAdd);

    mergeBoard->onPress(&convergeBoard,std::ref(gui),1);
    logOut->onPress(&logout,std::ref(gui));

    backlogAdd->onPress(&addBoard, std::ref(gui), 1);
    iprAdd->onPress(&addBoard, std::ref(gui), 2);
    readyAdd->onPress(&addBoard, std::ref(gui), 3);
    completeAdd->onPress(&addBoard, std::ref(gui), 4);
    notAddedAdd->onPress(&addBoard, std::ref(gui), 5);
    
}

void loadWidgets2(tgui::GuiBase &gui)
{
    gui.removeAllWidgets();
    convergeBoard(gui,0);
    const float windowHeight = gui.getView().getRect().height;
    gui.setTextSize(static_cast<unsigned int>(0.04f * windowHeight)); 
    tgui::Label::Ptr welcomeMessage = tgui::Label::create();
    welcomeMessage->setSize({"100.0%", "100.0%"});
    welcomeMessage->setPosition({"0%", "2.0%"});
    welcomeMessage->setText("Welcome: " + endUser.getUserName());
    //cout << endUser.getHash() << endl;
    //message->getRenderer()->setTextColor(sf::Color(0, 200, 0));
    gui.add(welcomeMessage);

    // Create the merge button
    auto mergeBoard = tgui::Button::create("Merge Board");
    mergeBoard ->setSize({"10%", "10%"});
    mergeBoard ->setPosition({"87%", "90%"});
    mergeBoard->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    mergeBoard ->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(mergeBoard);

    //Log Out Button
    auto logOut = tgui::Button::create("Logout");
    logOut ->setSize({"10%", "10%"});
    logOut ->setPosition({"87%", "2.0%"});
    logOut ->getRenderer()->setBackgroundColor(tgui::Color::Red);
    logOut ->getRenderer()->setTextColor(tgui::Color::White);
    gui.add(logOut); 

    // Priority List Button
    auto priorityListHeader = tgui::Button::create("Priority");
    priorityListHeader ->setSize({"10%", "10%"});
    priorityListHeader ->setPosition({"87%", "15.0%"});
    priorityListHeader ->getRenderer()->setBackgroundColor(tgui::Color::White);
    priorityListHeader ->getRenderer()->setTextColor(tgui::Color::Red);
    gui.add(priorityListHeader); 

    //Backlog button
    auto backlog = tgui::Button::create("Backlog");
    backlog->setSize({"12%", "12%"});
    backlog->setPosition({"3%", "15%"});
    backlog->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    backlog->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(backlog);

    //Add a backlog board button
    auto backlogAdd = tgui::Button::create("+");
    backlogAdd->setSize({"2%", "4%"});
    backlogAdd->setPosition({"13%", "23%"});
    backlogAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    backlogAdd->getRenderer()->setTextColor(tgui::Color::Black);
    backlogAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(backlogAdd);

    //In progress button
    auto ipr = tgui::Button::create("In Progress");
    ipr->setSize({"12%", "12%"});
    ipr->setPosition({"20%", "15%"});
    ipr->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    ipr->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(ipr);  

    //Add an in progress board button
    auto iprAdd = tgui::Button::create("+");
    iprAdd->setSize({"2%", "4%"});
    iprAdd->setPosition({"30.05%", "23%"});
    iprAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    iprAdd->getRenderer()->setTextColor(tgui::Color::Black);
    iprAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(iprAdd);

    //Ready to test button
    auto ready = tgui::Button::create("Ready to Test");
    ready->setSize({"12%", "12%"});
    ready->setPosition({"37%", "15%"});
    ready->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    ready->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(ready);

    //Add a ready board button
    auto readyAdd = tgui::Button::create("+");
    readyAdd->setSize({"2%", "4%"});
    readyAdd->setPosition({"47.05%", "23%"});
    readyAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    readyAdd->getRenderer()->setTextColor(tgui::Color::Black);
    readyAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(readyAdd);

    //Complete button
    auto complete = tgui::Button::create("Complete");
    complete->setSize({"12%", "12%"});
    complete->setPosition({"54%", "15%"});
    complete->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    complete->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(complete);    

    //Add a complete board button
    auto completeAdd = tgui::Button::create("+");
    completeAdd->setSize({"2%", "4%"});
    completeAdd->setPosition({"64%", "23%"});
    completeAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    completeAdd->getRenderer()->setTextColor(tgui::Color::Black);
    completeAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(completeAdd);

    //Not added button
    auto notAdded = tgui::Button::create("Not Added");
    notAdded->setSize({"12%", "12%"});
    notAdded->setPosition({"71%", "15%"});
    notAdded->getRenderer()->setBackgroundColor(sf::Color(153, 204, 255));
    notAdded->getRenderer()->setTextColor(tgui::Color::Black);
    gui.add(notAdded);   

    //Add a not added board button
    auto notAddedAdd = tgui::Button::create("+");
    notAddedAdd->setSize({"2%", "4%"});
    notAddedAdd->setPosition({"81%", "23%"});
    notAddedAdd->getRenderer()->setBackgroundColor(sf::Color(0, 240, 0));
    notAddedAdd->getRenderer()->setTextColor(tgui::Color::Black);
    notAddedAdd->getRenderer()->setTextStyle(tgui::Bold);
    gui.add(notAddedAdd);

    mergeBoard->onPress(&convergeBoard,std::ref(gui),1);
    logOut->onPress(&logout,std::ref(gui));

    backlogAdd->onPress(&addBoard, std::ref(gui), 1);
    iprAdd->onPress(&addBoard, std::ref(gui), 2);
    readyAdd->onPress(&addBoard, std::ref(gui), 3);
    completeAdd->onPress(&addBoard, std::ref(gui), 4);
    notAddedAdd->onPress(&addBoard, std::ref(gui), 5);
    
}

// -------------------------------------------------------------------------------------------------------------//


// -------------------------------- Window Screen 1 ----------------------------------------------------------//
void updateGUI(tgui::GuiBase &gui, int handler, tgui::Label::Ptr &message)
{
    gui.remove(message);
    if (handler == 1)
    {
        message->setSize({"100.0%", "100.0%"});
        message->setPosition({"35%", "51.0%"});
        message->setText("Succesfully created account");
        message->getRenderer()->setTextColor(sf::Color(0, 200, 0));
        gui.add(message);
    }
    else if (handler == 2)
    {
        message->setSize({"100.0%", "100.0%"});
        message->setPosition({"42%", "51.0%"});
        message->setText("Account exists.");
        message->getRenderer()->setTextColor(sf::Color::Red);
        gui.add(message);
    }
    else if (handler == 3)
    {
        message->setSize({"80.0%", "10.0%"});
        message->setPosition({"42%", "51.0%"});
        message->setText("Please fill in field");
        message->getRenderer()->setTextColor(sf::Color::Red);
        gui.add(message);
    }
    else if (handler == 4)
    {
        message->setSize({"80.0%", "10.0%"});
        message->setPosition({"32%", "51.0%"});
        message->setText("Incorrect Login. Please try again");
        message->getRenderer()->setTextColor(sf::Color::Red);
        gui.add(message);
    }
    else if (handler == 5)
    {
        message->setSize({"80.0%", "10.0%"});
        message->setPosition({"42%", "51.0%"});
        message->setText("User is logged in.");
        message->getRenderer()->setTextColor(sf::Color::Red);
        gui.add(message);
    }
}

void newUser(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password, tgui::GuiBase &gui, tgui::Label::Ptr &message)
{

    string _username = (string)username->getText();
    string _password = (string)password->getText();
    userLogin createUser;
    if (_username.empty() || _password.empty())
    {
        updateGUI(gui, 3, message);
        return;
    }
    if (!createUser.doesExist((string)username->getText(), (string)password->getText()))
    {

        std::ofstream fs("../../trello_application/TextDB/" + (string)username->getText() + (string)password->getText() + ".txt");
        string s = (string)username->getText() + (string)password->getText();
        hash<string> hashValue;
        fs << (string)username->getText() << endl;
        fs << (string)password->getText() << endl;
        fs << hashValue(s) << endl;
        fs << "User status: 0" << endl;
        fs.close();
        updateGUI(gui, 1, message);
        loadWidgets(gui, message);
    }
    else
    {
        updateGUI(gui, 2, message);
        loadWidgets(gui, message);
    }
   // std::cout << "Username: " << username->getText() << std::endl;
   // std::cout << "Password: " << password->getText() << std::endl;
}

void login(tgui::EditBox::Ptr username, tgui::EditBox::Ptr password, tgui::GuiBase &gui, tgui::Label::Ptr &message)
{
    string _username = (string)username->getText();
    string _password = (string)password->getText();
    if (_username.empty() || _password.empty())
    {
        updateGUI(gui, 3, message);
        return;
    }
    userLogin createUser;
    if (createUser.doesExist((string)username->getText(), (string)password->getText()))
    {
        if (endUser.getUserStatus(_username,_password) == 1) {
            updateGUI(gui, 5, message);
            return;
        }
        cout << "Login In Successful" << endl;
        endUser.parseFile(_username,_password);
        endUser.setUserName(_username);
        endUser.setupCRDTHandler();
        endUser.setUserStatus(1);
        endUser.mergeVectorCRDT();
        loadWidgets2(gui);
        // Background thread to automatically update board
        //automatic_merge_thread = std::thread(automatic_merge, std::ref(gui));
    }
    else
    {
        updateGUI(gui, 4, message);
        cout << "Try again!" << endl;
    }

    // Set id of all crdts
    backlogList.id = endUser.uniqueID;
    backlogServer.id = endUser.uniqueID;
    inprogressList.id = endUser.uniqueID;
    inprogressServer.id = endUser.uniqueID;
    readytotestList.id = endUser.uniqueID;
    readytotestServer.id = endUser.uniqueID;
    completeList.id = endUser.uniqueID;
    completeServer.id = endUser.uniqueID;
    notaddedList.id = endUser.uniqueID;
    notaddedServer.id = endUser.uniqueID;
    priorityListServer.setID(endUser.uniqueID);

    // Set id of all crdt counters
    numTasksBacklog.id = endUser.uniqueID;
    numTasksBacklogServer.id = endUser.uniqueID;
    numTasksInprogress.id = endUser.uniqueID;
    numTasksInprogressServer.id = endUser.uniqueID;
    numTasksReadytotest.id = endUser.uniqueID;
    numTasksReadytotestServer.id = endUser.uniqueID;
    numTasksComplete.id = endUser.uniqueID;
    numTasksCompleteServer.id = endUser.uniqueID;
    numTasksNotadded.id = endUser.uniqueID;
    numTasksNotaddedServer.id = endUser.uniqueID;
}

void loadWidgets(tgui::GuiBase &gui, tgui::Label::Ptr &message)
{
    updateTextSize(gui);
    gui.onViewChange([&gui]
                     { updateTextSize(gui); });

    auto titleHead = tgui::Label::create();
    titleHead->setSize({"100.0%", "100.0%"});
    //titleHead->setSize({"1000%", "1000%"});

    titleHead->setPosition({"46.0%", "4.0%"});
    titleHead->setText("TrelloRDT");
    titleHead->getRenderer()->setTextStyle(tgui::Bold);
    //titleHead->setTextSize(2);
    // titleHead->setTextFont()
    gui.add(titleHead);

    auto editBoxUsername = tgui::EditBox::create();
    editBoxUsername->setSize({"66.67%", "12.5%"});
    editBoxUsername->setPosition({"16.67%", "22.0%"});
    editBoxUsername->setDefaultText("Username");
    gui.add(editBoxUsername);

    // Create the password edit box
    // We copy the previous edit box here and keep the same size
    auto editBoxPassword = tgui::EditBox::copy(editBoxUsername);
    editBoxPassword->setPosition({"16.67%", "39.0%"});
    editBoxPassword->setPasswordCharacter('*');
    editBoxPassword->setDefaultText("Password");
    gui.add(editBoxPassword);

    // Create the login button
    auto loginButton = tgui::Button::create("Login");
    loginButton->setSize({"35%", "16.67%"});
    loginButton->setPosition({"35%", "62%"});
    gui.add(loginButton);

    //Create the create Account Button
    // Create the login button
    auto createAccountButton = tgui::Button::create("Create Account");
    createAccountButton->setSize({"35%", "16.67%"});
    createAccountButton->setPosition({"35%", "82%"});
    gui.add(createAccountButton);

    createAccountButton->onPress(&newUser, editBoxUsername, editBoxPassword, std::ref(gui), std::ref(message)); //creates a new user
    loginButton->onPress(&login, editBoxUsername, editBoxPassword, std::ref(gui), std::ref(message));
}

// -------------------------------------------------------------------------------------------------------------------//

//void automatic_merge(tgui::GuiBase &gui)
//{
//    while(true)
//    {
//        sleep(10);
//        convergeBoard(gui, 1);
//    }
//}

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "TrelloRDT");
    tgui::Gui gui(window);
    sf::Image Icon;
    Icon.loadFromFile("../../icon.png");
    window.setIcon(32,32,Icon.getPixelsPtr());

    usersOnline = tgui::Label::create();
    tgui::Label::Ptr message = tgui::Label::create();
    gui.setFont("../../blackjack.otf");
    loadWidgets(gui, message);

    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed) {
                endUser.setUserStatus(0);
                window.close();
            }
                
            gui.handleEvent(event);
        }
        window.clear(sf::Color(238, 238, 228));
        gui.draw();
        window.display();
    }
    endUser.setUserStatus(0);
    //exit(0);
    return 0;
}
