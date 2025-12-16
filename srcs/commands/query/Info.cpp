#include "../../../includes/Server.hpp"
#include "../../../includes/Utils.hpp"

/*
* this fonction will handle the INFO command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleInfo(const int &clientFd, const std::string &line) {
	(void)clientFd;
	(void)line;
}
