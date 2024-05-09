#pragma once
#include <unordered_map>
#include <map>
#include <algorithm>    
#include "..\..\Instance.hpp"
#include "..\..\Solution.hpp"
#include "Heuristique_v1.h"

#define HOTEL_OBSOLETE -2

#define POI_OBSOLETE false
#define POI_NON_OBSOLETE true

using namespace std;

/**
 * Adaptation de l'algorithme de colonie de fourmis :
 *	- n fourmis
 *  - m iterations
 * 
 * Pour chaque jours 
 *   |
 *   |	Cr�ation d'une matrice de ph�romones pour tous les liens possibles entre les POI du rayon de l'hotel de fin de journ�e et l'hotel de debut de journ�e (taille : n+1 x n)
 *   |
 *   |	Generer n fourmis et les placer al�atoirement
 *   |	
 *   |	Pour i de 0 � un nb d'iterations fix�
 *   |	  |
 *   |	  |		Pour chaque fourmis 
 *   |	  |		  |
 *   |	  |		  |		Si elle peut avancer sans depasser la duree max de la journee
 *   |	  |		  |			|	
 *   |	  |		  |			|	La faire avancer de un dans une direction al�atoire (en fonction des taux de pheromones)
 *   |	  |		  |			|	
 *   |	  |		  |			|	Elle depose des pheremones sur l'ensemble du chemin qu'elle a parcouru(pour donner plus de poids au long chemin), valant le score du nouveau POI atteind
 *   |	  |		  |			|	
 *   |	  |		  |		Sinon
 *   |	  |		  |			|	
 *   |	  |		  |			|	La faire partir d'un POI aleatoire en fonction des pheromones entre l'hotel de depart et les POI
 *   |	  |		  |			|	
 *   |	  |		  |		FinSi
 *   |	  |		FinPour
 *   |	  |
 *   |	  |		Evaporer l'ensemble des pheromones
 *	 |	FinPour
 *   |	
 *   |	Faire le meilleur trajet :
 *   |		- Partir du POI qui a le plus haut taux de pheromones sur son lien avec l'hotel de depart
 *   |		- Tant que la dur�e n'est pas d�pass� : Aller au POI qui a le plus grand taux de pheromones sur son lien avec le POI actuel
 *   |	
 *   |	Sauvegarder ce trajet dans Solution s'il am�liore la fonction objective
 */
class MetaHeuristique_Fourmis : public Heuristique_v1
{
public :
	/// <summary>
	/// Constructeur de MetaHeuristique_Fourmis
	/// </summary>
	/// <param name="instanceParam"> Instance a resoudre </param>
	MetaHeuristique_Fourmis(Instance* instanceParam);

	/// <summary>
	/// M�thode appel� dans le main pour lancer l'Heuristique puis la MetaHeuristique et stocker la meilleure solution dans Solution
	/// </summary>
	/// <param name="instanceParam"> Instance a resoudre </param>
	/// <returns></returns>
	static Solution* ExtraireSolution(Instance* instanceParam);

	/// <summary>
	/// D�roule l'algorithme de colonie de fourmis
	/// </summary>
	void SolutionMetaHeuristique();
};

/// <summary>
/// M�thode permettant de choisir al�artoirement un index � partir d'une ligne de la matrice de pheromones
/// </summary>
/// <param name="array"> Ligne de la matrice de pheromones </param>
/// <returns></returns>
int choisirIndex(const std::vector<double>& array);

/// <summary>
/// M�thode permettant de choisir al�artoirement un index � partir d'une ligne de la matrice de pheromones
/// <param name="array"> Ligne de la matrice de pheromones </param>
/// <param name="pi_POI"> POI possibles </param>
/// <param name="pi_POI_Disponibles"> Indique si les POI ont deja etes parcourus par la fourmis </param>
/// <param name="f_Duree_Trajet"> Dur�e actuelle du parcours de la fourmis</param>
/// <param name="instance"></param>
/// <param name="i_POI_Depart"> POI actuel de la fourmis </param>
/// <param name="i_Hotel_Arrive"> Hotel de fin de journ�e </param>
/// <param name="f_Duree_Max"> Dur�e maximale de la journ�e </param>
/// <param name="f_Heure_Debut"> Heure de d�part de la fourmis </param>
/// <returns></returns>
int choisirIndex_Ameliore(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const float f_Duree_Trajet, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, float f_Duree_Max, float f_Heure_Debut);

/// <summary>
/// M�thode permettant de choisir le "meilleur" index � partir d'une ligne de la matrice de pheromones
/// </summary>
/// <param name="array"> Ligne de la matrice de pheromones </param>
/// <returns></returns>
int choisirIndex_Fin(const std::vector<double>& array);

/// <summary>
/// M�thode permettant de choisir le "meilleur" index � partir d'une ligne de la matrice de pheromones
/// <param name="array"> Ligne de la matrice de pheromones </param>
/// <param name="pi_POI"> POI possibles </param>
/// <param name="pi_POI_Disponibles"> Indique si les POI ont deja etes parcourus par la fourmis </param>
/// <param name="f_Duree_Trajet"> Dur�e actuelle du parcours de la fourmis</param>
/// <param name="instance"></param>
/// <param name="i_POI_Depart"> POI actuel de la fourmis </param>
/// <param name="i_Hotel_Arrive"> Hotel de fin de journ�e </param>
/// <param name="f_Duree_Max"> Dur�e maximale de la journ�e </param>
/// <param name="f_Heure_Debut"> Heure de d�part de la fourmis </param>
/// <returns></returns>
int choisirIndex_Fin(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const float f_Duree_Trajet, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, float f_Duree_Max, float f_Heure_Debut);