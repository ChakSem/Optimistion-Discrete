#ifndef Instance_hpp
#define Instance_hpp

#include <stdio.h>
#include <string>
#include <vector>

using namespace std;

class Instance
{
    private:
        string s_chemin;            //Chemin du fichier de données
        int i_Nombre_Hotel;         //Nombre d'hotêls numéroté (id) de 0 à i_Nombre_Hotel-1
        int i_Nombre_POI;           //Nombre de POIs numéroté (id) de 0 à i_Nombre_POI-1
        int i_Nombre_Jour;          //Nombre de jours numéroté (id) de 0 à i_Nombre_Jour-1
        
        vector<float> v_f_Duree_Max_Voyage_Par_Jour; //Durée maxiamel d'une journée (heure arrivé - heure départ pour chaque jour)
    
        class POI
        {
        public:
            float f_coord_X;        //Coordonnée X
            float f_coord_Y;        //Coordonnée Y
            int i_score;            //Score du POI
            float f_heure_ouverture;//Heure d'ouverture
            float f_heure_fermeture;//Heure de fermeture
        };
        vector<POI> v_POI;          //L'ensemble des POIs (Id = position dans le tableau)
        
        int i_Id_Hotel_Depart;      //Id de l'hôtel de départ
        int i_Id_Hotel_Arrivee;     //Id de l'hôtel d'arrivée
        class Hotel
        {
        public:
            float f_coord_X;        //Coordonnée X
            float f_coord_Y;        //Coordonnée Y
        };
        vector<Hotel> v_Hotel;      //L'ensemble des hôtels
    
        vector<vector<float>> v_v_f_distances; // Matrice symétrique de distance (Hotel puis POI)
    
    public:
    /* Constructeurs et destructeur  */
        Instance();
        virtual ~Instance();
    
    /* Instancie les données (écrase les données précédentes), retourne VRAI si chargement correct, FAUX sinon */
        bool chargement_Instance(string s_chemin) throw(string);
        
    /* LES Getters */
        string get_chemin(void);
        int get_Nombre_Hotel(void);
        int get_Nombre_POI(void);
        int get_Nombre_Jour(void);
        float get_POI_Duree_Max_Voyage(int i_Id_jour) throw(string);
        float get_POI_Coord_X(int i_Id_POI) throw(string);
        float get_POI_Coord_Y(int i_Id_POI) throw(string);
        int get_POI_Score(int i_Id_POI) throw(string);
        float get_POI_Heure_ouverture(int i_Id_POI) throw(string);
        float get_POI_Heure_fermeture(int i_Id_POI) throw(string);
        int get_Id_Hotel_depart(void);
        int get_Id_Hotel_Arrivee(void);
        float get_Hotel_Coord_X(int i_Id_Hotel) throw(string);
        float get_Hotel_Coord_Y(int i_Id_Hotel) throw(string);
        float get_distance_Hotel_POI(int i_Id_Hotel ,int i_Id_POI) throw(string);
        float get_distance_POI_POI(int i_Id_POI1 ,int i_Id_POI2) throw(string);
        float get_distance_Hotel_Hotel(int i_Id_Hotel1,int i_Id_Hotel2) throw(string);
};


#endif
