# Architecture du serveur IRC

## 📋 Table des matières

- [Vue d'ensemble de l'architecture](#vue-densemble-de-larchitecture)
- [Structure des classes](#structure-des-classes)
- [Flux de données](#flux-de-données)
- [Diagrammes](#diagrammes)
- [Modules principaux](#modules-principaux)

## 🏗️ Vue d'ensemble de l'architecture

### Modèle conceptuel

Le serveur IRC suit une architecture **événementielle** basée sur le multiplexage I/O avec `poll()` (ou équivalent). Il gère plusieurs clients simultanément dans un **seul thread** avec des opérations **non-bloquantes**.

```
┌─────────────────────────────────────────────────┐
│              Serveur IRC (ircserv)              │
├─────────────────────────────────────────────────┤
│                                                 │
│  ┌──────────────────────────────────────────┐  │
│  │    Boucle principale (poll/select)       │  │
│  │  - Surveillance des file descriptors     │  │
│  │  - Détection événements I/O              │  │
│  └──────────────────────────────────────────┘  │
│                      │                          │
│          ┌───────────┼───────────┐             │
│          ▼           ▼           ▼             │
│    ┌─────────┐ ┌─────────┐ ┌─────────┐        │
│    │ Client  │ │ Client  │ │ Client  │        │
│    │    1    │ │    2    │ │    3    │        │
│    └─────────┘ └─────────┘ └─────────┘        │
│          │           │           │             │
│          └───────────┼───────────┘             │
│                      ▼                          │
│          ┌───────────────────────┐             │
│          │   Gestionnaire de     │             │
│          │      Canaux           │             │
│          └───────────────────────┘             │
│                                                 │
└─────────────────────────────────────────────────┘
```

### Principes de conception

1. **Single-threaded** : Un seul thread principal
2. **Event-driven** : Réaction aux événements I/O
3. **Non-blocking I/O** : Aucune opération bloquante
4. **Multiplexing** : Un seul `poll()` pour tous les FD
5. **Stateful** : Chaque client a un état

## 🎯 Structure des classes

### Classe Server (Serveur principal)

**Responsabilités :**
- Initialisation du socket serveur
- Boucle principale avec `poll()`
- Gestion de la liste des clients
- Acceptation de nouvelles connexions
- Distribution des événements

**Attributs principaux :**
```cpp
class Server {
private:
    int                          _serverSocket;      // Socket d'écoute
    int                          _port;              // Port d'écoute
    std::string                  _password;          // Mot de passe
    std::vector<struct pollfd>   _pollfds;           // FDs surveillés
    std::map<int, Client*>       _clients;           // Clients connectés
    std::map<std::string, Channel*> _channels;       // Canaux actifs
    
public:
    void start();                    // Démarrage du serveur
    void acceptNewClient();          // Accepter nouvelle connexion
    void handleClientData(int fd);   // Traiter données client
    void removeClient(int fd);       // Déconnecter client
    void broadcast(std::string msg); // Diffuser message
};
```

### Classe Client

**Responsabilités :**
- Représenter un client connecté
- Stocker l'état du client
- Buffer des données reçues/envoyées
- Gestion de l'authentification

**Attributs principaux :**
```cpp
class Client {
private:
    int             _fd;                    // File descriptor
    std::string     _nickname;              // Pseudonyme
    std::string     _username;              // Nom d'utilisateur
    std::string     _realname;              // Nom réel
    std::string     _hostname;              // Nom d'hôte
    std::string     _recvBuffer;            // Buffer réception
    std::string     _sendBuffer;            // Buffer envoi
    bool            _authenticated;         // Authentifié ?
    bool            _registered;            // Enregistré ?
    
public:
    void appendToRecvBuffer(std::string data);
    std::string extractMessage();           // Extraire message complet
    void queueMessage(std::string msg);     // Ajouter au buffer envoi
    bool isRegistered() const;
    std::string getPrefix() const;          // :nick!user@host
};
```

### Classe Channel

**Responsabilités :**
- Représenter un canal de discussion
- Gérer la liste des membres
- Gérer les opérateurs
- Appliquer les modes du canal
- Diffuser messages aux membres

**Attributs principaux :**
```cpp
class Channel {
private:
    std::string                 _name;          // Nom du canal (#general)
    std::string                 _topic;         // Thème du canal
    std::string                 _key;           // Mot de passe (mode +k)
    std::set<Client*>          _members;        // Membres du canal
    std::set<Client*>          _operators;      // Opérateurs
    std::set<Client*>          _inviteList;     // Liste invitations
    size_t                     _userLimit;      // Limite utilisateurs
    
    // Modes
    bool                       _inviteOnly;     // Mode +i
    bool                       _topicRestricted; // Mode +t
    bool                       _hasKey;         // Mode +k actif
    bool                       _hasLimit;       // Mode +l actif
    
public:
    void addMember(Client* client);
    void removeMember(Client* client);
    void addOperator(Client* client);
    void broadcast(std::string msg, Client* except = NULL);
    bool canJoin(Client* client, std::string key);
    void setMode(char mode, bool add, std::string param);
};
```

### Classe Command (Gestionnaire de commandes)

**Responsabilités :**
- Parser les commandes IRC
- Valider les paramètres
- Exécuter les commandes
- Générer les réponses appropriées

**Structure :**
```cpp
class Command {
protected:
    Server*     _server;
    Client*     _client;
    std::string _command;
    std::vector<std::string> _params;
    
public:
    virtual void execute() = 0;
    virtual bool checkParams() = 0;
};

// Commandes spécifiques héritent de Command
class NickCommand : public Command { /* ... */ };
class JoinCommand : public Command { /* ... */ };
class PrivmsgCommand : public Command { /* ... */ };
class ModeCommand : public Command { /* ... */ };
// etc.
```

### Classe CommandParser

**Responsabilités :**
- Analyser les messages IRC bruts
- Extraire commande et paramètres
- Créer l'objet Command approprié

**Méthodes principales :**
```cpp
class CommandParser {
public:
    static Command* parse(std::string rawMessage, Client* client, Server* server);
    
private:
    static std::string extractCommand(std::string msg);
    static std::vector<std::string> extractParams(std::string msg);
};
```

## 🔄 Flux de données

### 1. Démarrage du serveur

```
1. main()
   └─> Server::Server(port, password)
       ├─> Créer socket serveur
       ├─> bind() sur le port
       ├─> listen()
       └─> Ajouter socket serveur aux pollfds
   └─> Server::start()
       └─> Boucle principale
```

### 2. Connexion d'un nouveau client

```
1. poll() détecte POLLIN sur socket serveur
2. Server::acceptNewClient()
   ├─> accept() → nouveau fd
   ├─> fcntl(fd, F_SETFL, O_NONBLOCK)
   ├─> Créer objet Client(fd)
   ├─> Ajouter à _clients
   └─> Ajouter fd aux pollfds
```

### 3. Réception de données

```
1. poll() détecte POLLIN sur fd client
2. Server::handleClientData(fd)
   ├─> recv(fd, buffer, ...)
   ├─> Client::appendToRecvBuffer(buffer)
   └─> Tant que message complet disponible:
       ├─> Client::extractMessage()
       ├─> CommandParser::parse(message)
       ├─> Command::execute()
       └─> Générer réponses → Client::queueMessage()
```

### 4. Envoi de données

```
1. poll() détecte POLLOUT sur fd client
2. Server::sendToClient(fd)
   ├─> Client récupère _sendBuffer
   ├─> send(fd, buffer, ...)
   └─> Retirer données envoyées du buffer
```

### 5. Traitement d'une commande

```
Exemple: JOIN #general

1. Client envoie: "JOIN #general\r\n"
2. Message ajouté au _recvBuffer
3. Message complet détecté
4. CommandParser::parse()
   └─> Crée JoinCommand
5. JoinCommand::execute()
   ├─> Vérifier si canal existe
   ├─> Si non, créer Channel("#general")
   ├─> Channel::addMember(client)
   ├─> Channel::addOperator(client) // si créateur
   └─> Générer réponses:
       ├─> :nick!user@host JOIN #general
       ├─> 353 RPL_NAMREPLY (liste membres)
       └─> 366 RPL_ENDOFNAMES
6. Réponses ajoutées au _sendBuffer
7. send() lors du prochain POLLOUT
```

### 6. Diffusion dans un canal

```
Exemple: PRIVMSG #general :Hello!

1. PrivmsgCommand::execute()
   ├─> Trouver Channel("#general")
   ├─> Channel::broadcast(":nick!user@host PRIVMSG #general :Hello!")
   └─> Pour chaque membre (sauf expéditeur):
       └─> member->queueMessage(message)
2. Chaque membre recevra le message lors du prochain cycle poll()
```

## 📊 Diagrammes

### Diagramme de séquence : Connexion et JOIN

```
Client          Server          Channel
  │               │               │
  │─ connect ────>│               │
  │<── accept ────│               │
  │               │               │
  │─ PASS xxx ───>│               │
  │─ NICK john ──>│               │
  │─ USER ... ───>│               │
  │<─ 001 RPL ────│               │
  │               │               │
  │─JOIN #gen ───>│               │
  │               │─ create ─────>│
  │               │<─ ok ─────────│
  │<─ JOIN ───────│               │
  │<─ 353 ────────│               │
  │<─ 366 ────────│               │
  │               │               │
  │─PRIVMSG #g:Hi>│               │
  │               │─ broadcast ──>│
  │<─ PRIVMSG ────│<──────────────│
  │               │               │
```

### Diagramme d'états : Client

```
┌──────────┐
│  SOCKET  │ Connexion TCP établie
│  OPENED  │
└────┬─────┘
     │ PASS
     ▼
┌──────────┐
│ PASSWORD │ Mot de passe vérifié
│   SENT   │
└────┬─────┘
     │ NICK + USER
     ▼
┌──────────┐
│  FULLY   │ Peut utiliser toutes
│REGISTERED│ les commandes
└────┬─────┘
     │ QUIT ou erreur
     ▼
┌──────────┐
│  CLOSED  │ Déconnecté
└──────────┘
```

## 🧩 Modules principaux

### Module Network (Réseau)

**Fichiers :** `Server.cpp`, `Socket.cpp`

**Fonctions :**
- Création et configuration socket serveur
- `bind()`, `listen()`, `accept()`
- Configuration mode non-bloquant
- Gestion des pollfds

### Module Client Management

**Fichiers :** `Client.cpp`, `ClientManager.cpp`

**Fonctions :**
- Création/destruction clients
- Gestion buffers réception/envoi
- État d'authentification
- Extraction messages complets

### Module Channel Management

**Fichiers :** `Channel.cpp`, `ChannelManager.cpp`

**Fonctions :**
- Création/destruction canaux
- Gestion membres et opérateurs
- Application des modes
- Diffusion messages

### Module Command Processing

**Fichiers :** `CommandParser.cpp`, `commands/*.cpp`

**Fonctions :**
- Parsing messages IRC
- Validation paramètres
- Exécution commandes
- Génération réponses

### Module Reply Generator

**Fichiers :** `Reply.cpp`, `NumericReplies.cpp`

**Fonctions :**
- Formatage réponses numériques
- Messages d'erreur
- Messages système

## 📝 Organisation des fichiers suggérée

```
ft_irc/
├── includes/
│   ├── Server.hpp
│   ├── Client.hpp
│   ├── Channel.hpp
│   ├── Command.hpp
│   ├── CommandParser.hpp
│   ├── Reply.hpp
│   └── Utils.hpp
│
├── srcs/
│   ├── main.cpp
│   │
│   ├── server/
│   │   ├── Server.cpp
│   │   └── Socket.cpp
│   │
│   ├── client/
│   │   ├── Client.cpp
│   │   └── ClientManager.cpp
│   │
│   ├── channel/
│   │   ├── Channel.cpp
│   │   └── ChannelManager.cpp
│   │
│   ├── commands/
│   │   ├── CommandParser.cpp
│   │   ├── NickCommand.cpp
│   │   ├── UserCommand.cpp
│   │   ├── JoinCommand.cpp
│   │   ├── PartCommand.cpp
│   │   ├── PrivmsgCommand.cpp
│   │   ├── KickCommand.cpp
│   │   ├── InviteCommand.cpp
│   │   ├── TopicCommand.cpp
│   │   ├── ModeCommand.cpp
│   │   └── QuitCommand.cpp
│   │
│   └── utils/
│       ├── Reply.cpp
│       ├── NumericReplies.cpp
│       └── Utils.cpp
│
└── Makefile
```

## 🔑 Points clés de l'architecture

### 1. Un seul poll() pour tout

```cpp
void Server::start() {
    while (true) {
        int ret = poll(&_pollfds[0], _pollfds.size(), -1);
        
        for (size_t i = 0; i < _pollfds.size(); i++) {
            if (_pollfds[i].revents & POLLIN) {
                if (_pollfds[i].fd == _serverSocket)
                    acceptNewClient();
                else
                    handleClientData(_pollfds[i].fd);
            }
            if (_pollfds[i].revents & POLLOUT) {
                sendToClient(_pollfds[i].fd);
            }
        }
    }
}
```

### 2. Buffers pour gérer messages fragmentés

```cpp
void Client::appendToRecvBuffer(std::string data) {
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
```

### 3. Pattern Command pour extensibilité

Facilite l'ajout de nouvelles commandes sans modifier le code existant.

### 4. Séparation des responsabilités

Chaque classe a une responsabilité claire et limitée (SRP - Single Responsibility Principle).

---

**Retour à :** [README principal](../README.md)
