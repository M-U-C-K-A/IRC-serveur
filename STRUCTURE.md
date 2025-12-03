# STRUCTURE — Architecture recommandée pour Webserv

## Vue d’ensemble
Une architecture orientée événements, avec gestion d’état par connexion et découplage des responsabilités, facilite la conformité aux contraintes (C++98, non-bloquant, un seul poll/select).

## Modules principaux
- `Server`: initialise les sockets d’écoute (multi-port), charge la configuration, orchestre la boucle d’événements.
- `EventLoop`: encapsule `poll/select/epoll/kqueue`, surveille lecture/écriture, dispatch des événements.
- `Connection`: état par client (buffers lecture/écriture, timeouts, étape du parsing, file system ops en cours, CGI si présent).
- `HttpParser`: parse la requête (ligne de requête, entêtes, corps ; support chunked, limites de taille).
- `HttpResponse`: construit la réponse (statut, entêtes, corps ; pages d’erreur par défaut/perso, redirections, index, autoindex).
- `Router`: résout la route selon config (méthodes autorisées, root, index, autoindex, upload dir, CGI mapping, redirection).
- `CgiManager`: prépare env, `fork`/`execve`, pipes non-bloquants, lecture/écriture via EventLoop, gestion EOF/content-length.
- `Config`: parsing du fichier de configuration (blocs serveur, routes, erreurs, tailles, ports/interfaces).

## Flux des données
1. `EventLoop` signale la lisibilité/écriture des FDs.
2. `Connection` lit quand prêt, alimente `HttpParser` (state machine). Pour chunked, dé-chunk avant usage/CGI.
3. `Router` détermine l’action : fichier statique, directory listing, redirection, upload, CGI.
4. `HttpResponse` prépare les entêtes/corps ; `Connection` écrit lorsque prêt.
5. `CgiManager` utilise des pipes et FDs non-bloquants, intégrés à `EventLoop` (aucun I/O hors readiness).

## Gestion des erreurs et résilience
- Timeouts par connexion et par étapes (lecture entêtes, corps, CGI).
- Pages d’erreur par défaut si aucune fournie.
- Codes précis (404, 405, 413, 301/302/308, 500, etc.).
- Déconnexions clients correctement gérées.

## Organisation des fichiers (exemple)
- `src/server/Server.cpp`, `src/server/EventLoop.cpp`
- `src/http/HttpParser.cpp`, `src/http/HttpResponse.cpp`
- `src/routing/Router.cpp`, `src/config/Config.cpp`
- `src/cgi/CgiManager.cpp`, `src/core/Connection.cpp`
- `include/...` pour les headers
- `configs/example.conf` pour les exemples de configuration
- `www/` pour les ressources statiques de test

## Style et C++98
- Respect strict du C++98 (pas de `auto`, pas de `nullptr`).
- Pas de bibliothèques externes ; privilégier `<cstring>` à `<string.h>`.
- `-Wall -Wextra -Werror -std=c++98`.

## Non-bloquant et unique poll
- Tous les sockets/pipes/FDs non-bloquants.
- Aucune lecture/écriture sans readiness signalée par l’EventLoop.
- Lecture et écriture surveillées simultanément.

## Tests
- Scripts de tests (Python/Go) pour : statique, autoindex, redirections, erreurs, uploads, CGI, multi-port, déconnexions, charge.
- Comparaison avec NGINX pour entêtes et comportements.