#pragma once

#include <iostream>
#include <poll.h>
#include <sys/types.h>
#include <sys/socket.h>
#include <netdb.h>
#include <unistd.h>
#include <set>
#include <arpa/inet.h>
#include <sys/epoll.h>
#include <signal.h>
#include <map>
#include <fcntl.h>
#include <vector>
#include <fstream>
#include <cstdlib>

#include "User.hpp"
#include "Channel.hpp"
#include "Utils.hpp"
#include "IrcReplies.hpp"

#define MAX_USER 1024
#define MAX_EVENTS 10

typedef struct {
	std::string	dcc;
	std::string	mode;
	std::string	filename;
	std::string ip;
	std::string port;
	std::string fileSize;
	uint16_t	realPort;
}				t_dcc;

class Server
{
private:
	int			port;
	int			socketfd;
	std::string	password;
	std::vector<Channel>	channelList;
	int			epollFd;
	epoll_event	event;
	epoll_event	events[MAX_EVENTS];
	std::map<int, User>	Users;
	User		ircBot;
public:
	Server();
	Server(const Server &src);
	Server operator=(const Server &src);
	~Server();

	static bool running;
	void	initSocket();
	void	initEpoll();
	void	initServer(const int &port, const std::string &password);
	void	runServer();
	static void	signalHandler(int signum);

	int 			findIdByName(const std::string &name) const;
	std::string		findNameById(const int &clientFd) const;
	Channel			&findChannelByName(const std::string &channelName);

	bool	nickAlreadyInUse(const std::string &nick);
	void	welcomeUser(const int &code, const std::string &name) const;
	void	acceptUser();
	void	parseInput(int userFd);
	bool	hasPassword() const;
	void	sendRPL(const int &clientFd, const int code, const std::string &nick, const std::string &message) const;
	std::vector<std::string> getReqCap(const std::string line);

	void	handleCap(const int &clientFd, const std::string &line);
	void	handleNick(const int &clientFd, const std::string &line);
	void	handleUsername(const int &clientFd, const std::string &line);
	void	handleLine(const int &clientFd, const std::string &line);
	void	handleJoin(const int &clientFd, const std::string &line);
	void	handlePass(const int &clientFd, const std::string &line);
	void	handleTopic(const int &clientFd, const std::string &line);
	void	handleKick(const int &clientFd, const std::string &line);
	void	handlePing(const int &clientFd, const std::string &line);
	void	handleInvite(const int &clientFd, const std::string &line);
	void	handlePart(const int &clientFd, const std::string &line);
	void	handleMode(const int & clientFd, const std::string &line);
	void	handleDCC(const int &clientFd, const std::string &targetNick, const std::string &message);
	
	// Query commands
	void	handleWho(const int &clientFd, const std::string &line);
	void	handleWhowas(const int &clientFd, const std::string &line);
	void	handleUserhost(const int &clientFd, const std::string &line);
	void	handleIson(const int &clientFd, const std::string &line);
	void	handleStats(const int &clientFd, const std::string &line);
	void	handleLinks(const int &clientFd, const std::string &line);
	void	handleAdmin(const int &clientFd, const std::string &line);
	void	handleInfo(const int &clientFd, const std::string &line);
	
	// Maintenance commands
	void	handleTrace(const int &clientFd, const std::string &line);
	void	handleError(const int &clientFd, const std::string &line);
	void	handleUsers(const int &clientFd, const std::string &line);
	void	handleSummon(const int &clientFd, const std::string &line);

	// KICK
	const	std::string getUserToKick(const std::string &line) const;
	const	std::string getReason(const std::string &line) const;
	void	broadcastKickConfirmation(const std::string &channelName, const std::string &kicker, const std::string &victim, const std::string &reason);

	const	std::string getTopic(const std::string &line) const;
	void	sendTopic(const int &clientFd, const Channel &channel);
	void	broadcastNewTopic(const std::string &topic, const std::string &channelName, const int &clientFd, Channel &channel);

	std::vector<std::string>	parseJoinChannelName(const std::string &line);
	bool						channelExists(const std::string &channelName);
	void						createChannel(const std::string &channelName, int creatorFd);
	bool						joinExistingChannel(const std::string &channelName, const std::string &key, int userFd);
	void		broadcastToChannel(const std::string &channelName, const std::string &message, int senderFd);
	void		sendChannelError(const int &clientFd, const int code, const std::string &nick, const std::string &channel, const std::string &message) const;

	void		handlePrivateMessage(int clientFd, const std::string &line);
	void		sendPrivateMessage(const std::string &targetNick, const std::string &message, int senderFd);

	void 		execMode(int clientFd, const std::string &channelName, const std::string &mode, std::string arg);
	void 		setMode(int clientFd, const std::string &channelName, char mode, bool set_or_unset, std::string arg);
	char		extractFlag(const std::string &mode);

	void		notifyJoin(const std::string &channelName, int clientFd);
	void		broadcastToAllMember(Channel chanel, const std::string message);

	void	processToInvite(const int &clientFd, const std::string &toInvite, Channel &channel);

	void	notifyInvite(const int &clientFd, const std::string &toInvite, Channel &channel);
	void	notifyMode(const int &clientFd, const std::string &channelName, const char &mode, const bool status, const std::string &arg);
	void	notifyPart(const int &clientFd, const std::string &channelName);

	void	sendFile(const int &clientFd, const std::string &targetNick, t_dcc &dccData);
	void	getFile(const int &clientFd, t_dcc &dccData);
	t_dcc	getDCCInfo(const std::string &message);
	int		initDccSocket(t_dcc &dccData);
	int		openDccSocket(t_dcc &dccData);
	bool	hasAllDCCData(const t_dcc &dccData);
	void	notifyDCCsend(const int &clientFd, const std::string &targetNick, const t_dcc &dccData);

	void	sendRPL_CHANNELMODEIS(const int &clientFd, const Channel &channel);
	void	sendRPL_TOPICWHOTIME(const int &clientFd, const Channel &channel);
	void	sendRPL_TOPIC(const int &clientFd, const Channel &channel);
	void	sendRPL_NOTOPIC(const int &clientFd, const Channel &channel);
	void	sendRPL_INVITED(const int &clientFd, const std::string &toInvite, const Channel &channel);
	void	sendRPL_NAMEREPLY(const int &clientFd, Channel &channel);
	void	sendRPL_ENDOFNAMES(const int &clientFd, Channel &channel);

	// IrcReplies.hpp - Error and reply functions
	void sendNumericReply(const int &clientFd, int code, const std::string &params, const std::string &message);
	void sendERR_NOSUCHNICK(const int &clientFd, const std::string &nickname);
	void sendERR_NOSUCHSERVER(const int &clientFd, const std::string &servername);
	void sendERR_NOSUCHCHANNEL(const int &clientFd, const std::string &channel);
	void sendERR_CANNOTSENDTOCHAN(const int &clientFd, const std::string &channel);
	void sendERR_TOOMANYCHANNELS(const int &clientFd, const std::string &channel);
	void sendERR_WASNOSUCHNICK(const int &clientFd, const std::string &nickname);
	void sendERR_TOOMANYTARGETS(const int &clientFd, const std::string &target);
	void sendERR_NOORIGIN(const int &clientFd);
	void sendERR_NORECIPIENT(const int &clientFd, const std::string &command);
	void sendERR_NOTEXTTOSEND(const int &clientFd);
	void sendERR_NOTOPLEVEL(const int &clientFd, const std::string &mask);
	void sendERR_WILDTOPLEVEL(const int &clientFd, const std::string &mask);
	void sendERR_UNKNOWNCOMMAND(const int &clientFd, const std::string &command);
	void sendERR_NOMOTD(const int &clientFd);
	void sendERR_NOADMININFO(const int &clientFd, const std::string &server);
	void sendERR_FILEERROR(const int &clientFd, const std::string &fileop, const std::string &file);
	void sendERR_NONICKNAMEGIVEN(const int &clientFd);
	void sendERR_ERRONEUSNICKNAME(const int &clientFd, const std::string &nick);
	void sendERR_NICKNAMEINUSE(const int &clientFd, const std::string &nick);
	void sendERR_NICKCOLLISION(const int &clientFd, const std::string &nick);
	void sendERR_USERNOTINCHANNEL(const int &clientFd, const std::string &nick, const std::string &channel);
	void sendERR_NOTONCHANNEL(const int &clientFd, const std::string &channel);
	void sendERR_USERONCHANNEL(const int &clientFd, const std::string &user, const std::string &channel);
	void sendERR_NOLOGIN(const int &clientFd, const std::string &user);
	void sendERR_SUMMONDISABLED(const int &clientFd);
	void sendERR_USERSDISABLED(const int &clientFd);
	void sendERR_NOTREGISTERED(const int &clientFd);
	void sendERR_ALREADYREGISTRED(const int &clientFd);
	void sendERR_PASSWDMISMATCH(const int &clientFd);
	void sendERR_YOUREBANNEDCREEP(const int &clientFd);
	void sendERR_KEYSET(const int &clientFd, const std::string &channel);
	void sendERR_CHANNELISFULL(const int &clientFd, const std::string &channel);
	void sendERR_UNKNOWNMODE(const int &clientFd, char c);
	void sendERR_INVITEONLYCHAN(const int &clientFd, const std::string &channel);
	void sendERR_BANNEDFROMCHAN(const int &clientFd, const std::string &channel);
	void sendERR_BADCHANNELKEY(const int &clientFd, const std::string &channel);
	void sendERR_NOPRIVILEGES(const int &clientFd);
	void sendERR_CHANOPRIVSNEEDED(const int &clientFd, const std::string &channel);
	void sendERR_CANTKILLSERVER(const int &clientFd);
	void sendERR_NOOPERHOST(const int &clientFd);
	void sendERR_UMODEUNKNOWNFLAG(const int &clientFd);
	void sendERR_USERSDONTMATCH(const int &clientFd);
	void sendRPL_NONE(const int &clientFd);
	void sendRPL_AWAY(const int &clientFd, const std::string &nick, const std::string &message);
	void sendRPL_USERHOST(const int &clientFd, const std::string &reply);
	void sendRPL_ISON(const int &clientFd, const std::string &nicks);
	void sendRPL_UNAWAY(const int &clientFd);
	void sendRPL_NOWAWAY(const int &clientFd);
	void sendRPL_WHOISUSER(const int &clientFd, const std::string &nick, const std::string &user, const std::string &host, const std::string &realname);
	void sendRPL_WHOISSERVER(const int &clientFd, const std::string &nick, const std::string &server, const std::string &serverinfo);
	void sendRPL_WHOISOPERATOR(const int &clientFd, const std::string &nick);
	void sendRPL_WHOISIDLE(const int &clientFd, const std::string &nick, int idle);
	void sendRPL_ENDOFWHOIS(const int &clientFd, const std::string &nick);
	void sendRPL_WHOISCHANNELS(const int &clientFd, const std::string &nick, const std::string &channels);
	void sendRPL_LISTSTART(const int &clientFd);
	void sendRPL_LIST(const int &clientFd, const std::string &channel, int visible, const std::string &topic);
	void sendRPL_LISTEND(const int &clientFd);
	void sendRPL_CHANNELMODEIS(const int &clientFd, const std::string &channel, const std::string &mode, const std::string &mode_params);
	void sendRPL_NOTOPIC(const int &clientFd, const std::string &channel);
	void sendRPL_VERSION(const int &clientFd, const std::string &version, const std::string &debuglevel, const std::string &server, const std::string &comments);
	void sendRPL_TIME(const int &clientFd, const std::string &server, const std::string &timestr);
	
	void sendERR_NEEDMOREPARAMS(const int &clientFd, const std::string &command);
	void sendERR_BADCHANMASK(const int &clientFd, const std::string &channel);
	void sendRPL_YOUREOPER(const int &clientFd);
	void sendRPL_REHASHING(const int &clientFd);
	void sendError(const int &clientFd, const std::string &message);

	// AWAY command
	std::string parseAwayMessage(const std::string &line);
	void handleAway(const int &clientFd, const std::string &line);
	
	// NOTICE command
	void handleNotice(const int &clientFd, const std::string &line);
	
	// PING/PONG commands
	std::string parsePingToken(const std::string &line);
	void sendPong(const int &clientFd, const std::string &token);
	void handlePong(const int &clientFd, const std::string &line);
};