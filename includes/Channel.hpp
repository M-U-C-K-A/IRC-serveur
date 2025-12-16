#pragma once

#include <iostream>
#include <string>
#include <set>
#include <vector>
#include <ctime>

class Channel
{
private:
	std::string		name;
	std::string		key;
	bool			invite_only;
	bool			topic_op_only;
	bool			has_key;
	int				user_limit;

	std::string		topic;
	std::string		topicSetter;
	time_t			topicTimeSet;

	int				host;
	std::set<int>	operators;
	std::set<int>	users;
	std::set<int>	invited;

public:
	Channel();
	Channel(const std::string &name, int creatorFd);
	Channel(const Channel &src);
	Channel &operator=(const Channel &src);
	~Channel();



	bool	canJoin(int fd, const std::string &key, std::string &reason) const;
	bool	addMember(int fd);
	bool	removeMember(int fd);
	bool	isMember(int fd) const;
	bool	isEmpty() const;
	bool	hasPerm(const int & clientFd) const;
	bool	isHost(const int &clientFd) const;

	bool	isOperator(int fd) const;
	bool	addOperator(int fd);
	bool	removeOperator(int fd);

	void	setInviteOnly(bool on);
	void	setTopicOpOnly(bool on);
	void	setKey(const std::string &key);
	void	clearKey();
	void	setUserLimit(int limit);
	void	resetUserLimit();

	bool	setTopic(int byFd, const std::string &topic, const std::string &setterName);
	const std::string &getTopic() const;

	void	invite(int fd);
	bool	isInvited(int fd) const;
	void	clearInvite(int fd);

	const std::string	&getName() const;
	std::vector<int>	getAllMembers() const;
	const int 			&getHost() const;

	bool	getInviteOnly() const;
	bool	getTopicOpOnly() const;
	bool	getHasKey() const;
	int		getUserLimit() const;
	const std::string &getKey() const;

	const std::string &getTopicSetter() const {return(this->topicSetter);};
	const time_t &getTopicTime() const {return(this->topicTimeSet);};
};