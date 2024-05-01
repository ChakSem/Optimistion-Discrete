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
 * Amélioration de la solution de l'heuristique :
 *  - On concidère que la séquence d'hotels est optimale, on cherchera donc à améliorer les séquences de POI à chaque jours
 * 
 * Données :
 *  - Solution :
 *		- Hotel de debut / fin trouvés à chaque jours
 *		- Sequence de POI à chaque jour
 * - Instance
 * - POI inserables à chaque jours
 * 
 * Déroulement :
 *  - Identifier POI communs a plusieurs hotels 
 *  - Trouver bonne solution pour chaque jour à l'aide de meilleure sequence
 *  - Identifier POI communs à plusieurs sequences
 *  - Gerer conflits
 * 
 * Idées :
 *  - Faire un calcul de meilleure journée plus poussé
 *  - Déterminer la "valeur" des POI partagés pour chaque journée, le placé dans la journée avec sa plus grande valeur
 */
class MetaHeuristique_Fourmis : public Heuristique_v1
{
private:	
	unordered_map<int, float> map_Score_POI; // Stocke les score recalculé des POI



	vector<vector<vector<int>>> pppi_Sequence_par_Jour;

	

	bool tri_par_score(const int& i1, const int& i2);
public :
	MetaHeuristique_Fourmis(Instance* instanceParam);
	static Solution* ExtraireSolution(Instance* instanceParam);
	void SolutionMetaHeuristique();
	vector<int> Randomisateur(vector<int> pi_POI);
	vector<vector<int>> GenerationNSequence(vector<int> pi_POI, int i_Nombre_de_Liste_A_Construire, int i_Jour);
};