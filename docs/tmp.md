La distinction entre l'Opérateur de canal et l'Opérateur IRC est fondamentale dans le protocole Internet Relay Chat (IRC), car elle définit les niveaux de pouvoir et les domaines de responsabilité au sein du réseau,,.

### 1. Opérateur IRC (IRCOP)

L'Opérateur IRC (souvent appelé IRCOP) est une **classe spéciale de clients** autorisée à effectuer des **fonctions de maintenance générales au niveau du réseau**,. Leurs pouvoirs sont considérés comme "dangereux" mais nécessaires pour maintenir l'ordre et la stabilité de l'infrastructure IRC.

| Caractéristique | Détails |
| :--- | :--- |
| **Portée** | **Réseau entier** et infrastructure du serveur,. |
| **Acquisition** | Le statut est obtenu en envoyant la commande **`OPER`** avec la combinaison correcte de `<user>` et `<password>`. Après succès, le serveur informe le réseau en émettant un `MODE +o` pour le pseudonyme du client. |
| **Pouvoirs Clés** | Ils peuvent effectuer des tâches réseau fondamentales, comme déconnecter et reconnecter des serveurs si nécessaire pour empêcher un mauvais routage (`SQUIT` - Server Quit),,. Ils possèdent également le pouvoir controversé de **retirer un utilisateur du réseau par la force** en fermant sa connexion (`KILL`),. La commande `CONNECT` est également disponible uniquement aux Opérateurs IRC pour forcer une connexion entre serveurs. |
| **Identification** | L'Opérateur IRC est marqué par l'indicateur d'opérateur (`o` dans les modes utilisateur). |
| **Contrôle** | Les tentatives d'utiliser la commande `KILL` sur un serveur sont refusées, et une erreur `ERR_CANTKILLSERVER` (code 482) est renvoyée. |

### 2. Opérateur de Canal (Chanop)

L'Opérateur de canal (souvent appelé "chanop" ou "chop") est un statut qui s'applique uniquement à un **canal spécifique**,.

| Caractéristique | Détails |
| :--- | :--- |
| **Portée** | **Un canal donné** (un groupe de clients),. |
| **Acquisition** | Un client devient opérateur de canal implicitement s'il est le **premier client à rejoindre un canal** qui n'existe pas encore. Le statut peut être donné ou retiré par un autre opérateur de canal en utilisant la commande `MODE`. |
| **Pouvoirs Clés** | Les chanops sont considérés comme les "propriétaires" du canal et sont dotés de pouvoirs pour **garder le contrôle et l'ordre dans leur canal**. Ils ne sont pas tenus d'avoir des raisons pour leurs actions au sein de leur canal. |
| **Commandes exclusives** | `KICK`, `MODE`, `INVITE`, et `TOPIC` (si le mode `+t` est actif),,,. |
| **Identification** | Ils sont identifiés par le symbole **`@`** affiché à côté de leur pseudonyme dans les listes associées au canal (telles que les réponses aux commandes `NAMES`, `WHO` et `WHOIS`),,. |
| **Limitation** | Un utilisateur qui tente d'exécuter une commande réservée aux opérateurs de canal sans en avoir le statut reçoit l'erreur `ERR_CHANOPRIVSNEEDED` (code 482). |

### Schéma conceptuel de la hiérarchie

Bien que les sources n'offrent pas de schéma graphique direct, on peut représenter la différence de portée comme suit :

```mermaid
graph TD
    A[Opérateur IRC (IRCOP)] --> B(Réseau IRC : Gestion des Serveurs et Suppression de Clients Kill);
    B --> C(Serveur IRC : Master Socket, Flux de Données);
    C --> D(Canal #A)
    C --> E(Canal #B)
    D --> F[Opérateur de Canal (Chanop) @User_F];
    E --> G[Opérateur de Canal (Chanop) @User_G];
    D --> H(Utilisateur Simple);
    E --> I(Utilisateur Simple);
```

**L'Opérateur IRC** opère au niveau supérieur (A), affectant la santé globale du réseau (B) et des serveurs (C). **L'Opérateur de Canal** opère au niveau des canaux (D, E), où ses pouvoirs sont limités aux membres et aux modes de ce canal spécifique (F, G).

### Récapitulatif des commandes IRC mentionnées dans les sources

Les commandes IRC sont divisées en plusieurs catégories. La commande est soit un mot, soit un code à trois chiffres représentant une réponse numérique,.

#### A. Commandes d'enregistrement et de connexion (Client/Serveur)

Ces commandes sont utilisées pour établir ou mettre fin à une connexion :

| Catégorie | Commande | Description |
| :--- | :--- | :--- |
| **Authentification** | `PASS` | Définit le mot de passe de connexion. |
| | `NICK` | Définit ou change le pseudonyme de l'utilisateur. |
| | `USER` | Définit le nom d'utilisateur, le nom d'hôte, le nom de serveur et le nom réel. |
| **Déconnexion** | `QUIT` | Met fin à la session client et ferme la connexion. |

#### B. Commandes Opérateur IRC (IRCOP)

Ces commandes sont réservées aux opérateurs pour la maintenance du réseau :

| Catégorie | Commande | Description |
| :--- | :--- | :--- |
| **Privilèges** | `OPER` | Permet à un utilisateur normal d'obtenir les privilèges d'opérateur. |
| **Réseau/Serveur** | `SQUIT` | Indique la fermeture ou l'arrêt d'un serveur, disponible aux opérateurs pour maintenir le réseau,. |
| | `CONNECT` | Force un serveur à essayer d'établir une nouvelle connexion à un autre serveur. |
| **Utilisateur** | `KILL` | Permet de fermer la connexion d'un client par la force, utilisé par les serveurs en cas de collision de pseudonymes ou par les opérateurs,. |
| | `REHASH` | Force le serveur à relire son fichier de configuration. |
| | `RESTART` | Force le redémarrage d'un serveur (commande optionnelle). |

#### C. Commandes d'opérations de canal

Ces commandes manipulent les canaux ou leur contenu :

| Catégorie | Commande | Description |
| :--- | :--- | :--- |
| **Adhésion** | `JOIN` | Utilisé par un client pour commencer à écouter un canal spécifique. |
| | `PART` | Retire le client des utilisateurs actifs du canal. |
| | `INVITE` | Utilisé pour inviter des utilisateurs à un canal. **Réservé aux chanops** si le canal est en mode `+i`,. |
| | `KICK` | Utilisé pour retirer de force un utilisateur d'un canal,. **Réservé aux chanops**. |
| **Propriétés** | `TOPIC` | Change ou affiche le sujet d'un canal. **Réservé aux chanops** si le canal est en mode `+t`,. |
| | `MODE` | Commande à double usage pour changer les modes des utilisateurs ou des canaux. **Réservé aux chanops** pour changer les modes de canal `+o`, `+i`, `+t`, `+k`, `+l`,. |

#### D. Commandes de messagerie

Ces commandes effectuent la livraison de messages textuels :

| Catégorie | Commande | Description |
| :--- | :--- | :--- |
| **Message** | `PRIVMSG` | Utilisé pour envoyer des messages privés à des utilisateurs ou des canaux. |
| | `NOTICE` | Similaire à `PRIVMSG`, mais aucune réponse automatique ou erreur ne doit être envoyée par le serveur en retour. |
| | `WALLOPS` | Envoie un message à tous les opérateurs actuellement en ligne (commande optionnelle). |
| **Statut** | `AWAY` | Définit un message de réponse automatique pour les messages privés (commande optionnelle). |

#### E. Commandes de requête et diverses

Ces commandes servent à obtenir des informations ou à maintenir la connexion :

| Catégorie | Commande | Description |
| :--- | :--- | :--- |
| **Listes** | `NAMES` | Liste tous les pseudonymes visibles sur un canal ou sur tous les canaux. |
| | `LIST` | Liste les canaux et leurs sujets. |
| **Informations** | `WHO` | Génère une liste d'informations correspondant à un paramètre donné. |
| | `WHOIS` | Interroge des informations sur un utilisateur particulier. |
| | `WHOWAS` | Demande des informations sur un pseudonyme qui n'existe plus (recherche dans l'historique). |
| | `USERHOST` | Prend une liste de pseudonymes et retourne des informations. |
| | `ISON` | Vérifie si les pseudonymes donnés sont actuellement sur IRC. |
| | `VERSION` | Interroge la version du programme serveur. |
| | `STATS` | Interroge les statistiques du serveur. |
| | `LINKS` | Liste tous les serveurs connus du serveur. |
| | `TIME` | Interroge l'heure locale du serveur spécifié. |
| | `ADMIN` | Sert à trouver le nom de l'administrateur du serveur donné. |
| | `INFO` | Retourne des informations décrivant le serveur (version, compilation, etc.). |
| **Maintenance** | `PING` | Utilisé pour tester la présence d'un client ou serveur actif ; nécessite une réponse `PONG`,. |
| | `TRACE` | Utilisé pour trouver l'itinéraire vers un serveur spécifique. |
| | `ERROR` | Utilisé par les serveurs pour signaler une erreur sérieuse ou fatale. |
| | `USERS` | Retourne une liste des utilisateurs connectés au serveur (commande optionnelle). |
| | `SUMMON` | Utilisé pour inviter un utilisateur à rejoindre IRC s'il est connecté sur l'hôte du serveur (commande optionnelle). |