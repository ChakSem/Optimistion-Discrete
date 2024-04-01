#pragma once
#include <unordered_map>
#include "..\..\Instance.hpp"

#define HOTEL_OBSOLETE -2

#define POI_OBSOLETE false
#define POI_NON_OBSOLETE true

/**
 * Determination d'une solution :
 * 
 * Etape 1 : Calculer un score pour chaque POI / Score :    + score du POI
 *                                                          / temps ouvet sur une journée
 * 
 * On définit H <= Hotel de départ
 * 
 * Repeter  
 *    |     (Eliminer les Hotels qui sont à une distance supérieure au nombre de jours restants + qui font revenir en arriere (absice negative sur la projection de H à l'Hotel d'arrivé))
 *    |     Etape 2 : Identifier hotel atteignable en 1 journée à partir de l'hotel H
 *    |
 *    |     (Eliminer les POI qui sont à une distance supérieure au nombre de jours restants)
 *    |     Etape 3 : Pour chacun de ses hotels, calculer et associer un score / Score :    + score de chaque POI à moins de 1 journée (H => POI => Hotel d'arrive)
 *    |                                                                                     => diviser par la distance de l'Hotel au POI
 *    |                                                                                          
 *    |     Etape 4 : Hotel H' <= Hotel avec le score le plus élevé
 *    |     
 *    |     Etape 5 : Calcul du chemin qui maximise le score de H à H'
 *    |     (Eliminer les POI selectionnés)
 *    |     
 *    |     Etape 6 : H <= H'
 * Jusqu'à nb de jour restant = 0 (à la dernière iteration H' est d'office l'hotel d'arrivé
 * 
 * ============================================================================================================================================================
 * Explorer de nouvelles solutions :
 * 
 * Solution 2 ) Refaire l'algo du 1er Hotel selectionné (après l'hotel de départ) à l'hotel d'arrivé, puis trouver le calcul maximal de l'hotel de depart 
 *              à cet hotel (sans les POI deja selectionné)
 * 
 * Solution 3 ) Recommencer à partir du 2eme hotel, puis refaire l'algo de l'hotel de départ au 2e hotel (sans les POI selctionnée lors du 1er algo)
 * 
 * ... 
 * (Attention à choisir le n-ième hotel à partir de la meilleure solution)
 * 
 * 
 * Solution n+1 )   Refaire tout ca dans l'autre sens
 * 
 * ...
 * 
 */


class Heuristique_v1
{
private:
    int i_Jour;
    int i_Duree_Max;
    int i_Duree_Journee_En_Cours;
    int H;

    Instance *Instance;
    vector<double> pi_Score_POI;          // Liste de taille n, stockant le score calculee du POI
    vector<int> pi_Rayon_Hotels;          /* Liste contenant les hotels dans le rayon de l'hotel choisit en debut d'iteration*/

    vector<int> pi_Solution; // Liste des id dans l'ordre des solution

    vector<int> pi_POI_Coherents;
    vector<int> pi_Hotels_Coherents;

public:
    void Solution1();

    void CalculScorePOI(int iPOI_Param);
    /* Permet d'identifier les hotels sur un demi rayon de */
    void IdentifierHotelsDemiRayon(int i_Hotel_Param);
    /* Permet d'intialiser les scores des hotels sur un demi rayon de */
    vector<int> IdentifierPOIRayonHotel(int i_Hotel_Param);
    /* Permet de calculer le score d'un hotel */
    double CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon);
    /* Permet de calculer la meilleure journée entre deux hotels*/
    void CalculMeilleurJournee(int i_Hotel_Depart, int i_Hotel_Arrive);

    void Initialisation();
};