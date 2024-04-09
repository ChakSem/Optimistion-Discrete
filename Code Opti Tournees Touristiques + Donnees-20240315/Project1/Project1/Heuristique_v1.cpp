#include "Heuristique_v1.h"
#include "..\..\Solution.hpp"

#define INFINI 99999

#define POI_SCORE_INITIAL 1.0
#define POI_TEMPS_OUVERT 1.0

#define HOTEL_SCORE_POI 1.0
#define HOTEL_DISTANCE_AU_POI 1.0

#define POI_DISTANCES_AUX_HOTELS 1.0


Solution* Heuristique_v1::ExtraireSolution(Instance* instanceParam) {
    Solution* nouvelleSolution = new Solution();
    Heuristique_v1* Algorithme = new Heuristique_v1(instanceParam, nouvelleSolution);

    /*Algorithme->Initialisation();
    Algorithme->Solution1();*/

    Algorithme->i_Hotel_Debut_Journee = 2;
    Algorithme->i_Hotel_Fin_Journee = 1;
    vector<int> sequence = Algorithme->CalculMeilleureJournee();//MeilleureSequence({ 24, 32, 40, 33, 25, 19, 26, 34, 41, 52, 56, 59, 61 }, 47);
    printf("score %f, : ", sequence);

    for (int i : sequence) {

        printf("%d, ", i);
    }


    //printf("%f - %f = %f", Algorithme->instance->get_distance_Hotel_POI(1, 1), Algorithme->GetVraieDistanceHotelPOI(1, 1), Algorithme->instance->get_distance_Hotel_POI(1, 1) - Algorithme->GetVraieDistanceHotelPOI(1, 1));

    //Algorithme->H = 0;
    //Algorithme->i_Meilleur_Hotel = 1;
    //Algorithme->MeilleureSequence({ 0, 2, 5, 3, 1, 4 }, 7);

    //printf("\n\n");
      /*  for ( int i = 0; i < Algorithme->instance->get_Nombre_Jour(); i ++)
        {
            printf("%f, ", Algorithme->GetScoreSequence(Algorithme->solution->v_v_Sequence_Id_Par_Jour[i]));

        }*/



    return nouvelleSolution;
}

Heuristique_v1::Heuristique_v1(Instance* instanceParam, Solution* nouvelleSolution) {
    instance = instanceParam;
    solution = nouvelleSolution;

    i_Jour = 0;
    f_Duree_Totale_Restante = 0.0;
    f_Duree_Journee_En_Cours = 0.0;
    i_Hotel_Debut_Journee = 0;//ID Hotel de Depart

    map_Score_POI;          // Map de taille n, stockant le score calculee du POI
    pi_Rayon_Hotels = {};          /* Liste contenant les hotels dans le rayon de l'hotel choisit en debut d'iteration*/

    i_Hotel_Fin_Journee = 0;
    pi_POI_Joignables = {};

    pi_Hotels_Coherents = {};

    i_Score_Solution = 0;
}

/// <summary>
/// Déroule l'algorithme
/// </summary>
void Heuristique_v1::Solution1()
{
    // Bon
    for (int i_POI_Coherent = 0; i_POI_Coherent < instance->get_Nombre_POI(); i_POI_Coherent ++) {
        CalculScorePOI(i_POI_Coherent);
    }

    i_Hotel_Debut_Journee = instance->get_Id_Hotel_depart();

    for (i_Jour = 0; i_Jour < instance->get_Nombre_Jour(); i_Jour++)
    {
        pi_Rayon_Hotels = {};

        printf("\n\nJournee %d :\nOn part de l'hotel %d\n", i_Jour, i_Hotel_Debut_Journee);
        f_Duree_Journee_En_Cours = instance->get_POI_Duree_Max_Voyage(i_Jour);

        printf("NB hotels coherents : %d et NB POI coherents : %d\n", pi_Hotels_Coherents.size(), map_Score_POI.size());

        printf("La journee dure %f\n", f_Duree_Journee_En_Cours);

        IdentifierHotelsFinJournee(i_Hotel_Debut_Journee);

        printf("Les hotels accessibles sont : ");
        for (int i : pi_Rayon_Hotels) {
            printf("%d, ", i);
        }
        printf("\n");

        i_Hotel_Fin_Journee = -1;
        float f_Score_Meilleur_Hotel = -1;
        for (int i_Hotel_Possible : pi_Rayon_Hotels)
        {
            vector<int> pi_Rayon = IdentifierPOIRayonHotel(i_Hotel_Possible);

            float f_Score = CalculScoreHotel(i_Hotel_Possible, pi_Rayon);

            if (f_Score > f_Score_Meilleur_Hotel)
            {
                i_Hotel_Fin_Journee = i_Hotel_Possible;
                f_Score_Meilleur_Hotel = f_Score;
                pi_POI_Joignables = pi_Rayon;
            }
        }
        printf("Le meilleur hotel est %d avec un score de %f\nMeilleure journee : ", i_Hotel_Fin_Journee, f_Score_Meilleur_Hotel);

        CalculMeilleureJournee();
        i_Hotel_Debut_Journee = i_Hotel_Fin_Journee;

        printf("\n\n");

        f_Duree_Totale_Restante -= f_Duree_Journee_En_Cours;
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
    // TODO : Revoir les constantes

    float f_Score_Total = POI_SCORE_INITIAL * instance->get_POI_Score(i_POI_Param) +(instance->get_POI_Heure_fermeture(i_POI_Param) - instance->get_POI_Heure_ouverture(i_POI_Param) / POI_TEMPS_OUVERT);
    map_Score_POI[i_POI_Param] = f_Score_Total;
}

/// <summary>
/// Identifie les hotels joignables en une moins d'1 journée depuis l'hotel en parametre
/// </summary>
/// <param name="i_Hotel_Depart"></param>
void Heuristique_v1::IdentifierHotelsFinJournee(int i_Hotel_Depart)
{
    // Bon
    
    // Si on en est à la dernière journée, on ne regarde que l'hotel d'arrivée
    if (i_Jour + 1 == instance->get_Nombre_Jour()) {
        pi_Rayon_Hotels.push_back(instance->get_Id_Hotel_Arrivee());
        return;
    }

    int i_id_Hotels_Coherents = 0;

    while (i_id_Hotels_Coherents < pi_Hotels_Coherents.size())
    {
        int i_Hotel_Coherent = pi_Hotels_Coherents[i_id_Hotels_Coherents];

        // Si l'hotel est à une distance de l'hotel d'arrivé supérieur au nb de jours restants après cette journée
        if (f_Duree_Totale_Restante - f_Duree_Journee_En_Cours < instance->get_distance_Hotel_Hotel(i_Hotel_Coherent, instance->get_Id_Hotel_Arrivee()))
        {
            pi_Hotels_Coherents.erase(pi_Hotels_Coherents.begin() + i_id_Hotels_Coherents); // L'hotel est éliminé definitivement 
        }
        else
        {
            // Si l'hotel peut etre rejoint en moins d'une journée
            if (instance->get_distance_Hotel_Hotel(i_Hotel_Depart, i_Hotel_Coherent) <= f_Duree_Journee_En_Cours)
            {
                pi_Rayon_Hotels.push_back(i_Hotel_Coherent); // Il est candidat aux hotels de fin
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
    // TODO : Prendre en compte horraires POI

    vector<int> pi_POI_DANS_LE_RAYON = {};

    for (auto it = map_Score_POI.begin(); it != map_Score_POI.end(); ++it) {
        int i_POI_Coherent = it->first;


        float f_Distance_Depuis_H = GetVraieDistanceHotelPOI(i_Hotel_Debut_Journee, i_POI_Coherent);
        float f_Visite_Distance_a_Hfin = instance->get_distance_Hotel_POI(i_Hotel_Param, i_POI_Coherent);
        
      
        if (f_Distance_Depuis_H <= instance->get_POI_Heure_fermeture(i_POI_Coherent) &&
            f_Distance_Depuis_H + f_Visite_Distance_a_Hfin < f_Duree_Journee_En_Cours &&
            instance->get_POI_Heure_ouverture(i_POI_Coherent) + f_Visite_Distance_a_Hfin < f_Duree_Journee_En_Cours)
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
    // TODO : Revoir constantes
    // AXE D'AMELIORATION : Prendre en compte GetAbsiceProjectionHotel(int i_Hotel_Param)
    float f_Score_Total = 0;

    for (int Id_POI : pi_POI_Dans_Le_Rayon)
    {
        float f_Score = map_Score_POI[Id_POI] * HOTEL_SCORE_POI;

        f_Score = HOTEL_DISTANCE_AU_POI * (f_Score / GetVraieDistanceHotelPOI(i_Hotel_Param, Id_POI));

        f_Score_Total += f_Score;
    }

    return f_Score_Total;
}

/// <summary>
/// Determine la meilleure succession de POI pour une journée, à partir des hotels de départ et d'arrivé (de la journée) et des POI cohérents
/// </summary>
vector<int> Heuristique_v1::CalculMeilleureJournee()
{
    // Bon
    vector<int> pi_Journee = { 24, 32, 40, 33, 25, 19, 26, 34, 41, 52, 56, 59, 61, 47 };

    // On modifie le score pour donner plus de poids au POI proche de la droite H à Ha
    unordered_map<int, float> pi_score_POI_Pour_H;
    for (int i_POI : pi_POI_Joignables) {
        pi_score_POI_Pour_H[i_POI] = map_Score_POI[i_POI]; // ((GetVraieDistanceHotelPOI(i_Hotel_Debut_Journee, i_POI) + GetVraieDistanceHotelPOI(i_Hotel_Fin_Journee, i_POI)) * POI_DISTANCES_AUX_HOTELS);
    }

    std::vector<std::pair<int, float>> vec_POI_Tries(pi_score_POI_Pour_H.begin(), pi_score_POI_Pour_H.end());

    // Trie par valeur (en fonction du score)
    /*std::sort(vec_POI_Tries.begin(), vec_POI_Tries.end(),
        [](const std::pair<int, float>& a, const std::pair<int, float>& b) {
            return a.second > b.second; 
        }
    );*/

    for (auto i : vec_POI_Tries)
    {
        int i_POI = i.first;

        pi_Journee = MeilleureSequence(pi_Journee, i_POI);
    }

    return pi_Journee;

    // TODO : A suppr
    for (int i : pi_Journee)
        printf("%d, ", i);
        printf("\n");

    solution->v_Date_Depart.push_back(0.0);
    solution->v_v_Sequence_Id_Par_Jour.push_back(pi_Journee);

    if (i_Jour + 1 == instance->get_Nombre_Jour())
        solution->v_Id_Hotel_Intermedaire.push_back(i_Hotel_Fin_Journee);
}

/// <summary>
/// Détermine la meilleure séquence (qui laisse le plus de place pour insérer d'autre POI), à partir d'une séquence en paramètre et un POI à insérer
/// </summary>
/// <param name="pi_Sequence"></param>
/// <param name="p_Infos"></param>
/// <param name="id_POI"></param>
/// <param name="info"></param>
/// <returns></returns>
vector<int> Heuristique_v1::MeilleureSequence(vector<int> pi_Sequence, int i_POI) 
{
    // TODO : Trouver alternative a l'enumeration
    vector<int> pi_Meilleure_Sequence = pi_Sequence;
    float f_Meilleur_Score_Sequence = -1; // On met à -1 car on préferera une sequence qui inclu id_POI

    for (int i = 0; i < pi_Sequence.size() + 1; i ++) {
        vector<int> pi_Sequence_i = pi_Sequence;

        pi_Sequence_i.insert(pi_Sequence_i.begin() + i, i_POI);

        printf("Insertion a la position %d : ", i);
        float f_Score_Sequence = GetScoreSequence(pi_Sequence_i);

        if (f_Score_Sequence > f_Meilleur_Score_Sequence) {
            pi_Meilleure_Sequence = pi_Sequence_i;
            f_Meilleur_Score_Sequence = f_Score_Sequence;
        }
    }

    // Si le POI a réussie à être ajouté, on le supprime de la liste des POI
    if (f_Meilleur_Score_Sequence > -1) {
        //printf("Temps Libre Max pour le POI %d : %f\n", i_POI, f_Meilleur_Temps_Libre);
        map_Score_POI.erase(i_POI);
        i_Score_Solution += instance->get_POI_Score(i_POI);
    }

    for (int i : pi_Meilleure_Sequence) {

        printf("%d, ", i);
    }

    return pi_Meilleure_Sequence;
}

/// <summary>
/// Renvoie un score basé sur les temps libres (attente avant ouverture POI, nb d'heures restantes à la fin) de la journée, donnant plus d'importance au long
/// </summary>
/// <param name="pi_Trajet"></param>
/// <returns> score ou -1 si impossible </returns>
float Heuristique_v1::GetScoreSequence(vector<int> pi_Trajet)
{
    float f_Duree_Trajet = 0.0;
    float f_Score = 0.0;

    f_Duree_Trajet = f_Duree_Trajet + instance->get_distance_Hotel_POI(i_Hotel_Debut_Journee, pi_Trajet[0]);
    int j = 0;
    do
    {
        float f_HeureOuverture = instance->get_POI_Heure_ouverture(pi_Trajet[j]);
        if (f_Duree_Trajet < f_HeureOuverture) {
            f_Score += pow(f_HeureOuverture - f_Duree_Trajet, 2);
            f_Duree_Trajet = f_HeureOuverture;
        }
        else
        {
            if (f_Duree_Trajet > instance->get_POI_Heure_fermeture(pi_Trajet[j]))
            {
                printf("%d ferme\n\n", pi_Trajet[j]);
                return -1;
            }
        }
        j++;
        if (j < pi_Trajet.size())
        {
            f_Duree_Trajet = f_Duree_Trajet + instance->get_distance_POI_POI(pi_Trajet[j - 1], pi_Trajet[j]);
        }
    } while (j < pi_Trajet.size());

    f_Duree_Trajet = f_Duree_Trajet + instance->get_distance_Hotel_POI(i_Hotel_Fin_Journee, pi_Trajet[j - 1]);
    if ((f_Duree_Trajet - 0.0) > instance->get_POI_Duree_Max_Voyage(i_Jour))
    {
        printf("Trop longue\n\n");
        return -1;
    }
    f_Score += pow(instance->get_POI_Duree_Max_Voyage(i_Jour) - (f_Duree_Trajet - 0.0), 2);

    printf("Journee valide \n\n");

    return f_Score;
}

/// <summary>
/// Initialise les listes à partir des données du problèmes
/// </summary>
void Heuristique_v1::Initialisation()
{
    // Bon
   
    // Initialisation des Hotels
    for (int i = 0; i < instance->get_Nombre_Hotel(); ++i)
    {
        pi_Hotels_Coherents.push_back(i);
    }

    // Initialisation des durées
    f_Duree_Totale_Restante = 0.0;
    for (int i = 0; i < instance->get_Nombre_Jour(); ++i)
    {
        f_Duree_Totale_Restante += instance->get_POI_Duree_Max_Voyage(i);
    }

    // Initialisation de champs POI de Solution
    solution->v_v_Sequence_Id_Par_Jour = {};
}

/// <summary>
/// Methode pour realiser la projection d'un POI sur la droite joignant le couple d'Hotels de la journée
/// </summary>
/// <param name="i_POI_Param"></param>
/// <returns> absice du POI sur la droite de projection </returns>
float Heuristique_v1::GetAbsiceProjectionPOI(int i_POI_Param)
{
    // Bon

    /* Coordonnées de l'hotel de depart */
    int xs = instance->get_Hotel_Coord_X(i_Hotel_Debut_Journee);
    int ys = instance->get_Hotel_Coord_Y(i_Hotel_Debut_Journee);

    /* Coordonnées de l'hotel d'arrivee */
    int xa = instance->get_Hotel_Coord_X(i_Hotel_Fin_Journee);
    int ya = instance->get_Hotel_Coord_Y(i_Hotel_Fin_Journee);

    /* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    float Vx = xa - xs;
    float Vy = ya - ys;

    float a = Vy / Vx;       // Droite : aX
    float c = ya - (a * xa); // Droite : aX + c

    float ap = -1 / Vx; // Droite orthogonale : apX

    int xpoi = instance->get_POI_Coord_X(i_POI_Param);
    int ypoi = instance->get_POI_Coord_Y(i_POI_Param);

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
    //int xs = instance->get_Hotel_Coord_X(H);
    //int ys = instance->get_Hotel_Coord_Y(H);

    ///* Coordonnées de l'hotel d'arrivee */
    //int xa = instance->get_Hotel_Coord_X(i_Meilleur_Hotel);
    //int ya = instance->get_Hotel_Coord_Y(i_Meilleur_Hotel);

    ///* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    //float Vx = xa - xs;
    //float Vy = ya - ys;

    //float a = Vy / Vx;       // Droite : aX
    //float c = ya - (a * xa); // Droite : aX + c

    //float ap = -1 / Vx; // Droite orthogonale : apX

    //int xhotel = instance->get_Hotel_Coord_X(i_Hotel_Param);
    //int yhotel = instance->get_Hotel_Coord_Y(i_Hotel_Param);

    //float cp = yhotel - (ap * xhotel); // Droite orthogonale : apX + cp

    //float xp = (cp - c) / (a - ap); // Intersection des deux droites

    //float yp = a * xp + c; // Intersection des deux droites

    //float d = sqrt(pow(xp - xs, 2) + pow(yp - ys, 2)); // Distance entre l'hotel de depart et le POI

    return 0;// d;
}


/// <summary>
/// Donne la distance d'un hotel au POI en fonction de leurs coordonnees
/// </summary>
/// <param name="i_Hotel"></param>
/// <param name="i_POI"></param>
/// <returns></returns>
float Heuristique_v1::GetVraieDistanceHotelPOI(int i_Hotel, int i_POI) {
    return sqrt(pow(instance->get_Hotel_Coord_X(i_Hotel) - instance->get_POI_Coord_X(i_POI), 2) 
        + pow(instance->get_Hotel_Coord_X(i_Hotel) - instance->get_POI_Coord_Y(i_POI), 2));
}
