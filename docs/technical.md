# Concepts techniques - ft_irc

## 📋 Table des matières

- [I/O non-bloquantes](#io-non-bloquantes)
- [Multiplexage avec poll()](#multiplexage-avec-poll)
- [Sockets TCP/IP](#sockets-tcpip)
- [Parsing de protocole](#parsing-de-protocole)
- [Gestion des buffers](#gestion-des-buffers)
- [Gestion des erreurs](#gestion-des-erreurs)

## 🚫 I/O non-bloquantes

### Problème avec les I/O bloquantes

En mode bloquant par défaut :
```cpp
// Bloque jusqu'à réception de données
int n = recv(fd, buffer, size, 0);  // ⚠️ BLOQUANT !
```

**Problème :** Si un client ne send rien, le serveur reste bloqué et ne peut pas gérer les autres clients.

### Solution : I/O non-bloquantes

Mettre les sockets en mode non-bloquant :

```cpp
#include <fcntl.h>

// Rendre un fd non-bloquant
int setNonBlocking(int fd) {
    int flags = fcntl(fd, F_GETFL, 0);
    if (flags == -1)
        return -1;
    return fcntl(fd, F_SETFL, flags | O_NONBLOCK);
}
```

**Sur MacOS uniquement :**
```cpp
// Version simplifiée autorisée pour MacOS
fcntl(fd, F_SETFL, O_NONBLOCK);
```

### Comportement en mode non-bloquant

```cpp
int n = recv(fd, buffer, size, 0);

if (n > 0) {
    // Données reçues
} else if (n == 0) {
    // Connexion fermée par le client
} else {  // n == -1
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Pas de données disponibles, c'est normal
    } else {
        // Vraie erreur
    }
}
```

## 🔀 Multiplexage avec poll()

### Pourquoi poll() ?

**Problème :** Comment surveiller plusieurs file descriptors simultanément ?

**Solution :** `poll()` permet de surveiller plusieurs FDs et de savoir lesquels sont prêts pour I/O.

### Structure pollfd

```cpp
#include <poll.h>

struct pollfd {
    int   fd;        // File descriptor à surveiller
    short events;    // Événements à surveiller (POLLIN, POLLOUT)
    short revents;   // Événements qui se sont produits
};
```

### Événements poll

| Événement | Description |
|-----------|-------------|
| `POLLIN` | Données disponibles en lecture |
| `POLLOUT` | Prêt pour écriture |
| `POLLERR` | Erreur sur le FD |
| `POLLHUP` | Connexion fermée |
| `POLLNVAL` | FD invalide |

### Utilisation de poll()

```cpp
#include <poll.h>
#include <vector>

std::vector<struct pollfd> pollfds;

// Ajouter le socket serveur
struct pollfd serverPfd;
serverPfd.fd = serverSocket;
serverPfd.events = POLLIN;  // Surveiller connexions entrantes
pollfds.push_back(serverPfd);

// Boucle principale
while (true) {
    // Attendre des événements (timeout infini avec -1)
    int ret = poll(&pollfds[0], pollfds.size(), -1);
    
    if (ret < 0) {
        // Erreur
        perror("poll");
        break;
    }
    
    // Parcourir tous les FDs
    for (size_t i = 0; i < pollfds.size(); i++) {
        
        // Nouvelle connexion sur le socket serveur
        if (pollfds[i].fd == serverSocket && 
            pollfds[i].revents & POLLIN) {
            acceptNewClient();
        }
        
        // Données disponibles d'un client
        else if (pollfds[i].revents & POLLIN) {
            handleClientData(pollfds[i].fd);
        }
        
        // Prêt pour envoi
        if (pollfds[i].revents & POLLOUT) {
            sendToClient(pollfds[i].fd);
        }
        
        // Erreur ou déconnexion
        if (pollfds[i].revents & (POLLERR | POLLHUP)) {
            removeClient(pollfds[i].fd);
        }
    }
}
```

### Gestion dynamique des pollfds

```cpp
// Ajouter un nouveau client
void Server::addClient(int clientFd) {
    struct pollfd pfd;
    pfd.fd = clientFd;
    pfd.events = POLLIN | POLLOUT;  // Lecture et écriture
    pfd.revents = 0;
    
    _pollfds.push_back(pfd);
    _clients[clientFd] = new Client(clientFd);
}

// Retirer un client
void Server::removeClient(int clientFd) {
    // Fermer le socket
    close(clientFd);
    
    // Retirer des pollfds
    for (size_t i = 0; i < _pollfds.size(); i++) {
        if (_pollfds[i].fd == clientFd) {
            _pollfds.erase(_pollfds.begin() + i);
            break;
        }
    }
    
    // Supprimer l'objet Client
    delete _clients[clientFd];
    _clients.erase(clientFd);
}
```

### Alternatives à poll()

Le sujet autorise des équivalents :

#### select()
```cpp
fd_set readfds, writefds;
FD_ZERO(&readfds);
FD_SET(fd, &readfds);

int ret = select(max_fd + 1, &readfds, &writefds, NULL, NULL);
```

**Limites :** Maximum ~1024 FDs sur la plupart des systèmes.

#### epoll() (Linux uniquement)
```cpp
int epollfd = epoll_create1(0);
struct epoll_event ev;
ev.events = EPOLLIN | EPOLLOUT;
ev.data.fd = clientfd;
epoll_ctl(epollfd, EPOLL_CTL_ADD, clientfd, &ev);

epoll_wait(epollfd, events, MAX_EVENTS, -1);
```

**Avantage :** Très performant avec beaucoup de FDs.

#### kqueue() (BSD/macOS)
```cpp
int kq = kqueue();
struct kevent ev;
EV_SET(&ev, fd, EVFILT_READ, EV_ADD, 0, 0, NULL);
kevent(kq, &ev, 1, NULL, 0, NULL);
```

**Recommandation :** Utiliser `poll()` pour la simplicité et la portabilité.

## 🌐 Sockets TCP/IP

### Création d'un socket serveur

#### Étape 1 : Créer le socket
```cpp
#include <sys/socket.h>

int serverSocket = socket(AF_INET, SOCK_STREAM, 0);
if (serverSocket < 0) {
    perror("socket");
    exit(1);
}
```

- `AF_INET` : IPv4 (utilisez `AF_INET6` pour IPv6)
- `SOCK_STREAM` : TCP (vs `SOCK_DGRAM` pour UDP)

#### Étape 2 : Configurer le socket

```cpp
#include <sys/socket.h>

// Permettre réutilisation de l'adresse
int opt = 1;
if (setsockopt(serverSocket, SOL_SOCKET, SO_REUSEADDR, 
               &opt, sizeof(opt)) < 0) {
    perror("setsockopt");
    exit(1);
}
```

**Pourquoi SO_REUSEADDR ?** Permet de relancer le serveur immédiatement après arrêt sans attendre.

#### Étape 3 : Bind (lier à une adresse et port)

```cpp
#include <netinet/in.h>
#include <arpa/inet.h>

struct sockaddr_in serverAddr;
memset(&serverAddr, 0, sizeof(serverAddr));

serverAddr.sin_family = AF_INET;
serverAddr.sin_addr.s_addr = INADDR_ANY;  // Toutes interfaces
serverAddr.sin_port = htons(port);         // Conversion host to network

if (bind(serverSocket, (struct sockaddr*)&serverAddr, 
         sizeof(serverAddr)) < 0) {
    perror("bind");
    exit(1);
}
```

**Note :** `htons()` convertit du format host au format network (big-endian).

#### Étape 4 : Listen (écouter les connexions)

```cpp
if (listen(serverSocket, SOMAXCONN) < 0) {
    perror("listen");
    exit(1);
}
```

`SOMAXCONN` : Taille maximale de la file d'attente des connexions.

#### Étape 5 : Accept (accepter connexions)

```cpp
struct sockaddr_in clientAddr;
socklen_t clientLen = sizeof(clientAddr);

int clientSocket = accept(serverSocket, 
                          (struct sockaddr*)&clientAddr, 
                          &clientLen);
                          
if (clientSocket < 0) {
    perror("accept");
    return;
}

// Rendre le client socket non-bloquant
fcntl(clientSocket, F_SETFL, O_NONBLOCK);
```

### Communication avec les clients

#### Réception de données

```cpp
char buffer[1024];
int n = recv(clientSocket, buffer, sizeof(buffer) - 1, 0);

if (n > 0) {
    buffer[n] = '\0';
    // Traiter les données reçues
    std::string data(buffer, n);
    
} else if (n == 0) {
    // Client a fermé la connexion
    close(clientSocket);
    
} else {  // n < 0
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
        // Erreur réelle
        perror("recv");
    }
}
```

#### Envoi de données

```cpp
std::string message = "Hello client\r\n";
int n = send(clientSocket, message.c_str(), message.length(), 0);

if (n > 0) {
    // n octets envoyés (peut être < message.length())
    // Retirer les données envoyées du buffer
    
} else if (n < 0) {
    if (errno != EAGAIN && errno != EWOULDBLOCK) {
        perror("send");
    }
}
```

**Important :** `send()` peut n'envoyer qu'une partie des données en mode non-bloquant !

### Obtenir des informations sur la connexion

```cpp
struct sockaddr_in addr;
socklen_t len = sizeof(addr);

// Adresse du client
getpeername(clientSocket, (struct sockaddr*)&addr, &len);
char* clientIP = inet_ntoa(addr.sin_addr);
int clientPort = ntohs(addr.sin_port);

// Adresse locale (notre serveur)
getsockname(clientSocket, (struct sockaddr*)&addr, &len);
```

## 📝 Parsing de protocole

### Défi : Messages fragmentés

Un client peut envoyer :
```
"NICK jo"          (paquet 1)
"hn\r\nUSER j"     (paquet 2)
"ohn 0 * :J"       (paquet 3)
"ohn Doe\r\n"      (paquet 4)
```

Le serveur doit reconstituer :
```
"NICK john\r\n"
"USER john 0 * :John Doe\r\n"
```

### Solution : Buffer de réception

```cpp
class Client {
private:
    std::string _recvBuffer;
    
public:
    void appendData(const char* data, size_t len) {
        _recvBuffer.append(data, len);
    }
    
    // Extraire un message complet (terminé par \r\n)
    std::string extractMessage() {
        size_t pos = _recvBuffer.find("\r\n");
        
        if (pos == std::string::npos)
            return "";  // Pas de message complet
        
        std::string message = _recvBuffer.substr(0, pos);
        _recvBuffer = _recvBuffer.substr(pos + 2);  // Retirer \r\n
        
        return message;
    }
};
```

### Utilisation

```cpp
void Server::handleClientData(int fd) {
    char buffer[1024];
    int n = recv(fd, buffer, sizeof(buffer), 0);
    
    if (n > 0) {
        Client* client = _clients[fd];
        client->appendData(buffer, n);
        
        // Traiter tous les messages complets
        std::string message;
        while (!(message = client->extractMessage()).empty()) {
            processCommand(client, message);
        }
    }
}
```

### Parsing d'un message IRC

```cpp
// Format: [:prefix] <command> [params] [:trailing]\r\n

struct IRCMessage {
    std::string prefix;      // Optionnel
    std::string command;     // NICK, JOIN, etc.
    std::vector<std::string> params;
    std::string trailing;    // Paramètre après ':'
};

IRCMessage parseMessage(const std::string& raw) {
    IRCMessage msg;
    std::string line = raw;
    size_t pos = 0;
    
    // 1. Extraire prefix (si présent)
    if (line[0] == ':') {
        pos = line.find(' ');
        msg.prefix = line.substr(1, pos - 1);
        line = line.substr(pos + 1);
    }
    
    // 2. Extraire command
    pos = line.find(' ');
    if (pos == std::string::npos) {
        msg.command = line;
        return msg;
    }
    msg.command = line.substr(0, pos);
    line = line.substr(pos + 1);
    
    // 3. Extraire params
    while (!line.empty()) {
        if (line[0] == ':') {
            // Trailing parameter
            msg.trailing = line.substr(1);
            break;
        }
        
        pos = line.find(' ');
        if (pos == std::string::npos) {
            msg.params.push_back(line);
            break;
        }
        
        msg.params.push_back(line.substr(0, pos));
        line = line.substr(pos + 1);
    }
    
    return msg;
}
```

## 📦 Gestion des buffers

### Buffer d'envoi

**Problème :** `send()` peut ne pas tout envoyer en une fois.

**Solution :** Buffer de sortie.

```cpp
class Client {
private:
    std::string _sendBuffer;
    
public:
    void queueMessage(const std::string& msg) {
        _sendBuffer += msg;
        if (_sendBuffer.back() != '\n')
            _sendBuffer += "\r\n";
    }
    
    bool sendPending(int fd) {
        if (_sendBuffer.empty())
            return true;
        
        int n = send(fd, _sendBuffer.c_str(), 
                    _sendBuffer.length(), 0);
        
        if (n > 0) {
            _sendBuffer = _sendBuffer.substr(n);
            return _sendBuffer.empty();
        }
        
        if (errno != EAGAIN && errno != EWOULDBLOCK)
            return false;  // Erreur
        
        return true;
    }
    
    bool hasPendingData() const {
        return !_sendBuffer.empty();
    }
};
```

### Optimisation poll avec POLLOUT

```cpp
void Server::updatePollEvents(int fd) {
    for (size_t i = 0; i < _pollfds.size(); i++) {
        if (_pollfds[i].fd == fd) {
            _pollfds[i].events = POLLIN;
            
            // Activer POLLOUT seulement si données à envoyer
            if (_clients[fd]->hasPendingData())
                _pollfds[i].events |= POLLOUT;
            
            break;
        }
    }
}
```

**Pourquoi ?** Éviter de réveiller inutilement le serveur sur POLLOUT quand rien à envoyer.

## ⚠️ Gestion des erreurs

### Erreurs fréquentes

#### 1. recv() retourne -1

```cpp
if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Normal en mode non-bloquant, pas de données
        return;
    }
    if (errno == EINTR) {
        // Interrompu par signal, réessayer
        return;
    }
    // Erreur réelle
    perror("recv");
    removeClient(fd);
}
```

#### 2. send() retourne -1

```cpp
if (n < 0) {
    if (errno == EAGAIN || errno == EWOULDBLOCK) {
        // Buffer plein, réessayer plus tard
        return;
    }
    if (errno == EPIPE) {
        // Broken pipe, client déconnecté
        removeClient(fd);
        return;
    }
    // Autre erreur
    perror("send");
    removeClient(fd);
}
```

#### 3. poll() retourne -1

```cpp
int ret = poll(&_pollfds[0], _pollfds.size(), -1);
if (ret < 0) {
    if (errno == EINTR) {
        // Interrompu par signal, continuer
        continue;
    }
    perror("poll");
    break;
}
```

### Gestion des signaux

```cpp
#include <signal.h>

volatile sig_atomic_t g_shutdown = 0;

void signalHandler(int signum) {
    (void)signum;
    g_shutdown = 1;
}

int main() {
    // Gérer SIGINT (Ctrl+C) proprement
    signal(SIGINT, signalHandler);
    
    // Ignorer SIGPIPE (broken pipe)
    signal(SIGPIPE, SIG_IGN);
    
    while (!g_shutdown) {
        // Boucle serveur
    }
    
    // Nettoyage propre
}
```

### Validation des données

```cpp
bool isValidNickname(const std::string& nick) {
    if (nick.empty() || nick.length() > 9)
        return false;
    
    // Doit commencer par une lettre
    if (!isalpha(nick[0]))
        return false;
    
    // Contenir seulement lettres, chiffres, -, _
    for (size_t i = 0; i < nick.length(); i++) {
        char c = nick[i];
        if (!isalnum(c) && c != '-' && c != '_')
            return false;
    }
    
    return true;
}
```

## 🎯 Résumé des bonnes pratiques

1. ✅ **Toujours** mettre les sockets en mode non-bloquant
2. ✅ **Un seul** `poll()` pour tous les file descriptors
3. ✅ **Gérer** les messages fragmentés avec des buffers
4. ✅ **Vérifier** toutes les valeurs de retour
5. ✅ **Gérer** EAGAIN/EWOULDBLOCK correctement
6. ✅ **Valider** toutes les entrées utilisateur
7. ✅ **Nettoyer** proprement les ressources
8. ✅ **Ignorer** SIGPIPE pour éviter les crashes
9. ✅ **Tester** avec des données fragmentées
10. ✅ **Logger** les erreurs pour le débogage

---

**Retour à :** [README principal](../README.md)
