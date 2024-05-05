#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "MetaHeuristique_Fourmis.h"
#include "..\..\Solution.hpp"
#include <fstream>
#include <chrono>
#define POI 0
#define IDXPOI 1
#define DUREE 2
#define SCORE 3
#define QUANTITE 4
#define DEBUT 5
#define PREMIERIDXPOI 6

#define NB_ITERATIONS 5
#define NB_FOURMIS 20000

#define PHEROMONES_INIT 10000.0
#define EVAPORATION 4000.0
#define AUGMENTATION_EVAPORATION 0

#define CHEMIN_FICHIER_SORTIE_RESULTATS "ResParamFourmis.txt"
int choisirIndex(const std::vector<double>& array);
int choisirIndex_Ameliore(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const double duree, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, double d_Duree_Max);
void SupprimerElement(vector<int>* pi_Array, int i_Element);

MetaHeuristique_Fourmis::MetaHeuristique_Fourmis(Instance* instanceParam) : Heuristique_v1(instanceParam) {
	pppi_Sequence_par_Jour = {};
	
}

Solution* MetaHeuristique_Fourmis::ExtraireSolution(Instance* instanceParam) 
{
	MetaHeuristique_Fourmis metaHeuristique(instanceParam);

	metaHeuristique.Solution1();

	//printf("solution MetaHeuristique\n");
	metaHeuristique.SolutionMetaHeuristique();

	//printf("\nvs solution Heuristique :\n");
	Instance* instance = metaHeuristique.instance;
	Solution* solution = metaHeuristique.solution;

	/*for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		int i_Score_Sequence = 0;
		for (int i_POI : solution->v_v_Sequence_Id_Par_Jour[i_Jour]) {
			i_Score_Sequence += instance->get_POI_Score(i_POI);
		}
		
		printf("Meilleure sequence jour %d : %d\n", i_Jour, i_Score_Sequence);

		printf("{ ");
		for (int i : solution->v_v_Sequence_Id_Par_Jour[i_Jour]) {
			printf("%d, ", i);
		}
		printf("}\n");
	}*/

	return solution;
}

void MetaHeuristique_Fourmis::SolutionMetaHeuristique() {
	/*for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];

		vector<vector<int>> ppi_Sequences = GenerationNSequence(pi_POI, 500, i_Jour);

		pppi_Sequence_par_Jour.push_back(ppi_Sequences);
	}*/

	// TODO : Trouver les meilleures sequences possibles (en temps polynomial) pour chaque jours, en fonctions des sets de POI (unique) calcul� � chaque jour dans Initialisation

	/*for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		printf("Jour %d :", i_Jour);
		pair<float, vector<int>> pp = pp_Meilleure_Sequence_par_Jour[i_Jour];
		int i_Meilleur_Score = pp.first;
		vector<int> pi_Meilleure_Sequence = pp.second;

		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];
		int idx = 0;

		while (idx < pi_POI.size()) {

			vector<int> pi_POI_i = pi_POI;
			pi_POI_i.erase(pi_POI_i.begin() + idx);

			vector<int> pi_Sequence = Heuristique_v1::CalculMeilleureJournee(instance, pi_POI_i, map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, { pi_POI[idx]});
		
			int score = GetScoreSequence(pi_Sequence);

			if (score > i_Meilleur_Score) {
				i_Meilleur_Score = score;
				pi_Meilleure_Sequence = pi_Sequence;

			}

			idx++;
		}
		printf("Meilleure sequence : %d\n", i_Meilleur_Score);

		printf("{ ");
		for (int i : pi_Meilleure_Sequence) {
			printf("%d, ", i);
		}
		printf("}\n");

		pp.first = i_Meilleur_Score;
		pp.second = pi_Meilleure_Sequence;
		pp_Meilleure_Sequence_par_Jour[i_Jour] = pp;
	}*/
	int i_FO = 0;
	vector <pair<float, vector<int>>> pp_Meilleure_Sequence_par_Jour_Fourmis(pi_Jours_Tries.size());

	vector<vector<double>> ppd_Pheromones;
	vector<double> pd_Pheromones_Depart;
	vector<double> pd_Pheromones_Arrive;
	//Activer le chorno
	auto chrono_start = chrono::system_clock::now();

	// ALGO FOURMIS
	for (int i_Jour : pi_Jours_Tries) {

		// Initialisation variables
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];

		if (pi_POI.size() > 0) {
			int i_Hotel_Depart = pii_Hotels_par_Jour[i_Jour].first;
			int i_Hotel_Arrive = pii_Hotels_par_Jour[i_Jour].second;
			double d_Duree_Max = instance->get_POI_Duree_Max_Voyage(i_Jour);
			double d_Debut_Au_Plus_Tard = 24.0 - d_Duree_Max;

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
						d_Score = max(0.0, d_Score * (d_Duree_Max - instance->get_distance_POI_POI(i_POI_x, i_POI_y)));

						// TODO : Modifier initialisation
						ppd_Pheromones[idx_POI_x][idx_POI_y] = PHEROMONES_INIT;//d_Score;
					}
				}
				double d_Score = instance->get_POI_Score(i_POI_x);
				d_Score = max(0.0, d_Score * (d_Duree_Max - instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_x)));

				pd_Pheromones_Depart[idx_POI_x] = PHEROMONES_INIT;//d_Score;
			}

			// DEROULEMENT
			int iteration = 0;
			bool stop = false;

			vector<vector<int>> ppi_Fourmis(NB_FOURMIS, { -1,-1,0,0,0,0,-1 });
			vector<vector<int>> ppi_ParcoursFourmis(NB_FOURMIS);
			vector<vector<int>> ppi_POI_Disponibles(NB_FOURMIS, vector<int>(pi_POI.size(), 1));

			// INITIALISATION DES POSITIONS DES FOURMIS
			double d_Evaporation_Pheromones = EVAPORATION;

			for (int i_Fourmis = 0; i_Fourmis < NB_FOURMIS; i_Fourmis++) {
				double d_Duree_Debut = static_cast<double>(rand()) / (static_cast<double>(RAND_MAX / d_Duree_Max));
				int idx_POI = choisirIndex(pd_Pheromones_Depart);
				int i_POI = pi_POI[idx_POI];
				double d_Duree;
				double d_Premiere_Distance = instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI);
				double d_Ouverture_POI_Choisit = instance->get_POI_Heure_ouverture(i_POI);
				double d_Debut = 0.0;
				if (d_Premiere_Distance < d_Ouverture_POI_Choisit) {
					d_Debut = min(d_Debut_Au_Plus_Tard, d_Ouverture_POI_Choisit - d_Premiere_Distance);
					d_Duree = d_Ouverture_POI_Choisit - d_Debut;
				}
				else {
					d_Duree = d_Premiere_Distance;
				}
				ppi_Fourmis[i_Fourmis][DEBUT] = d_Debut;

				ppi_Fourmis[i_Fourmis][PREMIERIDXPOI] = idx_POI;
				do {
					ppi_Fourmis[i_Fourmis][POI] = i_POI;
					ppi_Fourmis[i_Fourmis][IDXPOI] = idx_POI;
					ppi_Fourmis[i_Fourmis][SCORE] += instance->get_POI_Score(i_POI);
					ppi_Fourmis[i_Fourmis][QUANTITE]++;

					ppi_ParcoursFourmis[i_Fourmis].push_back(idx_POI);
					ppi_POI_Disponibles[i_Fourmis][idx_POI] = 0;

					idx_POI = choisirIndex_Ameliore(ppd_Pheromones[idx_POI], pi_POI, ppi_POI_Disponibles[i_Fourmis], d_Duree, instance, i_POI, i_Hotel_Arrive, d_Duree_Max);


					if (idx_POI > -1) {
						i_POI = pi_POI[idx_POI];
						double heureOuverture = instance->get_POI_Heure_ouverture(i_POI);
						d_Duree += instance->get_distance_POI_POI(ppi_Fourmis[i_Fourmis][POI], i_POI);
						if (d_Duree < heureOuverture) {
							d_Duree = heureOuverture;
						}
					}
					else {
						d_Duree = d_Duree_Debut + 1;
					}
				} while (d_Duree /* + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI)*/ < d_Duree_Debut);
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

					int idx_POI_Choisit = choisirIndex_Ameliore(ppd_Pheromones[idx_POI_Depart], pi_POI, ppi_POI_Disponibles[i_Fourmis], ppi_Fourmis[i_Fourmis][DUREE], instance, i_POI_Depart, i_Hotel_Arrive, d_Duree_Max);

					if (idx_POI_Choisit == -1) {

						// REINITIALISATION DE LA FOURMIS
						idx_POI_Choisit = choisirIndex(pd_Pheromones_Depart);
						int i_POI_Choisit = pi_POI[idx_POI_Choisit];

						ppi_Fourmis[i_Fourmis][POI] = i_POI_Choisit;
						ppi_Fourmis[i_Fourmis][IDXPOI] = idx_POI_Choisit;
						ppi_Fourmis[i_Fourmis][PREMIERIDXPOI] = idx_POI_Choisit;

						double d_Premiere_Distance = instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_Choisit);
						double d_Ouverture_POI_Choisit = instance->get_POI_Heure_ouverture(i_POI_Choisit);
						double d_Debut = 0.0;
						if (d_Premiere_Distance < d_Ouverture_POI_Choisit) {
							d_Debut = min(d_Debut_Au_Plus_Tard, d_Ouverture_POI_Choisit - d_Premiere_Distance);
							ppi_Fourmis[i_Fourmis][DUREE] = d_Ouverture_POI_Choisit - d_Debut;
						}
						else {
							ppi_Fourmis[i_Fourmis][DUREE] = d_Premiere_Distance;
						}

						ppi_Fourmis[i_Fourmis][SCORE] = instance->get_POI_Score(i_POI_Choisit);
						ppi_Fourmis[i_Fourmis][QUANTITE] = 1;
						ppi_Fourmis[i_Fourmis][DEBUT] = d_Debut;

						ppi_ParcoursFourmis[i_Fourmis] = { idx_POI_Choisit };
						ppi_POI_Disponibles[i_Fourmis][idx_POI_Choisit] = 0;


						// DEPOT DE PHEROMONES SUR L'ARRETE DU POI DE DEPART
						double d_Depot_Pheromone = ppi_Fourmis[i_Fourmis][SCORE] + 1; // TODO: ADAPTER SCORE ICI

						pd_Pheromones_Depart_Copie[idx_POI_Choisit] += d_Depot_Pheromone;
					}
					else {
						int i_POI_Choisit = pi_POI[idx_POI_Choisit];

						double d_Duree = ppi_Fourmis[i_Fourmis][DUREE] + instance->get_distance_POI_POI(i_POI_Depart, i_POI_Choisit);
						double distance = ppi_Fourmis[i_Fourmis][DUREE] - d_Duree;

						ppi_Fourmis[i_Fourmis][POI] = i_POI_Choisit;
						ppi_Fourmis[i_Fourmis][IDXPOI] = idx_POI_Choisit;
						ppi_Fourmis[i_Fourmis][DUREE] = d_Duree;
						ppi_Fourmis[i_Fourmis][SCORE] += instance->get_POI_Score(i_POI_Choisit);
						ppi_Fourmis[i_Fourmis][QUANTITE]++;

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
			int i_POI_Choisit = 0;
			double d_Meilleur_Score = 0.0;
			int idx_Choisit = 0;
			int i_Score_Jour = 0;
			for (int idx_POI = 0; idx_POI < pi_POI.size(); idx_POI++) {
				int i_POI = pi_POI[idx_POI];
				double d_Score = pd_Pheromones_Depart[idx_POI];

				if (d_Score > d_Meilleur_Score) {
					d_Meilleur_Score = d_Score;
					i_POI_Choisit = i_POI;
					idx_Choisit = idx_POI;
				}
			}

			vector<int> pi_POI_Restants(pi_POI.size(), 1);
			pi_POI_Restants[idx_Choisit] = 0;

			double d_Depart = max(0.0, 0.0 + instance->get_POI_Heure_ouverture(i_POI_Choisit) - instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_Choisit));
			if (d_Depart > d_Debut_Au_Plus_Tard) {
				d_Depart = d_Debut_Au_Plus_Tard;
			}
			double d_Duree = max(instance->get_POI_Heure_ouverture(i_POI_Choisit) - d_Depart, instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_Choisit) + 0.0);

			while (d_Duree /* + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI)*/ < d_Duree_Max) {
				pi_Sequence.push_back(i_POI_Choisit);
				i_Score_Jour += instance->get_POI_Score(i_POI_Choisit);
				d_Meilleur_Score = -1.0;
				int idx_retenu = -1;

				vector<double> pd_Pheromones_POI_Choisit = ppd_Pheromones[idx_Choisit];
				for (int idx_POI = 0; idx_POI < pi_POI.size(); idx_POI++) {
					if (pi_POI_Restants[idx_POI] == 1) { // Si le POI n'a pas deja ete elimine
						int i_POI = pi_POI[idx_POI];

						double d_Score = pd_Pheromones_POI_Choisit[idx_POI];
						if (d_Score > d_Meilleur_Score) {
							if (d_Duree + instance->get_distance_POI_POI(pi_Sequence.back(), i_POI) <= instance->get_POI_Heure_fermeture(i_POI))
							{
								d_Meilleur_Score = d_Score;
								i_POI_Choisit = i_POI;
								idx_retenu = idx_POI;
							}
							else {
								pi_POI_Restants[idx_POI] = 0;
							}
						}
					}
				}

				if (idx_retenu > -1) {
					pi_POI_Restants[idx_retenu] = 0;
					d_Duree = max(instance->get_POI_Heure_ouverture(i_POI_Choisit) + 0.0, d_Duree + instance->get_distance_POI_POI(pi_Sequence.back(), i_POI_Choisit));
				}
				else {
					d_Duree = d_Duree_Max;
				}
			}

			//printf("Meilleure sequence jour %d : %d\n{ ", i_Jour, i_Score_Jour);
			for (int idx = 0; idx < pi_Sequence.size(); idx++) {
				int i_POI = pi_Sequence[idx];

				for (int i_Jour_Communs : map_conflit_POI[i_POI]) {
					if (i_Jour != i_Jour_Communs) {
						SupprimerElement(&(ppi_POI_par_Jour[i_Jour_Communs]), i_POI);
					}
				}

				//printf(" %d,", i_POI);
			}
			//printf("}\n");

			i_FO += i_Score_Jour;
			pp_Meilleure_Sequence_par_Jour_Fourmis[i_Jour] = pair<float, vector<int>>(i_Score_Jour, pi_Sequence);
		}
		else {
			//printf("Meilleure sequence jour %d : 0\n{ }\n", i_Jour);
		}
	}
	//arret du chrono
	auto chrono_end = chrono::system_clock::now();
	printf("Score Final de la Metaheuristique : %d\n\n", i_FO);

	ofstream fichier(CHEMIN_FICHIER_SORTIE_RESULTATS, ios::app);
	if (fichier)
	{
		static int Instance = 1;
		if (Instance == 1)
		{
			//on ecris une seule fois les parametrages suivant
			fichier << "Parametrage de la Metaheuristique des Fourmis" << endl;
			fichier << "Nombre d'iterations : " << NB_ITERATIONS << endl;
			fichier << "Nombre de fourmis : " << NB_FOURMIS << endl;
			fichier << "Pheromones initiales : " << PHEROMONES_INIT << endl;
			fichier << "Evaporation : " << EVAPORATION << endl;
			fichier << "Augmentation de l'evaporation : " << AUGMENTATION_EVAPORATION << endl;
		}
		//on cree une liste des FO
		vector<int> liste_FO;
		liste_FO.push_back(i_FO);
		vector<int> liste_FO_Optimal;
		liste_FO_Optimal.push_back(816);
		liste_FO_Optimal.push_back(900);
		liste_FO_Optimal.push_back(1062);
		liste_FO_Optimal.push_back(1062);
		liste_FO_Optimal.push_back(1116);
		liste_FO_Optimal.push_back(1236);
		liste_FO_Optimal.push_back(1236);
		liste_FO_Optimal.push_back(1236);
		liste_FO_Optimal.push_back(1284);
		liste_FO_Optimal.push_back(1284);
		liste_FO_Optimal.push_back(1284);
		liste_FO_Optimal.push_back(1670);
		liste_FO_Optimal.push_back(173);
		liste_FO_Optimal.push_back(241);
		liste_FO_Optimal.push_back(367);
		liste_FO_Optimal.push_back(412);
		liste_FO_Optimal.push_back(412);
		liste_FO_Optimal.push_back(504);
		liste_FO_Optimal.push_back(504);
		liste_FO_Optimal.push_back(504);
		liste_FO_Optimal.push_back(590);
		liste_FO_Optimal.push_back(1114);
		liste_FO_Optimal.push_back(1164);
		liste_FO_Optimal.push_back(1234);
		liste_FO_Optimal.push_back(1234);
		liste_FO_Optimal.push_back(1261);
		liste_FO_Optimal.push_back(1306);

		vector<float> liste_Pourcentage;
		vector<float> liste_temps;
		liste_temps.push_back(chrono::duration_cast<chrono::seconds>(chrono_end - chrono_start).count());
		liste_Pourcentage.push_back((float)i_FO / liste_FO_Optimal[Instance - 1] * 100);

		if (chrono::duration_cast<chrono::seconds>(chrono_end - chrono_start).count() > 180)
			fichier << "FO Instance " << Instance << ": " << i_FO << endl << " / t" << "Temps" << ": \033[31m" << chrono::duration_cast<chrono::seconds>(chrono_end - chrono_start).count() << " s\033[0m" << " DELAI DE RESOLUTION DEPASSE" << endl;
		else
		fichier << "FO Instance "<< Instance << ": " << i_FO << endl <<" \t" << "Temps" << ": " << chrono::duration_cast<chrono::seconds>(chrono_end - chrono_start).count() << " s" << "| Pourcentage de la solution optimale : " << liste_Pourcentage[0] << "%" << endl;

		
		//quand Instance = 27 , on fait la moyenne totale par rapport a la liste des FO
		if (Instance == 27)
		{
			int somme = 0;
			for (int i = 0; i < liste_FO.size(); i++)
			{
				somme += liste_FO[i];
			}
			float moyenne = somme / liste_FO.size();
			float somme_Pourcentage = 0;
			for (int i = 0; i < liste_Pourcentage.size(); i++)
			{
				somme_Pourcentage += liste_Pourcentage[i];
			}
			float moyenne_Pourcentage = somme_Pourcentage / liste_Pourcentage.size();
			fichier << "Moyenne des FO : " << moyenne << endl << "Moyenne des pourcentages : " << moyenne_Pourcentage << "%" << endl;
			//on affiche le total des temps
			int somme_temps = 0;
			for (int i = 0; i < liste_temps.size(); i++)
			{
				somme_temps += liste_temps[i];
			}
			fichier << "Temps total : " << somme_temps << " s" << endl;
		}
		
		Instance++;
	}
	else
		cerr << "Impossible d'ouvrir le fichier !" << endl;


	// TODO : Check si meilleur, remplacer si c'est le cas 
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


int choisirIndex_Ameliore(const std::vector<double>& array, const std::vector<int>& pi_POI, const std::vector<int>& pi_POI_Disponibles, const double duree, Instance* instance, int i_POI_Depart, int i_Hotel_Arrive, double d_Duree_Max) {
	double sommeTotale = 0.0;
	unordered_map<int, double> map_POI_Choix;

	for (int idx_POI = 0; idx_POI < array.size(); idx_POI++) {
		int i_POI = pi_POI[idx_POI];
		double valeur = array[idx_POI] * pi_POI_Disponibles[idx_POI];

		if (valeur > 0.0) {
			double d_Duree_Hypo = max(duree + instance->get_distance_POI_POI(i_POI_Depart, i_POI), instance->get_POI_Heure_ouverture(i_POI) + 0.0);

			if (d_Duree_Hypo <= instance->get_POI_Heure_fermeture(i_POI) && d_Duree_Hypo + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI) < d_Duree_Max) {
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

	// Cet �tat ne devrait jamais �tre atteint, mais retourner -1 en cas d'erreur
	return -1;
}
