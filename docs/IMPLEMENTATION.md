# Guide d'implémentation - ft_irc

## 📋 Table des matières

- [Plan de développement](#plan-de-développement)
- [Étape 1 : Socket serveur de base](#étape-1--socket-serveur-de-base)
- [Étape 2 : Gestion des clients](#étape-2--gestion-des-clients)
- [Étape 3 : Authentification](#étape-3--authentification)
- [Étape 4 : Canaux basiques](#étape-4--canaux-basiques)
- [Étape 5 : Commandes opérateur](#étape-5--commandes-opérateur)
- [Étape 6 : Modes de canal](#étape-6--modes-de-canal)
- [Détails des commandes](#détails-des-commandes)
- [Tests et validation](#tests-et-validation)

## 🗺️ Plan de développement

### Ordre recommandé d'implémentation

```
1. Socket serveur avec poll()          [2-3 jours]
   ├─ Créer socket serveur
   ├─ Boucle poll() basique
   └─ Accept connexions

2. Gestion clients basique            [2-3 jours]
   ├─ Classe Client
   ├─ Buffers recv/send
   └─ Parser commandes simples

3. Authentification                    [2-3 jours]
   ├─ PASS
   ├─ NICK
   ├─ USER
   └─ Réponses numériques de bienvenue

4. Canaux et messages                  [3-4 jours]
   ├─ Classe Channel
   ├─ JOIN / PART
   ├─ PRIVMSG
   └─ Broadcast messages

5. Commandes opérateur                 [2-3 jours]
   ├─ KICK
   ├─ INVITE
   └─ TOPIC

6. Modes de canal                      [3-4 jours]
   ├─ MODE +i (invite-only)
   ├─ MODE +t (topic restriction)
   ├─ MODE +k (channel key)
   ├─ MODE +o (operator)
   └─ MODE +l (user limit)

7. Tests et robustesse                 [2-3 jours]
   ├─ Tests avec client IRC réel
   ├─ Gestion erreurs
   └─ Cas limites

Total estimé: 16-24 jours
```

## 🏗️ Étape 1 : Socket serveur de base

### Objectif
Créer un serveur qui accepte des connexions et surveille les FDs avec `poll()`.

### Implémentation minimale

```cpp
// main.cpp
#include <iostream>
#include <sys/socket.h>
#include <netinet/in.h>
#include <fcntl.h>
#include <poll.h>
#include <vector>
#include <cstring>
#include <unistd.h>

int main(int argc, char** argv) {
    if (argc != 3) {
        std::cerr << "Usage: ./ircserv <port> <password>\n";
        return 1;
    }
    
    int port = atoi(argv[1]);
    std::string password = argv[2];
    
    // 1. Créer socket serveur
    int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
    if (serverSocket < 0) {
        perror("socket");
        return 1;
    }
    
    // 2. Options socket
    int opt = 1;
    setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, &opt, sizeof(opt));
    fcntl(serverSocket, F_SETFL, O_NONBLOCK);
    
    // 3. Bind
    struct sockaddr_in addr;
    memset(&addr, 0, sizeof(addr));
    addr.sin_family = AF_INET;
    addr.sin_addr.s_addr = INADDR_ANY;
    addr.sin_port = htons(port);
    
    if (bind(serverSocket, (struct sockaddr*)&addr, sizeof(addr)) < 0) {
        perror("bind");
        return 1;
    }
    
    // 4. Listen
    if (listen(serverSocket, SOMAXCONN) < 0) {
        perror("listen");
        return 1;
    }
    
    std::cout << "Server listening on port " << port << std::endl;
    
    // 5. Setup poll
    std::vector<struct pollfd> pollfds;
    struct pollfd serverPfd;
    serverPfd.fd = serverSocket;
    serverPfd.events = POLLIN;
    pollfds.push_back(serverPfd);
    
    // 6. Boucle principale
    while (true) {
        int ret = poll(&pollfds[0], pollfds.size(), -1);
        
        if (ret < 0) {
            perror("poll");
            break;
        }
        
        for (size_t i = 0; i < pollfds.size(); i++) {
            if (pollfds[i].revents & POLLIN) {
                if (pollfds[i].fd == serverSocket) {
                    // Nouvelle connexion
                    int clientFd = accept(serverSocket, NULL, NULL);
                    if (clientFd >= 0) {
                        fcntl(clientFd, F_SETFL, O_NONBLOCK);
                        
                        struct pollfd clientPfd;
                        clientPfd.fd = clientFd;
                        clientPfd.events = POLLIN;
                        pollfds.push_back(clientPfd);
                        
                        std::cout << "New client connected: " << clientFd << std::endl;
                    }
                } else {
                    // Données d'un client
                    char buffer[512];
                    int n = recv(pollfds[i].fd, buffer, sizeof(buffer) - 1, 0);
                    
                    if (n > 0) {
                        buffer[n] = '\0';
                        std::cout << "Received from " << pollfds[i].fd << ": " << buffer;
                    } else if (n == 0) {
                        std::cout << "Client disconnected: " << pollfds[i].fd << std::endl;
                        close(pollfds[i].fd);
                        pollfds.erase(pollfds.begin() + i);
                        i--;
                    }
                }
            }
        }
    }
    
    close(serverSocket);
    return 0;
}
```

### Tests
```bash
# Terminal 1
./ircserv 6667 secret123

# Terminal 2
nc localhost 6667
Hello server
^D
```

## 👥 Étape 2 : Gestion des clients

### Créer la classe Client

```cpp
// Client.hpp
#ifndef CLIENT_HPP
#define CLIENT_HPP

#include <string>

class Client {
private:
    int         _fd;
    std::string _nickname;
    std::string _username;
    std::string _realname;
    std::string _hostname;
    std::string _recvBuffer;
    std::string _sendBuffer;
    bool        _authenticated;
    bool        _registered;

public:
    Client(int fd);
    ~Client();
    
    // Getters
    int getFd() const { return _fd; }
    std::string getNickname() const { return _nickname; }
    bool isAuthenticated() const { return _authenticated; }
    bool isRegistered() const { return _registered; }
    
    // Setters
    void setNickname(const std::string& nick) { _nickname = nick; }
    void setUsername(const std::string& user) { _username = user; }
    void setRealname(const std::string& real) { _realname = real; }
    void setAuthenticated(bool auth) { _authenticated = auth; }
    void setRegistered(bool reg) { _registered = reg; }
    
    // Buffer management
    void appendToRecvBuffer(const std::string& data);
    std::string extractMessage();
    void queueMessage(const std::string& msg);
    bool sendPending();
    bool hasPendingData() const;
    
    // Prefix pour messages IRC
    std::string getPrefix() const;
};

#endif
```

```cpp
// Client.cpp
#include "Client.hpp"
#include <sys/socket.h>
#include <unistd.h>

Client::Client(int fd) 
    : _fd(fd), _authenticated(false), _registered(false) {
    _hostname = "localhost";  // Simplification
}

Client::~Client() {
    close(_fd);
}

void Client::appendToRecvBuffer(const std::string& data) {
    _recvBuffer += data;
}

std::string Client::extractMessage() {
    size_t pos = _recvBuffer.find("\r\n");
    if (pos == std::string::npos)
        return "";
    
    std::string message = _recvBuffer.substr(0, pos);
    _recvBuffer = _recvBuffer.substr(pos + 2);
    return message;
}

void Client::queueMessage(const std::string& msg) {
    _sendBuffer += msg;
    if (_sendBuffer.empty() || _sendBuffer[_sendBuffer.length() - 1] != '\n')
        _sendBuffer += "\r\n";
}

bool Client::sendPending() {
    if (_sendBuffer.empty())
        return true;
    
    int n = send(_fd, _sendBuffer.c_str(), _sendBuffer.length(), 0);
    
    if (n > 0) {
        _sendBuffer = _sendBuffer.substr(n);
        return true;
    }
    
    if (n < 0 && errno != EAGAIN && errno != EWOULDBLOCK)
        return false;  // Erreur
    
    return true;
}

bool Client::hasPendingData() const {
    return !_sendBuffer.empty();
}

std::string Client::getPrefix() const {
    return ":" + _nickname + "!" + _username + "@" + _hostname;
}
```

## 🔐 Étape 3 : Authentification

### Séquence d'authentification

```
Client → Server: PASS <password>
Client → Server: NICK <nickname>
Client → Server: USER <username> 0 * :<realname>
Server → Client: :server 001 <nick> :Welcome...
```

### Commande PASS

```cpp
void Server::cmdPass(Client* client, const std::vector<std::string>& params) {
    if (client->isRegistered()) {
        client->queueMessage(":server 462 * :You may not reregister");
        return;
    }
    
    if (params.empty()) {
        client->queueMessage(":server 461 PASS :Not enough parameters");
        return;
    }
    
    if (params[0] == _password) {
        client->setAuthenticated(true);
    } else {
        client->queueMessage(":server 464 * :Password incorrect");
        client->setAuthenticated(false);
    }
}
```

### Commande NICK

```cpp
void Server::cmdNick(Client* client, const std::vector<std::string>& params) {
    if (params.empty()) {
        client->queueMessage(":server 431 * :No nickname given");
        return;
    }
    
    std::string nick = params[0];
    
    // Validation
    if (!isValidNickname(nick)) {
        client->queueMessage(":server 432 * " + nick + " :Erroneous nickname");
        return;
    }
    
    // Vérifier unicité
    if (isNicknameTaken(nick)) {
        client->queueMessage(":server 433 * " + nick + " :Nickname is already in use");
        return;
    }
    
    client->setNickname(nick);
    
    // Si tous les champs sont définis, marquer comme registered
    checkRegistration(client);
}
```

### Commande USER

```cpp
void Server::cmdUser(Client* client, const std::vector<std::string>& params) {
    if (client->isRegistered()) {
        client->queueMessage(":server 462 " + client->getNickname() + 
                           " :You may not reregister");
        return;
    }
    
    if (params.size() < 4) {
        client->queueMessage(":server 461 USER :Not enough parameters");
        return;
    }
    
    client->setUsername(params[0]);
    client->setRealname(params[3]);  // params[3] contient le realname
    
    checkRegistration(client);
}
```

### Enregistrement complet

```cpp
void Server::checkRegistration(Client* client) {
    if (client->isRegistered())
        return;
    
    if (!client->isAuthenticated() ||
        client->getNickname().empty() ||
        client->getUsername().empty()) {
        return;  // Pas encore prêt
    }
    
    client->setRegistered(true);
    
    // Envoyer messages de bienvenue
    std::string nick = client->getNickname();
    
    client->queueMessage(":server 001 " + nick + 
                        " :Welcome to the IRC Network " + 
                        client->getPrefix());
    client->queueMessage(":server 002 " + nick + 
                        " :Your host is server, running version 1.0");
    client->queueMessage(":server 003 " + nick + 
                        " :This server was created 2025-12-03");
    client->queueMessage(":server 004 " + nick + 
                        " server 1.0 io itkol");
}
```

## 📺 Étape 4 : Canaux basiques

### Classe Channel

```cpp
// Channel.hpp
#ifndef CHANNEL_HPP
#define CHANNEL_HPP

#include <string>
#include <set>
#include "Client.hpp"

class Channel {
private:
    std::string        _name;
    std::string        _topic;
    std::set<Client*>  _members;
    std::set<Client*>  _operators;

public:
    Channel(const std::string& name);
    
    // Getters
    std::string getName() const { return _name; }
    std::string getTopic() const { return _topic; }
    size_t getMemberCount() const { return _members.size(); }
    
    // Member management
    void addMember(Client* client);
    void removeMember(Client* client);
    bool isMember(Client* client) const;
    
    // Operator management
    void addOperator(Client* client);
    void removeOperator(Client* client);
    bool isOperator(Client* client) const;
    
    // Communication
    void broadcast(const std::string& msg, Client* except = NULL);
    std::string getMemberList() const;
    
    // Topic
    void setTopic(const std::string& topic) { _topic = topic; }
};

#endif
```

### Commande JOIN

```cpp
void Server::cmdJoin(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered()) {
        client->queueMessage(":server 451 * :You have not registered");
        return;
    }
    
    if (params.empty()) {
        client->queueMessage(":server 461 " + client->getNickname() + 
                           " JOIN :Not enough parameters");
        return;
    }
    
    std::string channelName = params[0];
    
    // Validation du nom
    if (channelName.empty() || channelName[0] != '#') {
        client->queueMessage(":server 403 " + client->getNickname() + 
                           " " + channelName + " :No such channel");
        return;
    }
    
    // Trouver ou créer canal
    Channel* channel = findChannel(channelName);
    if (!channel) {
        channel = createChannel(channelName);
        channel->addOperator(client);  // Créateur = opérateur
    }
    
    // Ajouter client au canal
    channel->addMember(client);
    
    // Envoyer JOIN à tous les membres
    std::string joinMsg = client->getPrefix() + " JOIN " + channelName;
    channel->broadcast(joinMsg);
    
    // Envoyer topic (si défini)
    if (!channel->getTopic().empty()) {
        client->queueMessage(":server 332 " + client->getNickname() + 
                           " " + channelName + " :" + channel->getTopic());
    } else {
        client->queueMessage(":server 331 " + client->getNickname() + 
                           " " + channelName + " :No topic is set");
    }
    
    // Envoyer liste des membres
    client->queueMessage(":server 353 " + client->getNickname() + 
                        " = " + channelName + " :" + 
                        channel->getMemberList());
    client->queueMessage(":server 366 " + client->getNickname() + 
                        " " + channelName + " :End of /NAMES list");
}
```

### Commande PRIVMSG

```cpp
void Server::cmdPrivmsg(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered())
        return;
    
    if (params.size() < 2) {
        if (params.empty())
            client->queueMessage(":server 411 " + client->getNickname() + 
                               " :No recipient given (PRIVMSG)");
        else
            client->queueMessage(":server 412 " + client->getNickname() + 
                               " :No text to send");
        return;
    }
    
    std::string target = params[0];
    std::string message = params[1];
    
    std::string fullMsg = client->getPrefix() + " PRIVMSG " + 
                         target + " :" + message;
    
    // Canal ou utilisateur ?
    if (target[0] == '#') {
        Channel* channel = findChannel(target);
        if (!channel) {
            client->queueMessage(":server 403 " + client->getNickname() + 
                               " " + target + " :No such channel");
            return;
        }
        
        if (!channel->isMember(client)) {
            client->queueMessage(":server 404 " + client->getNickname() + 
                               " " + target + " :Cannot send to channel");
            return;
        }
        
        channel->broadcast(fullMsg, client);  // Ne pas envoyer à soi-même
    } else {
        Client* targetClient = findClientByNick(target);
        if (!targetClient) {
            client->queueMessage(":server 401 " + client->getNickname() + 
                               " " + target + " :No such nick/channel");
            return;
        }
        
        targetClient->queueMessage(fullMsg);
    }
}
```

## 👑 Étape 5 : Commandes opérateur

### Commande KICK

```cpp
void Server::cmdKick(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered())
        return;
    
    if (params.size() < 2) {
        client->queueMessage(":server 461 " + client->getNickname() + 
                           " KICK :Not enough parameters");
        return;
    }
    
    std::string channelName = params[0];
    std::string targetNick = params[1];
    std::string reason = (params.size() >= 3) ? params[2] : client->getNickname();
    
    Channel* channel = findChannel(channelName);
    if (!channel) {
        client->queueMessage(":server 403 " + client->getNickname() + 
                           " " + channelName + " :No such channel");
        return;
    }
    
    if (!channel->isOperator(client)) {
        client->queueMessage(":server 482 " + client->getNickname() + 
                           " " + channelName + " :You're not channel operator");
        return;
    }
    
    Client* target = findClientByNick(targetNick);
    if (!target || !channel->isMember(target)) {
        client->queueMessage(":server 441 " + client->getNickname() + 
                           " " + targetNick + " " + channelName + 
                           " :They aren't on that channel");
        return;
    }
    
    // Diffuser KICK à tous
    std::string kickMsg = client->getPrefix() + " KICK " + channelName + 
                         " " + targetNick + " :" + reason;
    channel->broadcast(kickMsg);
    
    // Retirer du canal
    channel->removeMember(target);
}
```

### Commande INVITE

```cpp
void Server::cmdInvite(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered())
        return;
    
    if (params.size() < 2) {
        client->queueMessage(":server 461 " + client->getNickname() + 
                           " INVITE :Not enough parameters");
        return;
    }
    
    std::string targetNick = params[0];
    std::string channelName = params[1];
    
    Channel* channel = findChannel(channelName);
    if (!channel) {
        client->queueMessage(":server 403 " + client->getNickname() + 
                           " " + channelName + " :No such channel");
        return;
    }
    
    if (!channel->isMember(client)) {
        client->queueMessage(":server 442 " + client->getNickname() + 
                           " " + channelName + " :You're not on that channel");
        return;
    }
    
    // Si mode +i, vérifier que client est opérateur
    if (channel->isInviteOnly() && !channel->isOperator(client)) {
        client->queueMessage(":server 482 " + client->getNickname() + 
                           " " + channelName + " :You're not channel operator");
        return;
    }
    
    Client* target = findClientByNick(targetNick);
    if (!target) {
        client->queueMessage(":server 401 " + client->getNickname() + 
                           " " + targetNick + " :No such nick/channel");
        return;
    }
    
    if (channel->isMember(target)) {
        client->queueMessage(":server 443 " + client->getNickname() + 
                           " " + targetNick + " " + channelName + 
                           " :is already on channel");
        return;
    }
    
    // Ajouter à la liste d'invitation
    channel->addToInviteList(target);
    
    // Notifier
    client->queueMessage(":server 341 " + client->getNickname() + 
                        " " + targetNick + " " + channelName);
    target->queueMessage(client->getPrefix() + " INVITE " + 
                        targetNick + " " + channelName);
}
```

### Commande TOPIC

```cpp
void Server::cmdTopic(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered())
        return;
    
    if (params.empty()) {
        client->queueMessage(":server 461 " + client->getNickname() + 
                           " TOPIC :Not enough parameters");
        return;
    }
    
    std::string channelName = params[0];
    Channel* channel = findChannel(channelName);
    
    if (!channel) {
        client->queueMessage(":server 403 " + client->getNickname() + 
                           " " + channelName + " :No such channel");
        return;
    }
    
    if (!channel->isMember(client)) {
        client->queueMessage(":server 442 " + client->getNickname() + 
                           " " + channelName + " :You're not on that channel");
        return;
    }
    
    // Afficher topic
    if (params.size() == 1) {
        if (channel->getTopic().empty()) {
            client->queueMessage(":server 331 " + client->getNickname() + 
                               " " + channelName + " :No topic is set");
        } else {
            client->queueMessage(":server 332 " + client->getNickname() + 
                               " " + channelName + " :" + channel->getTopic());
        }
        return;
    }
    
    // Modifier topic
    if (channel->isTopicRestricted() && !channel->isOperator(client)) {
        client->queueMessage(":server 482 " + client->getNickname() + 
                           " " + channelName + " :You're not channel operator");
        return;
    }
    
    std::string newTopic = params[1];
    channel->setTopic(newTopic);
    
    // Diffuser le changement
    std::string topicMsg = client->getPrefix() + " TOPIC " + 
                          channelName + " :" + newTopic;
    channel->broadcast(topicMsg);
}
```

## ⚙️ Étape 6 : Modes de canal

### Commande MODE (structure)

```cpp
void Server::cmdMode(Client* client, const std::vector<std::string>& params) {
    if (!client->isRegistered())
        return;
    
    if (params.empty()) {
        client->queueMessage(":server 461 " + client->getNickname() + 
                           " MODE :Not enough parameters");
        return;
    }
    
    std::string target = params[0];
    
    // Mode canal
    if (target[0] == '#') {
        handleChannelMode(client, params);
    }
    // Mode utilisateur (non requis pour ft_irc)
}

void Server::handleChannelMode(Client* client, 
                               const std::vector<std::string>& params) {
    std::string channelName = params[0];
    Channel* channel = findChannel(channelName);
    
    if (!channel) {
        client->queueMessage(":server 403 " + client->getNickname() + 
                           " " + channelName + " :No such channel");
        return;
    }
    
    // Afficher modes actuels
    if (params.size() == 1) {
        client->queueMessage(":server 324 " + client->getNickname() + 
                           " " + channelName + " " + 
                           channel->getModeString());
        return;
    }
    
    // Modifier modes (opérateur requis)
    if (!channel->isOperator(client)) {
        client->queueMessage(":server 482 " + client->getNickname() + 
                           " " + channelName + " :You're not channel operator");
        return;
    }
    
    std::string modeString = params[1];
    size_t paramIndex = 2;
    
    bool adding = true;
    for (size_t i = 0; i < modeString.length(); i++) {
        char mode = modeString[i];
        
        if (mode == '+') {
            adding = true;
        } else if (mode == '-') {
            adding = false;
        } else {
            std::string param;
            
            // Modes nécessitant un paramètre
            if (mode == 'k' || mode == 'o' || (mode == 'l' && adding)) {
                if (paramIndex < params.size()) {
                    param = params[paramIndex++];
                } else {
                    continue;  // Paramètre manquant, ignorer
                }
            }
            
            applyMode(channel, mode, adding, param, client);
        }
    }
}
```

### Application des modes

```cpp
void Server::applyMode(Channel* channel, char mode, bool add, 
                      const std::string& param, Client* client) {
    std::string modeChange;
    
    switch (mode) {
        case 'i':  // Invite-only
            channel->setInviteOnly(add);
            modeChange = std::string(add ? "+" : "-") + "i";
            break;
            
        case 't':  // Topic restriction
            channel->setTopicRestricted(add);
            modeChange = std::string(add ? "+" : "-") + "t";
            break;
            
        case 'k':  // Channel key
            if (add) {
                channel->setKey(param);
                modeChange = "+k " + param;
            } else {
                channel->removeKey();
                modeChange = "-k";
            }
            break;
            
        case 'o':  // Operator privilege
            {
                Client* target = findClientByNick(param);
                if (!target || !channel->isMember(target))
                    return;
                
                if (add)
                    channel->addOperator(target);
                else
                    channel->removeOperator(target);
                
                modeChange = std::string(add ? "+" : "-") + "o " + param;
            }
            break;
            
        case 'l':  // User limit
            if (add) {
                int limit = atoi(param.c_str());
                if (limit > 0) {
                    channel->setUserLimit(limit);
                    modeChange = "+l " + param;
                }
            } else {
                channel->removeUserLimit();
                modeChange = "-l";
            }
            break;
            
        default:
            client->queueMessage(":server 472 " + client->getNickname() + 
                               " " + mode + " :is unknown mode char to me");
            return;
    }
    
    // Diffuser le changement de mode
    std::string modeMsg = client->getPrefix() + " MODE " + 
                         channel->getName() + " " + modeChange;
    channel->broadcast(modeMsg);
}
```

## ✅ Tests et validation

### Tests essentiels

#### 1. Test de connexion fragmentée
```bash
nc localhost 6667
PASS sec^D
ret12^D
3^D
NICK al^D
ice^D
USER alice 0 * :Alice^D
^D
```

#### 2. Test avec irssi
```bash
irssi
/connect localhost 6667 secret123
/nick alice
/join #test
/msg #test Hello everyone
/topic #test Welcome!
/mode #test +it
/invite bob #test
/kick #test bob Spamming
/quit
```

#### 3. Test multi-clients
```bash
# Terminal 1
irssi
/connect localhost 6667 secret123
/nick alice
/join #general

# Terminal 2
irssi
/connect localhost 6667 secret123
/nick bob
/join #general

# Terminal 3
irssi
/connect localhost 6667 secret123
/nick charlie
/join #general
```

### Checklist de validation

- [ ] Le serveur démarre avec port et password
- [ ] Accepte plusieurs connexions simultanées
- [ ] PASS + NICK + USER fonctionne
- [ ] Messages de bienvenue corrects (001-004)
- [ ] JOIN crée un canal s'il n'existe pas
- [ ] PRIVMSG vers canal diffuse à tous les membres
- [ ] PRIVMSG vers utilisateur fonctionne
- [ ] KICK éjecte un utilisateur (opérateur requis)
- [ ] INVITE invite un utilisateur
- [ ] TOPIC affiche/modifie le thème
- [ ] MODE +i empêche JOIN sans invitation
- [ ] MODE +t restreint TOPIC aux opérateurs
- [ ] MODE +k nécessite mot de passe pour JOIN
- [ ] MODE +o donne privilèges opérateur
- [ ] MODE +l limite le nombre d'utilisateurs
- [ ] Messages fragmentés sont reconstitués
- [ ] Déconnexions brutales gérées proprement
- [ ] Pas de crash en cas de mémoire insuffisante
- [ ] Gestion propre des erreurs

### Cas limites à tester

1. **Nickname déjà pris**
2. **Mot de passe incorrect**
3. **Commandes avant authentification**
4. **Canal inexistant**
5. **Canal plein (MODE +l)**
6. **Mauvais mot de passe canal (MODE +k)**
7. **Invitation requise (MODE +i)**
8. **Non-opérateur essaie commande opérateur**
9. **Messages très longs (>512 caractères)**
10. **Connexion/déconnexion rapide multiple**

---

**Retour à :** [README principal](../README.md)
