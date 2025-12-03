# TESTS ET EXERCICES — Récapitulatif

## Tableau récapitulatif

| Exercice / Test | Notions liées | Exigences du sujet |
|---|---|---|
| Servir fichiers statiques | HTTP, Parsing/Réponse, Routing | Site statique, codes précis, index par défaut, pages d’erreur |
| Autoindex (listing de répertoire) | Parsing/Réponse, Routing | Activer/désactiver par route |
| Pages d’erreur personnalisées | HTTP, Routing | Pages par défaut si absentes, config pour pages perso |
| Redirections HTTP | HTTP, Routing | Config de redirection par route, codes exacts |
| Uploads (POST) | Parsing/Réponse, CGI & Uploads, Routing | Autoriser uploads et définir emplacement, taille max du corps |
| Exécution CGI (PHP/Python) | CGI & Uploads, I/O non-bloquantes | `fork` uniquement pour CGI, env vars, dé-chunk input, EOF si pas de content-length |
| Multi-port / multi-interface | I/O non-bloquantes, Routing | Définir interface:port multiples ; un seul poll/select pour tout |
| Méthode DELETE | HTTP, Parsing/Réponse, Routing | Support GET, POST, DELETE ; 405 si non autorisée |
| Compatibilité navigateur + stress | I/O non-bloquantes, HTTP | Ne jamais bloquer indéfiniment ; gérer déconnexions ; robustesse |

## Conseils de test
- Comparer entêtes et comportements à NGINX.
- Utiliser `telnet` pour scénarios bruts et navigateurs pour validation UX.
- Écrire scripts (Python/Go/C++) pour charge, uploads, CGI, multi-port, timeouts.
- Fournir `configs/*.conf` et `www/` pour reproductions rapides.
