#include "Heuristique_v1.h"
#include "..\..\Solution.hpp"

#define INFINI 99999

#define POI_SCORE_INITIAL 1.0
#define POI_TEMPS_OUVERT 1.0

#define HOTEL_SCORE_POI 1.0
#define HOTEL_DISTANCE_AU_POI 1.0

#define POI_DISTANCES_AUX_HOTELS 1.0


Solution* Heuristique_v1::ExtraireSolution(Instance* problemeParam) {
    Solution* nouvelleSolution = new Solution();
    Heuristique_v1* Algorithme = new Heuristique_v1(problemeParam, nouvelleSolution);

    Algorithme->Initialisation();
    Algorithme->Solution1();

    //Algorithme->H = 0;
    //Algorithme->i_Meilleur_Hotel = 1;
    //Algorithme->MeilleureSequence({ 0, 2, 5, 3, 1, 4 }, 7);//16, 7, 1, 8, 10 }, 4);

    printf("\n\n");


    return nouvelleSolution;
}

Heuristique_v1::Heuristique_v1(Instance* problemeParam, Solution* nouvelleSolution) {
    probleme = problemeParam;
    solution = nouvelleSolution;

    i_Jour = 0;
    f_Duree_Max = 0.0;
    f_Duree_Journee_En_Cours = 0.0;
    H = 0;//ID Hotel de Depart

    map_Score_POI;          // Map de taille n, stockant le score calculee du POI
    pi_Rayon_Hotels = {};          /* Liste contenant les hotels dans le rayon de l'hotel choisit en debut d'iteration*/

    i_Meilleur_Hotel = 0;
    pi_Rayon_Meilleur_Hotel = {};

    pi_Hotels_Coherents = {};

    i_Score_Solution = 0;
}

/// <summary>
/// Déroule l'algorithme
/// </summary>
void Heuristique_v1::Solution1()
{
    // A tester
    for (int i_POI_Coherent = 0; i_POI_Coherent < probleme->get_Nombre_POI(); i_POI_Coherent ++) {
        CalculScorePOI(i_POI_Coherent);
    }

    H = probleme->get_Id_Hotel_depart();

    for (i_Jour = 0; i_Jour < probleme->get_Nombre_Jour(); i_Jour++)
    {
        pi_Rayon_Hotels = {};

        printf("\n\nJournee %d :\nOn part de l'hotel %d\n", i_Jour, H);
        f_Duree_Journee_En_Cours = probleme->get_POI_Duree_Max_Voyage(i_Jour);

        printf("NB hotels coherents : %d et NB POI coherents : %d\n", pi_Hotels_Coherents.size(), map_Score_POI.size());

        printf("La journee dure %f\n", f_Duree_Journee_En_Cours);

        IdentifierHotelsDemiRayon(H);

        printf("Les hotels accessibles sont : ");
        for (int i : pi_Rayon_Hotels) {
            printf("%d, ", i);
        }
        printf("\n");

        i_Meilleur_Hotel = -1;
        float f_Score_Meilleur_Hotel = -1;
        for (int i_Hotel_Possible : pi_Rayon_Hotels)
        {
            vector<int> pi_Rayon = IdentifierPOIRayonHotel(i_Hotel_Possible);

            float f_Score = CalculScoreHotel(i_Hotel_Possible, pi_Rayon);

            if (f_Score > f_Score_Meilleur_Hotel)
            {
                i_Meilleur_Hotel = i_Hotel_Possible;
                f_Score_Meilleur_Hotel = f_Score;
                pi_Rayon_Meilleur_Hotel = pi_Rayon;
            }
        }
        printf("Le meilleur hotel est %d avec un score de %f\nMeilleure journee : ", i_Meilleur_Hotel, f_Score_Meilleur_Hotel);

        CalculMeilleureJournee();
        H = i_Meilleur_Hotel;

        printf("\n\n");

        f_Duree_Max -= f_Duree_Journee_En_Cours;
    }

    solution->i_valeur_fonction_objectif = i_Score_Solution;
    printf("Score final de : %d\n\n", i_Score_Solution);
}

/// <summary>
/// Determine le score du POI en paramètre
/// </summary>
/// <param name="i_POI_Param"></param>
void Heuristique_v1::CalculScorePOI(int i_POI_Param)
{
    // A tester
    float f_Score_Total = POI_SCORE_INITIAL * probleme->get_POI_Score(i_POI_Param) + (probleme->get_POI_Heure_fermeture(i_POI_Param) - probleme->get_POI_Heure_ouverture(i_POI_Param) / POI_TEMPS_OUVERT);
    map_Score_POI[i_POI_Param] = f_Score_Total;
}

/// <summary>
/// Identifie les hotels joignables en une moins d'1 journée depuis l'hotel en parametre
/// </summary>
/// <param name="i_Hotel_Depart"></param>
void Heuristique_v1::IdentifierHotelsDemiRayon(int i_Hotel_Depart)
{
    // Si on en est à la dernière journée, on ne regarde que l'hotel d'arrivée
    if (i_Jour + 1 == probleme->get_Nombre_Jour()) {
        pi_Rayon_Hotels.push_back(probleme->get_Id_Hotel_Arrivee());
        return;
    }

    int i_id_Hotels_Coherents = 0;

    // On parcourt la liste des hotels coherents pour trier
    //  -les hotels qui sont trop loin (non atteignable en une journee) sont supprimées
    //  -les autres hotels on les ajoute à la liste des hotels dans le rayon
    while (i_id_Hotels_Coherents < pi_Hotels_Coherents.size())
    {
        int i_Hotel_Coherent = pi_Hotels_Coherents[i_id_Hotels_Coherents];

        if (f_Duree_Max - f_Duree_Journee_En_Cours < probleme->get_distance_Hotel_Hotel(i_Hotel_Coherent, probleme->get_Id_Hotel_Arrivee()))
        {
            pi_Hotels_Coherents.erase(pi_Hotels_Coherents.begin() + i_id_Hotels_Coherents);
        }
        else
        {
            if (probleme->get_distance_Hotel_Hotel(i_Hotel_Depart, i_Hotel_Coherent) <= probleme->get_POI_Duree_Max_Voyage(i_Jour))
            {
                pi_Rayon_Hotels.push_back(i_Hotel_Coherent);
            }

            i_id_Hotels_Coherents++;
        }
    }
}

/// <summary>
/// Methode qui permet d'identifier les POI dans joignable en moins d'1 journée
/// </summary>
/// <param name="i_Hotel_Param"></param>
/// <returns></returns>
vector<int> Heuristique_v1::IdentifierPOIRayonHotel(int i_Hotel_Param)
{
    /*TO DO :
    // A tester
    // AXE d'AMELIORATION : Pendre en compte les horaires d'ouverture et de fermeture (la durée)
     */
    vector<int> pi_POI_DANS_LE_RAYON = {};

    for (auto it = map_Score_POI.begin(); it != map_Score_POI.end(); ++it) {
        int i_POI_Coherent = it->first;

        if (probleme->get_distance_Hotel_POI(i_Hotel_Param, i_POI_Coherent) +
                probleme->get_distance_Hotel_POI(H, i_POI_Coherent) <
            f_Duree_Journee_En_Cours)
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
float Heuristique_v1::CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon)
{
    // A tester +
    // AXE D'AMELIORATION : Prendre en compte GetAbsiceProjectionHotel(int i_Hotel_Param)
    float f_Score_Total = 0;

    for (int Id_POI : pi_POI_Dans_Le_Rayon)
    {
        float f_Score = map_Score_POI[Id_POI] * HOTEL_SCORE_POI;

        f_Score = HOTEL_DISTANCE_AU_POI * (f_Score / probleme->get_distance_Hotel_POI(H, Id_POI));

        f_Score_Total += f_Score;
    }

    return f_Score_Total;
}

/// <summary>
/// Determine la meilleure succession de POI pour une journée, à partir des hotels de départ et d'arrivé (de la journée) et des POI cohérents
/// </summary>
void Heuristique_v1::CalculMeilleureJournee()
{
    vector<int> pi_Journee = {};
    float f_Duree_Journee = 0;

    unordered_map<int, float> pi_score_POI_Pour_H;
    for (int i_POI : pi_Rayon_Meilleur_Hotel) {
        pi_score_POI_Pour_H[i_POI] = map_Score_POI[i_POI] / ((probleme->get_distance_Hotel_POI(H, i_POI) + probleme->get_distance_Hotel_POI(i_Meilleur_Hotel, i_POI)) * POI_DISTANCES_AUX_HOTELS);
    }

    std::map<int, int> map_POI_Tries(pi_score_POI_Pour_H.begin(), pi_score_POI_Pour_H.end());

    for (auto i : map_POI_Tries)
    {
        int i_POI = i.first;

        pi_Journee = MeilleureSequence(pi_Journee, i_POI);
    }

    // TODO : A suppr
    printf("\n");
    for (int i : pi_Journee)
        printf("%d, ", i);
        printf("\n");

    solution->v_Date_Depart.push_back(0.0);
    solution->v_v_Sequence_Id_Par_Jour.push_back(pi_Journee);

    if (i_Jour + 1 == probleme->get_Nombre_Jour())
        solution->v_Id_Hotel_Intermedaire.push_back(i_Meilleur_Hotel);
}

/// <summary>
/// Détermine la meilleure séquence (qui laisse le plus de place pour insérer d'autre POI), à partir d'une séquence en paramètre et un POI à insérer
/// </summary>
/// <param name="pi_Sequence"></param>
/// <param name="p_Infos"></param>
/// <param name="id_POI"></param>
/// <param name="info"></param>
/// <returns></returns>
vector<int> Heuristique_v1::MeilleureSequence(vector<int> pi_Sequence, int i_POI) {
    vector<int> pi_Meilleure_Sequence = pi_Sequence;
    float f_Meilleur_Temps_Libre = -1; // On met à -1 car on préferera une sequence qui inclu id_POI

    for (int i = 0; i < pi_Sequence.size() + 1; i ++) {
        vector<int> pi_Sequence_i = pi_Sequence;

        pi_Sequence_i.insert(pi_Sequence_i.begin() + i, i_POI);

        float f_Temps_Libre = GetTempsLibreTrajet(pi_Sequence_i);

        if (f_Temps_Libre > f_Meilleur_Temps_Libre) {
            pi_Meilleure_Sequence = pi_Sequence_i;
            f_Meilleur_Temps_Libre = f_Temps_Libre;
        }
    }

    // Si le POI a réussie à être ajouté, on le supprime de la liste des POI
    if (f_Meilleur_Temps_Libre > -1) {
        printf("Temps Libre Max pour le POI %d : %f\n", i_POI, f_Meilleur_Temps_Libre);
        map_Score_POI.erase(i_POI);
        i_Score_Solution += probleme->get_POI_Score(i_POI);
    }

    return pi_Meilleure_Sequence;
}


/// <summary>
/// Initialise les listes à partir des données du problèmes
/// </summary>
void Heuristique_v1::Initialisation()
{
    // Initialisation des Hotels
    for (int i = 0; i < probleme->get_Nombre_Hotel(); ++i)
    {
        pi_Hotels_Coherents.push_back(i);
    }

    // Initialisation des durées
    f_Duree_Max = 0.0;
    f_Duree_Max = probleme->get_POI_Duree_Max_Voyage(0);
    for (int i = 1; i < probleme->get_Nombre_Jour(); ++i)
    {
        f_Duree_Max += probleme->get_POI_Duree_Max_Voyage(i);
    }

    // Initialisation de champs POI de Solution
    solution->v_v_Sequence_Id_Par_Jour = {};
}

/// <summary>
/// Calcule la durée d'une journée pour une suite de POI en parametre
/// </summary>
/// <param name="pi_Trajet"></param>
/// <returns> temps libre ou -1 si impossible </returns>
float Heuristique_v1::GetTempsLibreTrajet(vector<int> pi_Trajet) {
    float f_TempsLibre = 0.0;

    float f_Duree_Trajet = probleme->get_distance_Hotel_POI(H, pi_Trajet[0]) + 0.0; // 0.0 = debut de la Journée
    float f_HeureOuverture_0 = probleme->get_POI_Heure_ouverture(pi_Trajet[0]);
    if (f_Duree_Trajet < f_HeureOuverture_0) {
        f_TempsLibre += f_HeureOuverture_0 - f_Duree_Trajet;
        //printf("H-%d : + %f;  ", 0, f_HeureOuverture_0 - f_Duree_Trajet);
        f_Duree_Trajet = f_HeureOuverture_0;
    }

    int i = 0;
    for (i; i < pi_Trajet.size() - 1; i++) {

        float distance =  probleme->get_distance_POI_POI(pi_Trajet[i], pi_Trajet[i + 1]);
        if (f_Duree_Trajet + distance > probleme->get_POI_Heure_fermeture(pi_Trajet[i + 1])){
            //printf("POI %d ferme\n\n", i);
            return -1; // Si l'heure de fermeture du i-ème POI est passée
        }

        float f_HeureOuverture_i = probleme->get_POI_Heure_ouverture(pi_Trajet[i + 1]);
        if (f_Duree_Trajet + distance < f_HeureOuverture_i) {
            f_TempsLibre += f_HeureOuverture_i - (f_Duree_Trajet + distance);
            //printf("%d-%d : + %f;  ", i, i+1, f_HeureOuverture_i - (f_Duree_Trajet + distance));

            f_Duree_Trajet = f_HeureOuverture_i;
        }
        else {
            f_Duree_Trajet += distance;
        }
    }

    f_Duree_Trajet += probleme->get_distance_Hotel_POI(i_Meilleur_Hotel, pi_Trajet.size() - 1);
    if (f_Duree_Trajet - 0.0 > f_Duree_Journee_En_Cours) {
        //printf("Journee trop longue \n\n");
        return -1; // Journée trop longue
    }


    //printf("%d-H : + %f;", pi_Trajet.size() - 1, f_Duree_Journee_En_Cours - (f_Duree_Trajet - 0.0));
    f_TempsLibre += f_Duree_Journee_En_Cours - (f_Duree_Trajet - 0.0); // 0.0 = debut de la Journée

    //printf("Journee valide, temps libre : %f\n\n", f_TempsLibre);

    return f_TempsLibre;
}

/// <summary>
/// Methode pour realiser la projection d'un POI sur la droite joignant le couple d'Hotels de la journée
/// </summary>
/// <param name="i_POI_Param"></param>
/// <returns> absice du POI sur la droite de projection </returns>
float Heuristique_v1::GetAbsiceProjectionPOI(int i_POI_Param)
{
    /* Coordonnées de l'hotel de depart */
    int xs = probleme->get_Hotel_Coord_X(H);
    int ys = probleme->get_Hotel_Coord_Y(H);

    /* Coordonnées de l'hotel d'arrivee */
    int xa = probleme->get_Hotel_Coord_X(i_Meilleur_Hotel);
    int ya = probleme->get_Hotel_Coord_Y(i_Meilleur_Hotel);

    /* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    float Vx = xa - xs;
    float Vy = ya - ys;

    float a = Vy / Vx;       // Droite : aX
    float c = ya - (a * xa); // Droite : aX + c

    float ap = -1 / Vx; // Droite orthogonale : apX

    int xpoi = probleme->get_POI_Coord_X(i_POI_Param);
    int ypoi = probleme->get_POI_Coord_Y(i_POI_Param);

    float cp = ypoi - (ap * xpoi); // Droite orthogonale : apX + cp

    float xp = (cp - c) / (a - ap); // Intersection des deux droites

    float yp = a * xp + c; // Intersection des deux droites

    float d = sqrt(pow(xp - xs, 2) + pow(yp - ys, 2)); // Distance entre l'hotel de depart et le POI

    return d;
}

/// TO DO : A utiliser pour la partie on l'on determines les meilleurs hotels,
// on peut realiser une projection et donner un meilleur score aux hotels qui sont "sur le chemin"
//  de l'hotel de depart a l'hotel d'arrivee (ça implique une modification de la methode CalculScoreHotel)
float Heuristique_v1::GetAbsiceProjectionHotel(int i_Hotel_Param)
{
    // TODO : SI BESOIN

    ///* Coordonnées de l'hotel de depart */
    //int xs = probleme->get_Hotel_Coord_X(H);
    //int ys = probleme->get_Hotel_Coord_Y(H);

    ///* Coordonnées de l'hotel d'arrivee */
    //int xa = probleme->get_Hotel_Coord_X(i_Meilleur_Hotel);
    //int ya = probleme->get_Hotel_Coord_Y(i_Meilleur_Hotel);

    ///* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    //float Vx = xa - xs;
    //float Vy = ya - ys;

    //float a = Vy / Vx;       // Droite : aX
    //float c = ya - (a * xa); // Droite : aX + c

    //float ap = -1 / Vx; // Droite orthogonale : apX

    //int xhotel = probleme->get_Hotel_Coord_X(i_Hotel_Param);
    //int yhotel = probleme->get_Hotel_Coord_Y(i_Hotel_Param);

    //float cp = yhotel - (ap * xhotel); // Droite orthogonale : apX + cp

    //float xp = (cp - c) / (a - ap); // Intersection des deux droites

    //float yp = a * xp + c; // Intersection des deux droites

    //float d = sqrt(pow(xp - xs, 2) + pow(yp - ys, 2)); // Distance entre l'hotel de depart et le POI

    return 0;// d;
}

bool comparator(const pair<string, int>& p1, const pair<string, int>& p2) {
    if (p1.second == p2.second) { // If numbers are same, sort by string
        return p1.first < p2.first;
    }
    return p1.second > p2.second; // Descending order
}