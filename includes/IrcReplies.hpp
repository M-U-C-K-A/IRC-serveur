#pragma once

#include <string>

// ============================================================================
//                          IRC NUMERIC REPLIES
// ============================================================================

// Command definitions
#define CMD_OPER "OPER"
#define CMD_KILL "KILL"
#define CMD_SQUIT "SQUIT"
#define CMD_CONNECT "CONNECT"
#define CMD_REHASH "REHASH"
#define CMD_RESTART "RESTART"
#define CMD_QUIT "QUIT"
#define CMD_NOTICE "NOTICE"
#define CMD_WALLOPS "WALLOPS"
#define CMD_AWAY "AWAY"
#define CMD_NAMES "NAMES"
#define CMD_LIST "LIST"
#define CMD_WHOIS "WHOIS"
#define CMD_VERSION "VERSION"
#define CMD_TIME "TIME"
#define CMD_PONG "PONG"

// Command lengths
#define OPER_CMD_LENGTH 5
#define KILL_CMD_LENGTH 5
#define SQUIT_CMD_LENGTH 6
#define CONNECT_CMD_LENGTH 8
#define QUIT_CMD_LENGTH 5
#define WALLOPS_CMD_LENGTH 8
#define AWAY_CMD_LENGTH 5
#define NAMES_CMD_LENGTH 6
#define LIST_CMD_LENGTH 5
#define WHOIS_CMD_LENGTH 6
#define VERSION_CMD_LENGTH 8
#define TIME_CMD_LENGTH 5

// Server name
#ifndef SERVER_NAME
#define SERVER_NAME "irc.server.local"
#endif

// ============================================================================
//                          SUCCESS REPLIES (RPL_*)
// ============================================================================

// 001-099: Connection registration replies
#define RPL_WELCOME 001
#define RPL_YOURHOST 002
#define RPL_CREATED 003
#define RPL_MYINFO 004

// 200-399: Command responses
#define RPL_AWAY 301
#define RPL_UNAWAY 305
#define RPL_NOWAWAY 306
#define RPL_WHOISUSER 311
#define RPL_WHOISSERVER 312
#define RPL_WHOISOPERATOR 313
#define RPL_ENDOFWHOIS 318
#define RPL_WHOISCHANNELS 319
#define RPL_LIST 322
#define RPL_LISTEND 323
#define RPL_CHANNELMODEIS 324
#define RPL_NOTOPIC 331
#define RPL_TOPIC 332
#define RPL_INVITING 341
#define RPL_VERSION 351
#define RPL_NAMREPLY 353
#define RPL_ENDOFNAMES 366
#define RPL_YOUREOPER 381
#define RPL_REHASHING 382
#define RPL_TIME 391

// ============================================================================
//                          ERROR REPLIES (ERR_*)
// ============================================================================

// 400-599: Error replies
#define ERR_NOSUCHNICK 401
#define ERR_NOSUCHCHANNEL 403
#define ERR_NOSUCHSERVER 402
#define ERR_NOORIGIN 409
#define ERR_NORECIPIENT 411
#define ERR_NOTEXTTOSEND 412
#define ERR_NONICKNAMEGIVEN 431
#define ERR_ERRONEUSNICKNAME 432
#define ERR_INVALIDUSERNAME 432
#define ERR_NICKNAMEINUSE 433
#define ERR_USERONCHANNEL 443
#define ERR_USERNOTINCHANNEL 441
#define ERR_NOTONCHANNEL 442
#define ERR_NOTREGISTERED 451
#define ERR_NEEDMOREPARAMS 461
#define ERR_ALREADYREGISTRED 462
#define ERR_PASSWDMISMATCH 464
#define ERR_CHANNELISFULL 471
#define ERR_UNKNOWNMODE 472
#define ERR_INVITEONLYCHAN 473
#define ERR_BANNEDFROMCHAN 474
#define ERR_BADCHANNELKEY 475
#define ERR_BADCHANMASK 476
#define ERR_NOPRIVILEGES 481
#define ERR_CHANOPRIVSNEEDED 482
#define ERR_CANTKILLSERVER 483
#define ERR_NOOPERHOST 491

// ============================================================================
//                          ERROR MESSAGES
// ============================================================================

#define MSG_ERR_NOSUCHNICK "No such nick/channel"
#define MSG_ERR_NOSUCHSERVER "No such server"
#define MSG_ERR_NOORIGIN "No origin specified"
#define MSG_ERR_NORECIPIENT "No recipient given"
#define MSG_ERR_NOTEXTTOSEND "No text to send"
#define MSG_ERR_NONICKNAMEGIVEN "No nickname given"
#define MSG_ERR_ERRONEUSNICKNAME "Erroneous nickname"
#define MSG_ERR_INVALIDNICK "Erroneous nickname"
#define MSG_ERR_NICKNAMEINUSE "Nickname is already in use"
#define MSG_ERR_WRONGUSER "Invalid username"
#define MSG_ERR_USERNOTINCHANNEL "They aren't on that channel"
#define MSG_ERR_USERONCHANNEL "is already on channel"
#define MSG_ERR_NOTONCHANNEL "You're not on that channel"
#define MSG_ERR_NOTREGISTERED "You have not registered"
#define MSG_ERR_NEEDMOREPARAMS "Not enough parameters"
#define MSG_ERR_ALREADYREGISTRED "You may not reregister"
#define MSG_ERR_PASSWDMISMATCH "Password incorrect"
#define MSG_ERR_CHANNELISFULL "Cannot join channel (+l)"
#define MSG_ERR_INVITEONLYCHAN "Cannot join channel (+i)"
#define MSG_ERR_BANNEDFROMCHAN "Cannot join channel (+b)"
#define MSG_ERR_BADCHANNELKEY "Cannot join channel (+k)"
#define MSG_ERR_BADCHANMASK "Bad Channel Mask"
#define MSG_ERR_NOPRIVILEGES "Permission Denied- You're not an IRC operator"
#define MSG_ERR_CHANOPRIVSNEEDED "You're not channel operator"
#define MSG_ERR_CANTKILLSERVER "You can't kill a server!"
#define MSG_ERR_NOOPERHOST "No O-lines for your host"
#define MSG_ERR_SINGLE_SERVER_CONNECT "CONNECT not available in single-server mode"
#define MSG_ERR_SINGLE_SERVER_SQUIT "Cannot SQUIT this server (single-server mode)"

// ============================================================================
//                          SUCCESS MESSAGES
// ============================================================================

#define MSG_RPL_UNAWAY "You are no longer marked as being away"
#define MSG_RPL_NOWAWAY "You have been marked as being away"
#define MSG_RPL_YOUREOPER "You are now an IRC operator"
#define MSG_RPL_REHASHING "Rehashing"
#define MSG_RPL_ENDOFWHOIS "End of /WHOIS list"
#define MSG_RPL_LISTEND "End of /LIST"
#define MSG_RPL_ENDOFNAMES "End of /NAMES list"
#define MSG_ERR_NOSUCHCHANNEL "No such channel"
#define MSG_NOTOPIC "No topic is set"
#define RPL_TOPICWHOTIME 333

// ============================================================================
//                          CHANNEL MESSAGES
// ============================================================================

#define MSG_PART "Leaving"
#define MSG_KICK "Kicked by operator"

// ============================================================================
//                          PROTOCOL CONSTANTS
// ============================================================================

#define IRC_MAX_NICKNAME_LENGTH 9
#define IRC_MAX_MESSAGE_LENGTH 512
#define IRC_CRLF "\r\n"

// Valid nickname characters
#define IRC_NICK_SPECIAL_CHARS "[]\\`_^{|}-"
