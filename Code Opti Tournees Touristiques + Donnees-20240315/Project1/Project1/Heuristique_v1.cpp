#include "Heuristique_v1.h"

#define POI_SCORE_INITIAL 1.0
#define POI_TEMPS_OUVERT 1.0

#define HOTEL_SCORE_POI 1.0
#define HOTEL_DISTANCE_AU_POI 1.0

void Heuristique_v1::Solution1()
{
    // A tester
    for (int i : pi_POI_Coherents)
    {
        CalculScorePOI(i);
    }

    H = probleme->get_Id_Hotel_depart();
    pi_Solution.push_back(H);

    for (i_Jour = 0; i_Jour < probleme->get_Nombre_Jour(); i_Jour++)
    {
        i_Duree_Journee_En_Cours = probleme->get_POI_Duree_Max_Voyage(i_Jour);

        IdentifierHotelsDemiRayon(H);

        i_Meilleur_Hotel = -1;
        double i_Score_Meilleur_Hotel = -1;
        for (int i_Hotel_Possible : pi_Rayon_Hotels)
        {

            vector<int> pi_Rayon = IdentifierPOIRayonHotel(i_Hotel_Possible);

            double i_Score = CalculScoreHotel(i_Hotel_Possible, pi_Rayon);

            if (i_Score > i_Score_Meilleur_Hotel)
            {
                i_Meilleur_Hotel = i_Hotel_Possible;
                i_Score_Meilleur_Hotel = i_Score;
                pi_Rayon_Meilleur_Hotel = pi_Rayon;
            }
        }

        CalculMeilleureJournee();
        H = i_Meilleur_Hotel;

        i_Duree_Max -= i_Duree_Journee_En_Cours;
    }
}

/*
Methode qui permet de calculer le score d'un POI
Entrée :
    - i_POI_Param : l'identifiant du POI
*/

void Heuristique_v1::CalculScorePOI(int i_POI_Param)
{
    // A tester
    double i_Score_Total = POI_SCORE_INITIAL * probleme->get_POI_Score(i_POI_Param) + (probleme->get_POI_Heure_fermeture(i_POI_Param) - probleme->get_POI_Heure_ouverture(i_POI_Param) / POI_TEMPS_OUVERT);
    pi_Score_POI[i_POI_Param] = i_Score_Total;
}

/* Permet d'identifier les hotels sur un demi rayon de */
void Heuristique_v1::IdentifierHotelsDemiRayon(int i_Hotel_Depart)
{
    // A tester
    int i_id_Hotels_Coherents = 0;

    // On parcourt la liste des hotels coherents pour trier
    //  -les hotels qui sont trop loin (non atteignable en une journee) sont supprimées
    //  -les autres hotels on les ajoute à la liste des hotels dans le rayon
    while (i_id_Hotels_Coherents < pi_Hotels_Coherents.size())
    {
        int i_Hotel_Coherent = pi_Hotels_Coherents[i_id_Hotels_Coherents];

        if (probleme->get_distance_Hotel_Hotel(i_Hotel_Coherent, probleme->get_Id_Hotel_Arrivee()) > i_Duree_Max - i_Duree_Journee_En_Cours)
        {
            pi_Hotels_Coherents.erase(pi_Hotels_Coherents.begin() + i_id_Hotels_Coherents);
        }
        else
        {
            if (probleme->get_distance_Hotel_Hotel(i_Hotel_Depart, i_Hotel_Coherent) > probleme->get_POI_Duree_Max_Voyage(i_Jour))
            {
                pi_Rayon_Hotels.push_back(i_Hotel_Coherent);
            }

            i_id_Hotels_Coherents++;
        }
    }
}
/*
Methode qui permet d'identifier les POI dans le rayon d'un hotel
Entrée :
    - i_Hotel_Param : l'identifiant de l'hotel
Sortie :
    - pi_POI_DANS_LE_RAYON : la liste (vecteurs) des POI dans le rayon de l'hotel
*/
vector<int> Heuristique_v1::IdentifierPOIRayonHotel(int i_Hotel_Param)
{
    /*TO DO :
    // A tester
    // AXE d'AMELIORATION : Pendre en compte les horaires d'ouverture et de fermeture (la durée)
     */
    vector<int> pi_POI_DANS_LE_RAYON = {};

    for (int i_POI_Coherent : pi_POI_Coherents)
    {
        if (probleme->get_distance_Hotel_POI(i_Hotel_Param, i_POI_Coherent) +
                probleme->get_distance_Hotel_POI(H, i_POI_Coherent) <
            i_Duree_Journee_En_Cours)
        {
            pi_POI_DANS_LE_RAYON.push_back(i_POI_Coherent);
        }
    }

    return pi_POI_DANS_LE_RAYON;
}

/*
Methode qui permet de calculer le score d'un hotel en fonction des POI dans le rayon
Entrée :
    - i_Hotel_Param : l'identifiant de l'hotel
    - pi_POI_Dans_Le_Rayon : la liste des POI dans le rayon de l'hotel
Sortie :
    - i_Score_Total : le score total de l'hotel
*/
double Heuristique_v1::CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon)
{
    // A tester +
    // AXE D'AMELIORATION : Prendre en compte GetAbsiceProjectionHotel(int i_Hotel_Param)
    double i_Score_Total = 0;

    for (int Id_POI : pi_POI_Dans_Le_Rayon)
    {
        double i_Score = pi_Score_POI[Id_POI] * HOTEL_SCORE_POI;

        i_Score = HOTEL_DISTANCE_AU_POI * (i_Score / probleme->get_distance_Hotel_POI(H, Id_POI));

        i_Score_Total += i_Score;
    }

    return i_Score_Total;
}

void Heuristique_v1::CalculMeilleureJournee()
{
    // On trie les POI par score
    // pi_Rayon_Meilleur_Hotel ;// POI acceptables
    // H ;// Hotel debut de journee
    // i_Meilleur_Hotel; // Id hotel fin de journee
    // pi_Score_POI; // Notre score pour chaque POI

    vector<int> pi_POI_Tries = pi_Rayon_Meilleur_Hotel;

    sort(pi_POI_Tries.begin(), pi_POI_Tries.end(), [this](int a, int b)
         { return pi_Score_POI[a] > pi_Score_POI[b]; });

    vector<int> pi_Journee = {};        // POI de la journee
    vector<double> pi_abs_Journee = {}; // Abscisse des POI dans la journee
    int i_Duree_Journee;

    for (int i_POI : pi_POI_Tries)
    {
        double d_abs = GetAbsiceProjectionPOI(i_POI);

        int idx;
        int i_Precedent = H;

        // GERER LES HEURES D'OUVERTURE

        int len = pi_abs_Journee.size();
        if (len > 0)
        {
            if (d_abs >= pi_abs_Journee[0])
            {
                i_Duree_Journee = probleme->get_distance_Hotel_POI(H, pi_Journee[0]);

                for (idx = 1; idx < pi_abs_Journee.size() && d_abs >= pi_abs_Journee[idx]; idx++)
                {
                    i_Duree_Journee += probleme->get_distance_POI_POI(pi_Journee[idx - 1], pi_Journee[idx]);
                }

                i_Duree_Journee += probleme->get_distance_POI_POI(pi_Journee[idx], i_POI);
            }
            else
            {
                i_Duree_Journee = probleme->get_distance_Hotel_POI(H, i_POI);
                i_Duree_Journee += probleme->get_distance_POI_POI(i_POI, pi_Journee[0]);

                idx = 0;
            }

            if (idx < len - 1)
            {
                idx++;
                for (idx; idx < pi_abs_Journee.size(); idx++)
                {
                    i_Duree_Journee += probleme->get_distance_POI_POI(pi_Journee[idx - 1], pi_Journee[idx]);
                }

                i_Duree_Journee += probleme->get_distance_Hotel_POI(i_Meilleur_Hotel, pi_Journee[len - 1]);
            }
            else
            {
                i_Duree_Journee += probleme->get_distance_Hotel_POI(i_Meilleur_Hotel, i_POI);
            }
        }
        else
        {
            i_Duree_Journee = probleme->get_distance_Hotel_POI(H, i_POI);
            +probleme->get_distance_Hotel_POI(i_Meilleur_Hotel, i_POI);
        }

        if (i_Duree_Journee < i_Duree_Journee_En_Cours)
        {
            pi_Journee.insert(pi_Journee.begin() + idx, i_POI);         // Ajouter à la position idx
            pi_abs_Journee.insert(pi_abs_Journee.begin() + idx, d_abs); // Ajouter à la position idx
        }
    }

    for (int i : pi_Journee)
    {
        pi_Solution.push_back(i);
    }
    pi_Solution.push_back(i_Meilleur_Hotel);
}

/*
Méthode d'initialisation des variables suivantes :
    - pi_POI_Coherents : liste des POI coherents
    - pi_Hotels_Coherents : liste des hotels coherents
*/
void Heuristique_v1::Initialisation()
{
    // A tester

    for (int i = 0; i <= probleme->get_Nombre_POI(); ++i)
    {
        pi_POI_Coherents.push_back(i);
    }

    for (int i = 0; i <= probleme->get_Nombre_Hotel(); ++i)
    {
        pi_Hotels_Coherents.push_back(i);
    }

    i_Duree_Max = probleme->get_POI_Duree_Max_Voyage(0);
    for (int i = 1; i < probleme->get_Nombre_Jour(); ++i)
    {
        i_Duree_Max += probleme->get_POI_Duree_Max_Voyage(i);
    }
}

/*
Methode pour realiser la projection
Entrée :
    - i_POI_Param : l'identifiant du POI
Sortie :
    - d : la distance entre l'hotel de depart et le POI
*/
double Heuristique_v1::GetAbsiceProjectionPOI(int i_POI_Param)
{
    /* Coordonnées de l'hotel de depart */
    int xs = probleme->get_Hotel_Coord_X(H);
    int ys = probleme->get_Hotel_Coord_Y(H);

    /* Coordonnées de l'hotel d'arrivee */
    int xa = probleme->get_Hotel_Coord_X(i_Meilleur_Hotel);
    int ya = probleme->get_Hotel_Coord_Y(i_Meilleur_Hotel);

    /* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    double Vx = xa - xs;
    double Vy = ya - ys;

    double a = Vy / Vx;       // Droite : aX
    double c = ya - (a * xa); // Droite : aX + c

    double ap = -1 / Vx; // Droite orthogonale : apX

    int xpoi = probleme->get_POI_Coord_X(i_POI_Param);
    int ypoi = probleme->get_POI_Coord_Y(i_POI_Param);

    double cp = ypoi - (ap * xpoi); // Droite orthogonale : apX + cp

    double xp = (cp - c) / (a - ap); // Intersection des deux droites

    double yp = a * xp + c; // Intersection des deux droites

    double d = sqrt(pow(xp - xs, 2) + pow(yp - ys, 2)); // Distance entre l'hotel de depart et le POI

    return d;
}

/// TO DO : A utiliser pour la partie on l'on determines les meilleurs hotels,
// on peut realiser une projection et donner un meilleur score aux hotels qui sont "sur le chemin"
//  de l'hotel de depart a l'hotel d'arrivee (ça implique une modification de la methode CalculScoreHotel)
double Heuristique_v1::GetAbsiceProjectionHotel(int i_Hotel_Param)
{
    /* Coordonnées de l'hotel de depart */
    int xs = probleme->get_Hotel_Coord_X(H);
    int ys = probleme->get_Hotel_Coord_Y(H);

    /* Coordonnées de l'hotel d'arrivee */
    int xa = probleme->get_Hotel_Coord_X(i_Meilleur_Hotel);
    int ya = probleme->get_Hotel_Coord_Y(i_Meilleur_Hotel);

    /* Coordonnées vectorielles de l'hotel de depart a l'hotel d'arrivee */
    double Vx = xa - xs;
    double Vy = ya - ys;

    double a = Vy / Vx;       // Droite : aX
    double c = ya - (a * xa); // Droite : aX + c

    double ap = -1 / Vx; // Droite orthogonale : apX

    int xhotel = probleme->get_Hotel_Coord_X(i_Hotel_Param);
    int yhotel = probleme->get_Hotel_Coord_Y(i_Hotel_Param);

    double cp = yhotel - (ap * xhotel); // Droite orthogonale : apX + cp

    double xp = (cp - c) / (a - ap); // Intersection des deux droites

    double yp = a * xp + c; // Intersection des deux droites

    double d = sqrt(pow(xp - xs, 2) + pow(yp - ys, 2)); // Distance entre l'hotel de depart et le POI

    return d;
}