#include "MetaHeuristique.h"
#include "..\..\Solution.hpp"


MetaHeuristique::MetaHeuristique(Instance* instanceParam) {
	
	instance = instanceParam;
	solution = Heuristique_v1::ExtraireSolution(instanceParam);

}

Solution* MetaHeuristique::ExtraireSolution(Instance* instanceParam) 
{
	MetaHeuristique metaHeuristique(instanceParam);
	return metaHeuristique.solution;
}

void MetaHeuristique::Solution() {


}
void MetaHeuristique::Initialisation() {

	// Initialisation de la liste des POI
	vector<int> pi_POI;
	for (int i_POI = 0; i_POI < instance->get_Nombre_POI(); i_POI++) {
		pi_POI.push_back(i_POI);
	}

	// Initialisation des attributs lié aux jours
	for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++)
	{
		// Hotel au debut du i_Jour-ième jour
		int i_Hotel_Debut_Journee;
		if (i_Jour == 0)
			i_Hotel_Debut_Journee = instance->get_Id_Hotel_depart();
		else
			i_Hotel_Debut_Journee = solution->v_Id_Hotel_Intermedaire[i_Jour - 1];

		// Hotel à la fin du i_Jour-ième jour
		int i_Hotel_Fin_Journee;
		if (i_Jour == instance->get_Nombre_Jour() - 1)
			i_Hotel_Fin_Journee = instance->get_Id_Hotel_Arrivee();
		else
			i_Hotel_Fin_Journee = solution->v_Id_Hotel_Intermedaire[i_Jour];

		float f_Duree_Max_Journee = instance->get_POI_Duree_Max_Voyage(i_Jour); // Récupération de la durée de la journée (TODO : Inutile ?)

		// Initialisation des POI potentiels à chaque jours
		ppi_POI_par_Jour.push_back(Heuristique_v1::IdentifierPOIRayonHotel(i_Hotel_Debut_Journee, i_Hotel_Fin_Journee, instance, pi_POI, i_Jour));

		// On trouve la meilleure sequence de la journée ( score : sequence )
		pair<int, vector<int>> p_Meilleure_Sequence;
		p_Meilleure_Sequence.first = 0;
		p_Meilleure_Sequence.second = Heuristique_v1::CalculMeilleureJournee(instance, ppi_POI_par_Jour[i_Jour], map_Score_POI, i_Hotel_Debut_Journee, i_Hotel_Fin_Journee, i_Jour);
	
		for (int i_POI : p_Meilleure_Sequence.second) {
			p_Meilleure_Sequence.first += instance->get_POI_Score(i_POI);
		}

		pp_Meilleure_Sequence_par_Jour.push_back(p_Meilleure_Sequence);
	}

	// Check des POI communs à plusieurs jours
	for (int idx = 0; idx < instance->get_Nombre_Jour(); idx++) {
		vector<int> pi_POI = ppi_POI_par_Jour[idx];
		for (int i_POI : pi_POI) {
			if (map_conflit_POI.find(i_POI) != map_conflit_POI.end()) {
				map_Score_POI[i_POI] = Heuristique_v1::CalculScorePOI(i_POI, instance); // Initialisation du score
				map_conflit_POI[i_POI] = { idx }; // Conflit initialisé avec l'id du 1er jour pouvant utiliser le POI
			}
			else {
				map_conflit_POI[i_POI].push_back(idx); // Conflit initialiser avec l'id du 1er jour pouvant utiliser le POI
			}
		}
	}
}