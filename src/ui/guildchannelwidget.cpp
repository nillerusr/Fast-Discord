#include "ui/guildchannelwidget.h"

namespace Ui {

GuildChannelWidget::GuildChannelWidget(const Api::Channel& guildChannel, QWidget *parent)
    : QWidget(parent)
{
    // Attributes initialization
    id = *guildChannel.id;
    type = guildChannel.type;
    clicked = false;

    //  Determine the icon of this channel with its type
    std::string iconName;
    switch (type) {
    case Api::GuildText:
        iconName = "0";
        break;
    case Api::GuildVoice:
        iconName = "2";
        break;
    case Api::GuildCategory:
        iconName = "4";
        break;
    case Api::GuildNews:
        iconName = "5";
        break;
    case Api::GuildStore:
        iconName = "5";
        break;
    default:
        iconName = "0";
        break;
    }

    // Set the different stylesheets
    if (type == Api::GuildCategory) {
        // It is a category, there is no stylesheet
        hoverStyleSheet = clickedStyleSheet = (char *)"color: #FFF;";
    } else {
        hoverStyleSheet = (char *)"color: #DCDDDE;"
                          "border-radius: 4px;"
                          "background-color: #35373D;";
        clickedStyleSheet = (char *)"color: #FFF;"
                            "border-radius: 4px;"
                            "background-color: #393D43;";
    }

    // I have to create an other widget otherwise
    // the background color is not applied everywhere, I don't know why
    QWidget *container = new QWidget(this);
    QHBoxLayout *layout = new QHBoxLayout(container);

    // Create the icon
    if (type != Api::GuildCategory) {
        icon = new QLabel(this);
        icon->setPixmap(QPixmap(std::string("res/images/svg/guild-channel-icon" + iconName + ".svg").c_str()));
        icon->setFixedSize(32, 32);
        icon->setStyleSheet("color: #8E9297");
    }

    // Create the name label
    name = new QLabel((*guildChannel.name).c_str(), this);
    name->setStyleSheet("color: #8E9297");

    // Add widgets to layout and style it
    if (type != Api::GuildCategory) {
        layout->addWidget(icon, Qt::AlignLeft);
    }

    layout->addWidget(name, Qt::AlignLeft);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);

    // Create the main layout and add the container
    QHBoxLayout *mainLayout = new QHBoxLayout(this);
    mainLayout->addWidget(container);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    // Set the size of this widget
    this->setFixedSize(224, 34);
}

void GuildChannelWidget::mouseReleaseEvent(QMouseEvent *event)
{
    // Emit signals when clicked to open the channel or to show infos
    if (event->button() == Qt::LeftButton) {
        emit leftClicked(id);
    } else if (event->button() == Qt::RightButton) {
        emit rightClicked(id); // Does nothing for now
    }
}

void GuildChannelWidget::unclicked()
{
    // Reset the stylesheet of this widget if currently clicked
    if (clicked) {
        clicked = false;
        this->setStyleSheet("color: #8E9297;"
                            "background-color: none;");
    }
}

void GuildChannelWidget::mousePressEvent(QMouseEvent *)
{
    // Widget clicked : change the stylesheet
    if (!clicked && type != Api::GuildVoice) {
        this->setStyleSheet(clickedStyleSheet);
        clicked = true;
    }
}

void GuildChannelWidget::enterEvent(QEvent *)
{
    // Mouse hover : change the stylesheet
    if (!clicked) {
        this->setStyleSheet(hoverStyleSheet);
    }
}

void GuildChannelWidget::leaveEvent(QEvent *)
{
    // Reset the stylesheet if not clicked
    if (!clicked) {
        this->setStyleSheet("color: #8E9297;"
                            "background-color: none;");
    }
}

} // namespace Ui
