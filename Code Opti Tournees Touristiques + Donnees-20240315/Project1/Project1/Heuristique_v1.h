#pragma once
#include <unordered_map>
#include "..\..\Instance.hpp"

#define HOTEL_OBSOLETE -2

#define POI_OBSOLETE false
#define POI_NON_OBSOLETE true

class Heuristique_v1
{
private:
    Instance *Instance;
    vector<int> pi_Score_POI;          // Liste de taille n, stockant le score calculee du POI
    vector<int> pi_Id_Hotels;          /* Liste de taille m (nb d'Hotels) :  -1 initialement
                                                                             -2 si l'Hotel est obsolete
                                                                             son score sinon*/
    vector<bool> pb_Id_POI_Pour_Score; /* Liste de taille n (nb de POI), puis chaque POI est elimines chaques fois qu'il devient obsolete
                                                                              - true : non_obsolete
                                                                              - false : obsolete*/

    vector<int> pi_Solution; // Liste des id dans l'ordre des solution

public:
    void CalculScorePOI(int iPOI_Param);
    /* Permet d'identifier les hotels sur un demi rayon de */
    void IdentifierHotelsDemiRayon(int i_Hotel_Param);
    /* Permet d'intialiser les scores des hotels sur un demi rayon de */
    vector<int> IdentifierPOIRayonHotel(int i_Hotel_Param, int i_Id_Journee);
    /* Permet de calculer le score d'un hotel */
    void CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon);
    /* Permet de determiner le meilleur hotel (celui qui a le meilleur score dans son rayon) */
    int SelectionnerMeilleurHotel();
    /* Permet de calculer la meilleure journée entre deux hotels*/
    void CalculMeilleurJournee(int i_Hotel_Depart, int i_Hotel_Arrive);
};
