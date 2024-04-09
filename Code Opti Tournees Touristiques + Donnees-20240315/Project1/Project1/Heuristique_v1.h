#pragma once
#include <unordered_map>
#include <map>
#include <algorithm>    
#include "..\..\Instance.hpp"
#include "..\..\Solution.hpp"

#define HOTEL_OBSOLETE -2

#define POI_OBSOLETE false
#define POI_NON_OBSOLETE true

using namespace std;

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
 *    |        |        Tri des POI selon leur score (pre
 *    |        |        Sequence <- {}
 *    |        |        
 *    |        |        Pour tout les i dans POI accessible depuis H à H'
 *    |        |           |    Inserer i à toutes les position de Sequence possibles : Sequences <- {j U Sequence, Sequence[0] U i U Sequence[1:], ...}
 *    |        |           |    Eliminer de Sequences les sequence impossibles
 *    |        |           |    
 *    |        |           |    Si Sequences != {} : Sequence <- Meilleure(Sequences)
 *    |        |        FinPour
 *    |       ---
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
 * ============================================================================================================================================================
 * Score hotel en fonction de la deformation sur la projection de H à Ha
 * 
 * Retour en arrière lors de CalculMeilleurJournee (POI 0 haut score qui ouvre le soir)
 * 
 * Ne pas eliminer les hotels qui font revenir en arriere (cas ou journee tres courte rend hotel Hm mauvais, donc joint hotel plus proche de Ha
 * mais journee d'apres plus longue, rend Hm interesssant)
 */

class Heuristique_v1
{
private:
    int i_Jour;
    float f_Duree_Totale_Restante;
    float f_Duree_Journee_En_Cours;
    int i_Hotel_Debut_Journee;
    int i_Hotel_Fin_Journee;

    Instance * instance;
    unordered_map<int, float> map_Score_POI;          // Map de taille n, stockant le score calculee du POI
    vector<int> pi_Rayon_Hotels;          /* Liste contenant les hotels dans le rayon de l'hotel choisit en debut d'iteration*/

    vector<int> pi_POI_Joignables;

    vector<int> pi_Hotels_Coherents;

    int i_Score_Solution;
    Solution* solution;

public:
    Heuristique_v1(Instance* problemeParam, Solution* nouvelleSolution);

    static Solution* ExtraireSolution(Instance* problemeParam);

    /// <summary>
    /// Déroule l'algorithme
    /// </summary>
    void Solution1();

    /// <summary>
    /// Determine le score du POI en paramètre
    /// </summary>
    /// <param name="i_POI_Param"></param>
    void CalculScorePOI(int iPOI_Param);
    
    /// <summary>
    /// Identifie les hotels joignables en une moins d'1 journée depuis l'hotel en parametre
    /// </summary>
    /// <param name="i_Hotel_Depart"></param>
    void IdentifierHotelsFinJournee(int i_Hotel_Param);


    /// <summary>
    /// Methode qui permet d'identifier les POI dans joignable en moins d'1 journée
    /// </summary>
    /// <param name="i_Hotel_Param"></param>
    /// <returns></returns>
    vector<int> IdentifierPOIRayonHotel(int i_Hotel_Param);

    /// <summary>
    /// Methode qui permet de calculer le score d'un hotel en fonction des POI dans le rayon
    /// </summary>
    /// <param name="i_Hotel_Param"></param>
    /// <param name="pi_POI_Dans_Le_Rayon"></param>
    /// <returns> Score total de l'hotel </returns>
    float CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon);

    /// <summary>
    /// Determine la meilleure succession de POI pour une journée, à partir des hotels de départ et d'arrivé (de la journée) et des POI cohérents
    /// </summary>
    vector<int> CalculMeilleureJournee();
    
    /// <summary>
    /// Détermine la meilleure séquence (qui laisse le plus de place pour insérer d'autre POI), à partir d'une séquence en paramètre et un POI à insérer
    /// </summary>
    /// <param name="pi_Sequence"></param>
    /// <param name="p_Infos"></param>
    /// <param name="id_POI"></param>
    /// <param name="info"></param>
    /// <returns></returns>
    vector<int> MeilleureSequence(vector<int> pi_Sequence, int id_POI);
    
    /// <summary>
    /// Initialise les listes à partir des données du problèmes
    /// </summary>
    void Initialisation();

private :
    /// <summary>
    /// Renvoie un score basé sur les temps libres (attente avant ouverture POI, nb d'heures restantes à la fin) de la journée, donnant plus d'importance au long
    /// </summary>
    /// <param name="pi_Trajet"></param>
    /// <returns> temps libre ou -1 si impossible </returns>
    float GetScoreSequence(vector<int> pi_Trajet);

    /// <summary>
    /// Methode pour realiser la projection d'un POI sur la droite joignant le couple d'Hotels de la journée
    /// </summary>
    /// <param name="i_POI_Param"></param>
    /// <returns> absice du POI sur la droite de projection </returns>
    float GetAbsiceProjectionPOI(int i_POI_Param);

    /// <summary>
    /// 
    /// </summary>
    float GetAbsiceProjectionHotel(int i_Hotel_Param);

    /// <summary>
    /// Donne la distance d'un hotel au POI en fonction de leurs coordonnees
    /// </summary>
    /// <param name="i_Hotel"></param>
    /// <param name="i_POI"></param>
    /// <returns></returns>
    float GetVraieDistanceHotelPOI(int i_Hotel, int i_POI);
};
