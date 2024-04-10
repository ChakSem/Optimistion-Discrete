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

	//IdentifierPOIRayonHotel(i_Hotel_Debut_Journee, i_Hotel_Possible);

	// Initialisation des variables

	vector<int> pi_POI;
	for (int i_POI = 0; i_POI < instance->get_Nombre_POI(); i_POI++) {
		pi_POI.push_back(i_POI);
	}

	// Initialisation des POI par jour
	for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++)
	{
		int i_Hotel_Debut_Journee;
		if (i_Jour == 0)
			i_Hotel_Debut_Journee = instance->get_Id_Hotel_depart();
		else
			i_Hotel_Debut_Journee = solution->v_Id_Hotel_Intermedaire[i_Jour - 1];


		int i_Hotel_Fin_Journee;
		if (i_Jour == instance->get_Nombre_Jour() - 1)
			i_Hotel_Fin_Journee = instance->get_Id_Hotel_Arrivee();
		else
			i_Hotel_Fin_Journee = solution->v_Id_Hotel_Intermedaire[i_Jour];

		float f_Duree_Max_Journee = instance->get_POI_Duree_Max_Voyage(i_Jour);

		ppi_POI_par_Jour.push_back(Heuristique_v1::IdentifierPOIRayonHotel(i_Hotel_Debut_Journee, i_Hotel_Fin_Journee, instance, pi_POI, i_Jour));
	}



}