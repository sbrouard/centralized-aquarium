Aquarium centralisé
==================

Orgnisation du projet:
---------------------

Cet aquarium est géré par:
* un controleur central (dont le code se trouve dans le répertoire `/server/`) 
* des vues (dont le code se trouve dans le répertoire `/client/`)
 

### Commandes du client (vue)

* Éxécuter la vue: `make client`
* Ouvrir la connexion avec le serveur: `hello [in as ID]`
* ajouter un poisson: `addFish FishName at posXxposY,widthxlength,moves_algo` (moves_algo = "RandomWayPoint")`
* Supprimer un poisson: `delFish FishName`
* Lancer un poisson: `startFish FishName`
* Récupérer les informations sur les poissons: `getFishes`
* Récupérer les informations sur les poissons en continu: `getFishesContinuously`
* Envoyer un ping au serveur: `ping` (géré automatiquement)
* Se déconnecter: `logout`

### Commandes du serveur (controlleur)

#### Liste des poissons gérés par les vues:
* barbicha
* barpau
* kyogre
* leviator
* loupio
* magicarpe
* poissirene<br>

Attention à bien respecter la syntaxe pour le bon fonctionnment de l'aquarium (celui-ci est sensible à la casse)




Centralized aquarium
====================

Introduction
-------------

This is a centralized aquarium: The server is the controller, he manages all the fishes: their position, their moves, their size, etc. And the views, each view shows a part of the aquarium.

Host (view)
------------

An host is a view, he displays a part of the aquarium. He can ask the controller to add or remove a fish, to get the fishes, etc.

### Instructions

First modify the file affichage.cfg, in particular the controller address.
The commands are:
* run the program: make aquarium
* initiate connexion with the server: hello
* add a fish: addFish FishName at posXxposY,widthxlength,moves_algo (moves_algo = "RandomWayPoint")
* delete a fish: delFish FishName
* run a fish: startFish FishName
* get the fishes informations: getFishes
* get the fishes informations regularly: getFishesContinuously
* tell the server the host is still connected: ping
* log out: log out

Server (controller):
--------------------

The commands are:
* run the programme: make then ./server