/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   User.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:10:00 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:21:47 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                              USER AUTHENTICATION
** ============================================================================
**
**     PASS → NICK → USER → [All set?] → isRegister = true → Welcome!
**
**  Registration requires 3 flags: hasPass, hasNickname, hasUsername
**  tryRegisterUser() validates all flags before marking user as registered
**
** ============================================================================
*/

#include "../includes/User.hpp"

/*
 * Default constructor for User class
 * Initializes all member variables to default values
 */
User::User() : nickname(""), username(""), fd(-1),
			   hasNickname(false), hasUsername(false), hasPass(false), isRegister(false), welcomeMessage(false) {}

/*
 * Copy constructor for User class
 * @param src the User object to copy from
 * @return void
 */
User::User(const User &src)
{
	*this = src;
}

/*
 * Assignment operator for User class
 * @param src the User object to copy from
 * @return reference to this User object
 */
User &User::operator=(const User &src)
{
	if (this == &src)
		return (*this);
	this->nickname = src.nickname;
	this->username = src.username;
	this->fd = src.fd;
	this->hasNickname = src.hasNickname;
	this->hasUsername = src.hasUsername;
	this->hasPass = src.hasPass;
	this->isRegister = src.isRegister;
	this->welcomeMessage = src.welcomeMessage;
	return (*this);
}

/*
 * Destructor for User class
 * Closes the user's connection if it is valid
 */
User::~User()
{
	closeConnection();
}

/*
 * Parameterized constructor for User class
 * @param nickname the user's nickname
 * @param username the user's username
 * @return void
 */
User::User(const std::string &nickname, const std::string &username) : nickname(nickname), username(username), fd(-1), buffer(""),
																	   hasNickname(false), hasUsername(false), hasPass(false), isRegister(false), welcomeMessage(false) {}

/*
 * Set the file descriptor for the user
 * @param fd the file descriptor to set
 * @return void
 */
void User::setFd(const int &fd)
{
	this->fd = fd;
}

/*
 * Set the nickname for the user
 * @param nickname the nickname to set
 * @return void
 */
void User::setNickname(const std::string &nickname)
{
	this->nickname = nickname;
}

/*
 * Set the username for the user
 * @param username the username to set
 * @return void
 */
void User::setUsername(const std::string &username)
{
	this->username = username;
}

/*
 * Close the user's connection
 * Closes the file descriptor if it is valid
 * @return void
 */
void User::closeConnection()
{
	if (this->fd > 0)
	{
		close(this->fd);
	}
}

/*
 * Try to register the user
 * User is registered if they have nickname, username, and password
 * @return void
 */
void User::tryRegisterUser()
{
	if (getHasNickname() && getHasUsername() && getHasPass())
	{
		this->isRegister = true;
	}
}
