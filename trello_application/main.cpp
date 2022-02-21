#include <bits/stdc++.h>
// #include <SFML/Graphics.hpp>
#include <TGUI/TGUI.hpp>
#include "userLogin.hpp"
using namespace std;



class userInfo {
    private:
    string userName;
    
    public:
    userInfo() {

    }

    void setUserName(string userName) {
        this->userName = userName;
    }

    string getUserName() {
        return this->userName;
    }

};

//Fuction Definations
void loadWidgets(tgui::GuiBase &gui, tgui::Label::Ptr &message);

//Class Definations
userInfo endUser;


// -------------------------------- Window Screen 2 ----------------------------------------------------------//

void loadWidgets2(tgui::GuiBase &gui)
{
    gui.removeAllWidgets();
    tgui::Label::Ptr welcomeMessage = tgui::Label::create();
    welcomeMessage->setSize({"100.0%", "100.0%"});
    welcomeMessage->setPosition({"0%", "2.0%"});
    welcomeMessage->setText("Welcome: " + endUser.getUserName());
    //message->getRenderer()->setTextColor(sf::Color(0, 200, 0));
    gui.add(welcomeMessage);
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
        fs.close();
        updateGUI(gui, 1, message);
        loadWidgets(gui, message);
    }
    else
    {
        updateGUI(gui, 2, message);
        loadWidgets(gui, message);
    }
    std::cout << "Username: " << username->getText() << std::endl;
    std::cout << "Password: " << password->getText() << std::endl;
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
        cout << "Login In Successful" << endl;
        endUser.setUserName(_username);
        loadWidgets2(gui);
    }
    else
    {
        updateGUI(gui, 4, message);
        cout << "Try again!" << endl;
    }
}

void updateTextSize(tgui::GuiBase &gui)
{
    // Update the text size of all widgets in the gui, based on the current window height
    const float windowHeight = gui.getView().getRect().height;
    gui.setTextSize(static_cast<unsigned int>(0.07f * windowHeight)); // 7% of height
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

int main()
{
    sf::RenderWindow window(sf::VideoMode(1920, 1080), "TrelloRDT");
    tgui::Gui gui(window);
    tgui::Label::Ptr message = tgui::Label::create();
    gui.setFont("../../blackjack.otf");
    loadWidgets(gui, message);
    while (window.isOpen())
    {
        sf::Event event;
        while (window.pollEvent(event))
        {
            if (event.type == sf::Event::Closed)
                window.close();
            gui.handleEvent(event);
        }
        window.clear(sf::Color(238, 238, 228));
        gui.draw();
        window.display();
    }
    return 0;
}
