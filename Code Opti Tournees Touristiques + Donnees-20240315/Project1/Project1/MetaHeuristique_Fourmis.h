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
 * Adaptation de l'algorithme de fourmis :
 *	- n fourmis
 *  - m iterations
 * 
 * Pour chaque jours 
 *   |
 *   |	Création d'une matrice de phéromones pour tous les liens possibles entre les POI du rayon de l'hotel de fin de journée et l'hotel de debut de journée (taille : n+1 x n)
 *   |
 *   |	Generer n fourmis et les placer aléatoirement
 *   |	
 *   |	Pour i de 0 à un nb d'iterations fixé
 *   |	  |
 *   |	  |		Pour chaque fourmis 
 *   |	  |		  |
 *   |	  |		  |		Si elle peut avancer sans depasser la duree max de la journee
 *   |	  |		  |			|	
 *   |	  |		  |			|	La faire avancer de un dans une direction aléatoire (en fonction des taux de pheromones)
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
 *   |		- Tant que la durée n'est pas dépassé : Aller au POI qui a le plus grand taux de pheromones sur son lien avec le POI actuel
 *   |	
 *   |	Sauvegarder ce trajet dans Solution s'il améliore la fonction objective
 */
class MetaHeuristique_Fourmis : public Heuristique_v1
{
private:	
	unordered_map<int, float> map_Score_POI; // Stocke les score recalculé des POI

	vector<vector<vector<int>>> pppi_Sequence_par_Jour;

public :
	MetaHeuristique_Fourmis(Instance* instanceParam);
	static Solution* ExtraireSolution(Instance* instanceParam);
	void SolutionMetaHeuristique();

private:
	bool tri_par_score(const int& i1, const int& i2);





	vector<int> Randomisateur(vector<int> pi_POI);
	vector<vector<int>> GenerationNSequence(vector<int> pi_POI, int i_Nombre_de_Liste_A_Construire, int i_Jour);
};

int choisirIndex(const std::vector<double>& array);
int choisirIndex_Ameliore(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const float f_Duree_Trajet, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, float f_Duree_Max, float f_Heure_Debut);
int choisirIndex_Fin(const std::vector<double>& array);
int choisirIndex_Fin(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const float f_Duree_Trajet, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, float f_Duree_Max, float f_Heure_Debut);