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
 *
 * Id�es :
 *  - Faire un calcul de meilleure journ�e plus pouss�
 *  - D�terminer la "valeur" des POI partag�s pour chaque journ�e, le plac� dans la journ�e avec sa plus grande valeur
 */
class MetaHeuristique
{
private:
	Instance *instance;
	Solution *solution;

	vector<int> pi_POI;
	vector<vector<int>> ppi_POI_par_Jour;	 // id_Jour : POI accessible entre hotel du debut de journee et hotel de fin de journee
	unordered_map<int, float> map_Score_POI; // Stocke les score recalcul� des POI

	unordered_map<int, vector<int>> map_conflit_POI; // Stocke la liste des journ�e qui peuvent int�grer le POI pour chaque POI
	vector<pair<int, int>> pii_Hotels_par_Jour;

	vector<pair<int, vector<int>>> pp_Meilleure_Sequence_par_Jour; // Stocke les meilleures s�quences � chaque jour

	vector<vector<vector<int>>> pppi_Sequence_par_Jour;

	bool tri_par_score(const int &i1, const int &i2);

public:
	MetaHeuristique(Instance *instanceParam);
	void SolutionGenetique();
	vector<int> SelectionTournoi(const vector<pair<int, vector<int>>> &scores);
	vector<int> Croisement(const vector<int> &parent1, const vector<int> &parent2);
	void Mutation(vector<int> &solution);

	static Solution *ExtraireSolution(Instance *instanceParam);
	void Solution();
	void Initialisation();
	vector<int> Randomisateur(vector<int> pi_POI);
	vector<vector<int>> GenerationNSequence(vector<int> pi_POI, int i_Nombre_de_Liste_A_Construire, int i_Jour);
	int GetScoreSequence(vector<int> pi_Sequence);
};