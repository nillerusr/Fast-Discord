#pragma once

#include "message.h"
#include "attachment.h"
#include "user.h"
#include "overwrite.h"
#include "channel.h"
#include "thread.h"
#include "team.h"
#include "application.h"
#include "guildmember.h"

#include "../../lib/nlohmann/json.hpp"

#include <string>
#include <vector>

using json = nlohmann::json;

namespace Api {

//Get the string value of a json object but handle exception if the value is null
std::string *valueNoNull(const json& jsonObj, const std::string& key, const std::string& defaultValue);


//All the functions used to get an object from a json object

User *getUserFromJson(const json& jsonObj, const std::string& key);
std::vector<User> *getUsersFromJson(const json& jsonObj, const std::string& key);

Overwrite *getOverwriteFromJson(const json& jsonObj, const std::string& key);
std::vector<Overwrite> *getOverwritesFromJson(const json& jsonObj, const std::string& key);

ThreadMember *getThreadMemberFromJson(const json& jsonObj, const std::string& key);

ThreadMetadata *getThreadMetadataFromJson(const json& jsonObj, const std::string& key);

Channel *getChannelFromJson(const json& jsonObj, const std::string& key);

TeamMember *getTeamMemberFromJson(const json& jsonObj, const std::string& key);
std::vector<TeamMember> *getTeamMembersFromJson(const json& jsonObj, const std::string& key);

Team *getTeamFromJson(const json& jsonObj, const std::string& key);

GuildMessageMember *getGuildMessageMemberFromJson(const json& jsonObj, const std::string& key);

MessageInteraction *getMessageInteractionFromJson(const json& jsonObj, const std::string& key);

Emoji *getEmojiFromJson(const json& jsonObj, const std::string& key);

Reaction *getReactionFromJson(const json& jsonObj, const std::string& key);
std::vector<Reaction> *getReactionsFromJson(const json& jsonObj, const std::string& key);

Attachment *getAttachmentFromJson(const json& jsonObj, const std::string& key);
std::vector<Attachment> *getAttachmentsFromJson(const json& jsonObj, const std::string& key);

ChannelMention *getChannelMentionFromJson(const json& jsonObj, const std::string& key);
std::vector<ChannelMention> *getChannelMentionsFromJson(const json& jsonObj, const std::string& key);

SelectOption *getSelectOptionFromJson(const json& jsonObj, const std::string& key);
std::vector<SelectOption> *getSelectOptionsFromJson(const json& jsonObj, const std::string& key);

MessageComponent *getMessageComponentFromJsonNR(const json& jsonObj, const std::string& key);
std::vector<MessageComponent> *getMessageComponentsFromJson(const json& jsonObj, const std::string& key);

std::vector<StickerItem> *getStickerItemsFromJson(const json& jsonObj, const std::string& key);
std::vector<Sticker> *getStickersFromJson(const json& jsonObj, const std::string& key);

MessageActivity *getMessageActivityFromJson(const json& jsonObj, const std::string& key);

Message *getMessageFromJson(const json& jsonObj, const std::string& key);
Message *getMessageFromJsonNR(const json& jsonObj, const std::string& key);

} // namespace Api
