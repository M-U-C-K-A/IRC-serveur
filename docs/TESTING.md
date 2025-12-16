# Guide de test - ft_irc

## 🚀 Démarrage rapide

### 1. Compilation
```bash
make
```

### 2. Lancer le serveur
```bash
./ircserv 6667 motdepasse123
```

Le serveur écoute maintenant sur le port 6667.

---

## ✅ Tests automatiques

### Script de test basique
```bash
chmod +x test.sh
./test.sh
```

Ce script teste :
- ✓ Démarrage du serveur
- ✓ Connexion client
- ✓ Authentification
- ✓ Messages fragmentés

---

## 🧪 Tests manuels

### Test 1 : Connexion avec netcat

**Terminal 1** (serveur) :
```bash
./ircserv 6667 secret123
```

**Terminal 2** (client) :
```bash
nc localhost 6667
```

Tapez :
```irc
PASS secret123
NICK alice
USER alice 0 * :Alice Wonderland
```

**Résultat attendu** : Messages de bienvenue (001, 002, 003, 004)

---

### Test 2 : Messages fragmentés (nc)

**Terminal 1** (serveur) :
```bash
./ircserv 6667 secret123
```

**Terminal 2** (client) :
```bash
nc localhost 6667
```

Tapez en faisant Ctrl+D entre chaque fragment :
```
PASS sec
^D
ret12
^D
3
^D
```

Puis :
```
NICK al
^D
ice
^D
```

**Résultat attendu** : Le serveur doit reconstituer correctement les commandes.

---

## 🎯 Tests avec client IRC réel

### Option 1 : irssi (recommandé)

**Installation** :
```bash
# Ubuntu/Debian
sudo apt install irssi

# macOS
brew install irssi
```

**Utilisation** :
```bash
irssi
```

Dans irssi :
```irc
/connect localhost 6667 secret123
/nick alice
/join #test
/msg #test Bonjour tout le monde !
/topic #test Bienvenue sur notre canal
/mode #test +it
/quit
```

### Option 2 : WeeChat

**Installation** :
```bash
# Ubuntu/Debian
sudo apt install weechat

# macOS
brew install weechat
```

**Utilisation** :
```bash
weechat
```

Dans WeeChat :
```irc
/server add local localhost/6667 -password=secret123
/connect local
/nick bob
/join #general
/msg #general Hello!
```

### Option 3 : HexChat (GUI)

**Installation** :
```bash
# Ubuntu/Debian
sudo apt install hexchat

# macOS
brew install --cask hexchat
```

Configuration :
- Server: localhost
- Port: 6667
- Password: secret123

---

## 🔬 Scénarios de test détaillés

### Scénario 1 : Utilisateur unique

```bash
# Terminal 1 : Serveur
./ircserv 6667 pass123

# Terminal 2 : Client
irssi
/connect localhost 6667 pass123
/nick alice
/join #test
/msg #test Hello world
/part #test
/quit
```

**Vérifier** :
- ✓ Connexion acceptée
- ✓ Messages de bienvenue
- ✓ Canal créé
- ✓ Message envoyé
- ✓ Déconnexion propre

---

### Scénario 2 : Plusieurs utilisateurs

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
/msg #general Bonjour Bob !
/msg bob Message privé pour toi
```

**Dans Bob** :
```irc
/msg #general Salut Alice !
/msg alice Merci pour le message !
```

**Vérifier** :
- ✓ Les deux utilisateurs voient les messages du canal
- ✓ Les messages privés fonctionnent
- ✓ La liste des utilisateurs est correcte (`/names #general`)

---

### Scénario 3 : Opérateurs et modes

**Terminal 1** : Serveur
```bash
./ircserv 6667 pass123
```

**Terminal 2** : Alice (créateur = opérateur)
```bash
irssi
/connect localhost 6667 pass123
/nick alice
/join #private
```

**Dans Alice** :
```irc
/topic #private Bienvenue dans notre canal privé
/mode #private +i          # Invitation uniquement
/mode #private +t          # Topic restreint
/mode #private +k secret   # Mot de passe
```

**Terminal 3** : Bob
```bash
irssi
/connect localhost 6667 pass123
/nick bob
/join #private secret      # Avec mot de passe
```

**Résultat attendu** : Bob ne peut pas rejoindre (mode +i)

**Dans Alice** :
```irc
/invite bob #private
```

**Dans Bob** :
```irc
/join #private secret      # Maintenant ça marche
/topic #private Test       # Devrait échouer (mode +t)
```

**Dans Alice** :
```irc
/mode #private +o bob      # Donner privilèges opérateur
```

**Dans Bob** :
```irc
/topic #private Nouveau topic  # Maintenant ça marche
```

---

### Scénario 4 : KICK et limite utilisateurs

**Alice** (opérateur) :
```irc
/join #test
/mode #test +l 2           # Limiter à 2 utilisateurs
/invite bob #test
/invite charlie #test
```

**Bob et Charlie** :
```irc
/join #test
```

**Terminal 4** : Dave
```bash
irssi
/connect localhost 6667 pass123
/nick dave
/join #test                # Devrait échouer (canal plein)
```

**Dans Alice** :
```irc
/kick bob Spamming
/mode #test -l             # Retirer la limite
```

**Dans Dave** :
```irc
/join #test                # Maintenant ça marche
```

---

## 📊 Checklist de validation

### Authentification
- [ ] PASS avec bon mot de passe → connexion réussie
- [ ] PASS avec mauvais mot de passe → refusé (464)
- [ ] NICK unique → accepté
- [ ] NICK déjà pris → erreur (433)
- [ ] USER défini → enregistrement complet
- [ ] Messages 001-004 reçus

### Canaux
- [ ] JOIN crée un canal s'il n'existe pas
- [ ] Le créateur devient opérateur
- [ ] PART quitte le canal
- [ ] Liste des membres correcte (353)
- [ ] Topic affiché/modifié

### Messages
- [ ] PRIVMSG vers canal → tous les membres reçoivent
- [ ] PRIVMSG vers utilisateur → message privé
- [ ] Messages fragmentés reconstitués
- [ ] Messages > 512 chars gérés

### Modes
- [ ] MODE +i → invitation requise
- [ ] MODE +t → topic restreint aux opérateurs
- [ ] MODE +k password → mot de passe requis
- [ ] MODE +o nick → donner privilèges opérateur
- [ ] MODE +l N → limiter à N utilisateurs

### Commandes opérateur
- [ ] KICK éjecte un utilisateur (opérateur requis)
- [ ] INVITE invite un utilisateur
- [ ] TOPIC modifie le thème (avec +t, opérateur requis)
- [ ] MODE change les modes (opérateur requis)

### Robustesse
- [ ] Connexions/déconnexions multiples
- [ ] Déconnexion brutale (Ctrl+C client)
- [ ] Commandes invalides ignorées
- [ ] Pas de crash avec beaucoup de clients
- [ ] Gestion mémoire (valgrind)

---

## 🐛 Tests de robustesse

### Test Valgrind (fuites mémoire)
```bash
valgrind --leak-check=full --show-leak-kinds=all ./ircserv 6667 pass123
```

Connectez-vous avec un client, testez quelques commandes, puis quittez.

**Résultat attendu** : 0 leaks

### Test de charge (plusieurs clients)
```bash
# Terminal 1
./ircserv 6667 pass123

# Terminaux 2-10 : Lancez plusieurs clients
for i in {1..10}; do
    (echo "PASS pass123\r\nNICK user$i\r\nUSER user$i 0 * :User $i\r\nJOIN #test\r\nPRIVMSG #test :Hello from user$i\r\nQUIT\r\n" | nc localhost 6667) &
done
```

**Vérifier** : Le serveur ne crash pas

### Test de déconnexion brutale
```bash
# Terminal 1
./ircserv 6667 pass123

# Terminal 2
nc localhost 6667
PASS pass123
# Ctrl+C (fermeture brutale)
```

**Vérifier** : Le serveur continue de fonctionner

---

## 📈 Tests de performance

### Mesurer le temps de réponse
```bash
time echo "PASS pass123\r\nNICK test\r\nUSER test 0 * :Test\r\nQUIT\r\n" | nc localhost 6667
```

### Mesurer l'utilisation CPU/mémoire
```bash
# Terminal 1
./ircserv 6667 pass123

# Terminal 2
top -p $(pgrep ircserv)
```

---

## 🎓 Exemples de commandes IRC

```irc
# Connexion
PASS secret123
NICK alice
USER alice 0 * :Alice Wonderland

# Canaux
JOIN #general
JOIN #private secretkey
PART #general :Au revoir !

# Messages
PRIVMSG #general :Bonjour tout le monde
PRIVMSG bob :Salut Bob !

# Topic
TOPIC #general
TOPIC #general :Nouveau sujet

# Modes
MODE #general +i
MODE #general +t
MODE #general +k password123
MODE #general +o bob
MODE #general +l 50
MODE #general -i
MODE #general

# Opérateur
KICK #general bob :Raison du kick
INVITE charlie #private
TOPIC #general :Sujet important

# Déconnexion
QUIT :À bientôt !
```

---

## 🔍 Debugging

### Afficher les messages du serveur
Ajoutez des `std::cout` dans votre code pour voir :
- Les connexions/déconnexions
- Les commandes reçues
- Les réponses envoyées

### Logger dans un fichier
```bash
./ircserv 6667 pass123 2>&1 | tee server.log
```

### Capturer le trafic réseau
```bash
sudo tcpdump -i lo -A port 6667
```

---

## ✨ Conseils

1. **Commencez simple** : Testez d'abord avec netcat, puis un vrai client IRC
2. **Un client à la fois** : Validez chaque fonctionnalité avant d'ajouter des clients
3. **Logs** : Ajoutez des logs pour comprendre ce qui se passe
4. **Comparaison** : Connectez-vous à un vrai serveur IRC pour comparer le comportement
5. **Documentation** : Gardez les RFCs sous la main (RFC 1459, 2812)

---

**Bon courage pour les tests ! 🚀**
