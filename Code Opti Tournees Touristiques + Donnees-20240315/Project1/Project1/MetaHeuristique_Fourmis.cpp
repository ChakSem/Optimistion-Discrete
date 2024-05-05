#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "MetaHeuristique_Fourmis.h"
#include "..\..\Solution.hpp"

#define POI 0
#define IDXPOI 1
#define SCORE 2
#define QUANTITE 3
#define PREMIERIDXPOI 4

#define DUREE 0
#define DEBUT 1

#define NB_ITERATIONS 5
#define NB_FOURMIS 1000//00
#define PHEROMONES_INIT 10000.0
#define EVAPORATION 4000.0
#define AUGMENTATION_EVAPORATION 0.0

MetaHeuristique_Fourmis::MetaHeuristique_Fourmis(Instance* instanceParam) : Heuristique_v1(instanceParam) {
	pppi_Sequence_par_Jour = {};
	
}

Solution* MetaHeuristique_Fourmis::ExtraireSolution(Instance* instanceParam) 
{
	MetaHeuristique_Fourmis metaHeuristique(instanceParam);

	metaHeuristique.Solution1();

	metaHeuristique.SolutionMetaHeuristique();

	Instance* instance = metaHeuristique.instance;
	Solution* solution = metaHeuristique.solution;

	return solution;
}

void MetaHeuristique_Fourmis::SolutionMetaHeuristique() {
	int i_FO_Fourmis = 0;
	vector <pair<float, vector<int>>> pp_Meilleure_Sequence_par_Jour_Fourmis(pi_Jours_Tries.size());

	vector<vector<double>> ppd_Pheromones;
	vector<double> pd_Pheromones_Depart;
	vector<double> pd_Pheromones_Arrive;

	// ALGO FOURMIS
	for (int i_Jour : pi_Jours_Tries) {

		// Initialisation variables
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];

		if (pi_POI.size() > 0) {
			int i_Hotel_Depart = pii_Hotels_par_Jour[i_Jour].first;
			int i_Hotel_Arrive = pii_Hotels_par_Jour[i_Jour].second;
			float f_Duree_Max = instance->get_POI_Duree_Max_Voyage(i_Jour);

			ppd_Pheromones = vector<vector<double>>(pi_POI.size(), vector<double>(pi_POI.size(), 0.0));
			pd_Pheromones_Depart = vector<double>(pi_POI.size(), 0.0);

			// Initialisation pheromones
			for (int idx_POI_x = 0; idx_POI_x < pi_POI.size(); idx_POI_x++) {
				vector<int> pi_Nouvelle_Ligne;
				int i_POI_x = pi_POI[idx_POI_x];

				for (int idx_POI_y = 0; idx_POI_y < pi_POI.size(); idx_POI_y++) {
					if (idx_POI_x == idx_POI_y) {
						ppd_Pheromones[idx_POI_x][idx_POI_y] = 0.0;
					}
					else {
						int i_POI_y = pi_POI[idx_POI_y];

						double d_Score = instance->get_POI_Score(i_POI_x) + instance->get_POI_Score(i_POI_y);
						d_Score = max(0.0, d_Score * (f_Duree_Max - instance->get_distance_POI_POI(i_POI_x, i_POI_y)));

						// TODO : Modifier initialisation
						ppd_Pheromones[idx_POI_x][idx_POI_y] = PHEROMONES_INIT;//d_Score;
					}
				}
				double d_Score = instance->get_POI_Score(i_POI_x);
				d_Score = max(0.0, d_Score * (f_Duree_Max - instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_x)));

				pd_Pheromones_Depart[idx_POI_x] = PHEROMONES_INIT;//d_Score;
			}

			// DEROULEMENT
			int iteration = 0;
			bool stop = false;

			vector<vector<int>> ppi_Fourmis(NB_FOURMIS, { -1,-1,0,0,-1 });
			vector<vector<float>> ppf_Fourmis(NB_FOURMIS, { 0.0, 0.0 });
			vector<vector<int>> ppi_ParcoursFourmis(NB_FOURMIS);
			vector<vector<int>> ppi_POI_Disponibles(NB_FOURMIS, vector<int>(pi_POI.size(), 1));

			// INITIALISATION DES POSITIONS DES FOURMIS
			double d_Evaporation_Pheromones = EVAPORATION;

			for (int i_Fourmis = 0; i_Fourmis < NB_FOURMIS; i_Fourmis++) {
				float f_Heure_Max = static_cast<float>(rand()) / (static_cast<float>(RAND_MAX / f_Duree_Max));
				int idx_POI = choisirIndex(pd_Pheromones_Depart);
				int i_POI = pi_POI[idx_POI];

				float f_Dist = instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI);
				float f_Ouverture = instance->get_POI_Heure_ouverture(i_POI);
				float f_Heure_Debut = 0.0;
				if (f_Dist < f_Ouverture) {
					f_Heure_Debut = f_Ouverture - f_Dist;

				}
				float f_Duree_Trajet = f_Dist + f_Heure_Debut;

				ppf_Fourmis[i_Fourmis][DEBUT] = f_Heure_Debut;

				ppi_Fourmis[i_Fourmis][PREMIERIDXPOI] = idx_POI;
				do {
					ppi_Fourmis[i_Fourmis][POI] = i_POI;
					ppi_Fourmis[i_Fourmis][IDXPOI] = idx_POI;
					ppi_Fourmis[i_Fourmis][SCORE] += instance->get_POI_Score(i_POI);
					ppi_Fourmis[i_Fourmis][QUANTITE]++;

					ppf_Fourmis[i_Fourmis][DUREE] = f_Duree_Trajet;

					ppi_ParcoursFourmis[i_Fourmis].push_back(idx_POI);
					ppi_POI_Disponibles[i_Fourmis][idx_POI] = 0;

					idx_POI = choisirIndex_Ameliore(ppd_Pheromones[idx_POI], pi_POI, ppi_POI_Disponibles[i_Fourmis], f_Duree_Trajet, instance, i_POI, i_Hotel_Arrive, f_Duree_Max, f_Heure_Debut);


					if (idx_POI > -1) {
						i_POI = pi_POI[idx_POI];
						double heureOuverture = instance->get_POI_Heure_ouverture(i_POI);
						f_Duree_Trajet += instance->get_distance_POI_POI(ppi_Fourmis[i_Fourmis][POI], i_POI);
						if (f_Duree_Trajet < heureOuverture) {
							f_Duree_Trajet = heureOuverture;
						}
					}
					else {
						f_Duree_Trajet = f_Heure_Max + 1;
					}
				} while (f_Duree_Trajet < f_Heure_Max);

			}

			// INITIALISATION D'UNE COPIE DES MATRICES DE PHEROMONES
			vector<vector<double>> ppd_Pheromones_Copie(ppd_Pheromones);
			vector<double> pd_Pheromones_Depart_Copie(pd_Pheromones_Depart);

			while (iteration < NB_ITERATIONS && !stop) {
				// ON ENREGISTRE LES MODIFICATIONS
				ppd_Pheromones = ppd_Pheromones_Copie;
				pd_Pheromones_Depart = pd_Pheromones_Depart_Copie;

				ppd_Pheromones_Copie = vector<vector<double>>(ppd_Pheromones);
				pd_Pheromones_Depart_Copie = vector<double>(pd_Pheromones_Depart);

				// AFFICHAGE DE LA MATRICE DE PHEROMONES
				/*if (iteration % 49 == 0) {
					for (int i_POI_x = 0; i_POI_x < pi_POI.size(); i_POI_x++) {
						printf("%.1lf, ", pd_Pheromones_Depart_Copie[i_POI_x]);
						for (int i_POI_y = 0; i_POI_y < pi_POI.size(); i_POI_y++) {
							printf("%.1lf, ", ppd_Pheromones_Copie[i_POI_x][i_POI_y]);
						}
						printf("\n");
					}
					printf(" Iteration %d\n\n", iteration);/*
				}*/


				// DEPLACEMENTS DES FOURMIS ET DEPOT DE PHEROMONES
				for (int i_Fourmis = 0; i_Fourmis < NB_FOURMIS; i_Fourmis++) {
					int idx_POI_Depart = ppi_Fourmis[i_Fourmis][IDXPOI];
					int i_POI_Depart = ppi_Fourmis[i_Fourmis][POI];

					int idx_POI_Choisit = choisirIndex_Ameliore(ppd_Pheromones[idx_POI_Depart], pi_POI, ppi_POI_Disponibles[i_Fourmis], ppf_Fourmis[i_Fourmis][DUREE], instance, i_POI_Depart, i_Hotel_Arrive, f_Duree_Max, ppf_Fourmis[i_Fourmis][DEBUT]);

					if (idx_POI_Choisit == -1) {

						// REINITIALISATION DE LA FOURMIS
						ppi_POI_Disponibles[i_Fourmis] = vector<int>(pi_POI.size(), 1);

						idx_POI_Choisit = choisirIndex(pd_Pheromones_Depart);
						int i_POI_Choisit = pi_POI[idx_POI_Choisit];

						ppi_Fourmis[i_Fourmis][POI] = i_POI_Choisit;
						ppi_Fourmis[i_Fourmis][IDXPOI] = idx_POI_Choisit;
						ppi_Fourmis[i_Fourmis][PREMIERIDXPOI] = idx_POI_Choisit;

						float f_Dist = instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_Choisit);
						float f_Ouverture = instance->get_POI_Heure_ouverture(i_POI_Choisit);
						float f_Heure_Debut = 0.0;
						if (f_Dist < f_Ouverture) {
							f_Heure_Debut = f_Ouverture - f_Dist;

						}
						float f_Duree_Trajet = f_Dist + f_Heure_Debut;

						ppi_Fourmis[i_Fourmis][SCORE] = instance->get_POI_Score(i_POI_Choisit);
						ppi_Fourmis[i_Fourmis][QUANTITE] = 1;

						ppf_Fourmis[i_Fourmis][DEBUT] = f_Heure_Debut;
						ppf_Fourmis[i_Fourmis][DUREE] = f_Duree_Trajet;

						ppi_ParcoursFourmis[i_Fourmis] = { idx_POI_Choisit };
						ppi_POI_Disponibles[i_Fourmis][idx_POI_Choisit] = 0;


						// DEPOT DE PHEROMONES SUR L'ARRETE DU POI DE DEPART
						double d_Depot_Pheromone = ppi_Fourmis[i_Fourmis][SCORE] + 1; // TODO: ADAPTER SCORE ICI

						pd_Pheromones_Depart_Copie[idx_POI_Choisit] += d_Depot_Pheromone;
					}
					else {
						int i_POI_Choisit = pi_POI[idx_POI_Choisit];

						float f_Dist = instance->get_distance_POI_POI(i_POI_Depart, i_POI_Choisit);
						float f_Duree_Trajet = ppi_Fourmis[i_Fourmis][DUREE] + f_Dist;

						ppi_Fourmis[i_Fourmis][POI] = i_POI_Choisit;
						ppi_Fourmis[i_Fourmis][IDXPOI] = idx_POI_Choisit;
						ppi_Fourmis[i_Fourmis][SCORE] += instance->get_POI_Score(i_POI_Choisit);
						ppi_Fourmis[i_Fourmis][QUANTITE]++;

						ppf_Fourmis[i_Fourmis][DUREE] = f_Duree_Trajet;

						ppi_ParcoursFourmis[i_Fourmis].push_back(idx_POI_Choisit);
						ppi_POI_Disponibles[i_Fourmis][idx_POI_Choisit] = 0;

						// DEPOT DE PHEROMONES SUR L'ARRETE DU POI PRECEDENT ET DU POI DE DEPART
						double d_Depot_Pheromone = ppi_Fourmis[i_Fourmis][SCORE] + ppi_Fourmis[i_Fourmis][QUANTITE];// TODO: ADAPTER SCORE ICI

						pd_Pheromones_Depart_Copie[ppi_Fourmis[i_Fourmis][PREMIERIDXPOI]] += d_Depot_Pheromone;
						for (int idx = 0; idx < ppi_ParcoursFourmis[i_Fourmis].size() - 1; idx++) {
							ppd_Pheromones_Copie[ppi_ParcoursFourmis[i_Fourmis][idx]][ppi_ParcoursFourmis[i_Fourmis][idx + 1]] += d_Depot_Pheromone;
						}
					}
				}

				// DISPERTION DE LA PHEROMONE
				stop = true;
				for (int idx_POI_x = 0; idx_POI_x < pi_POI.size(); idx_POI_x++) {
					vector<int> pi_Nouvelle_Ligne;

					for (int idx_POI_y = 0; idx_POI_y < pi_POI.size(); idx_POI_y++) {
						double d_Max_Pheromone = max(0.0, ppd_Pheromones_Copie[idx_POI_x][idx_POI_y] - d_Evaporation_Pheromones);
						ppd_Pheromones_Copie[idx_POI_x][idx_POI_y] = d_Max_Pheromone;

					}
					double d_Max_Pheromone = max(0.0, pd_Pheromones_Depart_Copie[idx_POI_x] - d_Evaporation_Pheromones);
					pd_Pheromones_Depart_Copie[idx_POI_x] = d_Max_Pheromone;

					stop = stop && d_Max_Pheromone == 0.0; // ON ARRETE SI LA 1ERE LIGNE EST VIDE
				}

				d_Evaporation_Pheromones += AUGMENTATION_EVAPORATION;
				iteration++;
			}

			vector<int> pi_Sequence;
			vector<int> pi_POI_Restants(pi_POI.size(), 1);
			int i_Score_Jour = 0;
			int idx_Choisit = choisirIndex_Fin(pd_Pheromones_Depart);
			int i_POI_Choisit = pi_POI[idx_Choisit];

			float f_Dist = instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_Choisit);
			float f_Ouverture = instance->get_POI_Heure_ouverture(i_POI_Choisit);
			float f_Heure_Debut = 0.0;
			if (f_Dist < f_Ouverture) {
				f_Heure_Debut = f_Ouverture - f_Dist;
			}
			float f_Duree_Trajet = f_Dist + f_Heure_Debut;

			while (idx_Choisit > -1) {
				pi_Sequence.push_back(i_POI_Choisit);
				i_Score_Jour += instance->get_POI_Score(i_POI_Choisit);
				pi_POI_Restants[idx_Choisit] = 0;

				vector<double> pd_Pheromones_POI_Choisit = ppd_Pheromones[idx_Choisit];
				idx_Choisit = choisirIndex_Fin(pd_Pheromones_POI_Choisit, pi_POI, pi_POI_Restants, f_Duree_Trajet, instance, i_POI_Choisit, i_Hotel_Arrive, f_Duree_Max, f_Heure_Debut);

				if (idx_Choisit > -1) {
					f_Duree_Trajet = max(instance->get_POI_Heure_ouverture(pi_POI[idx_Choisit]), f_Duree_Trajet + instance->get_distance_POI_POI(i_POI_Choisit, pi_POI[idx_Choisit]));

					i_POI_Choisit = pi_POI[idx_Choisit];
				}
			}

			for (int idx = 0; idx < pi_Sequence.size(); idx++) {
				int i_POI = pi_Sequence[idx];

				for (int i_Jour_Communs : map_conflit_POI[i_POI]) {
					if (i_Jour != i_Jour_Communs) {
						SupprimerElement(&(ppi_POI_par_Jour[i_Jour_Communs]), i_POI);
					}
				}

			}

			printf("Jour %d : {", i_Jour);
			for (int ik : pi_Sequence) {
				printf("%d, ", ik);
			}
			printf("}\n");

			i_FO_Fourmis += i_Score_Jour;
			pp_Meilleure_Sequence_par_Jour_Fourmis[i_Jour] = pair<float, vector<int>>(f_Heure_Debut, pi_Sequence);
		}
	}
	printf("Score Final MetaHeuristique de : %d\n\n", i_FO_Fourmis);


	//if (i_FO < i_FO_Fourmis) {
		i_FO = i_FO_Fourmis;
		pp_Meilleure_Sequence_par_Jour = pp_Meilleure_Sequence_par_Jour_Fourmis;

		SauvegarderSolution();
	//}
	printf("Score Final Retenu : %d\n\n", i_FO);
}

bool MetaHeuristique_Fourmis::tri_par_score(const int& i1, const int& i2) {
	return instance->get_POI_Score(i1) < instance->get_POI_Score(i2);
}

int choisirIndex(const std::vector<double>& array) {
	double sommeTotale = 0;
	for (double valeur : array) {
		sommeTotale += valeur;
	}

	if (sommeTotale == 0)
		return -1;

	// G�n�rer un nombre al�atoire entre 0 et la somme totale - 1
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, sommeTotale - 1);
	int choix = dis(gen);

	// Parcourir le vecteur et choisir l'index en fonction du nombre al�atoire g�n�r�
	int index = 0;
	double sommePartielle = 0;
	for (double valeur : array) {
		sommePartielle += valeur;
		if (choix < sommePartielle) {
			return index;
		}
		index++;
	}

	// Cet �tat ne devrait jamais �tre atteint, mais retourner -1 en cas d'erreur
	return -1;
}

int choisirIndex_Ameliore(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const float f_Duree_Trajet, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, float f_Duree_Max, float f_Heure_Debut) {
	double sommeTotale = 0.0;
	unordered_map<int, double> map_POI_Choix;

	for (int idx_POI = 0; idx_POI < array.size(); idx_POI++) {
		int i_POI = pi_POI[idx_POI];
		double valeur = array[idx_POI] * pi_POI_Disponibles[idx_POI];

		if (valeur > 0.0) {
			float f_Duree_Hypo = max(f_Duree_Trajet + instance->get_distance_POI_POI(i_POI_Depart, i_POI), instance->get_POI_Heure_ouverture(i_POI));
			float f_Duree_Fin = f_Duree_Hypo + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI) - f_Heure_Debut;

			if (f_Duree_Fin < f_Duree_Max && f_Duree_Hypo < instance->get_POI_Heure_fermeture(i_POI)) {
				sommeTotale += valeur;
			}
		}

		map_POI_Choix[idx_POI] = sommeTotale;
	}

	if (sommeTotale == 0.0)
		return -1;

	// G�n�rer un nombre al�atoire entre 0 et la somme totale - 1
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, sommeTotale - 1);
	int choix = dis(gen);

	// Parcourir le vecteur et choisir l'index en fonction du nombre al�atoire g�n�r�
	for (int idx_POI = 0; idx_POI < array.size(); idx_POI++) {
		double valeur = array[idx_POI] * pi_POI_Disponibles[idx_POI];

		if (choix < map_POI_Choix[idx_POI]) {
			return idx_POI;
		}
	}

	// Cet etat ne devrait jamais etre atteint, mais retourner -1 en cas d'erreur
	return -1;
}

int choisirIndex_Fin(const std::vector<double>& array) {
	double pheromone_max = -1;
	int idx_POI_Choisit = -1;

	for (int idx_POI = 0; idx_POI < array.size(); idx_POI++) {
		double valeur = array[idx_POI];

		if (valeur > 0.0 && pheromone_max < valeur) {
			pheromone_max = valeur;
			idx_POI_Choisit = idx_POI;
		}
	}

	return idx_POI_Choisit;
}

int choisirIndex_Fin(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const float f_Duree_Trajet, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, float f_Duree_Max, float f_Heure_Debut) {
	double pheromone_max = -1;
	int idx_POI_Choisit = -1;

	for (int idx_POI = 0; idx_POI < array.size(); idx_POI++) {
		int i_POI = pi_POI[idx_POI];
		double valeur = array[idx_POI] * pi_POI_Disponibles[idx_POI];

		if (valeur > 0.0) {
			float f_Duree_Hypo = max(f_Duree_Trajet + instance->get_distance_POI_POI(i_POI_Depart, i_POI), instance->get_POI_Heure_ouverture(i_POI));
			float f_Duree_Fin = f_Duree_Hypo + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI) - f_Heure_Debut;

			if (f_Duree_Fin < f_Duree_Max && f_Duree_Hypo < instance->get_POI_Heure_fermeture(i_POI)) {
				if (pheromone_max < array[idx_POI]) {

					pheromone_max = array[idx_POI];
					idx_POI_Choisit = idx_POI;
				}
			}
		}
	}

	return idx_POI_Choisit;
}














vector<int> MetaHeuristique_Fourmis::Randomisateur(vector<int> pi_POI) {
	std::random_device rd;
	std::mt19937 g(rd());

	std::shuffle(pi_POI.begin(), pi_POI.end(), g);

	return pi_POI;
}
//
//vector<vector<int>> MetaHeuristique_Fourmis::GenerationNSequence(vector<int> pi_POI, int i_Nombre_de_Liste_A_Construire, int i_Jour) {
//	vector<vector<int>> pi_NSequences = {};
//	unordered_map<int, int> map_Score_Sequence;
//	float f_Debut_au_Plus_Tard = (24.00 - instance->get_POI_Duree_Max_Voyage(i_Jour));
//
//	for (int i_N = 0; i_N < i_Nombre_de_Liste_A_Construire; i_N++) {
//		vector<int> pi_POI_Ordre_Aleatoire = Randomisateur(pi_POI);
//		float f_Debut = static_cast <float> (rand()) / (static_cast <float> (RAND_MAX / f_Debut_au_Plus_Tard));
//
//		vector<int> pi_Sequence = {};
//		for (int i_POI : pi_POI_Ordre_Aleatoire)
//		{
//			pi_Sequence = Heuristique_v1::MeilleureSequence(pi_Sequence, i_POI, instance, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour).second;//f_Debut);
//		}
//
//
//		printf("liste => {");
//		for (int idx = 0; idx < pi_Sequence.size(); idx++) {
//			printf(" %d ,", pi_Sequence[idx]);
//		}
//		printf("}\n");
//
//		pi_NSequences.push_back(pi_Sequence);
//
//		int i_Score_Sequence = 0;
//		for (int i_POI : pi_Sequence){
//			i_Score_Sequence += instance->get_POI_Score(i_POI);
//		}
//
//		map_Score_Sequence[i_N] = i_Score_Sequence;
//	}
//
//	vector<pair<int, int>> vec_Sequences_Triees(map_Score_Sequence.begin(), map_Score_Sequence.end());
//
//	std::sort(vec_Sequences_Triees.begin(), vec_Sequences_Triees.end(),
//		[](const pair<int, int>& a, const pair<int, int>& b) {
//			return a.second > b.second;
//		}
//	); 
//
//	vector<vector<int>> pi_NSequences_Triees;
//	for (const auto& paire : vec_Sequences_Triees) {
//		pi_NSequences_Triees.push_back(pi_NSequences[paire.first]);
//	}
//
//	printf("Meilleure sequence jour %d : %d\n", i_Jour, vec_Sequences_Triees[0].second);
//
//	return pi_NSequences_Triees;
//}
//