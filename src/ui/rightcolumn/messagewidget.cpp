#include "ui/rightcolumn/messagewidget.h"

#include "ui/rightcolumn/attachmentfile.h"
#include "api/jsonutils.h"

#include <QHBoxLayout>
#include <QVBoxLayout>
#include <QLabel>
#include <QDateTime>
#include <QTextStream>

namespace Ui {

MessageWidget::MessageWidget(Api::RessourceManager *rmp, const Api::Message *message, bool isFirstp, bool separatorBefore, QWidget *parent)
    : Widget(parent)
{
    // Attributes initialization
    rm = rmp;
    isFirst = isFirstp;

    switch (message->type) {
        case Api::Default:
            defaultMessage(message, separatorBefore);
            break;

        case Api::RecipientAdd:
        case Api::RecipientRemove:
            recipientMessage(message);
            break;

        case Api::CallT:
            callMessage(message);
            break;

        case Api::ChannelNameChange:
            channelNameChangeMessage(message);
            break;

        case Api::ChannelIconChange:
            channelIconChangeMessage(message);
            break;

        case Api::ChannelPinnedMessage:
            channelPinnedMessage(message);
            break;
        
        case Api::GuildMemberJoin:
            guildMemberJoinMessage(message);
            break;

        case Api::ChannelFollowAdd:
            channelFollowAdd(message);
            break;

        case Api::UserPremiumGuildSubscription:
        case Api::UserPremiumGuildSubscriptionTier1:
        case Api::UserPremiumGuildSubscriptionTier2:
        case Api::UserPremiumGuildSubscriptionTier3:
            userPremiumGuildSubscriptionMessage(message);
            break;

        case Api::Reply:
        default:
            defaultMessage(message, separatorBefore);
    }
}

void const MessageWidget::setAvatar(const QString& avatarFileName)
{
    avatar->setImage(avatarFileName);
}

void const MessageWidget::setReplyAvatar(const QString& avatarFileName)
{
    replyAvatar->setImage(avatarFileName);
}

void MessageWidget::addImage(const QString& filename, int width, int height)
{
    QLabel *imageLabel = new QLabel(this);

    if (width >= height && width > 400) {
        float ratio = width / 400;
        width /= ratio;
        height /= ratio;
    } else if (height > width && height > 400) {
        float ratio = height / 400;
        height /= ratio;
        width /= ratio;
    }
    imageLabel->setFixedSize(width, height);

    QPixmap pixmap(filename);
    imageLabel->setPixmap(pixmap.scaled(width, height, Qt::KeepAspectRatio, Qt::SmoothTransformation));
    dataLayout->addWidget(imageLabel);
}

void MessageWidget::enterEvent(QEvent *)
{
    if (textLabel != nullptr) {
        this->setBackgroundColor(Settings::BackgroundMessageHover);
    } else {
        if (mainMessage != nullptr)
            mainMessage->setBackgroundColor(Settings::BackgroundMessageHover);
        if (content != nullptr)
            content->setStyleSheet("color:" + Settings::colors[Settings::InteractiveHover].name());
        if (reply != nullptr)
            reply->setBackgroundColor(Settings::BackgroundMessageHover);
        if (!isFirst && timestampLabel != nullptr) timestampLabel->setText(" " + hoveredTimestamp);
    }
}

void MessageWidget::leaveEvent(QEvent *)
{
    if (textLabel != nullptr) {
        this->setBackgroundColor(Settings::None);
    } else {
        if (mainMessage != nullptr)
            mainMessage->setBackgroundColor(Settings::None);
        if (content != nullptr)
            content->setStyleSheet("color:" + Settings::colors[Settings::TextNormal].name());
        if (reply != nullptr)
            reply->setBackgroundColor(Settings::None);
        if (!isFirst && timestampLabel != nullptr) timestampLabel->setText("");
    }
}

QString const MessageWidget::processTime(const QTime& time)
{
    // Process the time to a nicer format

    // Get the hours and minutes
    int hour = time.hour();
    int minute = time.minute();
    // Process the hours and minutes
    QString hourString = hour > 12 ? QString::number(hour - 12) : QString::number(hour);
    QString minuteString = minute < 10 ? "0" + QString::number(minute) : QString::number(minute);
    // Create the final timestamp format
    QString messageTime = hourString + ":" + minuteString + " ";
    hour / 12 == 1 ? messageTime += "PM" : messageTime += "AM";
    return messageTime;
}

QString const MessageWidget::processTimestamp(const QDateTime& dateTime)
{
    // Get the date of the message and the current date to compare them
    QDate messageDate = dateTime.date();
    QDate currentDate = QDate::currentDate();
    QString date;
    if (messageDate != currentDate) {
        // The message was not sent today
        if (messageDate.year() == currentDate.year() &&
            messageDate.month() == currentDate.month() &&
            messageDate.day() == currentDate.day() - 1) {

            // The message was sent yesterday
            date = "Yesterday at " + processTime(dateTime.time());
        } else {
            // The message is older
            date = (messageDate.month() < 10 ? "0" + QString::number(messageDate.month()) : QString::number(messageDate.month())) + "/" +
                   (messageDate.day() < 10 ? "0" + QString::number(messageDate.day()) : QString::number(messageDate.day())) + "/" +
                   QString::number(messageDate.year());
        }
    } else {
        date = "Today at " + processTime(dateTime.time());
    }

    return date;
}

void MessageWidget::defaultMessage(const Api::Message *message, bool separatorBefore)
{
    // Create the main widgets
    QVBoxLayout *layout = new QVBoxLayout(this);
    mainMessage = new Widget(this);
    QHBoxLayout *mainLayout = new QHBoxLayout(mainMessage);
    Widget *data = new Widget(mainMessage);
    dataLayout = new QVBoxLayout(data);
    Widget *iconContainer = new Widget(mainMessage);
    QVBoxLayout *iconLayout = new QVBoxLayout(iconContainer);

    QFont font;
    font.setPixelSize(14);
    font.setFamily("whitney");

    // Get the date and time of the message
    QDateTime dateTime = QDateTime::fromString(message->timestamp, Qt::ISODate).toLocalTime();

    if (message->referencedMessage != nullptr && message->referencedMessage->id != 0) {
        Api::Message *ref = message->referencedMessage;
        height += 22;
        isFirst = true;

        reply = new Widget(this);
        reply->setFixedHeight(28);
        QHBoxLayout *replyLayout = new QHBoxLayout(reply);

        Widget *replyIcon = new Widget(reply);
        replyIcon->setImage("res/images/png/reply.png");
        replyIcon->setFixedSize(68, 22);
        replyLayout->addWidget(replyIcon);

        // Get the icon of the message
        if (ref->author.avatar.isNull()) {
            // Use an asset if the user doesn't have an icon
            replyLayout->addWidget(new RoundedImage("res/images/png/user-icon-asset0.png", 16, 16, 8, reply));
        } else {
            // Request the avatar
            QString avatarFileName = ref->author.avatar + (ref->author.avatar.indexOf("a_") == 0 ? ".gif" : ".png");
            replyAvatar = new RoundedImage(16, 16, 8, reply);
            replyLayout->addWidget(replyAvatar);
            rm->getImage([this](void *avatarFileName) {this->setReplyAvatar(*reinterpret_cast<QString *>(avatarFileName));}, "https://cdn.discordapp.com/avatars/" + ref->author.id + "/" + avatarFileName, avatarFileName);
        }

        QLabel *username = new QLabel(ref->author.username, reply);
        username->setFont(font);
        username->setFixedHeight(22);
        username->setSizePolicy(QSizePolicy::Minimum, QSizePolicy::Preferred);
        username->setTextInteractionFlags(Qt::TextSelectableByMouse);
        username->setCursor(QCursor(Qt::PointingHandCursor));
        username->setStyleSheet("color:" + Settings::colors[Settings::HeaderPrimary].name());

        Label *content = new Label(ref->content.replace('\n', "  "), nullptr);
        content->setFont(font);
        content->setFixedSize(QFontMetrics(font).horizontalAdvance(ref->content), 18);
        content->setSizePolicy(QSizePolicy::Expanding, QSizePolicy::Preferred);
        content->setCursor(QCursor(Qt::PointingHandCursor));
        content->setTextColor(Settings::InteractiveNormal);

        replyLayout->addWidget(username);
        replyLayout->addWidget(content);
        replyLayout->setSpacing(4);
        replyLayout->addStretch(1);

        layout->addWidget(reply);
    }

    if (isFirst) {
        // The message is not grouped to another message

        // Variable creation
        const Api::User& author = message->author;
        QString avatarId = author.avatar;

        height += 40;

        // Get the icon of the message
        if (avatarId.isNull()) {
            // Use an asset if the user doesn't have an icon
            avatar = new RoundedImage("res/images/png/user-icon-asset0.png", 40, 40, 20, iconContainer);
            iconLayout->addWidget(avatar);
            iconLayout->setAlignment(avatar, Qt::AlignTop | Qt::AlignHCenter);
        } else {
            // Request the avatar
            QString avatarFileName = avatarId + (avatarId.indexOf("a_") == 0 ? ".gif" : ".png");
            avatar = new RoundedImage(40, 40, 20, iconContainer);
            iconLayout->addWidget(avatar);
            iconLayout->setAlignment(avatar, Qt::AlignTop | Qt::AlignHCenter);
            rm->getImage([this](void *avatarFileName) {this->setAvatar(*reinterpret_cast<QString *>(avatarFileName));}, "https://cdn.discordapp.com/avatars/" + author.id + "/" + avatarFileName, avatarFileName);
        }

        // Widget to show some infos of the message
        Widget *messageInfos = new Widget(data);
        QHBoxLayout *infosLayout = new QHBoxLayout(messageInfos);
        QLabel *name = new QLabel(author.username, messageInfos);
        font.setPixelSize(16);
        name->setFont(font);
        name->setTextInteractionFlags(Qt::TextSelectableByMouse);
        name->setCursor(QCursor(Qt::PointingHandCursor));
        name->setStyleSheet("color:" + Settings::colors[Settings::HeaderPrimary].name());
        name->setCursor(QCursor(Qt::IBeamCursor));
        Label *date = new Label(processTimestamp(dateTime), messageInfos);
        font.setPixelSize(12);
        date->setTextColor(Settings::TextMuted);
        date->setFixedSize(QFontMetrics(font).horizontalAdvance(processTimestamp(dateTime)), 22);
        date->setFont(font);
        date->setTextColor(Settings::TextMuted);

        // Add widgets and style the infos layout
        infosLayout->addWidget(name);
        infosLayout->addWidget(date);
        infosLayout->insertStretch(-1);
        infosLayout->setContentsMargins(0, 0, 0, 0);

        // Add the message infos and style the data layout
        dataLayout->addWidget(messageInfos);
        dataLayout->setSpacing(0);

        // Style the message widget
        this->setContentsMargins(0, separatorBefore ? 0 : 20, 0, 0);
    } else {
        height += 22;

        // Add the label that shows the timestamp when the message is hovered
        hoveredTimestamp = processTime(dateTime.time());
        timestampLabel = new Label(this);
        font.setPixelSize(12);
        timestampLabel->setFont(font);
        timestampLabel->setFixedHeight(22);
        timestampLabel->setTextColor(Settings::TextMuted);
        timestampLabel->setFlags(Qt::AlignCenter);
        iconLayout->addWidget(timestampLabel);
        iconLayout->setContentsMargins(0, 0, 0, 0);
        mainLayout->setContentsMargins(0, 0, 0, 0);
    }

    // Style the icon container
    iconContainer->setFixedWidth(72);
    iconContainer->setSizePolicy(QSizePolicy::Fixed, QSizePolicy::Minimum);
    iconContainer->setLayout(iconLayout);

    // Create and style the content label
    content = nullptr;
    if (!message->content.isNull() && !message->content.isEmpty()) {
        content = new MarkdownLabel(message->content, rm, this);
        dataLayout->addWidget(content);
    }

    // Style the data layout
    dataLayout->setContentsMargins(0, 0, 0, 0);

    if (!message->attachments.empty()) {
        QVector<Api::Attachment *> attachments = message->attachments;
        for (int i = 0 ; i < attachments.size() ; i++) {
            Api::Attachment *attachment = attachments[i];
            if (attachment->contentType != nullptr
              && attachment->contentType.indexOf("image") != -1
              && attachment->contentType.indexOf("svg") == -1) {
                QString filename = attachment->id +
                        attachment->filename.mid(0, attachment->filename.lastIndexOf('.'));
                if (attachment->width >= attachment->height && attachment->width > 400)
                    height += attachment->height / (attachment->width / 400);
                else if (attachment->height > attachment->width && attachment->height > 400)
                    height += attachment->height / (attachment->height / 400);
                else 
                    height += attachment->height;
                rm->getImage([this, attachment](void *filename) {
                    this->addImage(*reinterpret_cast<QString *>(filename), attachment->width, attachment->height);
                }, attachment->proxyUrl, filename);
            } else {
                dataLayout->addWidget(new AttachmentFile(rm->requester, attachment, data));
                height += 52;
            }
        }
    }
    dataLayout->addStretch(1);

    mainMessage->setMinimumHeight(height);

    // Add widgets to the main layout and style it
    mainLayout->addWidget(iconContainer);
    mainLayout->addWidget(data);
    mainLayout->setSpacing(0);
    mainLayout->setContentsMargins(0, 0, 0, 0);

    layout->addWidget(mainMessage);
    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
}

void MessageWidget::iconMessage(const Api::Message *message, const QString &text, const QString& iconName)
{
    QFont font;
    font.setPixelSize(16);
    font.setFamily("whitney");

    QHBoxLayout *layout = new QHBoxLayout(this);
    QLabel *icon = new QLabel(this);
    textLabel = new Label(text, this);
    textLabel->setFixedSize(QFontMetrics(font).horizontalAdvance(text), 22);
    textLabel->setFont(font);

    font.setPixelSize(12);
    Label *timestampLabel = new Label(processTimestamp(QDateTime::fromString(message->timestamp, Qt::ISODate).toLocalTime()), this);
    timestampLabel->setFixedSize(QFontMetrics(font).horizontalAdvance(processTimestamp(QDateTime::fromString(message->timestamp, Qt::ISODate).toLocalTime())), 16);
    timestampLabel->setFont(font);

    icon->setFixedWidth(44);
    icon->setPixmap(QPixmap("res/images/svg/" + iconName));

    textLabel->setTextColor(Settings::ChannelIcon);
    textLabel->setCursor(QCursor(Qt::IBeamCursor));
    timestampLabel->setTextColor(Settings::TextMuted);

    layout->addSpacing(28);
    layout->addWidget(icon);
    layout->addWidget(textLabel);
    layout->addSpacing(4);
    layout->addWidget(timestampLabel);
    layout->addStretch(1);

    layout->setSpacing(0);
    layout->setContentsMargins(0, 0, 0, 0);
}

void MessageWidget::recipientMessage(const Api::Message *message)
{
    QString arrowName;
    QString text;

    if (message->type == Api::RecipientAdd) {
        arrowName = "green-right-arrow.svg";
        text = message->mentions[0]->username + " added " +
                message->author.username + " to the group.";
    } else {
        arrowName = "red-left-arrow.svg";
        text = message->mentions[0]->username + " left the group.";
    }

    iconMessage(message, text, arrowName);
}

void MessageWidget::callMessage(const Api::Message *message)
{
    rm->getClient([message, this](void *clientPtr){

    QString phoneName;
    QString text;
    Api::Snowflake clientId = reinterpret_cast<Api::Client *>(clientPtr)->id;
    QDateTime dateTime = QDateTime::fromString(message->timestamp, Qt::ISODate).toLocalTime();

    QVector<Api::Snowflake> participants = message->call->participants;
    bool participated = false;
    for (int i = 0 ; i < participants.size() ; i++) {
        if (participants[i] == clientId) participated = true;
    }

    if (!participated) {
        text = "You missed a call from " + message->author.username;
    } else {
        text = message->author.username + " started a call";
    }
    QString endedTimestampStr = message->call->endedTimestamp;
    if (!endedTimestampStr.isNull()) {
        phoneName = "gray-phone.svg";
        qint64 duration = QDateTime::fromString(endedTimestampStr, Qt::ISODate)
                .toLocalTime().toSecsSinceEpoch() -
                dateTime.toLocalTime().toSecsSinceEpoch();
        if (duration < 60) {
            text += " that lasted a few seconds";
        } else if (duration < 3600) {
            text += " that lasted ";
            if (duration < 120)
                text += "a minute";
            else
                text += QString::number(duration / 60) + " minutes";
        } else {
            text += " that lasted ";
            if (duration < 7200)
                text += "a hour";
            else
                text += QString::number(duration / 3600) + " hours";
        }
    } else {
        phoneName = "green-phone.svg";
    }
    text += ".";

    if (clientId == message->author.id)
        phoneName = "green-phone.svg";

    iconMessage(message, text, phoneName);

    });
}

void MessageWidget::channelNameChangeMessage(const Api::Message *message)
{
    iconMessage(message, message->author.username + " changed the channel name: " + message->content, "pen.svg");
}

void MessageWidget::channelIconChangeMessage(const Api::Message *message)
{
    iconMessage(message, message->author.username + " changed the channel icon.", "pen.svg");
}

void MessageWidget::channelPinnedMessage(const Api::Message *message)
{
    iconMessage(message, message->author.username + " pinned a message to this channel. See all pinned messages.", "pin.svg");
}

void MessageWidget::userPremiumGuildSubscriptionMessage(const Api::Message *message)
{
    QString text = message->author.username + " just boosted the server.";
    if (message->type == Api::UserPremiumGuildSubscriptionTier1)
        text += "(Tier 1)";
    else if (message->type == Api::UserPremiumGuildSubscriptionTier2)
        text += "(Tier 2)";
    else if (message->type == Api::UserPremiumGuildSubscriptionTier3)
        text += "(Tier 3)";

    iconMessage(message, text, "boost.svg");
}

void MessageWidget::guildMemberJoinMessage(const Api::Message *message)
{
    int randInt = rand() % 39;
    QFile messageFile("res/text/welcome-messages.txt");
    messageFile.open(QIODevice::ReadOnly);
    for (int i = 0 ; i < randInt ; i++) {
        messageFile.readLine();
    }
    QString text = messageFile.readLine();
    int index = text.indexOf("{}");
    while (index != -1){
        text.replace(index, 2, message->author.username);
        index = text.indexOf("{}");
    }

    iconMessage(message, text, "green-right-arrow.svg");
}

void MessageWidget::channelFollowAdd(const Api::Message *message)
{
    iconMessage(message, message->content + " has added " + message->author.username
     + " to this channel. Its most important updates will show up here.", "green-right-arrow.svg");
}

} // namespace Ui
