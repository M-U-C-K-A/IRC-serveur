/* ************************************************************************** */
/*                                                                            */
/*                                                        :::      ::::::::   */
/*   main.cpp                                           :+:      :+:    :+:   */
/*                                                    +:+ +:+         +:+     */
/*   By: hdelacou <hdelacou@student.42.fr>          +#+  +:+       +#+        */
/*                                                +#+#+#+#+#+   +#+           */
/*   Created: 2025/12/16 04:10:00 by hdelacou          #+#    #+#             */
/*   Updated: 2025/12/16 06:17:42 by hdelacou         ###   ########.fr       */
/*                                                                            */
/* ************************************************************************** */

/*
** ============================================================================
**                          IRC SERVER ENTRY POINT
** ============================================================================
**
**  Usage: ./ircserv <port> <password>
**
**  Startup: Validate args → Setup signals → initServer() → runServer()
**  Signals: SIGINT/SIGQUIT handled for graceful shutdown
**
** ============================================================================
*/

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
