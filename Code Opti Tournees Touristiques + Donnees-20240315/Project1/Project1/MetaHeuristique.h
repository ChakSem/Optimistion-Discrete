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
 * Am�lioration de la solution de l'heuristique :
 *  - On concid�re que la s�quence d'hotels est optimale, on cherchera donc � am�liorer les s�quences de POI � chaque jours
 * 
 * Donn�es :
 *  - Solution :
 *		- Hotel de debut / fin trouv�s � chaque jours
 *		- Sequence de POI � chaque jour
 * - Instance
 * - POI inserables � chaque jours
 * 
 * D�roulement :
 *  - Identifier POI communs a plusieurs hotels 
 *  - Trouver bonne solution pour chaque jour � l'aide de meilleure sequence
 *  - Identifier POI communs � plusieurs sequences
 *  - Gerer conflits
 */
class MetaHeuristique
{
private:
	Instance* instance;
	Solution* solution;
	
	vector<int> pi_POI;
	vector<vector<int>> ppi_POI_par_Jour; // id_Jour : POI accessible entre hotel du debut de journee et hotel de fin de journee
	unordered_map<int, float> map_Score_POI; // Stocke les score recalcul� des POI

	unordered_map<int, vector<int>> map_conflit_POI; // Stocke la liste des journ�e qui peuvent int�grer le POI pour chaque POI

	vector<pair<int, vector<int>>> pp_Meilleure_Sequence_par_Jour; // Stocke les meilleures s�quences � chaque jour
public :
	MetaHeuristique(Instance* instanceParam);
	static Solution* ExtraireSolution(Instance* instanceParam);
	void Solution();
	void Initialisation();
};