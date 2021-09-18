#pragma once

#include "messagearea.h"
#include "../api/channel.h"
#include "../api/message.h"
#include "../api/gateway.h"
#include "../api/jsonutils.h"

#include <QWidget>
#include <QGroupBox>
#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QScrollArea>
#include <QPushButton>

#include <string>
#include <vector>

namespace Ui {

class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow(QWidget *parent = nullptr);
    void setup();

signals:
    void messageRecieved(Api::Message);

private slots:
    void displayPrivateChannels();
    void openChannel(Api::Channel& privateChannel);
    void addMessage(const Api::Message& message);
    void sendMessage(const std::string& content);
    void sendTyping();

private:
    void setupInterface();
    void setupGateway(); //Creating and lauching the gateway
    void cleanRightColumn();
    void gatewayDispatchHandler(std::string& eventName, json& data); //Event handler for the gateway
    void userTyping(const json& data); //Used when we recieve TYPING_START event from the gateway

    //All the widget that need to be accessed from different functions
    QHBoxLayout *mainLayout;
    QGroupBox   *leftColumn;
    QGroupBox   *middleColumn;
    QGroupBox   *rightColumn;
    QVBoxLayout *leftColumnLayout;
    QVBoxLayout *middleColumnLayout;
    QVBoxLayout *rightColumnLayout;
    QScrollArea *leftScrollArea;
    QWidget     *leftScrollAreaContent;
    QGroupBox   *home;
    QVBoxLayout *homeLayout;
    QPushButton *homeButton;
    MessageArea *messageArea;
    QLabel      *typingLabel;

    Api::Gateway gw; //For the websocket connection

    //Storing channels and messages that we already gathered
    std::vector<Api::Channel *> privateChannels;
    std::map<std::string, std::vector<Api::Message>> channelsMessages;

    std::string currentOpenedChannel; //Current channel ID
    bool homePageShown;
};

} // namespace Ui
