#ifndef UTILS_HPP
#define UTILS_HPP

#include <string>
#include <iostream>

#define EMPTY_STRING ""
#define IRC_CRLF "\r\n"
#define BUFFER_SIZE 512

// Command Names
#define CMD_CAP "CAP"
#define CMD_PASS "PASS"
#define CMD_NICK "NICK"
#define CMD_USER "USER"
#define CMD_PING "PING"
#define CMD_JOIN "JOIN"
#define CMD_PART "PART"
#define CMD_KICK "KICK"
#define CMD_INVITE "INVITE"
#define CMD_TOPIC "TOPIC"
#define CMD_MODE "MODE"
#define CMD_PRIVMSG "PRIVMSG"
#define CMD_DCC "DCC"

#define DCC_MODE_SEND "SEND"
#define DCC_MODE_GET "GET"

#define SOH '\001'

// Command Lengths
#define CAP_CMD_LENGTH 3
#define PASS_CMD_LENGTH 4
#define NICK_CMD_LENGTH 4
#define USER_CMD_LENGTH 4
#define PING_CMD_LENGTH 4
#define JOIN_CMD_LENGTH 4
#define PART_CMD_LENGTH 4
#define KICK_CMD_LENGTH 4
#define INVITE_CMD_LENGTH 6
#define TOPIC_CMD_LENGTH 5
#define MODE_CMD_LENGTH 4
#define PRIVMSG_CMD_LENGTH 7

#define JUMP_TO_TOPIC 2
#define JUMP_TO_CHANNEL 1
#define JUMP_TO_MODE 1
#define JUMP_TO_ARG 4
#define MODE_FLAG 1
#define REMOVE_LIMIT 0

// Server Info
#define SERV_NAME "ircserv"
#define SERVER_NAME "ircserv" // Alias if needed
#define LIST_CAP "CAP * LS :"

// Error Codes (Numeric) - Moved to IrcReplies.hpp

// Reply Codes (Numeric) - Moved to IrcReplies.hpp

// Messages - Moved to IrcReplies.hpp
#define MSG_NEED_NICK "You need to set a nickname"
#define MSG_NEED_USER "You need to set a username"
#define MSG_ERR_UNKNOWNCOMMAND "Unknown command"
#define MSG_ERR_YOUWILLBEBANNED "Too many attempts, you will be banned"
#define MSG_ERR_NOORIGIN "No origin specified"
#define MSG_ERR_NICKCOLLISION "Nickname collision"

// Custom Error Codes not in IrcReplies.hpp
#define ERR_UNKNOWNCOMMAND 421
#define ERR_NICKCOLLISION 436
#define ERR_KEYSET 467
#define ERR_NOCHANMODES 477
#define ERR_UMODEUNKNOWNFLAG 501
#define ERR_USERSDONTMATCH 502
#define ERR_YOUWILLBEBANNED 465
#define MSG_RPL_INVITED "You have been invited"

const std::string getParam(int cmdLength, const std::string &line);
const std::string getChannelName(const std::string &line);

#endif
