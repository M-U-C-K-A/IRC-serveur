# IRC Commands Implementation

## Structure des Commandes

Ce dossier contient l'implémentation de toutes les commandes IRC organisées en 5 catégories selon le RFC 1459.

```
srcs/commands/
├── registration/    (A) Commandes d'enregistrement et connexion
├── operator/        (B) Commandes IRC Operator (IRCOP)
├── channel/         (C) Commandes d'opérations de canal
├── messaging/       (D) Commandes de messagerie
└── query/           (E) Commandes de requête, d'information et de maintenance
```

---

## 🔑 La Distinction : Opérateur IRC vs Opérateur de Canal

La distinction entre l'Opérateur de canal et l'Opérateur IRC est fondamentale dans le protocole Internet Relay Chat (IRC), car elle définit les niveaux de pouvoir et les domaines de responsabilité au sein du réseau.

### 1. Opérateur IRC (IRCOP)

L'Opérateur IRC (souvent appelé IRCOP) est une **classe spéciale de clients** autorisée à effectuer des **fonctions de maintenance générales au niveau du réseau**.

| Caractéristique | Détails |
| :--- | :--- |
| **Portée** | **Réseau entier** et infrastructure du serveur. |
| **Acquisition** | Obtenu via la commande **`OPER`** avec credentials. Résultat: `MODE +o`. |
| **Pouvoirs Clés** | Maintenance réseau (`SQUIT`, `CONNECT`), suppression forcée d'utilisateurs (`KILL`, `RESTART`). |
| **Identification** | Marqué par l'indicateur d'opérateur (`o` dans les modes utilisateur). |

### 2. Opérateur de Canal (Chanop)

L'Opérateur de canal (souvent appelé "chanop" ou "chop") est un statut qui s'applique uniquement à un **canal spécifique**.

| Caractéristique | Détails |
| :--- | :--- |
| **Portée** | **Un canal donné**. |
| **Acquisition** | Premier client à rejoindre un canal ou nommé par un autre chanop via `MODE`. |
| **Pouvoirs Clés** | Contrôle total du canal (`KICK`, `INVITE`, `TOPIC`, `MODE`). |
| **Identification** | Identifié par le symbole **`@`** (ex: `@User`). |

```mermaid
graph TD
    A[Opérateur IRC (IRCOP)] --> B(Réseau IRC : Gestion Global et KILL);
    B --> C(Serveur IRC);
    C --> D(Canal #A)
    C --> E(Canal #B)
    D --> F[Opérateur de Canal (Chanop) @User_F];
    E --> G[Opérateur de Canal (Chanop) @User_G];
```

---

## 📋 (A) Registration - Commandes d'Enregistrement

Ces commandes sont utilisées pour établir ou mettre fin à une connexion :

| Fichier | Commande | Description | RFC |
|---------|----------|-------------|-----|
| `Pass.cpp` | **PASS** | Définit le mot de passe de connexion | RFC 1459 |
| `Nick.cpp` | **NICK** | Définit ou change le pseudonyme | RFC 1459 |
| `User.cpp` | **USER** | Définit username, hostname, realname | RFC 1459 |
| `Quit.cpp` | **QUIT** | Termine la session et ferme la connexion | RFC 1459 |

### Séquence de connexion :
```
1. PASS <password>
2. NICK <nickname>
3. USER <username> 0 * :<realname>
4. [Server sends welcome messages (001-004)]
```

---

## ⚡ (B) Operator - Commandes IRC Operator

Ces commandes sont réservées aux opérateurs IRC pour la maintenance du réseau :

| Fichier | Commande | Description | Privilège |
|---------|----------|-------------|-----------|
| `Oper.cpp` | **OPER** | Obtenir les privilèges d'opérateur | User |
| `Kill.cpp` | **KILL** | Forcer la déconnexion d'un utilisateur | IRCOP |
| `Squit.cpp` | **SQUIT** | Déconnecter un serveur du réseau | IRCOP |
| `Connect.cpp` | **CONNECT** | Forcer la connexion entre serveurs | IRCOP |
| `Rehash.cpp` | **REHASH** | Recharger la configuration du serveur | IRCOP |
| `Restart.cpp` | **RESTART** | Redémarrer le serveur | IRCOP |

### Note importante
Les IRCOPs possèdent des pouvoirs étendus mais ne sont pas automatiquement opérateurs de canal, sauf s'ils rejoignent un canal vide ou se font donner le statut.

---

## 🏠 (C) Channel - Commandes de Canal

Ces commandes manipulent les canaux et leur contenu :

| Fichier | Commande | Description | Privilège requis |
|---------|----------|-------------|------------------|
| `Join.cpp` | **JOIN** | Rejoindre ou créer un canal | User |
| `Part.cpp` | **PART** | Quitter un canal | User |
| `Invite.cpp` | **INVITE** | Inviter un utilisateur dans un canal | Chanop (+i) |
| `Kick.cpp` | **KICK** | Expulser un utilisateur d'un canal | Chanop |
| `Topic.cpp` | **TOPIC** | Changer/afficher le sujet du canal | Chanop (+t) |
| `Mode.cpp` | **MODE** | Changer les modes du canal/utilisateur | Chanop |

### Modes de canal gérés :
- `+i` (Invite-only), `+t` (Topic-lock), `+k` (Key/Password), `+o` (Operator status), `+l` (User limit)

---

## 💬 (D) Messaging - Commandes de Messagerie

Ces commandes effectuent la livraison de messages :

| Fichier | Commande | Description | Particularité |
|---------|----------|-------------|---------------|
| `Privmsg.cpp` | **PRIVMSG** | Envoyer un message privé ou de canal | Messages normaux |
| `Notice.cpp` | **NOTICE** | Similaire à PRIVMSG, sans réponse auto | Pas de réponse |
| `Wallops.cpp` | **WALLOPS** | Message à tous les opérateurs en ligne | IRCOP only |
| `Away.cpp` | **AWAY** | Définir/enlever le statut absent | Status user |

---

## 🔍 (E) Query - Commandes de Requête et Maintenance

Ces commandes servent à obtenir des informations, des statistiques ou gérer la maintenance :

| Fichier | Commande | Description | Réponse |
|---------|----------|-------------|---------|
| `Names.cpp` | **NAMES** | Liste les pseudonymes sur un canal | RPL_NAMREPLY (353) |
| `List.cpp` | **LIST** | Liste les canaux et leurs sujets | RPL_LIST (322) |
| `Whois.cpp` | **WHOIS** | Informations sur un utilisateur | RPL_WHOISUSER (311) |
| `Ping.cpp` | **PING** | Tester la présence active | PONG |
| `Version.cpp` | **VERSION** | Version du serveur | RPL_VERSION (351) |
| `Time.cpp` | **TIME** | Heure locale du serveur | RPL_TIME (391) |
| `Who.cpp` | **WHO** | Liste d'informations sur les utilisateurs | RPL_WHOREPLY (352) |
| `Whowas.cpp` | **WHOWAS** | Historique d'un pseudonyme | RPL_WHOWASUSER (314) |
| `Userhost.cpp` | **USERHOST** | Informations hôte sur des utilisateurs | RPL_USERHOST (302) |
| `Ison.cpp` | **ISON** | Vérifier si des utilisateurs sont en ligne | RPL_ISON (303) |
| `Stats.cpp` | **STATS** | Statistiques du serveur | RPL_STATS* (211+) |
| `Links.cpp` | **LINKS** | Liste des serveurs connectés | RPL_LINKS (364) |
| `Admin.cpp` | **ADMIN** | Informations administrateur | RPL_ADMIN* (256+) |
| `Info.cpp` | **INFO** | Informations détaillées du serveur | RPL_INFO (371) |
| `Trace.cpp` | **TRACE** | Route vers un serveur/client | RPL_TRACE* (200+) |
| `Error.cpp` | **ERROR** | Signalement d'erreur fatale | ERROR message |
| `Users.cpp` | **USERS** | Liste des utilisateurs connectés | RPL_USERS (393) |
| `Summon.cpp` | **SUMMON** | Inviter un utilisateur (souvent désactivé) | RPL_SUMMONING (342) |

---

## 🎨 Format des Fichiers

Chaque fichier de commande contient :

1. **ASCII Art en en-tête** : Diagramme de flux visuel (sur certaines commandes)
2. **Fonctions helper** : Parsing, validation, etc.
3. **Handler principal** : `handleCommandName()`
4. **Fonctions de réponse** : `sendRPL_*()` et `sendERR_*()`

### Exemple de structure fonction :
```cpp
/*
* this fonction will handle the [COMMAND] command
* @param clientFd the client file descriptor
* @param line the line to parse
* @return void
*/
void Server::handleCommand(const int &clientFd, const std::string &line) {
    // Implementation
}
```

---

## 📚 Références

- [RFC 1459](https://tools.ietf.org/html/rfc1459) - Internet Relay Chat Protocol
- [RFC 2810](https://tools.ietf.org/html/rfc2810) - Internet Relay Chat: Architecture
- [RFC 2811](https://tools.ietf.org/html/rfc2811) - Internet Relay Chat: Channel Management
- [RFC 2812](https://tools.ietf.org/html/rfc2812) - Internet Relay Chat: Client Protocol
- [RFC 2813](https://tools.ietf.org/html/rfc2813) - Internet Relay Chat: Server Protocol

---

## ✅ Status d'Implémentation

**Total : 38 commandes implémentées** 🎉
(Voir tableaux ci-dessus pour le détail par catégorie)
