#include "Heuristique_v1.h"
#include "..\..\Solution.hpp"

#define INFINI 99999
#define N 3

#define POI_SCORE_INITIAL 1.0
#define POI_TEMPS_OUVERT 1.5

#define HOTEL_SCORE_POI 1
#define HOTEL_DISTANCE_AU_POI 1
#define HOTEL_PROJECTION 1.5

#define POI_DISTANCES_AUX_HOTELS 0.5

#define SEQUENCE_TPS_LIBRES 1.0

/// <summary>
/// Constructeur de l'Heuristique
/// </summary>
/// <param name="instanceParam"> Instance a resoudre </param>
Heuristique_v1::Heuristique_v1(Instance* instanceParam) {
    instance = instanceParam;
    solution = new Solution();

    int nbJour = instance->get_Nombre_Jour();

    solution->v_Date_Depart = vector<float>(nbJour);
    solution->v_v_Sequence_Id_Par_Jour = vector<vector<int>>(nbJour);
    solution->v_Id_Hotel_Intermedaire = vector<int>(nbJour - 1);

    i_FO = 0;
    f_Duree_Totale_Restante = 0.0;
    for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
        f_Duree_Totale_Restante += instance->get_POI_Duree_Max_Voyage(i_Jour);
    }
    pp_Meilleure_Sequence_par_Jour = vector <pair<float, vector<int>>>(nbJour);
    pmpi_Rayon_Hotels = {};
}

/// <summary>
/// Déroule l'algorithme
/// </summary>
void Heuristique_v1::Solution1() {
    /* Initialisation des listes */
    vector<int> pi_POI;
    for (int i_POI = 0; i_POI < instance->get_Nombre_POI(); i_POI++) {
        pi_POI.push_back(i_POI);
    }
    vector<int> pi_POI_Disponibles(pi_POI);

    vector<int> pi_Hotels;
    for (int i_Hotel = 0; i_Hotel < instance->get_Nombre_Hotel(); i_Hotel++) {
        pi_Hotels.push_back(i_Hotel);
    }

    /* Identification des hotels intermediaires */
    int i_Hotel_Debut_Journee = instance->get_Id_Hotel_depart();

    int i_Jour = 0;

    // ETAPE 1 : RECALCULER LES SCORES A CHAQUE POI
    for (int i_POI_Coherent = 0; i_POI_Coherent < instance->get_Nombre_POI(); i_POI_Coherent++) {
        map_Score_POI[i_POI_Coherent] = CalculScorePOI(i_POI_Coherent, instance);
    }

    // ETAPE 2 : DETERMINATION D'UNE PREMIERE SOLUTION (ON GARDERA LA SEQUENCE D'HOTELS)
    while (i_Jour < instance->get_Nombre_Jour()) {
        // ETAPE 2-1 : DETERMINATION DE L'HOTEL A LA FIN DU JOUR i_Jour

        pair<int, int> ii_Hotels_Jour_I(i_Hotel_Debut_Journee, 0); // On initie le couple d'hotels associes a la journee

        /* Identifies les hotels joignables a la fin de la journée i_Jour */
        vector<int> pi_Rayon_Hotels = IdentifierHotelsFinJournee(i_Hotel_Debut_Journee, i_Jour, pi_Hotels);

        int i_Hotel_Fin_Journee = -1;
        float f_Score_Meilleur_Hotel = -1;
        int i_idx_Hotel = 0;
        int i_idx_Meilleur_Hotel = 0;

        /* Pour chaque Hotels, on lui affecte un score, et on regarde si ce score est meilleur que celui de l'hotel sauvegardé  */
        for (int i_Hotel_Possible : pi_Rayon_Hotels)
        {
            vector<int> pi_Rayon = IdentifierPOIRayonHotel(i_Hotel_Debut_Journee, i_Hotel_Possible, instance, pi_POI, i_Jour);

            float f_Score = CalculScoreHotel(i_Hotel_Possible, pi_Rayon, i_Hotel_Debut_Journee);

            /* S'il s'agit du meilleur hotel, on le sauvegarde comme le meilleur */
            if (f_Score > f_Score_Meilleur_Hotel)
            {
                i_Hotel_Fin_Journee = i_Hotel_Possible;
                f_Score_Meilleur_Hotel = f_Score;
                i_idx_Meilleur_Hotel = i_idx_Hotel;
            }
            i_idx_Hotel++;
        }

        /* Si un hotel a ete retenu (si pi_Rayon_Hotels n'est pas vide) */
        if (i_Hotel_Fin_Journee > -1) {
            ii_Hotels_Jour_I.second = i_Hotel_Fin_Journee; // On sauvegarde l'hotel

            /* On sauvegarde les autres POI qui aurait pu être choisit (au cas où on arrive dans une impasse, plus d'hotels possibles) */
            pi_Rayon_Hotels.erase(pi_Rayon_Hotels.begin() + i_idx_Meilleur_Hotel);
            if (pmpi_Rayon_Hotels.size() <= i_Jour) {
                unordered_map<int, vector<int>> nv_map;
                nv_map[i_Hotel_Debut_Journee] = pi_Rayon_Hotels;
                pmpi_Rayon_Hotels.push_back(nv_map);
                pii_Hotels_par_Jour.push_back(ii_Hotels_Jour_I);
            }
            else {
                pmpi_Rayon_Hotels[i_Jour][i_Hotel_Debut_Journee] = pi_Rayon_Hotels;
                pii_Hotels_par_Jour[i_Jour] = ii_Hotels_Jour_I;
            }

            i_Hotel_Debut_Journee = i_Hotel_Fin_Journee; // On opere les changements pour la prochaine journée

            // ETAPE 2-2 : DETERMINER LES POI TRAVERSES POUR ATTEINDRE L'HOTEL CHOISIT
            pair<float, vector<int>> p_Sequence_Jour_I;
            p_Sequence_Jour_I = CalculMeilleureJournee_v1(i_Jour, &pi_POI_Disponibles);
            pp_Meilleure_Sequence_par_Jour[i_Jour] = p_Sequence_Jour_I;

            i_FO += CalculScoreSequence(p_Sequence_Jour_I.second, instance); // Sauvegarde du score de la sequence

            f_Duree_Totale_Restante -= instance->get_POI_Duree_Max_Voyage(i_Jour);

            i_Jour++;
        }
        /* Sinon si aucun hotel n'a pu etre trouve (backtrack) */
        else {
            // BACKTRACK : ON REVIENT SUR LE JOUR PRECEDENT POUR CHOISIR UN AUTRE HOTEL QUI NE SERA PAS BLOQUANT

            i_Jour--;

            i_Hotel_Debut_Journee = pii_Hotels_par_Jour[i_Jour].first;
            f_Duree_Totale_Restante += instance->get_POI_Duree_Max_Voyage(i_Jour);

            i_FO -= CalculScoreSequence(pp_Meilleure_Sequence_par_Jour[i_Jour].second, instance); // On revient au FO au debut de la journee precedente

            /* On restaure les POI que l'on avait choisit */
            for (int i_POI_A_Restaurer : pp_Meilleure_Sequence_par_Jour[i_Jour].second) {
                pi_POI_Disponibles.push_back(i_POI_A_Restaurer);
            }
        }
    }

    /* Sauvegarde des hotels choisit */
    for (i_Jour = 0; i_Jour < instance->get_Nombre_Jour() - 1; i_Jour++) {
        solution->v_Id_Hotel_Intermedaire[i_Jour] = pii_Hotels_par_Jour[i_Jour].second;
    }

    /* A chaque jours, on determine et on sauvergarde les POI possibles */
    for (i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
        ppi_POI_par_Jour.push_back(Heuristique_v1::IdentifierPOIRayonHotel(pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, instance, pi_POI, i_Jour));
    }

    // ETAPE 3 : DETERMINATION DES POI QUI SONT IDENTIFIES A PLUSIEURS JOURS
    vector<int> pi_POI_Partages;

    for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
        vector<int> pi_POI = ppi_POI_par_Jour[i_Jour];
        for (int i_POI : pi_POI) {
            /* Si on a pas encore vu le POI, on initie l'indice i_POI dans map_conflit_POI */
            if (map_conflit_POI.find(i_POI) == map_conflit_POI.end()) {
                map_Score_POI[i_POI] = Heuristique_v1::CalculScorePOI(i_POI, instance); // Initialisation du score
                map_conflit_POI[i_POI] = { i_Jour }; // Conflit initialisé avec l'id du 1er jour pouvant utiliser le POI
            }
            /* Sinon on met a jour les jours en conflits */
            else {
                if (map_conflit_POI[i_POI].size() == 1) {
                    pi_POI_Partages.push_back(i_POI);
                }
                map_conflit_POI[i_POI].push_back(i_Jour);
            }
        }
    }

    // ETAPE 4 : MEILLEUR ORDRE POUR TRAITER LES JOURS 

    /* Permettra de trier les Jours en fonction du nombre de fois qu'il est selectionne comme meilleur jour pour le POI */
    unordered_map<int, int> map_Meilleurs_Jours;
    for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
        map_Meilleurs_Jours[i_Jour] = 0;
    }

    /* Pour chaque POI en conflit, on determine quelle journée (parmis les journée qui se le partage) profite le mieux du POI */
    for (int i_POI_a_Affecter : pi_POI_Partages) {
        int i_Meilleur_Jour = -1;
        int i_Meilleur_Score = INT_MIN;

        for (int i_Jour : map_conflit_POI[i_POI_a_Affecter]) {
            vector<int> pi_POI_jour_i = ppi_POI_par_Jour[i_Jour];

            vector<int> pi_Sequence_Avec_POI = Heuristique_v1::CalculMeilleureJournee(instance, pi_POI_jour_i, map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, { i_POI_a_Affecter }).second;

            vector<int> pi_Meilleure_Sequence_Jour_i = Heuristique_v1::CalculMeilleureJournee(instance, ppi_POI_par_Jour[i_Jour], map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, {}).second;

            int i_Score = CalculScoreSequence(pi_Meilleure_Sequence_Jour_i, instance) - CalculScoreSequence(pi_Sequence_Avec_POI, instance);

            if (i_Score >= i_Meilleur_Score) {
                i_Meilleur_Jour = i_Jour;
                i_Meilleur_Score = i_Score;
            }
        }
        map_Meilleurs_Jours[i_Meilleur_Jour]++;
        map_conflit_POI[i_POI_a_Affecter].push_back(i_Meilleur_Jour); // On place le meilleur jour a l'arriere pour s'en souvenir
    }

    /* On tri en fonction de map_Meilleurs_Jours */
    vector<pair<int, int>> pp_Tri_Des_Jours(map_Meilleurs_Jours.begin(), map_Meilleurs_Jours.end());
    std::sort(pp_Tri_Des_Jours.begin(), pp_Tri_Des_Jours.end(), [](const auto& a, const auto& b) {
        return a.second > b.second;
        });
    for (const auto& pair : pp_Tri_Des_Jours) {
        pi_Jours_Tries.push_back(pair.first);
    }

    /* On empeche les premiers a jours a utiliser les POI qui sont meilleurs dans des jours qui sont traités plus tard */
    unordered_map<int, int> map_Jours_Traites;
    for (int i_Jour : pi_Jours_Tries) {
        int idx = 0;
        while (idx < ppi_POI_par_Jour[i_Jour].size()) {
            int i_POI_A_Pottentielment_Supprime = ppi_POI_par_Jour[i_Jour][idx];
            int i_Jour_Back = map_conflit_POI[i_POI_A_Pottentielment_Supprime].back();

            if (i_Jour_Back != i_Jour && map_Jours_Traites[i_Jour_Back] == 0) {
                ppi_POI_par_Jour[i_Jour].erase(ppi_POI_par_Jour[i_Jour].begin() + idx);
            }
            else {
                idx++;
            }
        }

        map_Jours_Traites[i_Jour] = 1; // On marque que le jour a ete traite
    }

    // ETAPE 5 : DETERMINATION D'UNE SECONDE SOLUTION, EN PRENNANT EN COMPTE L'ORDRE POUR TRAITER LES JOURS

    int i_FO_v2 = 0;
    vector<pair<float, vector<int>>> pp_Meilleure_Sequence_par_Jour_v2(instance->get_Nombre_Jour());

    vector<vector<int>> ppi_POI_par_Jour_Heuristique(ppi_POI_par_Jour);
    for (int i_Jour : pi_Jours_Tries) {
        pair<float, vector<int>> p_Meilleure_Sequence;
        p_Meilleure_Sequence = CalculMeilleureJournee_v2(i_Jour, &ppi_POI_par_Jour_Heuristique[i_Jour]);

        for (int i_POI : p_Meilleure_Sequence.second) {
            i_FO_v2 += instance->get_POI_Score(i_POI); // Mise a jour de la fonction objective

            /* On supprime les POI de la sequence pour empêcher qu'il soit utiliser 2 fois */
            for (int i_Jour_Communs : map_conflit_POI[i_POI]) {
                if (i_Jour != i_Jour_Communs) {
                    SupprimerElement(&(ppi_POI_par_Jour_Heuristique[i_Jour_Communs]), i_POI);
                }
            }
        }

        pp_Meilleure_Sequence_par_Jour_v2[i_Jour] = p_Meilleure_Sequence;
    }

    printf("Score final 1ere Heuristique : %d\n", i_FO);

    /* Si la seconde solution est meilleure, on la remplace */
    if (i_FO_v2 > i_FO) {
        i_FO = i_FO_v2;
        pp_Meilleure_Sequence_par_Jour = pp_Meilleure_Sequence_par_Jour_v2;
    }

    SauvegarderSolution(); // On sauvegarde dans Solutiob

    printf("Score final 2nde Heuristique : %d\n", i_FO_v2);
}

/// <summary>
/// Donne la distance d'un hotel au POI en fonction de leurs coordonnees
/// </summary>
/// <param name="i_Hotel"></param>
/// <param name="i_POI"></param>
/// <returns></returns>
float GetVraieDistanceHotelPOI(Instance* instanceParam, int i_Hotel, int i_POI) {
    return sqrt(pow(instanceParam->get_Hotel_Coord_X(i_Hotel) - instanceParam->get_POI_Coord_X(i_POI), 2)
        + pow(instanceParam->get_Hotel_Coord_X(i_Hotel) - instanceParam->get_POI_Coord_Y(i_POI), 2));
}

/// <summary>
/// Méthode appelé dans le main pour lancer l'heuristique et stocker la solution dans Solution
/// </summary>
/// <param name="instanceParam"> Instance a resoudre </param>
/// <returns></returns>
Solution* Heuristique_v1::ExtraireSolution(Instance* instanceParam) {
    Solution* nouvelleSolution = new Solution();
    Heuristique_v1* heuristique = new Heuristique_v1(instanceParam);

    heuristique->Solution1();

    return nouvelleSolution;
}

/// <summary>
/// Determine le score du POI en paramètre
/// </summary>
/// <param name="i_POI_Param"></param>
/// <param name="instance"></param>
/// <returns></returns>
double Heuristique_v1::CalculScorePOI(int i_POI_Param, Instance* instance)
{
    double f_Score_Total = POI_SCORE_INITIAL * instance->get_POI_Score(i_POI_Param) +(instance->get_POI_Heure_fermeture(i_POI_Param) - instance->get_POI_Heure_ouverture(i_POI_Param) * POI_TEMPS_OUVERT);
    return f_Score_Total;
}

/// <summary>
/// Identifie les hotels joignables en une moins d'1 journée depuis l'hotel en parametre
/// </summary>
/// <param name="i_Hotel_Depart"></param>
/// <param name="i_Jour"></param>
/// <param name="pi_Hotels"></param>
/// <returns> Identifies les hotels possible à la fin de la journée </returns>
vector<int> Heuristique_v1::IdentifierHotelsFinJournee(int i_Hotel_Depart, int i_Jour, vector<int> pi_Hotels)
{
    vector<int> pi_Rayon_Hotels;

    /* Si on en est à la dernière journée, on ne regarde que l'hotel d'arrivée */
    if (i_Jour + 1 == instance->get_Nombre_Jour()) {
        pi_Rayon_Hotels.push_back(instance->get_Id_Hotel_Arrivee());
        return pi_Rayon_Hotels;
    }

    /* Si on a déjà exploré les hotels(car on a fait un backtrack), on renvoit ce que l'on avait trouvé (sans l'hotel problematique, que l'on avait pris soin de ne pas sauvegarde) */
    if (pmpi_Rayon_Hotels.size() >= i_Jour + 1 && pmpi_Rayon_Hotels[i_Jour].find(i_Hotel_Depart) != pmpi_Rayon_Hotels[i_Jour].end()){
        return pmpi_Rayon_Hotels[i_Jour][i_Hotel_Depart];
    }

    vector<int> pi_Hotels_Supprimes = {};
    for (int i_Hotel_Coherent : pi_Hotels)
    {

        // Si l'hotel est à une distance de l'hotel d'arrivé supérieur au nb de jours restants après cette journée
        if (f_Duree_Totale_Restante - instance->get_POI_Duree_Max_Voyage(i_Jour) >= instance->get_distance_Hotel_Hotel(i_Hotel_Coherent, instance->get_Id_Hotel_Arrivee()))
        {
            // Si l'hotel peut etre rejoint en moins d'une journée
            if (instance->get_distance_Hotel_Hotel(i_Hotel_Depart, i_Hotel_Coherent) <= instance->get_POI_Duree_Max_Voyage(i_Jour))
            {
                pi_Rayon_Hotels.push_back(i_Hotel_Coherent); // Il est candidat aux hotels de fin
            }
        }
    }

    return pi_Rayon_Hotels;
}

/// <summary>
/// Methode qui permet d'identifier les POI dans joignable en moins d'1 journée
/// </summary>
/// <param name="i_Hotel_Debut_Journee_Param"></param>
/// <param name="i_Hotel_Param"> Le pottentiel hotel de fin de journée </param>
/// <param name="instanceParam"></param>
/// <param name="pi_POI_Coherents_Param"></param>
/// <param name="i_Jour_Param"></param>
/// <returns> Liste des POI </returns>
vector<int> Heuristique_v1::IdentifierPOIRayonHotel(int i_Hotel_Debut_Journee_Param, int i_Hotel_Param, Instance* instanceParam, vector<int> pi_POI_Coherents_Param, int i_Jour_Param)
{
    vector<int> pi_POI_DANS_LE_RAYON = {};

    for (int i_POI_Coherent : pi_POI_Coherents_Param) {
        float f_Distance_Depuis_H = instanceParam->get_distance_Hotel_POI(i_Hotel_Debut_Journee_Param, i_POI_Coherent);

        float f_Visite_Distance_a_Hfin = instanceParam->get_distance_Hotel_POI(i_Hotel_Param, i_POI_Coherent);
        
      
        if (f_Distance_Depuis_H <= instanceParam->get_POI_Heure_fermeture(i_POI_Coherent) &&
            f_Distance_Depuis_H + f_Visite_Distance_a_Hfin < instanceParam->get_POI_Duree_Max_Voyage(i_Jour_Param) &&
            instanceParam->get_POI_Heure_ouverture(i_POI_Coherent) + f_Visite_Distance_a_Hfin < instanceParam->get_POI_Duree_Max_Voyage(i_Jour_Param))
        {
            pi_POI_DANS_LE_RAYON.push_back(i_POI_Coherent);
        }
    }

    return pi_POI_DANS_LE_RAYON;
}

/// <summary>
/// Methode qui permet de calculer le score d'un hotel en fonction des POI dans le rayon
/// </summary>
/// <param name="i_Hotel_Param"></param>
/// <param name="pi_POI_Dans_Le_Rayon"></param>
/// <returns> Score total de l'hotel </returns>
float Heuristique_v1::CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon, int i_Hotel_Debut_Journee)
{
    float f_Score_Total = (float) max(0.0, HOTEL_PROJECTION * GetAbsiceProjectionHotel(i_Hotel_Param, i_Hotel_Debut_Journee));

    for (int Id_POI : pi_POI_Dans_Le_Rayon)
    {
        float f_Score = (map_Score_POI[Id_POI] * HOTEL_SCORE_POI);

        float distance = GetVraieDistanceHotelPOI(instance, i_Hotel_Param, Id_POI) + 1.0; // On evite distance = 0 et pour pas que le score soit enormement augmente
        f_Score = HOTEL_DISTANCE_AU_POI * (f_Score / distance);

        f_Score_Total += f_Score;
    }

    return f_Score_Total ;
}

/// <summary>
/// Determine la meilleure succession de POI pour une journée, à partir des hotels de départ et d'arrivé (de la journée) et des POI cohérents
/// </summary>
/// <param name="i_Jour"> Journée en cours </param>
/// <param name="pi_POI_Disponibles"> Liste des POI qui peuvent être choisit </param>
/// <returns> Meilleure sequence de POI trouvé ainsi que l'heure de départ </returns>
pair<float, vector<int>> Heuristique_v1::CalculMeilleureJournee_v1(int i_Jour, vector<int>* pi_POI_Disponibles)
{
    pair<float, vector<int>> p_Meilleure_Journee = CalculMeilleureJournee(instance, *pi_POI_Disponibles, map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, {});

    /* On supprime les POI qui ont été utilisés, ils ne sont plus disponibles pour les jours suivants */
    for (int i : p_Meilleure_Journee.second) {
        SupprimerElement(pi_POI_Disponibles, i);
    }

    return p_Meilleure_Journee;
}

/// <summary>
/// Determine la meilleure succession de POI pour une journée, à partir des hotels de départ et d'arrivé (de la journée) et des POI cohérents
/// </summary>
/// <param name="i_Jour"> Journée en cours </param>
/// <param name="pi_POI_Disponibles"> Liste des POI qui peuvent être choisit </param>
/// <returns> Meilleure sequence de POI trouvé ainsi que l'heure de départ </returns>
pair<float, vector<int>> Heuristique_v1::CalculMeilleureJournee_v2(int i_Jour, vector<int>* pi_POI_Disponibles)
{
    pair<float, vector<int>> p_Meilleure_Journee = CalculMeilleureJournee(instance, *pi_POI_Disponibles, map_Score_POI, pii_Hotels_par_Jour[i_Jour].first, pii_Hotels_par_Jour[i_Jour].second, i_Jour, {});

    return p_Meilleure_Journee;
}

/// <summary>
/// Determine la meilleure succession de POI pour une journée, à partir des hotels de départ et d'arrivé (de la journée) et des POI cohérents
/// </summary>
/// <param name="instance"></param>
/// <param name="pi_POI_Joignables"> Liste des POI qui peuvent être choisit </param>
/// <param name="map_Score_POI"> Score affecté à chaque POI </param>
/// <param name="i_Hotel_Debut_Journee"> Hotel choisit en début de journée </param>
/// <param name="i_Hotel_Fin_Journee"> Hotel en fin de journée </param>
/// <param name="i_Jour"> Journée en cours </param>
/// <param name="pi_Sequence_Initiale"> Séquence à partir de laquelle on part (utilisé pour l'affectation des POI aux jours (Etape 3))</param>
/// <returns> Meilleure sequence de POI trouvé ainsi que l'heure de départ </returns>
pair<float, vector<int>> Heuristique_v1::CalculMeilleureJournee(Instance* instance, vector<int> pi_POI_Joignables, unordered_map<int, double> map_Score_POI, int i_Hotel_Debut_Journee, int i_Hotel_Fin_Journee, int i_Jour, vector<int> pi_Sequence_Initiale)
{
    pair<float, vector<int>> fpi_Meilleure_paire(0.0, pi_Sequence_Initiale);
    int i_Score_Meilleure_Paire = CalculScoreSequence(pi_Sequence_Initiale, instance);

    /* On modifie le score pour donner plus de poids aux POI alignés avec les deux hotels */
    unordered_map<int, double> map_score_POI_Pour_H;
    for (int i_POI : pi_POI_Joignables) {
        map_score_POI_Pour_H[i_POI] = map_Score_POI[i_POI] + (POI_DISTANCES_AUX_HOTELS / ((GetVraieDistanceHotelPOI(instance, i_Hotel_Debut_Journee, i_POI) + GetVraieDistanceHotelPOI(instance, i_Hotel_Fin_Journee, i_POI) + 1.0)));
    }

    std::vector<std::pair<int, double>> vec_POI_Tries(map_score_POI_Pour_H.begin(), map_score_POI_Pour_H.end());

    /* Trie par valeur (en fonction du score) */
    std::sort(vec_POI_Tries.begin(), vec_POI_Tries.end(),
        [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
            return a.second > b.second; 
        }
    );

    /* On calcule N sequence et on garde la meilleure */
    int taille = vec_POI_Tries.size();
    for (int idx = 0; idx < N && taille > 0; idx ++) {
        pair<float, vector<int>> paire;

        /* On essaye d'insérer chaque POI un à un */
        for (auto i : vec_POI_Tries)
        {
            int i_POI = i.first;

            paire = MeilleureSequence(paire, i_POI, instance, i_Hotel_Debut_Journee, i_Hotel_Fin_Journee, i_Jour);
        }

        int i_Score = CalculScoreSequence(paire.second, instance);

        if (i_Score > i_Score_Meilleure_Paire) {
            i_Score_Meilleure_Paire = i_Score;
            fpi_Meilleure_paire = paire;
        }

        taille--;
        if (taille > 0)
            vec_POI_Tries.erase(vec_POI_Tries.begin()); // On reessayes en supprimant le 1er element (permet de varier les solutions)
    }

    return fpi_Meilleure_paire;
}

/// <summary>
/// Détermine la meilleure séquence (qui laisse le plus de place pour insérer d'autre POI), à partir d'une séquence en paramètre et un POI à insérer
/// </summary>
/// <param name="fpi_Paire_Sequence"> La meilleure séquence actuelle et son heure de départ </param>
/// <param name="i_POI"> Le POI à intégrer à la séquence </param>
/// <param name="instance"></param>
/// <param name="i_Hotel_Debut_Journee"> Hotel choisit en début de journée </param>
/// <param name="i_Hotel_Fin_Journee"> Hotel en fin de journée </param>
/// <param name="i_Jour"> Journée en cours </param>
/// <returns> Paire contenant l'heure de départ et la séquence de POI </returns>
pair<float, vector<int>> Heuristique_v1::MeilleureSequence(pair<float, vector<int>> fpi_Paire_Sequence, int i_POI, Instance* instance, int i_Hotel_Debut_Journee, int i_Hotel_Fin_Journee, int i_Jour)
{
    vector<int> pi_Meilleure_Sequence = fpi_Paire_Sequence.second;
    float f_Meilleur_Score_Sequence = -1; // On met à -1 car on préferera une sequence qui inclu id_POI
    float f_Meilleur_Depart = fpi_Paire_Sequence.first;

    /* On cherche la meilleure séquence en fixant l'heure de départ de sorte à arriver à l'ouverture du premier POI */
    for (int i = 0; i < fpi_Paire_Sequence.second.size() + 1; i++) {
        vector<int> pi_Sequence_i = fpi_Paire_Sequence.second;

        pi_Sequence_i.insert(pi_Sequence_i.begin() + i, i_POI);

        float f_Depart =  max(0.0, 0.0 + instance->get_POI_Heure_ouverture(pi_Sequence_i[0]) - instance->get_distance_Hotel_POI(i_Hotel_Debut_Journee, pi_Sequence_i[0]));

        float f_Score_Sequence = GetScoreSequence(pi_Sequence_i, instance, i_Hotel_Debut_Journee, i_Hotel_Fin_Journee, i_Jour, f_Depart);
            
        if (f_Score_Sequence > f_Meilleur_Score_Sequence) {
            pi_Meilleure_Sequence = pi_Sequence_i;
            f_Meilleur_Score_Sequence = f_Score_Sequence;
            f_Meilleur_Depart = f_Depart;
        }
    }

    /* On cherche à améliorer la meilleure séquence en fixant l'heure de départ à 0.0 */
    for (int i = 0; i < fpi_Paire_Sequence.second.size() + 1; i++) {
        vector<int> pi_Sequence_i = fpi_Paire_Sequence.second;

        pi_Sequence_i.insert(pi_Sequence_i.begin() + i, i_POI);

        float f_Score_Sequence = GetScoreSequence(pi_Sequence_i, instance, i_Hotel_Debut_Journee, i_Hotel_Fin_Journee, i_Jour, 0.0);

        if (f_Score_Sequence > f_Meilleur_Score_Sequence) {
            pi_Meilleure_Sequence = pi_Sequence_i;
            f_Meilleur_Score_Sequence = f_Score_Sequence;
            f_Meilleur_Depart = 0.0;
        }
    }

    pair<float, vector<int>> paire(f_Meilleur_Depart, pi_Meilleure_Sequence);

    return paire;
}

/// <summary>
/// Renvoie un score basé sur les temps libres (attente avant ouverture POI, nb d'heures restantes à la fin) de la journée, donnant plus d'importance aux longues périodes
/// </summary>
/// <param name="pi_Trajet"> Séquence de POI à scorer </param>
/// <param name="instance"></param>
/// <param name="i_Hotel_Debut_Journee"> Hotel choisit en début de journée </param>
/// <param name="i_Hotel_Fin_Journee"> Hotel en fin de journée </param>
/// <param name="i_Jour"> Journée en cours </param>
/// <param name="f_Heure_Debut"> Début de la journée choisit </param>
/// <returns></returns>
float Heuristique_v1::GetScoreSequence(vector<int> pi_Trajet, Instance* instance, int i_Hotel_Debut_Journee, int i_Hotel_Fin_Journee, int i_Jour, float f_Heure_Debut)
{
    float f_Duree_Trajet = 0.0;
    float f_Score = 0.0;
    float f_Dist_Totale_Parcourue = 0.0;
    float f_Dist = 0.0;

    f_Dist = instance->get_distance_Hotel_POI(i_Hotel_Debut_Journee, pi_Trajet[0]);
    f_Dist_Totale_Parcourue += f_Dist;
    f_Duree_Trajet = f_Duree_Trajet + f_Dist + f_Heure_Debut;
    int j = 0;
    do
    {
        float f_HeureOuverture = instance->get_POI_Heure_ouverture(pi_Trajet[j]);
        if (f_Duree_Trajet < f_HeureOuverture) {
            f_Score += (float) pow(f_HeureOuverture - (f_Duree_Trajet - f_Heure_Debut), 2);
            f_Duree_Trajet = f_HeureOuverture;
        }
        else
        {
            if (f_Duree_Trajet > instance->get_POI_Heure_fermeture(pi_Trajet[j]))
            {
                return -1;
            }
        }
        j++;
        if (j < pi_Trajet.size())
        {
            f_Dist = instance->get_distance_POI_POI(pi_Trajet[j - 1], pi_Trajet[j]);
            f_Dist_Totale_Parcourue += f_Dist;
            f_Duree_Trajet = f_Duree_Trajet + f_Dist;
        }
    } while (j < pi_Trajet.size());

    f_Dist = instance->get_distance_Hotel_POI(i_Hotel_Fin_Journee, pi_Trajet[j - 1]);
    f_Dist_Totale_Parcourue += f_Dist;
    f_Duree_Trajet = f_Duree_Trajet + f_Dist;
    if ((f_Duree_Trajet - f_Heure_Debut) > instance->get_POI_Duree_Max_Voyage(i_Jour))
    {
        return -1;
    }
    f_Score += (float) pow(instance->get_POI_Duree_Max_Voyage(i_Jour) - (f_Duree_Trajet - f_Heure_Debut), 2);

    return (f_Score * SEQUENCE_TPS_LIBRES);
}

/// <summary>
/// Permet de sauvegarder la solution trouvée dans Solution
/// </summary>
void Heuristique_v1::SauvegarderSolution() {

    for (int i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++) {
        pair<float, vector<int>> p_Meilleure_Sequence_Jour_I = pp_Meilleure_Sequence_par_Jour[i_Jour];

        solution->v_Date_Depart[i_Jour] = p_Meilleure_Sequence_Jour_I.first;
        solution->v_v_Sequence_Id_Par_Jour[i_Jour] = p_Meilleure_Sequence_Jour_I.second;
    }

    solution->i_valeur_fonction_objectif = i_FO;
}

/// <summary>
/// Permet de calculer le score d'une séquence de POI
/// </summary>
/// <param name="pi_Sequence"></param>
/// <param name="instance"></param>
/// <returns></returns>
int Heuristique_v1::CalculScoreSequence(vector<int> pi_Sequence, Instance* instance) {
    int i_Score_Sequence = 0;

    for (int i_POI : pi_Sequence) {
        i_Score_Sequence += instance->get_POI_Score(i_POI);
    }

    return i_Score_Sequence;
}

/// <summary>
/// Permet d'obtenir de combien i_Hotel_Param rapproche de l'hotel d'arrive par rapport à sa projection sur la droite reliant l'hotel d'arrive à i_Hotel_Debut_Journee 
/// </summary>
/// <param name="i_Hotel_Param"></param>
/// <param name="i_Hotel_Debut_Journee"></param>
/// <returns></returns>
float Heuristique_v1::GetAbsiceProjectionHotel(int i_Hotel_Param, int i_Hotel_Debut_Journee)
{
    int i_Hotel_Arrive = instance->get_Id_Hotel_Arrivee();
    float ds;
    float da;
    float renvoi; // Stockera de combien rapproche l'hotel en parametre (si chosit) de l'hotel d'arrive, en se basant sur sa projection sur la droite passant par l'hotel de depart et l'hotel d'arrive

    /* Coordonnées de l'hotel de depart */
    float xs = instance->get_Hotel_Coord_X(i_Hotel_Debut_Journee);
    float ys = instance->get_Hotel_Coord_Y(i_Hotel_Debut_Journee);

    /* Coordonnées de l'hotel d'arrivee */
    float xa = instance->get_Hotel_Coord_X(i_Hotel_Arrive);
    float ya = instance->get_Hotel_Coord_Y(i_Hotel_Arrive);

    /* Coordonnées de l'hotel en parametre */
    float xhotel = instance->get_Hotel_Coord_X(i_Hotel_Param);
    float yhotel = instance->get_Hotel_Coord_Y(i_Hotel_Param);

    /* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    float Vx = xa - xs;
    float Vy = ya - ys;

    if (Vx != 0.0 && Vy != 0.0) {
        float a = Vy / Vx;       // Droite : aX
        float c = ya - (a * xa); // Droite : aX + c

        float ap = -1 * (Vx / Vy); // Droite orthogonale : apX

        float cp = yhotel - (ap * xhotel); // Droite orthogonale : apX + cp

        float xp = (cp - c) / (a - ap); // Intersection des deux droites

        float yp = a * xp + c; // Intersection des deux droites

        ds = (float)sqrt(pow(xp - xs, 2) + pow(yp - ys, 2)); // Distance entre l'hotel de depart et l'hotel en parametre
        da = (float) sqrt(pow(xp - xa, 2) + pow(yp - ya, 2)); // Distance entre l'hotel d'arrive et l'hotel en parametre
        float d = (float) sqrt(pow(xs - xa, 2) + pow(ys - ya, 2));

        // Si l'hotel de depart se trouve "avant" l'hotel d'arrive
        if (xs < xa) 
        {
            // Si Hotel Param "fait reculer" par rapport a l'hotel d'arrive
            if (xs > xp)
            {
                renvoi = - ds;
            }
            else {
                // Si Hotel Param a "depasse" l'hotel d'arrive
                if (xa < xp)
                {
                    renvoi = d - ds;
                }
                else {
                    renvoi = ds;
                }
            }
        }
        // S'il se trouve "apres"
        else {
            // Si Hotel Param "fait reculer" par rapport a l'hotel d'arrive
            if (xs < xp)
            {
                renvoi = - ds;
            }
            else {
                // Si Hotel Param a "depasse" l'hotel d'arrive
                if (xa > xp)
                {
                    renvoi = d - ds;
                }
                else {
                    renvoi = ds;
                }
            }
        }
    }
    else {
        if (Vx == 0.0) {
            ds = ys - yhotel;
            da = ya - yhotel;

            // Si l'hotel de depart se trouve "avant" l'hotel d'arrive
            if (ys < ya)
            {
                // Si Hotel Param a "depasse" l'hotel d'arrive
                if (da > 0)
                {
                    renvoi = (ya - ys) - da;
                }
                else {
                    renvoi = ds;
                }
            }
            // S'il se trouve "apres"
            else {
                // Si Hotel Param a "depasse" l'hotel d'arrive
                if (da < 0)
                {
                    renvoi = (ys - ya) + da;
                }
                else {
                    renvoi = -1 * ds;
                }
            }
        }
        else {
            ds = xs - xhotel;
            da = ya - yhotel;

            // Si l'hotel de depart se trouve "avant" l'hotel d'arrive
            if (xs < xa)
            {
                // Si Hotel Param a "depasse" l'hotel d'arrive
                if (da > 0)
                {
                    renvoi = (xa - xs) - da;
                }
                else {
                    renvoi = ds;
                }
            }
            // S'il se trouve "apres"
            else {
                // Si Hotel Param a "depasse" l'hotel d'arrive
                if (da < 0)
                {
                    renvoi = (xs - xa) + da;
                }
                else {
                    renvoi = -1 * ds;
                }
            }
        }
    }

    return renvoi;
}

/// <summary>
/// Permet de supprimer un element en parametre de la liste en parametre
/// </summary>
/// <param name="pi_Array"> Liste dans laquelle l'element doit etre supprimé </param>
/// <param name="i_Element"> Element a trouver et supprimer </param>
void SupprimerElement(vector<int>* pi_Array, int i_Element) {
    int idx = 0;
    while (idx < pi_Array->size()) {
        if (i_Element == (*pi_Array)[idx]) {
            pi_Array->erase(pi_Array->begin() + idx);
            break;
        }
        idx++;
    }
}
