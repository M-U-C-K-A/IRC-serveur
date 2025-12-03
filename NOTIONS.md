# NOTIONS — 5 notions clés du projet Webserv

## 1) Bases du protocole HTTP
- Description: Comprendre la structure requête/réponse (ligne de requête, entêtes, corps), les codes de statut, les méthodes et la sémantique. HTTP/1.0 est la référence suggérée.
- Avantages: standardisation claire, compatibilité navigateurs, comparaison simple à NGINX, cadre robuste pour erreurs/redirections.
- Inconvénients: différences subtiles entre versions, cas limites d’entêtes, gestion correcte des codes et pages par défaut nécessaire.

## 2) I/O non-bloquantes et boucle d’événements unique
- Description: Utiliser `poll/select/epoll/kqueue` pour toutes les E/S sur sockets/pipes, avec FDs non-bloquants. Lecture/écriture surveillées simultanément, aucune I/O sans readiness.
- Avantages: scalabilité, contrôle unifié, efficace pour multi-ports, résistance aux blocages.
- Inconvénients: gestion d’état complexe par connexion, pas d’ajustement via `errno` après I/O, subtilités cross-platform (macOS `fcntl`).

## 3) Parsing de requêtes et construction de réponses
- Description: Parser ligne, entêtes, corps (chunked à dé-chunker), appliquer limites de taille, router correctement, construire réponse (entêtes, corps, erreurs, index, autoindex, redirections).
- Avantages: pipeline déterministe, flexibilité via config, mapping clair des erreurs FS → codes HTTP.
- Inconvénients: parsing multipart/upload complexe, gestion des timeouts pour éviter hangs, précision requise des codes et entêtes.

## 4) Routage piloté par configuration
- Description: Fichier de configuration inspiré de NGINX pour définir ports, pages d’erreur, limites, méthodes par route, `root`, `index`, autoindex, upload, redirections, CGI par extension.
- Avantages: séparation des politiques, grande flexibilité, démos faciles par variantes de config.
- Inconvénients: validation et priorité des routes, champs malformés, pas de regex (simplicité mais limitations), virtual hosts hors scope (optionnels).

## 5) Intégration CGI et gestion des uploads
- Description: Exécuter des programmes via CGI (PHP/Python, etc.) avec `fork/execve`, variables d’environnement, répertoire de travail, pipes non-bloquants intégrés à l’EventLoop ; uploads stockés à l’emplacement configuré.
- Avantages: contenu dynamique via interpréteurs existants, contrat environnemental clair, scénarios classiques couverts.
- Inconvénients: `fork` réservé au CGI, nuances EOF/`Content-Length`, complexité des pipes non-bloquants sous boucle unique, sécurité et chemins.

## Tableau de synthèse
Voir `TESTS_ET_EXERCICES.md` pour le mapping exercices ↔ notions ↔ exigences du sujet.