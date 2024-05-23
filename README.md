Un support d'explication de notre solution peut être trouvé ici :
https://www.canva.com/design/DAGE6JhWHuU/Pz1M2skjac1F5dXPPaeE-g/edit?utm_content=DAGE6JhWHuU&utm_campaign=designshare&utm_medium=link2&utm_source=sharebutton

# Sujet
Réaliser une Heuristique et une Méta-Heuristique pour résoudre un problème _de conception d’itinéraires touristiques_.

# Le Problème
## Les données :
- n hotels : coordonnees **x**, **y**
- m points d'intérêts (POI) : coordonnées **x**, **y** , un **score** et des **horaires d'ouvertures**
- k jours : une **durée maximale de trajet** (par jour)
- une matrice _n+m x n+m_ de distance (distance + temps de visite dans le cas des POI)
- un hotel de debut **Hi**
- un hotel de fin **Ha**

## Les variables :
- Un hotel de fin pour chaque jour
- Un hotel de début pour chaque jour (hotel de fin du jour précédent)
- Une séquence de POI pour chaque POI (POI traverse entre l'hôtel de début et l'hôtel de fin de journée)
- Une **heure de départ** pour chaque jour
- Une **durée de trajet** pour chaque jour (Soit x,y un couple d'élement successsif dans une journee i (hotel ou POI) :  
duree jour i = (Σ(x,y) max(dist x,y ; ouverture y) - heure de départ )


## Contraites :
- Chaque jour commence par un hôtel et finit par un hôtel
- Chaque POI ne peut être traversé qu'**une seule fois**
- Un POI ne peut être visité en dehors de ces horaires d'ouverture (si on arrive avant l'ouverture => on attend l'ouverture ce qui augmente donc le temps de trajet)
- La durée de trajet d'un jour ne peut dépasser la durée maximale de trajet de ce jour

## Objectif :
Trouver un itinéraire pour chaque jour de sorte à maximiser la somme des score des POI traversé

##

_Arthur Crochemore et Alâa Chakori Semmane_