# Webserv — Serveur HTTP en C++98

## Aperçu
Webserv est un serveur HTTP écrit en C++98, non-bloquant et piloté par un unique mécanisme d’attente (`poll`, `select`, `epoll` ou `kqueue`). Il sert des sites statiques, gère les méthodes HTTP essentielles (GET, POST, DELETE), les uploads, les pages d’erreur et l’exécution de CGI selon la configuration.

## Objectifs clés
- Implémenter un serveur HTTP compatible navigateurs (HTTP/1.0 comme référence).
- Utiliser une boucle d’événements unique et non-bloquante pour toutes les E/S réseau et pipes.
- Fournir un routage configurable à la manière d’un bloc `server` NGINX.
- Supporter au minimum GET, POST, DELETE, l’autoindex, redirections, upload et CGI.

## Exécution
```
./webserv [chemin/vers/configuration.conf]
```
- Si l’argument est omis, charger un chemin par défaut (défini par le projet).
- Plusieurs ports et interfaces peuvent être écoutés selon la configuration.

## Configuration (inspirée de NGINX)
Dans le fichier de configuration vous pouvez définir :
- Les paires `interface:port` à écouter (multi-sites).
- Les pages d’erreur par défaut ou personnalisées.
- La taille maximale du corps des requêtes.
- Des règles par route (sans regex) :
  - Méthodes HTTP acceptées.
  - Redirections (codes et destinations).
  - Racine de fichiers (`root`) et fichier index par défaut (`index`).
  - Activation/désactivation de l’autoindex.
  - Autorisation et emplacement de stockage des uploads.
  - Exécution de CGI par extension (ex. `.php`, `.py`).

## Contraintes et API système autorisées
- C++98, compilation avec `-Wall -Wextra -Werror -std=c++98`.
- Pas de bibliothèques externes (ni Boost).
- Robustesse : ne jamais crasher, ne jamais bloquer indéfiniment.
- Un seul mécanisme d’attente pour toutes les E/S (lecture et écriture surveillées).
- Interdiction d’appeler `read/recv` ou `write/send` sur sockets/pipes sans préparation par le mécanisme d’attente.
- `fork` uniquement pour CGI.
- Appels système autorisés : `execve, pipe, strerror, gai_strerror, errno, dup, dup2, fork, socketpair, htons, htonl, ntohs, ntohl, select, poll, epoll, kqueue, socket, accept, listen, send, recv, chdir, bind, connect, getaddrinfo, freeaddrinfo, setsockopt, getsockname, getprotobyname, fcntl (macOS: F_SETFL, O_NONBLOCK, FD_CLOEXEC), close, read, write, waitpid, kill, signal, access, stat, open, opendir, readdir, closedir`.

## Tests et validation
- Comparer le comportement à NGINX (entêtes, codes de statut, réponses).
- Tester avec `telnet` et via navigateurs.
- Écrire des tests de charge et de non-régression (Python/Go/C/C++).
- Fournir des fichiers de configuration et ressources par défaut pour la démonstration.

## Notions essentielles du projet
Voir `NOTIONS.md` pour une explication détaillée des 5 notions clés :
1. Bases du protocole HTTP.
2. I/O non-bloquantes et boucle d’événements.
3. Parsing de requêtes et construction de réponses.
4. Routage piloté par configuration.
5. Intégration CGI et gestion des uploads.

## Structure recommandée du code
Voir `STRUCTURE.md` pour une proposition d’architecture C++98 modulaire avec gestion d’états par connexion et un moteur d’événements unique.

## Exercices et démonstrations
Voir `TESTS_ET_EXERCICES.md` pour le tableau récapitulatif des tests à couvrir (statique, autoindex, erreurs, redirections, uploads, CGI, multi-port, DELETE).