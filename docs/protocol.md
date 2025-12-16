# Protocole IRC - Documentation détaillée

## 📋 Table des matières

- [Introduction au protocole IRC](#introduction-au-protocole-irc)
- [Format des messages](#format-des-messages)
- [Séquence de connexion](#séquence-de-connexion)
- [Commandes IRC obligatoires](#commandes-irc-obligatoires)
- [Codes de réponse](#codes-de-réponse)
- [Exemples de communication](#exemples-de-communication)

## 🌐 Introduction au protocole IRC

### Qu'est-ce que IRC ?

IRC (Internet Relay Chat) est un protocole de communication textuel créé en 1988. Il fonctionne selon le modèle client-serveur.

### Caractéristiques principales

- **Protocole texte** : Commandes et réponses en texte ASCII
- **Temps réel** : Communication instantanée
- **Multi-utilisateurs** : Support de nombreux clients simultanés
- **Canaux** : Salles de discussion publiques ou privées
- **Messages privés** : Communication directe entre utilisateurs

### Architecture

```
Client IRC 1 ─┐
Client IRC 2 ─┼─→ Serveur IRC (votre implémentation)
Client IRC 3 ─┘
```

**Note :** Dans ft_irc, nous n'implémentons PAS la communication serveur-à-serveur.

## 📝 Format des messages

### Structure générale

Format standard d'un message IRC :
```
[:prefix] <command> [param1] [param2] [...] [:trailing parameter]\r\n
```

### Composants

#### 1. Prefix (optionnel)
- Commence par `:`
- Indique l'origine du message
- Format : `:nickname!user@host` ou `:servername`
- **Exemple :** `:john!john@localhost`

#### 2. Command (obligatoire)
- Commande alphabétique (ex: `NICK`, `JOIN`, `PRIVMSG`)
- OU code numérique à 3 chiffres (ex: `001`, `353`)

#### 3. Parameters
- Séparés par des espaces
- Maximum 15 paramètres
- Le dernier paramètre peut contenir des espaces s'il est préfixé par `:`

#### 4. Fin de ligne
- **TOUJOURS** `\r\n` (CRLF)
- Jamais juste `\n` ou `\r`

### Exemples de messages

```irc
NICK john
USER john 0 * :John Doe
JOIN #general
PRIVMSG #general :Hello everyone!
PRIVMSG alice :Hi Alice!
TOPIC #general :Welcome to our channel
MODE #general +i
KICK #general bob :Spamming
```

### Limites importantes

- **Longueur maximale** : 512 caractères (incluant `\r\n`)
- **Longueur du message** : 510 caractères max (512 - 2 pour `\r\n`)
- **Nickname** : généralement 9 caractères max
- **Channel** : commence par `#` ou `&`

## 🔐 Séquence de connexion

### Étapes de connexion d'un client

```
1. Client se connecte au socket TCP
2. Serveur accepte la connexion
3. Client envoie : PASS <password>
4. Client envoie : NICK <nickname>
5. Client envoie : USER <username> <mode> <unused> :<realname>
6. Serveur valide et envoie RPL_WELCOME (001)
7. Client est connecté et peut utiliser les commandes
```

### Exemple complet

```irc
→ Client:  PASS secret123
→ Client:  NICK john
→ Client:  USER john 0 * :John Doe
← Serveur: :server 001 john :Welcome to the IRC Network john!john@localhost
← Serveur: :server 002 john :Your host is server, running version 1.0
← Serveur: :server 003 john :This server was created 2025-12-03
← Serveur: :server 004 john server 1.0 io itkol
```

## 📡 Commandes IRC obligatoires

### Commandes d'authentification et d'identité

#### PASS - Mot de passe de connexion
```irc
PASS <password>
```
- **Quand** : Première commande après connexion
- **Obligatoire** : Oui (avant NICK/USER)
- **Exemple** : `PASS secret123`

#### NICK - Définir/changer le nickname
```irc
NICK <nickname>
```
- **Description** : Définit ou change le pseudonyme
- **Exemple** : `NICK john`
- **Erreurs possibles** :
  - 431 ERR_NONICKNAMEGIVEN
  - 432 ERR_ERRONEUSNICKNAME
  - 433 ERR_NICKNAMEINUSE

#### USER - Informations utilisateur
```irc
USER <username> <mode> <unused> :<realname>
```
- **Paramètres** :
  - `username` : nom d'utilisateur
  - `mode` : généralement 0
  - `unused` : généralement *
  - `realname` : nom réel (peut contenir espaces)
- **Exemple** : `USER john 0 * :John Doe`

### Commandes de canal

#### JOIN - Rejoindre un canal
```irc
JOIN <channel> [key]
```
- **Description** : Rejoint un canal (le crée s'il n'existe pas)
- **Exemples** :
  - `JOIN #general`
  - `JOIN #private secret123` (avec mot de passe)
- **Réponse** : Liste des utilisateurs du canal

#### PART - Quitter un canal
```irc
PART <channel> [:<message>]
```
- **Exemple** : `PART #general :Goodbye!`

#### TOPIC - Voir/modifier le thème du canal
```irc
TOPIC <channel> [:<topic>]
```
- **Sans paramètre topic** : Affiche le thème actuel
- **Avec topic** : Change le thème (opérateur requis si mode +t)
- **Exemples** :
  - `TOPIC #general` (voir le thème)
  - `TOPIC #general :Welcome to our channel` (modifier)

### Commandes de communication

#### PRIVMSG - Envoyer un message
```irc
PRIVMSG <target> :<message>
```
- **Target** : nickname ou #channel
- **Exemples** :
  - `PRIVMSG #general :Hello everyone!` (message au canal)
  - `PRIVMSG alice :Hi Alice!` (message privé)

#### NOTICE - Envoyer une notice
```irc
NOTICE <target> :<message>
```
- Similaire à PRIVMSG mais ne déclenche pas de réponse automatique

### Commandes opérateur

#### KICK - Éjecter un utilisateur
```irc
KICK <channel> <user> [:<reason>]
```
- **Requis** : Privilèges opérateur
- **Exemple** : `KICK #general bob :Spamming`

#### INVITE - Inviter un utilisateur
```irc
INVITE <nickname> <channel>
```
- **Requis** : Opérateur (si mode +i actif)
- **Exemple** : `INVITE alice #private`

#### MODE - Modifier les modes

##### Mode utilisateur
```irc
MODE <channel> <+/-><modes> [<params>]
```

##### Modes de canal à implémenter

**Mode i (invite-only)**
```irc
MODE #general +i    # Activer invitation uniquement
MODE #general -i    # Désactiver
```

**Mode t (topic restriction)**
```irc
MODE #general +t    # Seuls les opérateurs changent le topic
MODE #general -t    # Tout le monde peut changer le topic
```

**Mode k (channel key/password)**
```irc
MODE #general +k secret123    # Définir mot de passe
MODE #general -k               # Retirer mot de passe
```

**Mode o (channel operator)**
```irc
MODE #general +o alice    # Donner privilèges opérateur à alice
MODE #general -o alice    # Retirer privilèges opérateur
```

**Mode l (user limit)**
```irc
MODE #general +l 10    # Limiter à 10 utilisateurs
MODE #general -l       # Retirer la limite
```

##### Exemples combinés
```irc
MODE #general +it              # Invite-only + topic restriction
MODE #general +kl secret 50    # Password + limite 50 users
MODE #general +o alice +o bob  # Donner op à alice et bob
```

### Commandes utilitaires

#### PING/PONG - Keep-alive
```irc
→ Serveur: PING :server
← Client:  PONG :server
```

#### QUIT - Déconnexion
```irc
QUIT [:<message>]
```
- **Exemple** : `QUIT :Goodbye!`

## 🔢 Codes de réponse

### Réponses de bienvenue (001-004)

```irc
001 RPL_WELCOME          :Welcome to the IRC Network <nick>!<user>@<host>
002 RPL_YOURHOST         :Your host is <servername>, running version <ver>
003 RPL_CREATED          :This server was created <date>
004 RPL_MYINFO           <servername> <version> <available user modes> <available channel modes>
```

### Réponses de commande

```irc
221 RPL_UMODEIS          <user mode string>
324 RPL_CHANNELMODEIS    <channel> <mode> <mode params>
331 RPL_NOTOPIC          <channel> :No topic is set
332 RPL_TOPIC            <channel> :<topic>
341 RPL_INVITING         <channel> <nick>
353 RPL_NAMREPLY         = <channel> :<nick list>
366 RPL_ENDOFNAMES       <channel> :End of /NAMES list
```

### Erreurs courantes

```irc
401 ERR_NOSUCHNICK       <nickname> :No such nick/channel
403 ERR_NOSUCHCHANNEL    <channel> :No such channel
404 ERR_CANNOTSENDTOCHAN <channel> :Cannot send to channel
411 ERR_NORECIPIENT      :No recipient given (<command>)
412 ERR_NOTEXTTOSEND     :No text to send
431 ERR_NONICKNAMEGIVEN  :No nickname given
432 ERR_ERRONEUSNICKNAME <nick> :Erroneous nickname
433 ERR_NICKNAMEINUSE    <nick> :Nickname is already in use
441 ERR_USERNOTINCHANNEL <nick> <channel> :They aren't on that channel
442 ERR_NOTONCHANNEL     <channel> :You're not on that channel
443 ERR_USERONCHANNEL    <user> <channel> :is already on channel
461 ERR_NEEDMOREPARAMS   <command> :Not enough parameters
462 ERR_ALREADYREGISTRED :You may not reregister
464 ERR_PASSWDMISMATCH   :Password incorrect
471 ERR_CHANNELISFULL    <channel> :Cannot join channel (+l)
473 ERR_INVITEONLYCHAN   <channel> :Cannot join channel (+i)
475 ERR_BADCHANNELKEY    <channel> :Cannot join channel (+k)
482 ERR_CHANOPRIVSNEEDED <channel> :You're not channel operator
```

## 💬 Exemples de communication

### Exemple 1 : Connexion complète

```irc
→ C: PASS secret123
→ C: NICK alice
→ C: USER alice 0 * :Alice Wonder
← S: :server 001 alice :Welcome to the IRC Network alice!alice@localhost
← S: :server 002 alice :Your host is server, running version 1.0
← S: :server 003 alice :This server was created 2025-12-03
← S: :server 004 alice server 1.0 io itkol
```

### Exemple 2 : Rejoindre un canal et discuter

```irc
→ C: JOIN #general
← S: :alice!alice@localhost JOIN #general
← S: :server 353 alice = #general :alice @operator
← S: :server 366 alice #general :End of /NAMES list
→ C: PRIVMSG #general :Hello everyone!
← S: :alice!alice@localhost PRIVMSG #general :Hello everyone!
```

### Exemple 3 : Opérations d'opérateur

```irc
→ C: MODE #general +i
← S: :alice!alice@localhost MODE #general +i
→ C: INVITE bob #general
← S: :server 341 alice #general bob
→ C: TOPIC #general :Welcome to our private channel
← S: :alice!alice@localhost TOPIC #general :Welcome to our private channel
→ C: MODE #general +o bob
← S: :alice!alice@localhost MODE #general +o bob
```

### Exemple 4 : Message privé

```irc
→ C: PRIVMSG bob :Hi Bob, how are you?
← S: :alice!alice@localhost PRIVMSG bob :Hi Bob, how are you?
```

### Exemple 5 : Gestion d'erreurs

```irc
→ C: JOIN #private secretwrong
← S: :server 475 alice #private :Cannot join channel (+k)
→ C: KICK #general charlie :Bye
← S: :server 482 alice #general :You're not channel operator
→ C: NICK bob
← S: :server 433 alice bob :Nickname is already in use
```

## 📚 Ressources

### Spécifications officielles (RFCs)

- **RFC 1459** : Internet Relay Chat Protocol (original)
- **RFC 2810** : IRC Architecture
- **RFC 2811** : IRC Channel Management
- **RFC 2812** : IRC Client Protocol (modern)
- **RFC 2813** : IRC Server Protocol

### Documentation recommandée

- Modern IRC Documentation : https://modern.ircdocs.horse/
- IRC Numeric List : https://www.alien.net.au/irc/irc2numerics.html

### Notes importantes pour ft_irc

- Implémenter **uniquement** les commandes demandées dans le sujet
- Assurer la compatibilité avec un client IRC standard
- Gérer correctement les messages fragmentés
- Respecter le format exact des réponses numériques
- Tester avec un vrai client IRC (irssi, WeeChat, HexChat)

---

**Retour à :** [README principal](../README.md)
