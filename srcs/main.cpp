/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:10:00 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 05:22:45 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

#include <iostream>
#include <cstdlib>
#include <signal.h>
#include "../includes/Server.hpp"
#include "../includes/Utils.hpp"

#define PORT 1
#define PASSWORD 2

/*
 * Setup signal handlers for graceful shutdown
 * Handles SIGINT (Ctrl+C) and SIGQUIT signals
 */
void setupSignal() {
	signal(SIGINT, Server::signalHandler);
	signal(SIGQUIT, Server::signalHandler);
}

/*
 * Validate command line arguments
 * @param port the port number to validate
 * @param password the password to validate
 * @return true if arguments are valid, false otherwise
 */
bool checkArgs(const int port, const std::string password) {
	if (port < 1024 || port > 65535) {
		std::cerr << "Port needs to be between 1024 and 65535" << std::endl;
		return (false);
	}
	if (password.length() > 18) {
		std::cerr << "Password too long" << std::endl;
		return (false);
	}
	return (true);
}

/*
 * Main entry point for IRC server
 * @param ac argument count
 * @param av argument vector
 * @return EXIT_SUCCESS on success, EXIT_FAILURE on error
 */
int main(int ac, char **av) {

	if (ac != 3 || !checkArgs(std::atoi(av[PORT]), av[PASSWORD])) {
		std::cerr << "usage: ./ircserv <port> <password>" << std::endl;
		return (EXIT_FAILURE);
	}
	try {
		Server serv;
		setupSignal();
		serv.initServer(std::atoi(av[PORT]), av[PASSWORD]);
		serv.runServer();
	}
	catch(const std::exception& e) {
		std::cerr << e.what() << '\n';
		Server::running = false;
	}
	return (EXIT_SUCCESS);
}

/*
** ============================================================================
**                         IRC SERVER - MAIN ENTRY POINT
** ============================================================================
**
**                       Server Startup Flow
**                              |
**                              v
**                    +-------------------+
**                    | Validate Args     |
**                    | - Port range      |
**                    | - Password length |
**                    +-------------------+
**                              |
**                       Valid  |  Invalid
**                              v
**                    +-------------------+
**                    | Setup Signals     |
**                    | - SIGINT          |
**                    | - SIGQUIT         |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | Create Server     |
**                    | Instance          |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | initServer()      |
**                    | - Create socket   |
**                    | - Setup epoll     |
**                    +-------------------+
**                              |
**                              v
**                    +-------------------+
**                    | runServer()       |
**                    | - Event loop      |
**                    | - Handle clients  |
**                    +-------------------+
**                              |
**                    Signal received or error
**                              |
**                              v
**                    +-------------------+
**                    | Graceful Shutdown |
**                    +-------------------+
**
**  COMMAND LINE USAGE:
**  ./ircserv <port> <password>
**
**  PARAMETERS:
**  - port: Port number (must be between 1024 and 65535)
**  - password: Server password (max 18 characters)
**
**  EXAMPLE:
**  ./ircserv 6667 mySecretPassword
**
**  SIGNAL HANDLING:
**  - SIGINT (Ctrl+C): Graceful shutdown
**  - SIGQUIT: Graceful shutdown
**
** ============================================================================
*/