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
class MetaHeuristique_Fourmis
{
private:
	Instance* instance;
	Solution* solution;
	
	vector<int> pi_POI;
	vector<vector<int>> ppi_POI_par_Jour; // id_Jour : POI accessible entre hotel du debut de journee et hotel de fin de journee
	unordered_map<int, float> map_Score_POI; // Stocke les score recalculé des POI

	vector<pair<int, int>> pii_Hotels_par_Jour;

	unordered_map<int, vector<int>> map_conflit_POI; // Stocke la liste des journée qui peuvent intégrer le POI pour chaque POI

	vector<pair<int, vector<int>>> pp_Meilleure_Sequence_par_Jour; // Stocke les meilleures séquences à chaque jour

	vector<int> pi_Jours_Tries; // Stocke l'ordre dans lequel traiter les jours

	vector<vector<vector<int>>> pppi_Sequence_par_Jour;

	

	bool tri_par_score(const int& i1, const int& i2);
public :
	MetaHeuristique_Fourmis(Instance* instanceParam);
	static Solution* ExtraireSolution(Instance* instanceParam);
	void Solution();
	void Initialisation();
	vector<int> Randomisateur(vector<int> pi_POI);
	vector<vector<int>> GenerationNSequence(vector<int> pi_POI, int i_Nombre_de_Liste_A_Construire, int i_Jour);
	int GetScoreSequence(vector<int> pi_Sequence);
};