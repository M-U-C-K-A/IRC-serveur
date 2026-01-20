/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   Mode.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: adrien <adrien@student.42.fr>              +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 02:39:09 by hdelacou          #+#    #+#             */
/*   Updated: 2026/01/21 00:51:49 by adrien           ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"
#include <sstream>
#include <cstdlib>

/*
* this fonction will handle the MODE command
* Format: MODE <channel> [<modes> [<mode params>]]
* Modes: i (invite-only), t (topic-op-only), k (key), o (operator), l (limit)
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleMode(const int &clientFd, const std::string &line) {
	// Parse: MODE <target> [<modes> [<params>]]
	size_t spacePos = line.find(' ');
	if (spacePos == std::string::npos) {
		sendERR_NEEDMOREPARAMS(clientFd, "MODE");
		return;
	}

	std::string params = line.substr(spacePos + 1);

	// Get target (channel or user)
	size_t targetEnd = params.find(' ');
	std::string target;
	std::string modeStr;
	std::string modeArgs;

	if (targetEnd != std::string::npos) {
		target = params.substr(0, targetEnd);
		std::string rest = params.substr(targetEnd + 1);

		size_t modeEnd = rest.find(' ');
		if (modeEnd != std::string::npos) {
			modeStr = rest.substr(0, modeEnd);
			modeArgs = rest.substr(modeEnd + 1);
		} else {
			modeStr = rest;
		}
	} else {
		target = params;
	}

	// Remove trailing \r\n
	size_t endPos = target.find_first_of("\r\n");
	if (endPos != std::string::npos)
		target = target.substr(0, endPos);
	endPos = modeStr.find_first_of("\r\n");
	if (endPos != std::string::npos)
		modeStr = modeStr.substr(0, endPos);
	endPos = modeArgs.find_first_of("\r\n");
	if (endPos != std::string::npos)
		modeArgs = modeArgs.substr(0, endPos);

	// Check if target is a channel
	if (target[0] != '#' && target[0] != '&') {
		// User mode - not fully implemented
		return;
	}

	// Find channel
	for (std::vector<Channel>::iterator it = channelList.begin(); it != channelList.end(); ++it) {
		if (it->getName() == target) {
			// If no mode specified, return current modes
			if (modeStr.empty()) {
				std::string modes = "+";
				std::string modeParams = "";

				if (it->getInviteOnly()) modes += "i";
				if (it->getTopicOpOnly()) modes += "t";
				if (it->getHasKey()) {
					modes += "k";
					modeParams += " " + it->getKey();
				}
				if (it->getUserLimit() > 0) {
					modes += "l";
					std::ostringstream oss;
					oss << it->getUserLimit();
					modeParams += " " + oss.str();
				}

				std::string reply = ":" + std::string(SERVER_NAME) + " 324 " +
				                    Users[clientFd].getNickname() + " " + target + " " +
				                    modes + modeParams + IRC_CRLF;
				send(clientFd, reply.c_str(), reply.length(), 0);
				return;
			}

			// Check if user is on channel
			if (!it->isMember(clientFd)) {
				sendERR_NOTONCHANNEL(clientFd, target);
				return;
			}

			// Check if user is operator
			if (!it->isOperator(clientFd)) {
				sendERR_CHANOPRIVSNEEDED(clientFd, target);
				return;
			}

			// Parse modes
			bool adding = true;
			std::string appliedModes = "";
			std::string appliedParams = "";
			size_t argIndex = 0;

			// Split modeArgs into vector
			std::vector<std::string> args;
			std::istringstream iss(modeArgs);
			std::string arg;
			while (iss >> arg) {
				args.push_back(arg);
			}

			for (size_t i = 0; i < modeStr.length(); i++) {
				char c = modeStr[i];

				if (c == '+') {
					adding = true;
					appliedModes += "+";
				} else if (c == '-') {
					adding = false;
					appliedModes += "-";
				} else if (c == 'i') {
					it->setInviteOnly(adding);
					appliedModes += "i";
				} else if (c == 't') {
					it->setTopicOpOnly(adding);
					appliedModes += "t";
				} else if (c == 'k') {
					if (adding) {
						if (argIndex < args.size()) {
							it->setKey(args[argIndex]);
							appliedModes += "k";
							appliedParams += " " + args[argIndex];
							argIndex++;
						} else {
							sendERR_NEEDMOREPARAMS(clientFd, "MODE");
						}
					} else {
						it->clearKey();
						appliedModes += "k";
					}
				} else if (c == 'o') {
					if (argIndex < args.size()) {
						std::string nickArg = args[argIndex];
						argIndex++;

						// Find user by nick
						int targetFd = -1;
						for (std::map<int, User>::iterator uit = Users.begin(); uit != Users.end(); ++uit) {
							if (uit->second.getNickname() == nickArg) {
								targetFd = uit->first;
								break;
							}
						}

						if (targetFd == -1) {
							sendERR_NOSUCHNICK(clientFd, nickArg);
							continue;
						}

						if (!it->isMember(targetFd)) {
							sendERR_USERNOTINCHANNEL(clientFd, nickArg, target);
							continue;
						}

						if (adding) {
							it->addOperator(targetFd);
						} else {
							it->removeOperator(targetFd);
						}
						appliedModes += "o";
						appliedParams += " " + nickArg;
					} else {
						sendERR_NEEDMOREPARAMS(clientFd, "MODE");
					}
				} else if (c == 'l') {
					if (adding) {
						if (argIndex < args.size()) {
							int limit = std::atoi(args[argIndex].c_str());
							if (limit > 0) {
								it->setUserLimit(limit);
								appliedModes += "l";
								appliedParams += " " + args[argIndex];
							}
							argIndex++;
						} else {
							sendERR_NEEDMOREPARAMS(clientFd, "MODE");
						}
					} else {
						it->resetUserLimit();
						appliedModes += "l";
					}
				} else {
					sendERR_UNKNOWNMODE(clientFd, c);
				}
			}

			// Broadcast mode change if any modes were applied
			if (!appliedModes.empty() && appliedModes != "+" && appliedModes != "-") {
				std::string modeMsg = ":" + Users[clientFd].getNickname() + "!" +
				                      Users[clientFd].getUsername() + "@localhost MODE " +
				                      target + " " + appliedModes + appliedParams + IRC_CRLF;
				std::vector<int> members = it->getAllMembers();
				for (size_t m = 0; m < members.size(); m++) {
					send(members[m], modeMsg.c_str(), modeMsg.length(), 0);
				}
			}
			return;
		}
	}

	sendERR_NOSUCHCHANNEL(clientFd, target);
}

/*
** ============================================================================
**                           MODE COMMAND
** ============================================================================
**
**  Format: MODE <channel> <modes> [params]
**
**  Action: Apply/Remove channel modes (+i, +t, +k, +l, +o).
**  Checks: Operator privileges required for most changes.
**
** ============================================================================
*/
