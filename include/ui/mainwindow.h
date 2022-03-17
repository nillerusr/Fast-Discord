#pragma once

#include "leftcolumn.h"
#include "middlecolumn.h"
#include "rightcolumn.h"
#include "api/ressourcemanager.h"

#include <QWidget>
#include <QHBoxLayout>

namespace Ui {

// The main window of the app
class MainWindow : public QWidget
{
    Q_OBJECT

public:
    MainWindow();

signals:
    void clientSettingsReceived();

private slots:
    void setup();

private:
    void addAccountInConfig(QSettings *settings, QMap<QString, QString> accountMap);
    QMap<QString, QString> getAccountsMap(QSettings *settings);
    QMap<QString, QString> getNewAccount();
    QString getAccountToken();

    //All the widget that need to be accessed from different functions
    QHBoxLayout  *mainLayout;
    LeftColumn   *leftColumn;
    MiddleColumn *middleColumn;
    RightColumn  *rightColumn;

    Api::RessourceManager *rm;           // Manage all the data we receive

    Api::Client *client;                 // The actual client
    Api::ClientSettings *clientSettings; // and the settings
};

} // namespace Ui
