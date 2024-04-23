#include <iostream>
#include <vector>
#include <algorithm>
#include <random>
#include "MetaHeuristique.h"
#include "..\..\Solution.hpp"

#define Nombre_Max_Iteration 100
#define EVAPORATION 70

int choisirIndex(const std::vector<double>& array);
MetaHeuristique::MetaHeuristique(Instance* instanceParam) {
	
	instance = instanceParam;
	solution = Heuristique_v1::ExtraireSolution(instanceParam);
	pppi_Sequence_par_Jour = {};
	
}

Solution* MetaHeuristique::ExtraireSolution(Instance* instanceParam) 
{
	MetaHeuristique metaHeuristique(instanceParam);

	metaHeuristique.Initialisation();
	metaHeuristique.Solution();

	printf("vs solution Heuristique :\n");
	for (int i_Jour = 0; i_Jour < metaHeuristique.instance->get_Nombre_Jour(); i_Jour++) {
		int i_Score_Sequence = 0;
		for (int i_POI : metaHeuristique.solution->v_v_Sequence_Id_Par_Jour[i_Jour]) {
			i_Score_Sequence += metaHeuristique.instance->get_POI_Score(i_POI);
		}

		printf("Meilleure sequence jour %d : %d\n", i_Jour, i_Score_Sequence);

		printf("{ ");
		for (int i : metaHeuristique.solution->v_v_Sequence_Id_Par_Jour[i_Jour]) {
			printf("%d, ", i);
		}
		printf("}\n");
	}

	return metaHeuristique.solution;
}

void MetaHeuristique::Solution() {
	/*for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];

		vector<vector<int>> ppi_Sequences = GenerationNSequence(pi_POI, 500, i_Jour);

		pppi_Sequence_par_Jour.push_back(ppi_Sequences);
	}*/

	// TODO : Trouver les meilleures sequences possibles (en temps polynomial) pour chaque jours, en fonctions des sets de POI (unique) calculé à chaque jour dans Initialisation

	/*for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		printf("Jour %d :", i_Jour);
		pair<int, vector<int>> pp = pp_Meilleure_Sequence_par_Jour[i_Jour];
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

	vector<vector<double>> ppd_Pheromones;
	vector<double> pd_Pheromones_Depart;
	vector<double> pd_Pheromones_Arrive;

	// ALGO FOURMIS
	for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		ppd_Pheromones = vector<vector<double>>(instance->get_Nombre_POI(), vector<double>(instance->get_Nombre_POI(), 0.0));
		pd_Pheromones_Depart = vector<double>(instance->get_Nombre_POI(), 0.0);
		pd_Pheromones_Arrive = vector<double>(instance->get_Nombre_POI(), 0.0);

		// Initialisation variables
		vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];
		int i_Hotel_Depart = pii_Hotels_par_Jour[i_Jour].first;
		int i_Hotel_Arrive= pii_Hotels_par_Jour[i_Jour].second;
		double d_Duree_Max = instance->get_POI_Duree_Max_Voyage(i_Jour);
	
		// Initialisation pheromones
		for (int i_POI_x = 0; i_POI_x < pi_POI.size(); i_POI_x++) {
			vector<int> pi_Nouvelle_Ligne;

			for (int i_POI_y = 0; i_POI_y < pi_POI.size(); i_POI_y++) {
				if (i_POI_x == i_POI_y) {
					ppd_Pheromones[i_POI_x][i_POI_y] = 0.0;
				}
				else {
					double d_Score = instance->get_POI_Score(i_POI_x) + instance->get_POI_Score(i_POI_y);
					d_Score = max(0.0, d_Score + d_Duree_Max - instance->get_distance_POI_POI(i_POI_x, i_POI_y));

					ppd_Pheromones[i_POI_x][i_POI_y] = d_Score;
				}
			}
			int i_Score = instance->get_POI_Score(i_POI_x);

			pd_Pheromones_Depart[i_POI_x] = max(0.0, i_Score + d_Duree_Max - instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_x));
		}
	
		// DEROULEMENT
		int i_Nb_Fourmis = 1000;

		int iteration = 0;
		bool stop = false;
		vector<vector<double>> ppd_Nouveaux_Pheromones(ppd_Pheromones);
		vector<double> pd_Nouveaux_Pheromones_Depart(pd_Pheromones_Depart);
		double d_Evaporation = EVAPORATION;

		while (iteration < Nombre_Max_Iteration && !stop) {
			pd_Pheromones_Depart = pd_Nouveaux_Pheromones_Depart;
			ppd_Pheromones = ppd_Nouveaux_Pheromones;

			for (int i_POI_x = 0; i_POI_x < pi_POI.size(); i_POI_x++) {
				//printf("%.1lf, ", pd_Pheromones_Depart[i_POI_x]);
				for (int i_POI_y = 0; i_POI_y < pi_POI.size(); i_POI_y++) {
					//printf("%.1lf, ", ppd_Pheromones[i_POI_x][i_POI_y]);
				}
				//printf("\n");
			}
			//printf(" Iteration %d\n\n", iteration);

			for (int i_Fourmis = 0; i_Fourmis < i_Nb_Fourmis; i_Fourmis++) {
				

				/* Parcourt fourmis */
				//printf("Fourmis %d :\n", i_Fourmis);
				vector<int> pi_POI_parcourus;
				int i_Score = 0;

				int i_POI = choisirIndex(pd_Pheromones_Depart);
				double d_Duree = max(instance->get_POI_Heure_ouverture(i_POI), instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI));


				//printf("Ouverture : %lf, Distance depuis H : %lf, Duree Max %lf", instance->get_POI_Heure_ouverture(i_POI), instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI), d_Duree_Max);
				while (d_Duree /* + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI)*/ < d_Duree_Max) {
					//printf("%d =>", i_POI);
					i_Score =  instance->get_POI_Score(i_POI);
					pi_POI_parcourus.push_back(i_POI);
					int i_POI_Choisit;

					do {
						i_POI_Choisit = choisirIndex(ppd_Pheromones[i_POI]);

						//ppd_Pheromones[i_POI][i_POI_Choisit] = max(0.0, ppd_Pheromones[i_POI][i_POI_Choisit] - 3);
					} while (d_Duree + instance->get_distance_POI_POI(i_POI, i_POI_Choisit) > instance->get_POI_Heure_fermeture(i_POI_Choisit) && i_POI_Choisit != -1);
					//ppd_Pheromones[i_POI][i_POI_Choisit] += 3;

					if (i_POI_Choisit != -1) {
						d_Duree = max(0.0 + instance->get_POI_Heure_ouverture(i_POI), d_Duree + instance->get_distance_POI_POI(i_POI, i_POI_Choisit));
						i_POI = i_POI_Choisit;
					}
					else {
						d_Duree = d_Duree_Max + 1.0; // On force l'arret
					}
				}


				/* Maj pheromone */
				double d_Changement = i_Score + pi_POI_parcourus.size();
				//printf("\nScore final : %d Mise a jour de %lf\n\n", i_Score, d_Changement);
				if (pi_POI_parcourus.size() > 0) {
					pd_Nouveaux_Pheromones_Depart[pi_POI_parcourus[0]] = max(0.0, pd_Nouveaux_Pheromones_Depart[pi_POI_parcourus[0]] + d_Changement);
					for (int idx = 0; idx < pi_POI_parcourus.size() - 1; idx++) {
						ppd_Nouveaux_Pheromones[pi_POI_parcourus[idx]][pi_POI_parcourus[idx + 1]] += d_Changement;
					}
				}
			}

			stop = true;
			/* On efface la pheromone */
			for (int i_POI_x = 0; i_POI_x < pi_POI.size(); i_POI_x++) {
				vector<int> pi_Nouvelle_Ligne;

				for (int i_POI_y = 0; i_POI_y < pi_POI.size(); i_POI_y++) {
					int i_Max_Pheromone = max(0.0, ppd_Nouveaux_Pheromones[i_POI_x][i_POI_y] - d_Evaporation);
					ppd_Nouveaux_Pheromones[i_POI_x][i_POI_y] = ppd_Nouveaux_Pheromones[i_POI_x][i_POI_y];

					//stop = stop || i_Max_Pheromone == 0.0;
				}
				int i_Max_Pheromone = max(0.0, pd_Nouveaux_Pheromones_Depart[i_POI_x] - d_Evaporation);
				pd_Nouveaux_Pheromones_Depart[i_POI_x] = i_Max_Pheromone;

				stop = stop && i_Max_Pheromone == 0.0;
			}

			d_Evaporation += 20.0;
			iteration++;
		}

		vector<int> pi_Sequence;
		int i_POI_Choisit = 0;
		double d_Meilleur_Score = 0.0;
		int idx = 0;
		int step = 0;
		int i_Score_Jour = 0;
		for (int i_POI : pi_POI) {
			double d_Score = pd_Pheromones_Depart[i_POI];

			if (d_Score > d_Meilleur_Score) {
				d_Score = d_Meilleur_Score;
				i_POI_Choisit = i_POI;
				idx = step;
			}
			step++;
		}
		pi_POI.erase(pi_POI.begin() + idx);

		double d_Duree = max(instance->get_POI_Heure_ouverture(i_POI_Choisit), instance->get_distance_Hotel_POI(i_Hotel_Depart, i_POI_Choisit));
		while (d_Duree /* + instance->get_distance_Hotel_POI(i_Hotel_Arrive, i_POI)*/ < d_Duree_Max) {
			pi_Sequence.push_back(i_POI_Choisit);
			i_Score_Jour += instance->get_POI_Score(i_POI_Choisit);
			i_POI_Choisit = 0;
			d_Meilleur_Score = 0.0;
			idx = 0;
			step = 0;

			vector<double> pd_Pheromones_POI_Choisit = ppd_Pheromones[i_POI_Choisit];
			for (int i_POI : pi_POI) {
				double d_Score = pd_Pheromones_POI_Choisit[i_POI];

				if (d_Score > d_Meilleur_Score) {
					d_Score = d_Meilleur_Score;
					i_POI_Choisit = i_POI;
					idx = step;
				}
				step++;
			}
			pi_POI.erase(pi_POI.begin() + idx);
			d_Duree = max(instance->get_POI_Heure_ouverture(i_POI_Choisit) + 0.0, d_Duree + instance->get_distance_POI_POI(pi_Sequence.back(), i_POI_Choisit));
		}

		printf("liste => {");
		for (int idx = 0; idx < pi_Sequence.size(); idx++) {
			printf(" %d ,", pi_Sequence[idx]);
		}
		printf("}, Score : %d\n",i_Score_Jour);

		i_FO += i_Score_Jour;
		pp_Meilleure_Sequence_par_Jour[i_Jour] = pair<int, vector<int>>(i_Score_Jour, pi_Sequence);
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

		pii_Hotels_par_Jour.push_back(ii_Hotels);
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
		/*int idx = 0;
		while (idx < pi_POI.size()) {
			int i_POI = pi_POI[idx];
			if (map_conflit_POI[i_POI].size() > 1) {
				pi_POI.erase(pi_POI.begin() + idx);
			}
			else {
				idx++;
			}
		}
		ppi_POI_par_Jour[i_Jour] = pi_POI;*/
	}

	// Affectation à un jour pour chaque POI partagé
	/*for (int i_POI_a_Affecter : pi_POI_Partages) {
		// TODO : Pour chaque, identifier la meilleure sequence cree avec le POI + la sequence des POI propres à la journée
		int i_Meilleur_Jour = 0;
		int i_Meilleur_Score = -1;

		for (int i_Jour : map_conflit_POI[i_POI_a_Affecter]) {
			vector<int> pi_POI_jour_i = ppi_POI_par_Jour[i_Jour];

			bool supprime = false;
			int idx = 0;
			while (!supprime) {
				if (pi_POI_jour_i[idx] == i_POI_a_Affecter) {
					pi_POI_jour_i.erase(pi_POI_jour_i.begin() + idx); 
					ppi_POI_par_Jour[i_Jour] = pi_POI_jour_i;
					supprime = true;
				}

				idx++;
			}

			vector<int> pi_Sequence_Avec_POI = Heuristique_v1::CalculMeilleureJournee(instance, pi_POI_jour_i, map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, { i_POI_a_Affecter });

			//pi_POI_jour_i.push_back(i_POI_a_Affecter);
			vector<int> pi_Meilleure_Sequence_Jour_i = Heuristique_v1::CalculMeilleureJournee(instance, ppi_POI_par_Jour[i_Jour], map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, {});

			int i_Score = GetScoreSequence(pi_Meilleure_Sequence_Jour_i) - GetScoreSequence(pi_Sequence_Avec_POI);

			if (i_Score > i_Meilleur_Score) {
				i_Meilleur_Jour = i_Jour;
				i_Meilleur_Score = i_Score;
			}
		}
		printf("POI %d ajoute au jour %d\n", i_POI_a_Affecter, i_Meilleur_Jour);
		ppi_POI_par_Jour[i_Meilleur_Jour].push_back(i_POI_a_Affecter);
	}*/

	// On trouve la meilleure sequence de la journée ( score : sequence )
	for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
		pair<int, vector<int>> p_Meilleure_Sequence;
		p_Meilleure_Sequence.first = 0;
		p_Meilleure_Sequence.second = Heuristique_v1::CalculMeilleureJournee(instance, ppi_POI_par_Jour[i_Jour], map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, {});

		for (int i_POI : p_Meilleure_Sequence.second) {
			p_Meilleure_Sequence.first += instance->get_POI_Score(i_POI);
		}

		pp_Meilleure_Sequence_par_Jour.push_back(p_Meilleure_Sequence);
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

int choisirIndex(const std::vector<double>& array) {
	double sommeTotale = 0;
	for (double valeur : array) {
		sommeTotale += valeur;
	}

	if (sommeTotale == 0)
		return -1;

	// Générer un nombre aléatoire entre 0 et la somme totale - 1
	std::random_device rd;
	std::mt19937 gen(rd());
	std::uniform_int_distribution<> dis(0, sommeTotale - 1);
	int choix = dis(gen);

	// Parcourir le vecteur et choisir l'index en fonction du nombre aléatoire généré
	int index = 0;
	double sommePartielle = 0;
	for (double valeur : array) {
		sommePartielle += valeur;
		if (choix < sommePartielle) {
			return index;
		}
		index++;
	}

	// Cet état ne devrait jamais être atteint, mais retourner -1 en cas d'erreur
	return -1;
}