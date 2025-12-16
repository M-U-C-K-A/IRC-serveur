# Guide complet irssi - Client IRC de référence

## 📖 Table des matières

- [Installation](#installation)
- [Démarrage rapide](#démarrage-rapide)
- [Commandes essentielles](#commandes-essentielles)
- [Navigation](#navigation)
- [Scénarios de test](#scénarios-de-test)
- [Configuration](#configuration)
- [Astuces](#astuces)

---

## 📦 Installation

### Ubuntu / Debian / Kali
```bash
sudo apt update
sudo apt install irssi
```

### macOS
```bash
brew install irssi
```

### Vérification
```bash
irssi --version
# Devrait afficher : irssi 1.x.x
```

---

## 🚀 Démarrage rapide

### 1. Lancer le serveur ft_irc

```bash
# Terminal 1
cd /home/hdelacou/Documents/irc
./ircserv 6667 secret123
```

### 2. Se connecter avec irssi

```bash
# Terminal 2
irssi
```

Une fois dans irssi :

```irc
/connect localhost 6667 secret123
/nick alice
/join #test
/msg #test Bonjour tout le monde !
```

### 3. Quitter

```irc
/quit Au revoir !
```

---

## 🎯 Commandes essentielles

### Connexion et identité

```irc
# Se connecter à un serveur
/connect <serveur> <port> [password]
/connect localhost 6667 secret123

# Définir votre pseudonyme
/nick <pseudo>
/nick alice

# Changer de pseudonyme
/nick bob

# Se déconnecter
/quit [message]
/quit À bientôt !

# Afficher infos serveur
/info
```

### Canaux

```irc
# Rejoindre un canal
/join <#canal> [mot_de_passe]
/join #general
/join #private motdepasse123

# Quitter un canal
/part <#canal> [raison]
/part #general
/part #general Au revoir !

# Liste des canaux disponibles (si implémenté)
/list

# Liste des utilisateurs du canal actif
/names
/names #general

# Quitter tous les canaux
/part *
```

### Messages

```irc
# Message public dans un canal
/msg <#canal> <message>
/msg #general Bonjour à tous !

# Message privé à un utilisateur
/msg <pseudo> <message>
/msg bob Salut Bob !

# Message dans le canal actif (sans commande)
Bonjour !

# Action (/me)
/me sourit
# Affiche : * alice sourit
```

### Informations

```irc
# Info sur un utilisateur
/whois <pseudo>
/whois bob

# Voir le sujet du canal
/topic
/topic #general

# Définir le sujet (opérateur requis)
/topic <#canal> <nouveau sujet>
/topic #general Bienvenue sur notre serveur IRC !
```

### Commandes opérateur

```irc
# Éjecter un utilisateur (KICK)
/kick <#canal> <pseudo> [raison]
/kick #general bob Comportement inapproprié

# Inviter un utilisateur
/invite <pseudo> <#canal>
/invite charlie #private

# Donner/retirer privilèges opérateur
/mode <#canal> +o <pseudo>
/mode #general +o bob
/mode #general -o bob

# Modes du canal
/mode <#canal> <+/-modes> [params]
/mode #general +i              # Invitation uniquement
/mode #general +t              # Topic restreint
/mode #general +k password123  # Mot de passe
/mode #general +l 50           # Limite 50 utilisateurs
/mode #general -i              # Retirer mode invitation

# Voir les modes actuels
/mode #general
```

---

## 🧭 Navigation dans irssi

### Fenêtres

```
Alt+1        - Fenêtre 1 (généralement statut)
Alt+2        - Fenêtre 2
Alt+[3-9]    - Fenêtres 3 à 9
Alt+0        - Fenêtre 10

/window 15   - Aller à la fenêtre 15
```

### Gestion des fenêtres

```irc
/window close              # Fermer la fenêtre actuelle
/window new                # Nouvelle fenêtre
/window list               # Liste des fenêtres
/window goto <#canal>      # Aller à la fenêtre du canal
/window move <numéro>      # Déplacer la fenêtre
```

### Affichage

```irc
/clear                     # Effacer l'écran
/lastlog                   # Voir l'historique
/lastlog alice             # Chercher "alice" dans l'historique
/set timestamp_format %H:%M  # Format de l'heure
```

---

## 🧪 Scénarios de test

### Test 1 : Connexion basique

```bash
# Terminal 1
./ircserv 6667 pass123

# Terminal 2
irssi
```

Dans irssi :
```irc
/connect localhost 6667 pass123
/nick alice
/join #test
/msg #test Mon premier message !
/quit
```

**Attendu :** 
- Messages de bienvenue (001-004)
- Rejoindre #test avec succès
- Message visible dans le canal

---

### Test 2 : Plusieurs utilisateurs

**Terminal 1** : Serveur
```bash
./ircserv 6667 pass123
```

**Terminal 2** : Alice
```bash
irssi
/connect localhost 6667 pass123
/nick alice
/join #general
```

**Terminal 3** : Bob
```bash
irssi
/connect localhost 6667 pass123
/nick bob
/join #general
```

**Dans Alice** :
```irc
/msg #general Salut Bob !
/msg bob Message privé
```

**Dans Bob** :
```irc
/msg #general Salut Alice !
/msg alice Merci !
```

**Attendu :**
- Les deux voient les messages du canal
- Messages privés fonctionnent
- Liste des membres correcte (`/names`)

---

### Test 3 : Opérateurs et modes

**Alice** (créateur = opérateur) :
```irc
/connect localhost 6667 pass123
/nick alice
/join #private
/topic #private Bienvenue dans notre canal privé
/mode #private +i
/mode #private +t
/mode #private +k secretpass
/invite bob #private
```

**Bob** :
```irc
/connect localhost 6667 pass123
/nick bob
/join #private secretpass
/topic #private Test
```

Le topic ne devrait pas changer (mode +t, Bob n'est pas opérateur)

**Alice donne les droits** :
```irc
/mode #private +o bob
```

**Bob réessaye** :
```irc
/topic #private Nouveau sujet
```

Maintenant ça devrait fonctionner !

---

### Test 4 : KICK et limites

**Alice** :
```irc
/join #test
/mode #test +l 2
/invite bob #test
/invite charlie #test
```

**Bob et Charlie** :
```irc
/join #test
```

**Dave** (3ème personne) :
```irc
/join #test
```

Dave ne devrait pas pouvoir rejoindre (limite atteinte)

**Alice** :
```irc
/kick bob Spamming
/mode #test -l
```

**Dave** :
```irc
/join #test
```

Maintenant Dave peut rejoindre !

---

## ⚙️ Configuration

### Fichier de configuration

Le fichier de configuration est dans : `~/.irssi/config`

### Configuration de base pour ft_irc

Créer `~/.irssi/config` :

```
servers = (
  {
    address = "localhost";
    chatnet = "FtIRC";
    port = "6667";
    password = "secret123";
    autoconnect = "yes";
  }
);

chatnets = {
  FtIRC = {
    type = "IRC";
    nick = "alice";
  };
};

channels = (
  { name = "#general"; chatnet = "FtIRC"; autojoin = "yes"; }
);

settings = {
  core = {
    real_name = "Alice Wonder";
    user_name = "alice";
    nick = "alice";
  };
  
  "fe-text" = {
    actlist_sort = "refnum";
  };
  
  "fe-common/core" = {
    theme = "default";
    autolog = "yes";
    timestamps = "ON";
    timestamp_format = "%H:%M:%S";
  };
};

ignores = ( );
```

Avec cette config, irssi se connecte automatiquement au lancement !

### Connexion automatique

```irc
/server add -auto -network FtIRC localhost 6667 secret123
/channel add -auto #general FtIRC
/save
```

Au prochain lancement d'irssi, connexion automatique !

---

## 💡 Astuces

### Raccourcis clavier utiles

```
Ctrl+P        - Commande précédente (historique)
Ctrl+N        - Commande suivante
Tab           - Complétion automatique
Ctrl+L        - Redessiner l'écran
Page Up/Down  - Défiler
```

### Couleurs

```irc
/set colors ON
/set colors OFF
```

### Logs

```irc
/set autolog ON                    # Activer logs automatiques
/set autolog_path ~/irclogs/%Y/$tag/$0.%m-%d.log
```

Les logs seront dans `~/irclogs/`

### Notifications

```irc
# Son lors de message privé
/set bell_beeps ON

# Surbrillance de votre pseudo
/hilight alice
```

### Scripts

irssi supporte des scripts Perl pour étendre les fonctionnalités :

```irc
/script load <script.pl>
/script list
/script unload <script.pl>
```

Scripts utiles : `awayproxy.pl`, `nickcolor.pl`, `usercount.pl`

### Debug

Pour voir les messages bruts IRC :

```irc
/set rawlog_lines 1000
/rawlog open ~/irc-raw.log
/rawlog save ~/irc-raw.log
/rawlog close
```

Utile pour débugger le protocole IRC !

---

## 🔍 Commandes avancées

### Alias

```irc
# Créer un alias
/alias wc window close
/alias j join

# Utilisation
/wc
/j #test
```

### Ignorer un utilisateur

```irc
/ignore <pseudo>
/ignore spammer
/unignore spammer
```

### Plusieurs serveurs

```irc
# Se connecter à plusieurs serveurs
/connect irc.freenode.net
/connect localhost 6667 secret123

# Changer de serveur actif
Ctrl+X
```

---

## 📚 Ressources

### Aide dans irssi

```irc
/help                    # Liste des commandes
/help <commande>         # Aide sur une commande
/help connect
/help mode
```

### Documentation officielle

- Site officiel : https://irssi.org/
- Documentation : https://irssi.org/documentation/
- Scripts : https://scripts.irssi.org/

### Thèmes

- https://irssi-import.github.io/themes/

---

## ✅ Checklist de test avec irssi

Pour tester complètement ft_irc avec irssi :

- [ ] Connexion au serveur
- [ ] Authentification avec mot de passe
- [ ] Définir un nickname
- [ ] Rejoindre un canal
- [ ] Envoyer des messages publics
- [ ] Envoyer des messages privés
- [ ] Voir/modifier le topic
- [ ] Utiliser les modes de canal (+i, +t, +k, +o, +l)
- [ ] Kick un utilisateur
- [ ] Inviter un utilisateur
- [ ] Tester avec plusieurs clients simultanés
- [ ] Déconnexion propre
- [ ] Déconnexion brutale (Ctrl+C)

---

**Bon test avec irssi ! 🚀**

Pour revenir au guide principal : [README.md](README.md)
