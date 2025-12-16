# État du Refactoring - 16 Décembre 2025

## ✅ Fichiers Créés et Complétés

### Infrastructure
- [x] `/includes/IrcReplies.hpp` - Toutes les constantes IRC centralisées
- [x] `/srcs/IrcReplies.cpp` - Toutes les fonctions d'erreur et réponses centralisées

### Fichiers Refactorisés
- [x] `/srcs/commands/registration/Pass.cpp` - Clean, utilise IrcReplies
- [x] `/srcs/commands/messaging/Away.cpp` - Clean, caractères corrects
- [x] `/srcs/commands/messaging/Notice.cpp` - Clean, caractères corrects

## 🔧 Fichiers à Corriger

### Problèmes Identifiés

Tous les fichiers générés ont les **mêmes problèmes** :

1. **Caractères échappés incorrectement** :
   - `\"` au lieu de `"`
   - `\\r\\n` au lieu de `\r\n`
   - `\\` au lieu de `\`

2. **Boîtes ASCII au-dessus de chaque fonction** :
   ```cpp
   /*
   **  ┌─────────────────────────────────────────┐
   **  │     Function Name                       │
   **  └─────────────────────────────────────────┘
   */
   ```
   **→ À RETIRER COMPLÈTEMENT**

3. **Commentaires inline partout** :
   ```cpp
   // Check if empty or too long
   if (nickname.empty() || nickname.length() > 9)
       return false;
   ```
   **→ À RETIRER**

4. **Fonctions d'erreur dans chaque fichier** :
   - Doivent utiliser celles de `IrcReplies.cpp`
   - **→ À RETIRER des fichiers individuels**

### Format Attendu

```cpp
/*
* Function description
* @param param1 - Description
* @param param2 - Description  
* @return Type - Description
*/
void Server::functionName(const int &param1, const std::string &param2)
{
    if (condition)
    {
        sendERR_XXX(clientFd);
        return;
    }
    
    std::string var = "value";
    doSomething();
}
```

## 📋 Liste des Fichiers à Corriger

### Registration (3 restants)
- [ ] `/srcs/commands/registration/Nick.cpp`
- [ ] `/srcs/commands/registration/User.cpp`
- [ ] `/srcs/commands/registration/Quit.cpp`

### Operator (6 fichiers)
- [ ] `/srcs/commands/operator/Oper.cpp`
- [ ] `/srcs/commands/operator/Kill.cpp`
- [ ] `/srcs/commands/operator/Squit.cpp`
- [ ] `/srcs/commands/operator/Connect.cpp`
- [ ] `/srcs/commands/operator/Rehash.cpp`
- [ ] `/srcs/commands/operator/Restart.cpp`

### Channel (1 fichier)
- [ ] `/srcs/commands/channel/Join.cpp`

### Messaging (2 restants)
- [ ] `/srcs/commands/messaging/Privmsg.cpp` (vide, à implémenter)
- [ ] `/srcs/commands/messaging/Wallops.cpp`

### Query (6 fichiers)
- [ ] `/srcs/commands/query/Names.cpp`
- [ ] `/srcs/commands/query/List.cpp`
- [ ] `/srcs/commands/query/Whois.cpp`
- [ ] `/srcs/commands/query/Version.cpp`
- [ ] `/srcs/commands/query/Ping.cpp`
- [ ] `/srcs/commands/query/Time.cpp`

## 🎯 Actions à Faire

### 1. Pour chaque fichier :

```bash
# Remplacer :
\"          → "
\\r\\n      → \r\n ou IRC_CRLF
\\          → \

# Retirer :
- Toutes les boîtes ASCII ┌─────┐
- Tous les commentaires inline //
- Toutes les fonctions sendERR_* et sendRPL_* (déjà dans IrcReplies.cpp)

# Ajouter :
#include "../../../includes/IrcReplies.hpp"

# Garder :
- ASCII art de flux en en-tête (grande boîte)
- Commentaires Doxygen au-dessus des fonctions
```

### 2. Vérifier Server.hpp

Toutes les fonctions doivent être déclarées :
```cpp
class Server {
    // Error functions
    void sendERR_NOSUCHNICK(const int &clientFd, const std::string &nickname);
    void sendERR_NEEDMOREPARAMS(const int &clientFd, const std::string &command);
    // ... etc
    
    // Command handlers
    void handlePass(const int &clientFd, const std::string &line);
    void handleNick(const int &clientFd, const std::string &line);
    // ... etc
};
```

### 3. Mettre à jour Makefile

Ajouter `IrcReplies.cpp` à la compilation :
```makefile
SRCS = srcs/main.cpp \
       srcs/Server.cpp \
       srcs/IrcReplies.cpp \
       srcs/commands/registration/Pass.cpp \
       # ... etc
```

## 📊 Progression

- **Fichiers créés** : 2/2 (100%) ✅
- **Fichiers refactorisés** : 3/21 (14%)
- **Fichiers restants** : 18/21 (86%)

## 🚀 Commande de Test Rapide

```bash
# Pour vérifier les caractères échappés :
grep -r '\\\"' srcs/commands/

# Pour trouver les boîtes ASCII :
grep -r '┌─' srcs/commands/

# Pour trouver les sendERR_ locaux :
grep -r 'void Server::sendERR_' srcs/commands/
```

## ✨ Résultat Final Attendu

- ✅ Code propre et lisible
- ✅ Pas de duplication
- ✅ Toutes les erreurs centralisées
- ✅ Format uniforme
- ✅ Prêt pour compilation
