#include "Instance.hpp"
#include <iostream>
#include <fstream>
#include <sstream>
#include "math.h"


#define LECTURE_INT_MAX 100000

bool Instance::chargement_Instance(string s_chemin) throw(string)
{
    int i_tmp, i_cpt_ligne,i,j;
    float f_tmp;
    string s_tmp;
    istringstream is_tmp;
    vector<string> v_s_tmp;
    
    i_Nombre_Hotel=0;
    i_Nombre_POI=0;
    i_Nombre_Jour=0;
    i_Id_Hotel_Depart=-1;
    i_Id_Hotel_Arrivee=-1;
    v_f_Duree_Max_Voyage_Par_Jour.clear();
    v_POI.clear();
    v_Hotel.clear();
    v_v_f_distances.clear();
    
    ifstream fichier(s_chemin.c_str(), std::ios::in);
    
    if(fichier)
    {
        fichier >> i_tmp >> i_Nombre_Hotel >> i_Nombre_Jour ;
        i_cpt_ligne=1;
    if((i_tmp<0)||(i_tmp>LECTURE_INT_MAX)||(i_Nombre_Hotel<0)||(i_Nombre_Hotel>LECTURE_INT_MAX)||(i_Nombre_Jour<0)||(i_Nombre_Jour>LECTURE_INT_MAX))
        {
            cout<<" Erreur lecture des données : ligne "<<  i_cpt_ligne <<". \n";
            return false;
        }
        i_Nombre_POI = i_tmp -1;
        i_Nombre_Hotel=i_Nombre_Hotel+2;
        
        getline(fichier, s_tmp);
        getline(fichier, s_tmp);
        getline(fichier, s_tmp);
        i_cpt_ligne=i_cpt_ligne+2;
        is_tmp.clear();
        is_tmp.str(s_tmp);
        while(getline(is_tmp,s_tmp,' '))
        {
            if((s_tmp!="")&&(s_tmp!="\r"))
            {
                f_tmp = atof(s_tmp.c_str());
                v_f_Duree_Max_Voyage_Par_Jour.push_back(f_tmp);
            }
        }
        if(v_f_Duree_Max_Voyage_Par_Jour.size()!=i_Nombre_Jour)
        {
            cout << " Erreur lecture des données : ligne "<<  i_cpt_ligne <<" : le nombre de durée max ne correspond pas au nombre de jour. \n";
            return false;
        }
        getline(fichier, s_tmp);
        getline(fichier, s_tmp);
        i_cpt_ligne=i_cpt_ligne+2;
        while(s_tmp!="")
        {
            v_s_tmp.clear();
            is_tmp.clear();
            is_tmp.str(s_tmp);
            while(getline(is_tmp,s_tmp,' '))
            {
                if((s_tmp!="")&&(s_tmp!="\r"))
                {
                    v_s_tmp.push_back(s_tmp);
                }
            }
            if(v_s_tmp.size()!=7)
            {
                cout << " Erreur lecture des données : ligne "<<  i_cpt_ligne <<" : nombre de caractéristique. \n";
                return false;
            }
            switch(v_s_tmp[0][0])
            {
                case 'H':
                    Hotel c_Hotel;
                    c_Hotel.f_coord_X=atof(v_s_tmp[1].c_str());
                    c_Hotel.f_coord_Y=atof(v_s_tmp[2].c_str());
                    v_Hotel.push_back(c_Hotel);
                    break;
                case 'C':
                    POI c_POI;
                    c_POI.f_coord_X=atof(v_s_tmp[1].c_str());
                    c_POI.f_coord_Y=atof(v_s_tmp[2].c_str());
                    c_POI.i_score=atoi(v_s_tmp[3].c_str());
                    c_POI.f_heure_ouverture=atof(v_s_tmp[5].c_str());
                    c_POI.f_heure_fermeture=atof(v_s_tmp[6].c_str());
                    v_POI.push_back(c_POI);
                    break;
                default :
                    cout << " Erreur lecture des données : ligne "<<  i_cpt_ligne <<" : identification de la ligne. \n";
                    return false;
                    break;
            }
            getline(fichier, s_tmp);
            i_cpt_ligne++;
        }
        if(v_POI.size()!=i_Nombre_POI)
        {
            cout << " Erreur lecture des données : Mauvais nombre de POIs. \n";
            return false;
        }
        if(v_Hotel.size()!=i_Nombre_Hotel)
        {
            cout << " Erreur lecture des données : Mauvais nombre d'hotel. \n";
            return false;
        }
        i_Id_Hotel_Depart=0;
        i_Id_Hotel_Arrivee=1;
        
        for(i=0;i<i_Nombre_Hotel;i++)
        {
            vector<float> v_f_tmp;
            for(j=0;j<i_Nombre_Hotel;j++)
            {
                v_f_tmp.push_back(sqrt(pow(v_Hotel[i].f_coord_X-v_Hotel[j].f_coord_X, 2) + pow(v_Hotel[i].f_coord_Y-v_Hotel[j].f_coord_Y, 2)));
            }
            for(j=0;j<i_Nombre_POI;j++)
            {
                v_f_tmp.push_back(sqrt(pow(v_Hotel[i].f_coord_X-v_POI[j].f_coord_X, 2) + pow(v_Hotel[i].f_coord_Y-v_POI[j].f_coord_Y, 2)));
            }
            v_v_f_distances.push_back(v_f_tmp);
        }
        
        for(i=0;i<i_Nombre_POI;i++)
        {
            vector<float> v_f_tmp;
            for(j=0;j<i_Nombre_Hotel;j++)
            {
                v_f_tmp.push_back(sqrt(pow(v_POI[i].f_coord_X-v_Hotel[j].f_coord_X, 2) + pow(v_POI[i].f_coord_Y-v_Hotel[j].f_coord_Y, 2)));
            }
            for(j=0;j<i_Nombre_POI;j++)
            {
                v_f_tmp.push_back(sqrt(pow(v_POI[i].f_coord_X-v_POI[j].f_coord_X, 2) + pow(v_POI[i].f_coord_Y-v_POI[j].f_coord_Y, 2)));
            }
            v_v_f_distances.push_back(v_f_tmp);
        }
        fichier.close();
        return true;
    }
    else
    {
        cout<<" Erreur lecture des données : chemin non valide. \n";
        return false;
    }
}

Instance::Instance()
{
    s_chemin="";
    i_Nombre_Hotel=0;
    i_Nombre_POI=0;
    i_Nombre_Jour=0;
    i_Id_Hotel_Depart=-1;
    i_Id_Hotel_Arrivee=-1;
}

Instance::~Instance()
{
    //RAS
}
string Instance::get_chemin(void)
{
    return s_chemin;
}
int Instance::get_Nombre_Hotel(void)
{
    return i_Nombre_Hotel;
}
int Instance::get_Nombre_POI(void)
{
    return i_Nombre_POI;
}
int Instance::get_Nombre_Jour(void)
{
    return i_Nombre_Jour;
}
float Instance::get_POI_Duree_Max_Voyage(int i_Id_jour) throw(string)
{
    if(v_f_Duree_Max_Voyage_Par_Jour.size()==0)
        return 0.0;
    if((i_Id_jour>=0)&&(i_Id_jour<i_Nombre_Jour))
        return v_f_Duree_Max_Voyage_Par_Jour[i_Id_jour];
    else
    {
        string err = "Instance.cpp : get_POI_Duree_Max_Voyage : le nombre de jours doit être compris entre 0 et " +to_string(i_Nombre_Jour)+ ".";
        throw string(err);
    }
}
float Instance::get_POI_Coord_X(int i_Id_POI) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_POI>=0)&&(i_Id_POI<i_Nombre_POI))
        return v_POI[i_Id_POI].f_coord_X;
    else
    {
        string err = "Instance.cpp : get_POI_Coord_X : l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}
float Instance::get_POI_Coord_Y(int i_Id_POI) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_POI>=0)&&(i_Id_POI<i_Nombre_POI))
        return v_POI[i_Id_POI].f_coord_Y;
    else
    {
        string err = "Instance.cpp : get_POI_Coord_Y : l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}
int Instance::get_POI_Score(int i_Id_POI) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_POI>=0)&&(i_Id_POI<i_Nombre_POI))
        return v_POI[i_Id_POI].i_score;
    else
    {
        string err = "Instance.cpp : get_POI_Score : l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}
float Instance::get_POI_Heure_ouverture(int i_Id_POI) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_POI>=0)&&(i_Id_POI<i_Nombre_POI))
        return v_POI[i_Id_POI].f_heure_ouverture;
    else
    {
        string err = "Instance.cpp : get_POI_Heure_ouverture : l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}
float Instance::get_POI_Heure_fermeture(int i_Id_POI) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_POI>=0)&&(i_Id_POI<i_Nombre_POI))
        return v_POI[i_Id_POI].f_heure_fermeture;
    else
    {
        string err = "Instance.cpp : get_POI_Heure_fermeture : l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}
int Instance::get_Id_Hotel_depart(void)
{
    return i_Id_Hotel_Depart;
}
int Instance::get_Id_Hotel_Arrivee(void)
{
    return i_Id_Hotel_Arrivee;
}
float Instance::get_Hotel_Coord_X(int i_Id_Hotel) throw(string)
{
    if(v_Hotel.size()==0)
        return 0.0;
    if((i_Id_Hotel>=0)&&(i_Id_Hotel<i_Nombre_Hotel))
        return v_Hotel[i_Id_Hotel].f_coord_X;
    else
    {
        string err = "Instance.cpp : get_Hotel_Coord_X : l'id de l'hotel doit être compris entre 0 et " +to_string(i_Nombre_Hotel)+ ".";
        throw string(err);
    }
}
float Instance::get_Hotel_Coord_Y(int i_Id_Hotel) throw(string)
{
    if(v_Hotel.size()==0)
        return 0.0;
    if((i_Id_Hotel>=0)&&(i_Id_Hotel<i_Nombre_Hotel))
        return v_Hotel[i_Id_Hotel].f_coord_Y;
    else
    {
        string err = "Instance.cpp : get_Hotel_Coord_Y : l'id de l'hotel doit être compris entre 0 et " +to_string(i_Nombre_Hotel)+ ".";
        throw string(err);
    }
}
float Instance::get_distance_Hotel_POI(int i_Id_Hotel ,int i_Id_POI) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_Hotel>=0)&&(i_Id_Hotel<i_Nombre_Hotel)&&(i_Id_POI>=0)&&(i_Id_POI<i_Nombre_POI))
        return v_v_f_distances[i_Id_Hotel][i_Id_POI+i_Nombre_Hotel];
    else
    {
        string err = "Instance.cpp : get_distance_Hotel_POI : l'id de l'hotel doit être compris entre 0 et " +to_string(i_Nombre_Hotel)+ " et  l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}
float Instance::get_distance_POI_POI(int i_Id_POI1 ,int i_Id_POI2) throw(string)
{
    if(v_POI.size()==0)
        return 0.0;
    if((i_Id_POI1>=0)&&(i_Id_POI1<i_Nombre_POI)&&(i_Id_POI2>=0)&&(i_Id_POI2<i_Nombre_POI))
        return v_v_f_distances[i_Id_POI1+i_Nombre_Hotel][i_Id_POI2+i_Nombre_Hotel];
    else
    {
        string err = "Instance.cpp : get_distance_POI_POI : l'id du POI doit être compris entre 0 et " +to_string(i_Nombre_POI)+ ".";
        throw string(err);
    }
}

float Instance::get_distance_Hotel_Hotel(int i_Id_Hotel1,int i_Id_Hotel2) throw(string)
{
    if(v_Hotel.size()==0)
        return 0.0;
    if((i_Id_Hotel1>=0)&&(i_Id_Hotel1<i_Nombre_Hotel)&&(i_Id_Hotel2>=0)&&(i_Id_Hotel2<i_Nombre_Hotel))
        return v_v_f_distances[i_Id_Hotel1][i_Id_Hotel2];
    else
    {
        string err = "Instance.cpp : get_distance_Hotel_Hotel : l'id de l'hotel doit être compris entre 0 et " +to_string(i_Nombre_Hotel)+".";
        throw string(err);
    }
}
