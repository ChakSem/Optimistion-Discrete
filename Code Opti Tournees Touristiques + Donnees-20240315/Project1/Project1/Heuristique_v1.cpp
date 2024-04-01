#include "Heuristique_v1.h"

#define POI_SCORE_INITIAL 1.0
#define POI_TEMPS_OUVERT 1.0

#define HOTEL_SCORE_POI 1.0
#define HOTEL_DISTANCE_AU_POI 1.0

void Heuristique_v1::Solution1() 
{
    // A tester
    for (int i : pi_POI_Coherents) {
        CalculScorePOI(i);
    }

    H = Instance->get_Id_Hotel_depart();

    for (i_Jour = 0; i_Jour < Instance->get_Nombre_Jour(); i_Jour++) {
        i_Duree_Journee_En_Cours = Instance->get_POI_Duree_Max_Voyage(i_Jour);

        IdentifierHotelsDemiRayon(H);

        int i_Meilleur_Hotel = -1;
        double i_Score_Meilleur_Hotel = -1;
        vector<int> pi_Rayon_Meilleur_Hotel;
        for (int i_Hotel_Possible : pi_Rayon_Hotels) {

            vector<int> pi_Rayon = IdentifierPOIRayonHotel(i_Hotel_Possible);

            double i_Score = CalculScoreHotel(i_Hotel_Possible, pi_Rayon);

            if (i_Score > i_Score_Meilleur_Hotel) {
                i_Meilleur_Hotel = i_Hotel_Possible;
                i_Score_Meilleur_Hotel = i_Score;
                pi_Rayon_Meilleur_Hotel = pi_Rayon;
            }
        }

        CalculMeilleurJournee(H, i_Meilleur_Hotel);
        H = i_Meilleur_Hotel;

        i_Duree_Max -= i_Duree_Journee_En_Cours;
    }
}

void Heuristique_v1::CalculScorePOI(int i_POI_Param)
{
    // A tester
    double i_Score_Total = POI_SCORE_INITIAL * Instance->get_POI_Score(i_POI_Param) + (Instance->get_POI_Heure_fermeture(i_POI_Param) - Instance->get_POI_Heure_ouverture(i_POI_Param) / POI_TEMPS_OUVERT);

    pi_Score_POI[i_POI_Param] = i_Score_Total;
}

/* Permet d'identifier les hotels sur un demi rayon de */
void Heuristique_v1::IdentifierHotelsDemiRayon(int i_Hotel_Depart)
{
    // A tester
    int i_id_Hotels_Coherents = 0;
    while (i_id_Hotels_Coherents < pi_Hotels_Coherents.size())
    {
        int i_Hotel_Coherent = pi_Hotels_Coherents[i_id_Hotels_Coherents];

        if (Instance->get_distance_Hotel_Hotel(i_Hotel_Coherent, Instance->get_Id_Hotel_Arrivee()) > i_Duree_Max - i_Duree_Journee_En_Cours) {
            pi_Hotels_Coherents.erase(pi_Hotels_Coherents.begin() + i_id_Hotels_Coherents);
        }
        else {
            if (Instance->get_distance_Hotel_Hotel(i_Hotel_Depart, i_Hotel_Coherent) > Instance->get_POI_Duree_Max_Voyage(i_Jour))
            {
                pi_Rayon_Hotels.push_back(i_Hotel_Coherent);
            }

            i_id_Hotels_Coherents++;
        }
    }
}

vector<int> Heuristique_v1::IdentifierPOIRayonHotel(int i_Hotel_Param)
{
    // A tester
    vector<int> pi_POI_DANS_LE_RAYON = {};

    for (int i_POI_Coherent : pi_POI_Coherents)
    {
        if (Instance->get_distance_Hotel_POI(i_Hotel_Param, i_POI_Coherent) + 
            Instance->get_distance_Hotel_POI(H, i_POI_Coherent) < i_Duree_Journee_En_Cours)
        {
            pi_POI_DANS_LE_RAYON.push_back(i_POI_Coherent);
        }
    }

    return pi_POI_DANS_LE_RAYON;
}

double Heuristique_v1::CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon)
{
    // A tester
    double i_Score_Total = 0;

    for (int Id_POI : pi_POI_Dans_Le_Rayon)
    {
        double i_Score = pi_Score_POI[Id_POI] * HOTEL_SCORE_POI;

        i_Score = HOTEL_DISTANCE_AU_POI * (i_Score / Instance->get_distance_Hotel_POI(H, Id_POI));

        i_Score_Total += i_Score;
    }
    
    return i_Score_Total;
}

void Heuristique_v1::CalculMeilleurJournee(int i_Hotel_Depart, int i_Hotel_Arrive)
{
    // TODO
}

void Heuristique_v1::Initialisation()
{
    for (int i = 0; i <= Instance->get_Nombre_POI(); ++i) {
        pi_POI_Coherents.push_back(i);
    }

    for (int i = 0; i <= Instance->get_Nombre_Hotel(); ++i) {
        pi_Hotels_Coherents.push_back(i);
    }

    i_Duree_Max = Instance->get_POI_Duree_Max_Voyage(0);
    for (int i = 1; i < Instance->get_Nombre_Jour(); ++i) {
        i_Duree_Max += Instance->get_POI_Duree_Max_Voyage(i);
    }
}