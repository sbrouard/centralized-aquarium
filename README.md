Centralized aquarium


This is a centralized aquarium: The server is the controller, he manages all the fishes: their position, their moves, their size, etc. And the views, each view shows a part of the aquarium.
An host is a view, he displays a part of the aquarium. He can ask the controller to add or remove a fish, to get the fishes, etc.
Client (view):

First modify the file affichage.cfg, in particular the controller address.
The commands are:
*run the program: make aquarium
*initiate connexion with the server: hello
*add a fish: addFish FishName at posXxposY,widthxlength,moves_algo (moves_algo = "RandomWayPoint" or "PathWay")
*delete a fish: delFish FishName
*run a fish: startFish FishName
*get the fishes informations: getFishes
*get the fishes informations regularly: getFishesContinuously
*tell the server the host is still connected: ping
*log out: log out
Server (controller):

The commands are:
*run the programme: make then ./server