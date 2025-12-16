/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Channel.cpp                                        :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 05:19:55 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:16:54 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                           CHANNEL MODES & PERMISSIONS
** ============================================================================
**
**  Modes: +i (invite-only) | +t (topic-op-only) | +k (key) | +l (limit)
**
**  canJoin() checks: invited? → key match? → under limit?
**  First member becomes host & operator
**  Operators control: topic (+t), MODE changes, KICK, INVITE
**
** ============================================================================
*/

#include "../includes/Channel.hpp"

/*
 * Default constructor for Channel class
 * Initializes all member variables to default values
 */
Channel::Channel() {
	this->invite_only = false;
	this->topic_op_only = false;
	this->has_key = false;
	this->user_limit = 0;
}

/*
 * Parameterized constructor for Channel class
 * @param name the channel name
 * @param creator the channel creator
 * @return void
 */
Channel::Channel(const std::string &name, int creator) {
	this->name = name;
	this->invite_only = false;
	this->topic_op_only = false;
	this->has_key = false;
	this->user_limit = 0;
	this->host = creator;
	this->users.insert(creator);
	this->operators.insert(creator);
}

/*
 * Copy constructor for Channel class
 * @param src the source Channel object
 * @return void
 */
Channel::Channel(const Channel &src) {
	*this = src;
}

/*
 * Copy assignment operator for Channel class
 * @param src the source Channel object
 * @return reference to the assigned Channel object
 */
Channel &Channel::operator=(const Channel &src) {
	if (this == &src)
		return (*this);

	this->name = src.name;
	this->topic = src.topic;
	this->host = src.host;
	this->key = src.key;
	this->invite_only = src.invite_only;
	this->topic_op_only = src.topic_op_only;
	this->has_key = src.has_key;
	this->user_limit = src.user_limit;
	this->operators = src.operators;
	this->users = src.users;
	this->invited = src.invited;
	this->topicSetter = src.topicSetter;
	this->topicTimeSet = src.topicTimeSet;

	return (*this);
}

/*
 * Destructor for Channel class
 * @return void
 */
Channel::~Channel() {
	this->users.clear();
	this->operators.clear();
	this->invited.clear();
}

/*
 * Checks if a client can join the channel
 * @param fd the client file descriptor
 * @param key the channel key
 * @param reason the reason for the join failure
 * @return true if the client can join, false otherwise
 */
bool Channel::canJoin(int fd, const std::string &key, std::string &reason) const {
	if (this->invite_only && this->invited.find(fd) == this->invited.end()) {
		reason = "You cannot join channel because you have not been invited";
		return (false);
	}

	if (this->has_key && key != this->key) {
		reason = "You cannot join channel because the password is incorrect";
		return (false);
	}

	if (this->user_limit > 0 && (int)this->users.size() >= this->user_limit) {
		reason = "You cannot join channel because it has reach its limit of users";
		return (false);
	}

	return (true);
}

/*
 * Adds a client to the channel
 * @param fd the client file descriptor
 * @return true if the client was added, false otherwise
 */
bool Channel::addMember(int fd) {
	return this->users.insert(fd).second;
}

/*
 * Removes a client from the channel
 * @param fd the client file descriptor
 * @return true if the client was removed, false otherwise
 */
bool Channel::removeMember(int fd) {
	this->operators.erase(fd);
	this->invited.erase(fd);

	return this->users.erase(fd) > 0;
}

/*
 * Checks if a client is a member of the channel
 * @param fd the client file descriptor
 * @return true if the client is a member, false otherwise
 */
bool Channel::isMember(int fd) const {
	return this->users.find(fd) != this->users.end();
}

/*
 * Checks if the channel is empty
 * @return true if the channel is empty, false otherwise
 */
bool Channel::isEmpty() const {
	return this->users.empty();
}

/*
 * Checks if a client is an operator of the channel
 * @param fd the client file descriptor
 * @return true if the client is an operator, false otherwise
 */
bool Channel::isOperator(int fd) const {
	return this->operators.find(fd) != this->operators.end();
}

/*
 * Adds a client to the channel operators
 * @param fd the client file descriptor
 * @return true if the client was added, false otherwise
 */
bool Channel::addOperator(int fd) {
	if (!this->isMember(fd))
		return (false);

	return this->operators.insert(fd).second;
}

/*
 * Removes a client from the channel operators
 * @param fd the client file descriptor
 * @return true if the client was removed, false otherwise
 */
bool Channel::removeOperator(int fd) {
	return this->operators.erase(fd) > 0;
}

/*
 * Sets the channel invite-only mode
 * @param on true to enable invite-only mode, false to disable it
 */
void Channel::setInviteOnly(bool on) {
	this->invite_only = on;
}

/*
 * Sets the channel topic-op-only mode
 * @param on true to enable topic-op-only mode, false to disable it
 */
void Channel::setTopicOpOnly(bool on) {
	this->topic_op_only = on;
}

/*
 * Sets the channel key
 * @param key the channel key
 */
void Channel::setKey(const std::string &key) {
	this->key = key;
	this->has_key = true;
}	

/*
 * Clears the channel key
 */
void Channel::clearKey() {
	this->key.clear();
	this->has_key = false;
}

/*
 * Sets the channel user limit
 * @param limit the channel user limit
 */
void Channel::setUserLimit(int limit) {
	this->user_limit = limit;
}

/*
 * Resets the channel user limit
 */
void Channel::resetUserLimit() {
	this->user_limit = 0;
}

/*
 * Sets the channel topic
 * @param user the client file descriptor
 * @param topic the channel topic
 * @param setterName the name of the client who set the topic
 * @return true if the topic was set, false otherwise
 */
bool Channel::setTopic(int user, const std::string &topic, const std::string &setterName) {
	if (this->topic_op_only && this->operators.find(user) == this->operators.end()) {
		return (false);
	}

	time(&this->topicTimeSet);
	this->topicSetter = setterName;
	this->topic = topic;
	return (true);
}

/*
 * Gets the channel topic
 * @return the channel topic
 */
const std::string &Channel::getTopic() const {
	return this->topic;
}

/*
 * Invites a client to the channel
 * @param fd the client file descriptor
 */
void Channel::invite(int fd) {
	this->invited.insert(fd);
}

/*
 * Checks if a client is invited to the channel
 * @param fd the client file descriptor
 * @return true if the client is invited, false otherwise
 */
bool Channel::isInvited(int fd) const {
	return this->invited.find(fd) != this->invited.end();
}

/*
 * Clears the invitation of a client from the channel
 * @param fd the client file descriptor
 */
void Channel::clearInvite(int fd) {
	this->invited.erase(fd);
}

/*
 * Gets the channel name
 * @return the channel name
 */
const std::string &Channel::getName() const {
	return this->name;
}

/*
 * Gets the channel host
 * @return the channel host
 */
const int &Channel::getHost() const { return (this->host); }

std::vector<int> Channel::getAllMembers() const {
	std::vector<int> vector;

	for (std::set<int>::const_iterator it = this->users.begin(); it != this->users.end(); ++it)
		vector.push_back(*it);

	return vector;
}


/*
 * Gets the channel invite-only mode
 * @return true if the channel is invite-only, false otherwise
 */
bool Channel::getInviteOnly() const {
	return this->invite_only;
}

/*
 * Gets the channel topic-op-only mode
 * @return true if the channel is topic-op-only, false otherwise
 */
bool Channel::getTopicOpOnly() const {
	return this->topic_op_only;
}

/*
 * Gets the channel key
 * @return true if the channel has a key, false otherwise
 */
bool Channel::getHasKey() const {
	return this->has_key;
}

/*
 * Gets the channel user limit
 * @return the channel user limit
 */
int Channel::getUserLimit() const {
	return this->user_limit;
}

/*
 * Gets the channel key
 * @return the channel key
 */
const std::string &Channel::getKey() const {
	return this->key;
}

/*
 * Checks if a client is the host of the channel
 * @param clientFd the client file descriptor
 * @return true if the client is the host, false otherwise
 */
bool Channel::isHost(const int &clientFd) const {
	if (this->host == clientFd) {
		return (true);
	}
	return (false);
}

/*
 * Checks if a client has permission to perform an action on the channel
 * @param clientFd the client file descriptor
 * @return true if the client has permission, false otherwise
 */
bool Channel::hasPerm(const int &clientFd) const {
	for (std::set<int>::iterator it = operators.begin(); it != operators.end(); ++it) {
		if (*it == clientFd) {
			return (true);
		}
	}
	return (false);
}

