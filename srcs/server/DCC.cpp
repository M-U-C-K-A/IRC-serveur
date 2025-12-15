/*
** ============================================================================
**                         DCC (Direct Client Connection)
** ============================================================================
**
**      Sender                 Server                    Receiver
**        |                       |                          |
**        | PRIVMSG user :DCC     |                          |
**        | SEND file ip port size|                          |
**        |---------------------> |                          |
**        |                       |                          |
**        |                       | :sender PRIVMSG receiver |
**        |                       |---------------------->   |
**        |                       |                          |
**        |                       |                          | Parse DCC info:
**        |                       |                          | - filename
**        |                       |                          | - IP address
**        |                       |                          | - port
**        |                       |                          | - file size
**        |                       |                          |
**        |                       |      DCC ACCEPT          |
**        |                       |<----------------------   |
**        |                       |                          |
**        |   Direct TCP Connection Established              |
**        |<===============================================> |
**        |                                                  |
**        |            File Transfer (binary)                |
**        |=================================================>|
**        |                                                  |
**
**                          📁  File Transfer  📁
**
**  Flow: DCC request -> Connection info exchanged -> Direct transfer
**
** ============================================================================
*/

#include "../../includes/Server.hpp"
#include "../../includes/Utils.hpp"
#include <fstream>

t_dcc Server::getDCCInfo(const std::string &message) {
	std::istringstream iss(message);
	t_dcc	dccData;

	iss >> dccData.dcc >> dccData.mode >> dccData.filename >> dccData.ip >> dccData.port >> dccData.fileSize;

	return(dccData);
}

bool Server::hasAllDCCData(const t_dcc &dccData) {
	if (dccData.dcc.empty() || dccData.mode.empty() || dccData.filename.empty() || dccData.ip.empty() ||
		dccData.port.empty() || dccData.fileSize.empty()) {
		return(false);
	}

	return (true);
}

void Server::handleDCC(const int &clientFd, const std::string &targetNick, const std::string &message) {
	t_dcc dccData = getDCCInfo(message);

	if (!this->hasAllDCCData(dccData)) {
		sendERR_NEEDMOREPARAMS(clientFd, CMD_DCC);
		return ;
	}

	if (dccData.mode == DCC_MODE_SEND) {
		sendFile(clientFd, targetNick, dccData);
	} else if (dccData.mode == DCC_MODE_GET) {
		getFile(clientFd, dccData);
	}
}

void Server::sendFile(const int &clientFd, const std::string &targetNick, t_dcc &dccData) {
	int dccsocket = initDccSocket(dccData);
	if (dccsocket < 0) return;

	notifyDCCsend(clientFd, targetNick, dccData);

	int clientDcc = accept(dccsocket, NULL, NULL);
	if (clientDcc < 0) {
		std::cerr << "Failed to accept" << std::endl;
		return;
	}

	std::string toOpen =  "/home/" + this->Users[clientFd].getUsername() + "/" + dccData.filename;
	std::ifstream file(toOpen.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Error: cannot open " << dccData.filename << std::endl;
		close(clientDcc);
		close(dccsocket);
		return;
	}

	char buffer[BUFFER_SIZE];
	while (file.good()) {
		file.read(buffer, sizeof(buffer));
		std::streamsize bytes = file.gcount();
		if (bytes > 0) {
			send(clientDcc, buffer, bytes, 0);
		}
	}

	file.close();
	close(clientDcc);
	close(dccsocket);
}


void Server::getFile(const int &clientFd, t_dcc &dccData) {
	(void)clientFd;
	int dccSocket = openDccSocket(dccData);
	if (dccSocket < 0) {
		return ;
	}

	std::string toOpen =  "/home/" + this->Users[clientFd].getUsername() + "/" + dccData.filename;
	std::ofstream file(toOpen.c_str(), std::ios::binary);
	if (!file.is_open()) {
		std::cerr << "Failed to open the file" << std::endl;
		close(dccSocket);
		return ;
	}

	char buffer[BUFFER_SIZE];
	int total = 0;
	int size = atoi(dccData.fileSize.c_str());
	while (total < size) {
		size_t len = read(dccSocket, buffer, BUFFER_SIZE);
		if (len <= 0) {
			break ;
		}
		file.write(buffer, len);
		total += len;
	}
	file.close();
	close(dccSocket);
}

int Server::openDccSocket(t_dcc &dccData) {
	int dccfd = socket(AF_INET, SOCK_STREAM, 0);
	if (dccfd < 0) {
		std::cerr << "Error while setting option for socket" << std::endl;
		return(-1);
	}

	struct sockaddr_in DccAdress;
	DccAdress.sin_family = AF_INET;
	DccAdress.sin_addr.s_addr = inet_addr(dccData.ip.c_str());
	DccAdress.sin_port = htons(std::atoi(dccData.port.c_str()));

	if (connect(dccfd, (struct sockaddr*)&DccAdress, sizeof(DccAdress)) < 0) {
		close(dccfd);
		std::cerr << "Error while connection to dcc socket" << std::endl;
		return(-1);
	}

	return (dccfd);
}

int Server::initDccSocket(t_dcc &dccData)
{
	int opt = 1;
	int dccfd = socket(AF_INET, SOCK_STREAM, 0);
	if (dccfd < 0) {
		std::cerr << "Error while setting option for socket" << std::endl;
		return(-1);
	}

	struct sockaddr_in DccAdress;
	DccAdress.sin_family = AF_INET;
	DccAdress.sin_addr.s_addr = htonl(INADDR_ANY);
	DccAdress.sin_port = htons(0);

	if (setsockopt(dccfd, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt)) < 0) {
		close(dccfd);
		std::cerr << "Error while setting option for socket" << std::endl;
		return(-1);
	}

	if (bind(dccfd, (struct sockaddr *) &DccAdress, sizeof(DccAdress)) < 0) {
		close(dccfd);
		std::cerr << "Error while binding address" << std::endl;
		return(-1);
	}

	socklen_t len = sizeof(DccAdress);
	if (getsockname(dccfd, (struct sockaddr*)&DccAdress, &len) == -1) {
		close(dccfd);
		std::cerr << "Error while getting port" << std::endl;
		return(-1);
	}

	dccData.realPort = ntohs(DccAdress.sin_port);
	if (listen(dccfd, MAX_USER) < 0) {
		close(dccfd);
		std::cerr << "Error while binding address" << std::endl;
		return(-1);
	}

	return (dccfd);
}

unsigned long ipToDecimal(const std::string &ip) {
	struct in_addr addr;
	inet_aton(ip.c_str(), &addr);
	return ntohl(addr.s_addr);
}

void Server::notifyDCCsend(const int &clientFd, const std::string &targetNick, const t_dcc &dccData) {
	std::string msg(":");
	msg += this->Users[clientFd].getNickname() + "!";
	msg += this->Users[clientFd].getUsername() + "@";
	msg += this->Users[clientFd].getIp();
	msg += " PRIVMSG " + targetNick + " :";
	msg += "\x01";
	msg += "DCC SEND " + dccData.filename;

	unsigned long ipDec = ipToDecimal(this->Users[clientFd].getIp());
	std::stringstream ssIp;
	ssIp << ipDec;
	msg += " " + ssIp.str();

	std::stringstream ssPort;
	ssPort << dccData.realPort;
	msg += " " + ssPort.str();

	msg += " " + dccData.fileSize;
	msg += "\x01\r\n";

	send(this->findIdByName(targetNick), msg.c_str(), msg.size(), 0);
}