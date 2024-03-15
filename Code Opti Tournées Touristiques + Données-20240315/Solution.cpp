#include "Solution.hpp"
#include <iostream>
#include <set>

Solution::Solution()
{ 
    i_valeur_fonction_objectif=0;
}
Solution::~Solution()
{
    
}

bool Solution::Verification_Solution(Instance *instance)
{
    bool b_solution_ok=true;
    int i,j,i_score;
    float f_date;
    set<int> set_POIs_Visites;
    i_score=0;
    //Vérification des Ids Hôtel et la taille du tableau associé
    if(v_Id_Hotel_Intermedaire.size()!=(instance->get_Nombre_Jour()-1))
    {
        cout<< "Erreur : Nombre d'hôtels de v_Id_Hotel_Intermedaire. " <<endl;
        b_solution_ok=false;
    }
    for(i=0;i<v_Id_Hotel_Intermedaire.size();i++)
    {
        if((v_Id_Hotel_Intermedaire[i]<0)||(v_Id_Hotel_Intermedaire[i]>=instance->get_Nombre_Hotel()))
        {
            cout<< "Erreur : Id hôtel dans v_Id_Hotel_Intermedaire ("<<i<<") : "<< v_Id_Hotel_Intermedaire[i]<<"." <<endl;
            b_solution_ok=false;
        }
    }
    //Vérification des Ids POIs (et unicité) et la taille du tableau associé
    if(v_v_Sequence_Id_Par_Jour.size()!=(instance->get_Nombre_Jour()))
    {
        cout<< "Erreur : La taille de la première dimension de v_v_Sequence_Id_Par_Jour doit être égale au nombre de jour. " <<endl;
        b_solution_ok=false;
    }
    for(i=0;i<v_v_Sequence_Id_Par_Jour.size();i++)
    {
        for(j=0;j<v_v_Sequence_Id_Par_Jour[i].size();j++)
        {
            if((v_v_Sequence_Id_Par_Jour[i][j]<0)||(v_v_Sequence_Id_Par_Jour[i][j]>=instance->get_Nombre_POI()))
            {
                cout<< "Erreur : Id POI dans v_v_Sequence_Id_Par_Jour ("<<i<<","<<j<<") : "<< v_v_Sequence_Id_Par_Jour[i][j]<<"." <<endl;
                b_solution_ok=false;
            }
            else
            {
                if(set_POIs_Visites.find(v_v_Sequence_Id_Par_Jour[i][j])==set_POIs_Visites.end())
                {
                    set_POIs_Visites.insert(v_v_Sequence_Id_Par_Jour[i][j]);
                    i_score=i_score+instance->get_POI_Score(v_v_Sequence_Id_Par_Jour[i][j]);
                }
                else
                {
                    cout<< "Erreur : le POI "<<v_v_Sequence_Id_Par_Jour[i][j]<<" est visité plsuieurs fois dans la solution. " <<endl;
                    b_solution_ok=false;
                }
            }
        }
    }
    //Vérification des dates de départs et la taille du tableau associé
    if(v_Date_Depart.size()!=(instance->get_Nombre_Jour()))
    {
        cout<< "Erreur : Nombre de date de départ de v_Date_Depart. " <<endl;
        b_solution_ok=false;
    }
    for(i=0;i<v_Date_Depart.size();i++)
    {
        if((v_Date_Depart[i]<0)||(v_Date_Depart[i]>=100000))
        {
            cout<< "Erreur : date de départ dans v_Date_Depart ("<<i<<") : "<< v_Date_Depart[i]<<"." <<endl;
            b_solution_ok=false;
        }
    }
    //Vérification du respect des fenêtres de temps des POIs et  des durées maximales d'une journée
    for(i=0;i<v_v_Sequence_Id_Par_Jour.size();i++)
    {
        if(v_v_Sequence_Id_Par_Jour[i].size()!=0)
        {
            f_date=v_Date_Depart[i];
            if(i==0)
                f_date=f_date+instance->get_distance_Hotel_POI(instance->get_Id_Hotel_depart(), v_v_Sequence_Id_Par_Jour[i][0]);
            else
                f_date=f_date+instance->get_distance_Hotel_POI(v_Id_Hotel_Intermedaire[i-1], v_v_Sequence_Id_Par_Jour[i][0]);
            j=0;
            do
            {
                if(f_date<instance->get_POI_Heure_ouverture(v_v_Sequence_Id_Par_Jour[i][j]))
                    f_date=instance->get_POI_Heure_ouverture(v_v_Sequence_Id_Par_Jour[i][j]);
                else
                {
                    if(f_date>instance->get_POI_Heure_fermeture(v_v_Sequence_Id_Par_Jour[i][j]))
                    {
                        cout<< "Erreur : arrivé après la fermeture du POI "<<v_v_Sequence_Id_Par_Jour[i][j]<<", heure : "<< f_date<<"." <<endl;
                        b_solution_ok=false;
                    }
                }
                j++;
                if(j<v_v_Sequence_Id_Par_Jour[i].size())
                {
                    f_date=f_date+instance->get_distance_POI_POI(v_v_Sequence_Id_Par_Jour[i][j-1], v_v_Sequence_Id_Par_Jour[i][j]);
                }
            }while(j<v_v_Sequence_Id_Par_Jour[i].size());
            if(i==(v_v_Sequence_Id_Par_Jour.size()-1))
                f_date=f_date+instance->get_distance_Hotel_POI(instance->get_Id_Hotel_Arrivee(), v_v_Sequence_Id_Par_Jour[i][j-1]);
            else
                f_date=f_date+instance->get_distance_Hotel_POI(v_Id_Hotel_Intermedaire[i], v_v_Sequence_Id_Par_Jour[i][j-1]);
            if((f_date-v_Date_Depart[i])>instance->get_POI_Duree_Max_Voyage(i))
            {
                cout<< "Erreur : durée max du jour "<<i<<" dépassée puisqu'elle dure : "<< (f_date-v_Date_Depart[i])<<"." <<endl;
                b_solution_ok=false;
            }
        }
        else
        {
            int i_ID_depart,i_ID_Arrivee;
            f_date=v_Date_Depart[i];
            if(instance->get_Nombre_Jour()>1)
            {
                if(i==0)
                    i_ID_depart=instance->get_Id_Hotel_depart();
                else
                    i_ID_depart=v_Id_Hotel_Intermedaire[i-1];

                if((instance->get_Nombre_Jour()-1)==i)
                    i_ID_Arrivee=instance->get_Id_Hotel_Arrivee();
                else
                    i_ID_Arrivee=v_Id_Hotel_Intermedaire[i];
            }
            else
            {
                i_ID_depart=instance->get_Id_Hotel_depart();
                i_ID_Arrivee=instance->get_Id_Hotel_Arrivee();
            }
            
            f_date=f_date+instance->get_distance_Hotel_Hotel(i_ID_depart, i_ID_Arrivee);
            if((f_date-v_Date_Depart[i])>instance->get_POI_Duree_Max_Voyage(i))
            {
                cout<< "Erreur : durée max du jour "<<i<<" dépassée puisqu'elle dure : "<< (f_date-v_Date_Depart[i])<<"." <<endl;
                b_solution_ok=false;
            }
        }
    }
    //Vérification de la fonction objectif
    if(i_score!=i_valeur_fonction_objectif)
    {
        cout<< "Erreur : la fonction objectif est mal calculée, cela devrait être : "<< i_score<<"." <<endl;
        b_solution_ok=false;
    }

    return b_solution_ok;
}
