#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "MetaHeuristique.h"
#include "..\..\Solution.hpp"

MetaHeuristique::MetaHeuristique(Instance* instanceParam) {
	
	instance = instanceParam;
	solution = Heuristique_v1::ExtraireSolution(instanceParam);
	pppi_Sequence_par_Jour = {};
}

Solution* MetaHeuristique::ExtraireSolution(Instance* instanceParam) 
{
	MetaHeuristique metaHeuristique(instanceParam);

	metaHeuristique.Initialisation();
	//metaHeuristique.Solution();

	printf("vs solution Heuristique :\n");
	for (int i_Jour = 0; i_Jour < metaHeuristique.instance->get_Nombre_Jour(); i_Jour++) {
		int i_Score_Sequence = 0;
		for (int i_POI : metaHeuristique.solution->v_v_Sequence_Id_Par_Jour[i_Jour]) {
			i_Score_Sequence += metaHeuristique.instance->get_POI_Score(i_POI);
		}

		printf("Meilleure sequence jour %d : %d\n", i_Jour, i_Score_Sequence);
	}

	return metaHeuristique.solution;
}

void MetaHeuristique::Solution() {
	for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];

		vector<vector<int>> ppi_Sequences = GenerationNSequence(pi_POI, 500, i_Jour);

		pppi_Sequence_par_Jour.push_back(ppi_Sequences);
	}
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
		pair<int, int> ii_Hotels;

		// Hotel au debut du i_Jour-ième jour
		if (i_Jour == 0)
			ii_Hotels.first = instance->get_Id_Hotel_depart();
		else
			ii_Hotels.first = solution->v_Id_Hotel_Intermedaire[i_Jour - 1];

		// Hotel à la fin du i_Jour-ième jour
		if (i_Jour == instance->get_Nombre_Jour() - 1)
			ii_Hotels.second = instance->get_Id_Hotel_Arrivee();
		else
			ii_Hotels.second = solution->v_Id_Hotel_Intermedaire[i_Jour];

		float f_Duree_Max_Journee = instance->get_POI_Duree_Max_Voyage(i_Jour); // Récupération de la durée de la journée (TODO : Inutile ?)

		// Initialisation des POI potentiels à chaque jours
		ppi_POI_par_Jour.push_back(Heuristique_v1::IdentifierPOIRayonHotel(ii_Hotels.first, ii_Hotels.second, instance, pi_POI, i_Jour));

		// On trouve la meilleure sequence de la journée ( score : sequence )
		pair<int, vector<int>> p_Meilleure_Sequence;
		p_Meilleure_Sequence.first = 0;
		p_Meilleure_Sequence.second = Heuristique_v1::CalculMeilleureJournee(instance, ppi_POI_par_Jour[i_Jour], map_Score_POI, ii_Hotels.first, ii_Hotels.second, i_Jour, {});
	
		for (int i_POI : p_Meilleure_Sequence.second) {
			p_Meilleure_Sequence.first += instance->get_POI_Score(i_POI);
		}

		pii_Hotels_par_Jour.push_back(ii_Hotels);
		pp_Meilleure_Sequence_par_Jour.push_back(p_Meilleure_Sequence);
	}

	unordered_map<int, vector<int>> map_conflit_POI; // Stocke la liste des journée qui peuvent intégrer le POI pour chaque POI
	vector<int> pi_POI_Partages;

	// Check des POI communs à plusieurs jours
	for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];
		for (int i_POI : pi_POI) {
			if (map_conflit_POI.find(i_POI) == map_conflit_POI.end()) {
				map_Score_POI[i_POI] = Heuristique_v1::CalculScorePOI(i_POI, instance); // Initialisation du score
				map_conflit_POI[i_POI] = { i_Jour }; // Conflit initialisé avec l'id du 1er jour pouvant utiliser le POI
			}
			else {
				if (map_conflit_POI[i_POI].size() == 1) {
					pi_POI_Partages.push_back(i_POI);
				} 
				map_conflit_POI[i_POI].push_back(i_Jour); // Conflit initialiser avec l'id du 1er jour pouvant utiliser le POI
			}
		}

		Instance* instancePourTri = instance;
		// Trie par valeur (en fonction du score)
		std::sort(pi_POI_Partages.begin(), pi_POI_Partages.end(),
			[instancePourTri](const int& a, const int& b) {
				return instancePourTri->get_POI_Score(a) > instancePourTri->get_POI_Score(b);
			}
		);

		// Supprimer les POI partages
		int idx = 0;
		while (idx < pi_POI.size()) {
			int i_POI = pi_POI[idx];
			if (map_conflit_POI[i_POI].size() > 1) {
				pi_POI.erase(pi_POI.begin() + idx);
			} else {
				idx++;			
			}
		}
		ppi_POI_par_Jour[i_Jour] = pi_POI;
	}

	// Affectation à un jour pour chaque POI partagé
	for (int i_POI_a_Affecter : pi_POI_Partages) {
		// TODO : Pour chaque, identifier la meilleure sequence cree avec le POI + la sequence des POI propres à la journée
		int i_Meilleur_Jour = 0;
		int i_Meilleur_Score = -1;

		printf("\nPOI %d en conflit entre les jours : ", i_POI_a_Affecter);

		for (int i_Jour : map_conflit_POI[i_POI_a_Affecter]) {
			printf("%d, ", i_Jour);

			vector<int> pi_Sequence = Heuristique_v1::CalculMeilleureJournee(instance, ppi_POI_par_Jour[i_Jour], map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, {i_POI_a_Affecter});
			int i_Score = GetScoreSequence(pi_Sequence);

			if (i_Score > i_Meilleur_Score) {
				i_Meilleur_Jour = i_Jour;
				i_Meilleur_Score = i_Score;
			}
		}
		printf("\nPOI %d affecte au jour %d pour un score de %d\n\n", i_POI_a_Affecter, i_Meilleur_Jour, i_Meilleur_Score);

		ppi_POI_par_Jour[i_Meilleur_Jour].push_back(i_POI_a_Affecter);
	}
}

vector<int> MetaHeuristique::Randomisateur(vector<int> pi_POI) {
	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(pi_POI.begin(), pi_POI.end(), g);

	return pi_POI;
}

vector<vector<int>> MetaHeuristique::GenerationNSequence(vector<int> pi_POI, int i_Nombre_de_Liste_A_Construire, int i_Jour) {
	vector<vector<int>> pi_NSequences = {};
	unordered_map<int, int> map_Score_Sequence;
	float f_Debut_au_Plus_Tard = (24.00 - instance->get_POI_Duree_Max_Voyage(i_Jour));

	for (int i_N = 0; i_N < i_Nombre_de_Liste_A_Construire; i_N++) {
		vector<int> pi_POI_Ordre_Aleatoire = Randomisateur(pi_POI);
		float f_Debut = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / f_Debut_au_Plus_Tard));

		vector<int> pi_Sequence = {};
		for (int i_POI : pi_POI_Ordre_Aleatoire)
		{
			pi_Sequence = Heuristique_v1::MeilleureSequence(pi_Sequence, i_POI, instance, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, 0.0);//f_Debut);
		}


		printf("liste => {");
		for (int idx = 0; idx < pi_Sequence.size(); idx++) {
			printf(" %d ,", pi_Sequence[idx]);
		}
		printf("}\n");

		pi_NSequences.push_back(pi_Sequence);

		int i_Score_Sequence = 0;
		for (int i_POI : pi_Sequence){
			i_Score_Sequence += instance->get_POI_Score(i_POI);
		}

		map_Score_Sequence[i_N] = i_Score_Sequence;
	}

	vector<pair<int, int>> vec_Sequences_Triees(map_Score_Sequence.begin(), map_Score_Sequence.end());

	std::sort(vec_Sequences_Triees.begin(), vec_Sequences_Triees.end(),
		[](const pair<int, int>& a, const pair<int, int>& b) {
			return a.second > b.second;
		}
	); 

	vector<vector<int>> pi_NSequences_Triees;
	for (const auto& paire : vec_Sequences_Triees) {
		pi_NSequences_Triees.push_back(pi_NSequences[paire.first]);
	}

	printf("Meilleure sequence jour %d : %d\n", i_Jour, vec_Sequences_Triees[0].second);

	return pi_NSequences_Triees;
}

int MetaHeuristique::GetScoreSequence(vector<int> pi_Sequence) {
	int i_Score_Sequence = 0;

	for (int i_POI : pi_Sequence) {
		i_Score_Sequence += instance->get_POI_Score(i_POI);
	}

	return i_Score_Sequence;
}


bool MetaHeuristique::tri_par_score(const int& i1, const int& i2) {
	return instance->get_POI_Score(i1) < instance->get_POI_Score(i2);
}