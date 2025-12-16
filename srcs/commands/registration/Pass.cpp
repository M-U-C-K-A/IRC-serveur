/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Pass.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:08:42 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 05:28:50 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include "../../../includes/IrcReplies.hpp"

/*
* this fonction will handle the PASS command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handlePass(const int &clientFd, const std::string &line) {
	// Check if already registered
	if (this->Users[clientFd].getIsRegister()) {
		sendERR_ALREADYREGISTRED(clientFd);
		return;
	}

	// Extract password from command
	std::string password = line.substr(5); // Skip "PASS "
	if (!password.empty() && password[0] == ':')
		password.erase(0, 1);

	// Validate password
	if (password.empty() || password != this->password) {
		sendERR_PASSWDMISMATCH(clientFd);
		return;
	}

	// Password is correct - mark as having valid password
	this->Users[clientFd].setHasPass();
	std::cout << "Password set for user " << clientFd << std::endl;
}

/*
** ============================================================================
**                         PASS COMMAND - RFC 1459
** ============================================================================
**
**                       Connection Flow
**                              |
**                              v
**                    +-------------------+
**                    |   Client Connect  |
**                    +-------------------+
**                              |
**                              | PASS <password>
**                              v
**                    +-------------------+
**                    |  Validate Format  |
**                    +-------------------+
**                         /         \
**                   Valid           Invalid
**                     |                |
**                     v                v
**            +---------------+  ERR_NEEDMOREPARAMS (461)
**            | Store Password|  "Not enough parameters"
**            +---------------+
**                     |
**                     v
**            +---------------+
**            | Wait for NICK |
**            | and USER cmds |
**            +---------------+
**                     |
**                     v
**            +---------------+
**            | Registration  |
**            | Complete      |
**            +---------------+
**
**  IMPORTANT NOTES:
**  - PASS must be sent BEFORE NICK and USER
**  - If sent after registration, ERR_ALREADYREGISTRED (462) is returned
**  - Password is stored but validated during full registration
**  - Empty password generates ERR_NEEDMOREPARAMS
**
**  Format: PASS <password>
**  Example: PASS secretpassword123
**
** ============================================================================
*/
