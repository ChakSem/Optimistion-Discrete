#pragma once


using namespace std;

#ifndef HEURISTIQUE
#define HEURISTIQUE

#include <unordered_map>
#include <map>
#include <algorithm>    
#include "..\..\Instance.hpp"
#include "..\..\Solution.hpp"

/**
 * Determination d'une solution :
 * 
 * Etape 1 : Calculer un score pour chaque POI / Score :    
 * 
 * On définit H <= Hotel de départ
 * 
 * Etape 2 : Premiere solution
 * Repeter  
 *    |     Etape 2-1 : Identifier hotel à atteindre en fin de journée
 *    |     
 *    |         Identifier les hotels candidats
 *    |     
 *    |         Pour chacun, identifier les POI dans un rayon autour d'eux
 *    |         Puis, leur attribuer un score, en fonction de ces POI
 *    |     
 *    |     
 *    |         Si Plus d'un hotel a été identifer
 *    |            |    H' = hotel avec le meilleur score
 *    |            |    
 *    |            |    Etape 2-2 : Trouver la meilleure sequence de POI
 *    |            |       
 *    |            |    Adapter le score des POI dans le rayon de H' en fonction de leur proximite avec la droite liant H et H'
 *    |            |    
 *    |            |    Sequence = {}   
 *    |            |    
 *    |            |    Repeter
 *    |            |       |
 *    |            |       |    Prendre le POI avec le plus gros score
 *    |            |       |
 *    |            |       |    Essayer de l'inserer à toutes les positions possibles dans sequence
 *    |            |       |
 *    |            |       |    Garder la meilleure sequence (qui est valide) (celle qui maximise les long temps d'attentes (pour faciliter l'insetion d'autres POI))
 *    |            |       |
 *    |            |    Tant que des POI n'ont pas encore été traités
 *    |            |    
 *    |            |    Sauvegarder Sequence
 *    |            |    
 *    |            |    H = H'
 *    |         Sinon
 *    |            |    Revenir d'un jour en arriere
 *    |            |    Choisir un autre hotel
 *    |            |    [...]
 *    |         FinSi
 * Jusqu'à nb de jour restant = 0 (à la dernière iteration H' est d'office l'hotel d'arrivé)
 * 
 * Etape 3 : Determiner les POI dans le rayons de plusieurs hotels choisits
 * 
 * Etape 4 : Determiner le meilleur ordre pour traiter les jours
 * 
 * Pour chque POI determiner le jour qui profite le mieux de lui
 * 
 * Trier les jours en de celui qui est le meilleur sur le plus de POI et celui qui l'est sur le moins
 * 
 * Etape 5 : Trouver une seconde solution en traitant les jours dans cet ordre
 * [...]
 * 
 * Sauvegarder la meilleure des 2 solutions
 */

class Heuristique_v1
{
protected:
    // Reference au objets externes
    Instance* instance;
    Solution* solution;

    
    vector<pair<int, int>> pii_Hotels_par_Jour;
    vector<vector<int>> ppi_POI_par_Jour; // id_Jour : POI accessible entre hotel du debut de journee et hotel de fin de journee

    unordered_map<int, double> map_Score_POI;          // Map de taille n, stockant le score calculee du POI
    vector<unordered_map<int, vector<int>>> pmpi_Rayon_Hotels;          /* Liste contenant les hotels dans le rayon de l'hotel choisit en debut d'iteration*/

    unordered_map<int, vector<int>> map_conflit_POI; // Stocke la liste des journée qui peuvent intégrer le POI pour chaque POI
    vector<int> pi_Jours_Tries; // Stocke l'ordre dans lequel traiter les jours

    int i_FO;
    vector<pair<float, vector<int>>> pp_Meilleure_Sequence_par_Jour; // Stocke les meilleures séquences à chaque jour

    float f_Duree_Totale_Restante;

public:
    /// <summary>
    /// Constructeur de l'Heuristique
    /// </summary>
    /// <param name="instanceParam"> Instance a resoudre </param>
    Heuristique_v1(Instance* problemeParam);

    /// <summary>
    /// Méthode appelé dans le main pour lancer l'heuristique et stocker la solution dans Solution
    /// </summary>
    /// <param name="instanceParam"> Instance a resoudre </param>
    /// <returns></returns>
    static Solution* ExtraireSolution(Instance* problemeParam);

    /// <summary>
    /// Déroule l'algorithme
    /// </summary>
    void Solution1();

    /// <summary>
    /// Determine le score du POI en paramètre
    /// </summary>
    /// <param name="i_POI_Param"></param>
    /// <param name="instance"></param>
    /// <returns></returns>
    static double CalculScorePOI(int iPOI_Param, Instance* instance);
    
    /// <summary>
    /// Identifie les hotels joignables en une moins d'1 journée depuis l'hotel en parametre
    /// </summary>
    /// <param name="i_Hotel_Depart"></param>
    /// <param name="i_Jour"></param>
    /// <param name="pi_Hotels"></param>
    /// <returns> Identifies les hotels possible à la fin de la journée </returns>
    vector<int> IdentifierHotelsFinJournee(int i_Hotel_Param, int i_Jour, vector<int> pi_Hotels);

    /// <summary>
    /// Methode qui permet d'identifier les POI dans joignable en moins d'1 journée
    /// </summary>
    /// <param name="i_Hotel_Debut_Journee_Param"></param>
    /// <param name="i_Hotel_Param"> Le pottentiel hotel de fin de journée </param>
    /// <param name="instanceParam"></param>
    /// <param name="pi_POI_Coherents_Param"></param>
    /// <param name="i_Jour_Param"></param>
    /// <returns> Liste des POI </returns>
    static vector<int> IdentifierPOIRayonHotel(int i_Hotel_Debut_Journee_Param, int i_Hotel_Param, Instance* instanceParam, vector<int> pi_POI_Coherents_Param, int i_Jour_Param);

    /// <summary>
    /// Methode qui permet de calculer le score d'un hotel en fonction des POI dans le rayon
    /// </summary>
    /// <param name="i_Hotel_Param"></param>
    /// <param name="pi_POI_Dans_Le_Rayon"></param>
    /// <returns> Score total de l'hotel </returns>
    float CalculScoreHotel(int i_Hotel_Param, vector<int> pi_POI_Dans_Le_Rayon, int i_Hotel_Debut_Journee);

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
    pair<float, vector<int>> CalculMeilleureJournee_v1(int i_Jour, vector<int>* pi_POI_Disponibles);
    pair<float, vector<int>> CalculMeilleureJournee_v2(int i_Jour, vector<int>* pi_POI_Disponibles);
    static pair<float, vector<int>> CalculMeilleureJournee(Instance* instance, vector<int> pi_POI_Joignables, unordered_map<int, double> map_Score_POI, int i_Hotel_Debut_Journee, int i_Hotel_Fin_Journee, int i_Jour, vector<int> pi_Sequence_Initiale);
    
    /// <summary>
    /// Détermine la meilleure séquence (qui laisse le plus de place pour insérer d'autre POI), à partir d'une séquence en paramètre et un POI à insérer
    /// </summary>
    /// <param name="pi_Sequence"></param>
    /// <param name="p_Infos"></param>
    /// <param name="id_POI"></param>
    /// <param name="info"></param>
    /// <returns></returns>
    static pair<float, vector<int>> MeilleureSequence(pair<float, vector<int>> fpi_Paire_Sequence, int id_POI, Instance* instance, int i_Hotel_Debut_Journee, int i_Hotel_Fin_Journee, int i_Jour);

private :
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
    static float GetScoreSequence(vector<int> pi_Trajet, Instance* instance, int i_Hotel_Debut_Journee, int i_Hotel_Fin_Journee, int i_Jour, float f_Heure_Debut);

    /// <summary>
    /// Permet de sauvegarder la solution trouvée dans Solution
    /// </summary>
    void SauvegarderSolution();

    /// <summary>
    /// Permet de calculer le score d'une séquence de POI
    /// </summary>
    /// <param name="pi_Sequence"></param>
    /// <param name="instance"></param>
    /// <returns></returns>
    static int CalculScoreSequence(vector<int> pi_Sequence, Instance* instance);

    /// <summary>
    /// Permet d'obtenir de combien i_Hotel_Param rapproche de l'hotel d'arrive par rapport à sa projection sur la droite reliant l'hotel d'arrive à i_Hotel_Debut_Journee 
    /// </summary>
    /// <param name="i_Hotel_Param"></param>
    /// <param name="i_Hotel_Debut_Journee"></param>
    /// <returns></returns>
    float GetAbsiceProjectionHotel(int i_Hotel_Param, int i_Hotel_Debut_Journee);
};

void SupprimerElement(vector<int>* pi_Array, int i_Element);

#endif