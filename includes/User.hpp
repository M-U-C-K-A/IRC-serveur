#pragma once

#include <iostream>
#include <unistd.h>

class User
{
private:
	std::string	nickname;
	std::string	username;
	int			fd;
	std::string	ip;

	std::string	buffer;
	bool		hasNickname;
	bool		hasUsername;
	bool		hasPass;
	bool		isRegister;

	bool		welcomeMessage;
public:
	User();
	User(const User &src);
	User &operator=(const User &src);
	~User();

	User(const std::string &nickname, const std::string &username);
	void setFd(const int &fd);
	void setNickname(const std::string &nickname);
	void setUsername(const std::string &username);

	void closeConnection();
	const std::string &getNickname() const {return (nickname);};
	const std::string &getUsername() const {return (username);};
	const std::string &getIp() const {return (ip);};
	const std::string &getBuffer() const {return (buffer);};
	std::string &getBufferRef() {return (buffer);};
	void addToBuffer(const std::string &toAdd) {this->buffer += toAdd;};
	void clearBuffer() {this->buffer = "";};
	const int &getFd() const {return (fd);};
	const bool &getIsRegister() const {return (isRegister);};
	void setHasNickname (const bool boolean) {this->hasNickname = boolean;};
	void setHasUsername() {this->hasUsername = true;};
	void setHasPass() {this->hasPass = true;};
	void setHasRegister(const bool boolean) {this->isRegister = boolean;};
	void setIp(const std::string &ip) {this->ip = ip;};
	const bool &getHasNickname() {return (this->hasNickname);};
	const bool &getHasUsername() {return (this->hasUsername);};
	const bool &getHasPass() {return (this->hasPass);};
	const bool &getWelcomeMessage() {return (this->welcomeMessage);};
	void tryRegisterUser();
	void hasWelcomeMessage() {this->welcomeMessage = true;};
};
