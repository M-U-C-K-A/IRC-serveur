# Architecture et Relations - ft_irc

## 📁 Arborescence du projet

```
ft_irc/
├── 📄 Makefile                     # Buildfile moderne avec couleurs
├── 📄 README.md                    # Documentation principale
├── 📁 docs/                       # Documentation détaillée
│   ├── architecture.md             # Ce fichier - Architecture complète
│   ├── commands.md                 # Référence des commandes IRC
│   ├── protocol.md                 # Spécification protocole IRC
│   ├── technical.md                # Concepts techniques (epoll, sockets)
│   ├── testing.md                  # Guide de tests
│   ├── status.md                   # État d'avancement du projet
│   └── guide_irssi.md              # Guide client irssi
│
├── 📁 includes/                   # Headers C++ (.hpp)
│   ├── Server.hpp                  # Classe serveur principale
│   ├── User.hpp                    # Classe utilisateur/client
│   ├── Channel.hpp                 # Classe canal IRC
│   ├── Utils.hpp                   # Macros et utilitaires
│   └── IrcReplies.hpp              # Codes de réponse IRC (ERR_*, RPL_*)
│
└── 📁 srcs/                       # Sources C++ (.cpp)
    ├── main.cpp                    # Point d'entrée du programme
    ├── User.cpp                    # Implémentation User
    ├── Channel.cpp                 # Implémentation Channel
    ├── Utils.cpp                   # Fonctions utilitaires
    ├── IrcReplies.cpp              # Implémentation réponses IRC
    │
    └── 📁 commands/                # Commandes IRC organisées par catégorie
        ├── 📁 registration/        # Authentification
        │   ├── Pass.cpp
        │   ├── Nick.cpp
        │   └── User.cpp
        ├── 📁 channel/             # Gestion canaux
        │   ├── Join.cpp
        │   ├── Part.cpp
        │   ├── Kick.cpp
        │   ├── Topic.cpp
        │   ├── Invite.cpp
        │   └── Mode.cpp
        ├── 📁 messaging/           # Messages
        │   ├── Privmsg.cpp
        │   ├── Notice.cpp
        │   └── Away.cpp
        ├── 📁 operator/            # Commandes opérateur
        └── 📁 query/               # Requêtes d'information
```

---

## 🏗️ Architecture des Classes

### Hiérarchie et Relations

```
┌─────────────────────────────────────────────────────────────┐
│                        MAIN.CPP                             │
│                     (Point d'entrée)                        │
└──────────────────────┬──────────────────────────────────────┘
                       │
                       │ crée et démarre
                       ▼
┌─────────────────────────────────────────────────────────────┐
│                      CLASS SERVER                           │
│  ┌───────────────────────────────────────────────────────┐  │
│  │  Attributs:                                           │  │
│  │  - int socketfd           // Socket serveur          │  │
│  │  - int epollFd            // File descriptor epoll   │  │
│  │  - std::map<int, User>    // Utilisateurs connectés  │  │
│  │  - std::vector<Channel>   // Liste des canaux        │  │
│  │  - std::string password   // Mot de passe serveur    │  │
│  └───────────────────────────────────────────────────────┘  │
│                                                              │
│  Méthodes principales:                                       │
│  - initSocket()          → Crée socket TCP                  │
│  - initEpoll()           → Configure epoll                  │
│  - runServer()           → Boucle principale événements     │
│  - acceptUser()          → Accepte nouvelles connexions     │
│  - parseInput()          → Parse commandes IRC              │
│                                                              │
│  Gestionnaires de commandes:                                │
│  - handlePass()          → Authentification                 │
│  - handleNick()          → Pseudonyme                       │
│  - handleUsername()      → Nom d'utilisateur                │
│  - handleJoin()          → Rejoindre canal                  │
│  - handlePart()          → Quitter canal                    │
│  - handleKick()          → Éjecter utilisateur              │
│  - handleTopic()         → Thème du canal                   │
│  - handleInvite()        → Inviter utilisateur              │
│  - handleMode()          → Modes du canal                   │
│  - handlePrivateMessage()→ Messages privés/publics          │
│  - handleDCC()           → Transfert fichiers (bonus)       │
└──────────┬───────────────────────────┬───────────────────────┘
           │                           │
           │ gère 1..n                 │ gère 0..n
           ▼                           ▼
┌──────────────────────┐    ┌──────────────────────────┐
│    CLASS USER        │    │    CLASS CHANNEL         │
│  (Client IRC)        │    │    (Canal IRC)           │
├──────────────────────┤    ├──────────────────────────┤
│ Attributs:           │    │ Attributs:               │
│ - nickname           │    │ - name                   │
│ - username           │    │ - topic                  │
│ - fd                 │    │ - key (password)         │
│ - buffer             │    │ - host (créateur)        │
│ - hasPass            │    │ - operators (set<int>)   │
│ - hasNickname        │    │ - users (set<int>)       │
│ - hasUsername        │    │ - invited (set<int>)     │
│ - isRegister         │    │ - invite_only (bool)     │
│                      │    │ - topic_op_only (bool)   │
│ Méthodes:            │    │ - has_key (bool)         │
│ - setNickname()      │    │ - user_limit (int)       │
│ - setUsername()      │    │                          │
│ - addToBuffer()      │    │ Méthodes:                │
│ - clearBuffer()      │    │ - canJoin()              │
│ - tryRegisterUser()  │    │ - addMember()            │
│ - closeConnection()  │    │ - removeMember()         │
└──────────────────────┘    │ - isMember()             │
                            │ - isOperator()           │
                            │ - addOperator()          │
                            │ - setInviteOnly()        │
                            │ - setTopicOpOnly()       │
                            │ - setKey()               │
                            │ - setUserLimit()         │
                            │ - setTopic()             │
                            │ - invite()               │
                            │ - isInvited()            │
                            └──────────────────────────┘
```

---

## 🔄 Flux de données et appels de fonctions

### 1. Démarrage du serveur

```
main()
  │
  ├─> Server server;
  ├─> server.initSocket()
  │     └─> socket(), bind(), listen()
  │
  ├─> server.initEpoll()
  │     └─> epoll_create1(), epoll_ctl()
  │
  └─> server.runServer()
        └─> Boucle infinie epoll_wait()
```

### 2. Connexion d'un nouveau client

```
runServer() détecte événement sur socket serveur
  │
  └─> acceptUser()
        ├─> accept() → nouveau fd client
        ├─> fcntl(fd, F_SETFL, O_NONBLOCK)
        ├─> epoll_ctl(EPOLL_CTL_ADD, fd)
        └─> Users[fd] = User()
```

### 3. Réception de données client

```
runServer() détecte EPOLLIN sur fd client
  │
  └─> parseInput(userFd)
        ├─> recv(userFd, buffer)
        ├─> Users[userFd].addToBuffer(buffer)
        │
        └─> Selon la commande reçue:
             ├─> "PASS"    → handlePass()
             ├─> "NICK"    → handleNick()
             ├─> "USER"    → handleUsername()
             ├─> "JOIN"    → handleJoin()
             ├─> "PART"    → handlePart()
             ├─> "PRIVMSG" → handlePrivateMessage()
             ├─> "KICK"    → handleKick()
             ├─> "INVITE"  → handleInvite()
             ├─> "TOPIC"   → handleTopic()
             ├─> "MODE"    → handleMode()
             └─> "PING"    → handlePing()
```

### 4. Authentification (PASS → NICK → USER)

```
Client envoie: "PASS secret123"
  │
  └─> handlePass(clientFd, line)
        ├─> Extrait password de line
        ├─> Compare avec server.password
        └─> Si OK: Users[clientFd].setHasPass()

Client envoie: "NICK alice"
  │
  └─> handleNick(clientFd, line)
        ├─> Extrait nickname
        ├─> Vérifie si déjà utilisé (nickAlreadyInUse())
        ├─> Users[clientFd].setNickname(nick)
        ├─> Users[clientFd].setHasNickname(true)
        └─> Users[clientFd].tryRegisterUser()

Client envoie: "USER alice 0 * :Alice Wonder"
  │
  └─> handleUsername(clientFd, line)
        ├─> Extrait username
        ├─> Users[clientFd].setUsername(user)
        ├─> Users[clientFd].setHasUsername()
        └─> Users[clientFd].tryRegisterUser()
              │
              └─> Si (hasPass && hasNick && hasUser):
                    ├─> setHasRegister(true)
                    └─> welcomeUser()
                          ├─> send RPL_WELCOME (001)
                          ├─> send RPL_YOURHOST (002)
                          └─> send RPL_CREATED (003)
```

### 5. Rejoindre un canal (JOIN)

```
Client envoie: "JOIN #general"
  │
  └─> handleJoin(clientFd, line)
        ├─> Extrait nom canal (#general)
        │
        ├─> Si canal n'existe pas:
        │     ├─> createChannel(name, clientFd)
        │     │     ├─> Channel newChan(name, clientFd)
        │     │     ├─> newChan.addOperator(clientFd)
        │     │     └─> channelList.push_back(newChan)
        │     │
        │     ├─> notifyJoin()
        │     ├─> sendTopic()
        │     └─> sendRPL_NAMEREPLY()
        │
        └─> Si canal existe:
              ├─> channel.canJoin(fd, key)
              │     └─> Vérifie: invite_only, key, user_limit
              │
              ├─> Si OK: channel.addMember(clientFd)
              ├─> notifyJoin()
              ├─> sendTopic()
              └─> sendRPL_NAMEREPLY()
```

### 6. Envoyer un message (PRIVMSG)

```
Client envoie: "PRIVMSG #general :Hello!"
  │
  └─> handlePrivateMessage(clientFd, line)
        ├─> Extrait target et message
        │
        ├─> Si target commence par '#':
        │     └─> broadcastToChannel(channel, msg, senderFd)
        │           └─> Pour chaque membre du canal (sauf sender):
        │                 └─> send(memberFd, msg)
        │
        └─> Sinon (message privé):
              ├─> findIdByName(targetNick)
              └─> send(targetFd, msg)
```

### 7. Commande opérateur (KICK)

```
Client envoie: "KICK #general bob :Spamming"
  │
  └─> handleKick(clientFd, line)
        ├─> Extrait: channelName, userToKick, reason
        ├─> findChannelByName(channelName)
        │
        ├─> Vérifications:
        │     ├─> channel.hasPerm(clientFd) → est opérateur?
        │     └─> channel.isMember(victimFd) → victime dans canal?
        │
        ├─> Si OK:
        │     ├─> broadcastKickConfirmation()
        │     └─> channel.removeMember(victimFd)
        │
        └─> Sinon: sendERR_CHANOPRIVSNEEDED()
```

### 8. Modes de canal (MODE)

```
Client envoie: "MODE #general +it"
  │
  └─> handleMode(clientFd, line)
        ├─> Extrait: channelName, modes, args
        ├─> findChannelByName(channelName)
        │
        ├─> Vérifie: channel.hasPerm(clientFd)
        │
        └─> Pour chaque mode:
              ├─> execMode(clientFd, channel, mode, arg)
                    │
                    └─> setMode(clientFd, channel, flag, set/unset, arg)
                          ├─> 'i': channel.setInviteOnly(true/false)
                          ├─> 't': channel.setTopicOpOnly(true/false)
                          ├─> 'k': channel.setKey(password)
                          ├─> 'o': channel.addOperator(userFd)
                          ├─> 'l': channel.setUserLimit(limit)
                          │
                          └─> notifyMode() → broadcast changement
```

---

## 📊 Diagramme de flux - Cas complet

```
┌──────────┐
│  Client  │
│  (irssi) │
└────┬─────┘
     │
     │ 1. Connexion TCP
     │
     ▼
┌─────────────────┐
│  accept()       │ ← Server.acceptUser()
│  Nouveau User   │
└────┬────────────┘
     │
     │ 2. PASS secret123
     │
     ▼
┌─────────────────┐
│ handlePass()    │ → Users[fd].setHasPass()
└────┬────────────┘
     │
     │ 3. NICK alice
     │
     ▼
┌─────────────────┐
│ handleNick()    │ → Users[fd].setNickname("alice")
└────┬────────────┘
     │
     │ 4. USER alice 0 * :Alice
     │
     ▼
┌─────────────────┐
│handleUsername() │ → Users[fd].tryRegisterUser()
│                 │ → welcomeUser() (RPL 001-003)
└────┬────────────┘
     │
     │ 5. JOIN #test
     │
     ▼
┌─────────────────┐
│ handleJoin()    │ → createChannel() ou joinExistingChannel()
│                 │ → notifyJoin()
│                 │ → sendRPL_NAMEREPLY()
└────┬────────────┘
     │
     │ 6. PRIVMSG #test :Hello!
     │
     ▼
┌─────────────────┐
│ handlePrivate   │ → broadcastToChannel()
│ Message()       │ → send() à tous les membres
└────┬────────────┘
     │
     │ 7. MODE #test +it
     │
     ▼
┌─────────────────┐
│ handleMode()    │ → channel.setInviteOnly(true)
│                 │ → channel.setTopicOpOnly(true)
│                 │ → notifyMode()
└─────────────────┘
```

---

## 🔗 Relations entre fichiers

### Dépendances d'inclusion

```
main.cpp
  └─> Server.hpp
        ├─> User.hpp
        ├─> Channel.hpp
        └─> Utils.hpp

User.cpp
  └─> User.hpp

Channel.cpp
  └─> Channel.hpp

Utils.cpp
  └─> Utils.hpp

Server.cpp
  └─> Server.hpp (inclut User, Channel, Utils)

handleUser.cpp
  └─> Server.hpp

Join.cpp
  └─> Server.hpp

Part.cpp
  └─> Server.hpp

Kick.cpp
  └─> Server.hpp

Topic.cpp
  └─> Server.hpp

Invite.cpp
  └─> Server.hpp

Mode.cpp
  └─> Server.hpp

message.cpp
  └─> Server.hpp

Replay.cpp
  └─> Server.hpp

DCC.cpp
  └─> Server.hpp
```

### Responsabilités par fichier

| Fichier | Responsabilité | Fonctions principales |
|---------|----------------|----------------------|
| **main.cpp** | Point d'entrée | `main()` - parse args, crée Server, démarre |
| **User.cpp** | Gestion utilisateur | Getters/setters, buffer, authentification |
| **Channel.cpp** | Gestion canal | Membres, opérateurs, modes, invitations |
| **Utils.cpp** | Utilitaires | `getParam()`, `getChannelName()` |
| **Server.cpp** | Serveur principal | Socket, epoll, boucle événements |
| **handleUser.cpp** | Authentification | `handlePass()`, `handleNick()`, `handleUsername()` |
| **Join.cpp** | Commande JOIN | `handleJoin()`, `createChannel()`, `joinExistingChannel()` |
| **Part.cpp** | Commande PART | `handlePart()`, `notifyPart()` |
| **Kick.cpp** | Commande KICK | `handleKick()`, `broadcastKickConfirmation()` |
| **Topic.cpp** | Commande TOPIC | `handleTopic()`, `setTopic()`, `broadcastNewTopic()` |
| **Invite.cpp** | Commande INVITE | `handleInvite()`, `processToInvite()`, `notifyInvite()` |
| **Mode.cpp** | Commande MODE | `handleMode()`, `execMode()`, `setMode()` |
| **message.cpp** | Messages privés | `handlePrivateMessage()`, `sendPrivateMessage()` |
| **Replay.cpp** | Réponses IRC | `sendRPL_*()`, `sendERR_*()` |
| **DCC.cpp** | Transfert fichiers | `handleDCC()`, `sendFile()`, `getFile()` (bonus) |

---

## 🎯 Points d'entrée des commandes IRC

```
Commande IRC     →  Fonction gestionnaire        →  Fichier
──────────────────────────────────────────────────────────────
PASS <password>  →  handlePass()                 →  handleUser.cpp
NICK <nickname>  →  handleNick()                 →  handleUser.cpp
USER <username>  →  handleUsername()             →  handleUser.cpp
JOIN <#channel>  →  handleJoin()                 →  Join.cpp
PART <#channel>  →  handlePart()                 →  Part.cpp
PRIVMSG <target> →  handlePrivateMessage()       →  message.cpp
KICK <#chan> <u> →  handleKick()                 →  Kick.cpp
INVITE <u> <#c>  →  handleInvite()               →  Invite.cpp
TOPIC <#channel> →  handleTopic()                →  Topic.cpp
MODE <#chan> +i  →  handleMode()                 →  Mode.cpp
PING <server>    →  handlePing()                 →  Server.cpp
DCC SEND <file>  →  handleDCC()                  →  DCC.cpp
```

---

## 📝 Résumé de l'architecture

### Modèle architectural
- **Type**: Événementiel (event-driven) avec epoll
- **Pattern**: Single-threaded avec I/O non-bloquantes
- **Multiplexage**: epoll (Linux) pour surveiller tous les sockets

### Composants principaux
1. **Server** - Cœur du système, gère sockets et événements
2. **User** - Représente un client IRC connecté
3. **Channel** - Représente un canal de discussion
4. **Utils** - Fonctions utilitaires de parsing

### Flux de données
1. Client se connecte → `acceptUser()` crée un `User`
2. Client envoie commandes → `parseInput()` dispatch vers handlers
3. Handlers modifient l'état (User, Channel)
4. Réponses envoyées via `send()` vers clients concernés

### Modularité
- Chaque commande IRC a son propre fichier
- Séparation claire des responsabilités
- Code réutilisable et maintenable

---

**Ce document permet de comprendre l'organisation complète du code et les interactions entre tous les composants du serveur IRC.**
