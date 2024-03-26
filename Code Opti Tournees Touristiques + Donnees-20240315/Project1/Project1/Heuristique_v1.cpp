#include "Heuristique_v1.h"

#define POI_SCORE_INITIAL 1
#define POI_TEMPS_OUVERT 1

#define HOTEL_SCORE_POI 1
#define HOTEL_DISTANCE_AU_POI 1

void Heuristique_v1::CalculScorePOI(int i_POI_Param)
{
    int i_Score_Total = POI_SCORE_INITIAL * Instance->get_POI_Score(i_POI_Param) + POI_TEMPS_OUVERT * (Instance->get_POI_Heure_fermeture(i_POI_Param) - Instance->get_POI_Heure_ouverture(i_POI_Param));

    pi_Score_POI[i_POI_Param] = i_Score_Total;
}

/* Permet d'identifier les hotels sur un demi rayon de */
void Heuristique_v1::IdentifierHotelsDemiRayon(int i_Hotel_Depart)
{
    // liste id hotel

    /*On recupere les hotels qui qui sont dans realisables dans un intervalle d'une journée (demi rayon)*/

    for (int i_Hotel = 0; i_Hotel < Instance->get_Nombre_Hotel(); i_Hotel++)
    {
        if (Instance->get_distance_Hotel_Hotel(i_Hotel_Depart, i_Hotel) > Instance->get_POI_Duree_Max_Voyage())
        {
            pi_Id_Hotels.push_back(i_Hotel);
        }
    }
}

vector<int> Heuristique_v1::IdentifierPOIRayonHotel(int i_Hotel_Param, int i_Id_Journee)
{
    // liste POI

    vector<int> pi_POI_DANS_LE_RAYON = {};

    for (int i_id_POI = 0; i_id_POI < Instance->get_Nombre_POI(); i_id_POI++)
    {
        if (pb_Id_POI_Pour_Score[i_id_POI] == POI_NON_OBSOLETE &&
            Instance->get_distance_Hotel_POI(i_Hotel_Param, i_id_POI) > Instance->get_POI_Duree_Max_Voyage(i_Id_Journee))
        {

            pi_POI_DANS_LE_RAYON.push_back(i_id_POI);
        }
    }

    return pi_POI_DANS_LE_RAYON;
}

void Heuristique_v1::CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon)
{
    // liste POI calculer precedment
    // on stocke la dessus pi_Id_Hotels

    int i_Score_Total = 0;

    for (int Id_POI : pi_POI_Dans_Le_Rayon)
    {
        //Pour obtenir le score total 
        i_Score_Total += pi_Score_POI[Id_POI] * HOTEL_SCORE_POI;
    }
    // pi_Id_Hotels[i_Hotel_Param] = i_Score_Total;
}

int Heuristique_v1::SelectionnerMeilleurHotel()
{
    int i_Meilleur_Hotel = 0;
    int i_Score_Meilleur_Hotel = -1;

    for (int i_Hotel = 0; i_Hotel < Instance->get_Nombre_Hotel(); i_Hotel++)
    {
        if (pi_Id_Hotels != HOTEL_OBSOLETE) {
            // TODO
        }
    }
}

void Heuristique_v1::CalculMeilleurJournee(int i_Hotel_Depart, int i_Hotel_Arrive)
{
}
