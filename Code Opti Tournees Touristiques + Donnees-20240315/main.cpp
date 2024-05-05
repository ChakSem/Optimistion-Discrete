//#define CHEMIN_DOSSIER_DONNEES "/Users/kergosien/Desktop/Cours Optim Discret/Conception itineraire Touristique/Format Etudiant Public/"
//#define CHEMIN_DOSSIER_DONNEES "S:/Polytech/DI4/S8/Optimistion-Discrete/Code Opti Tournees Touristiques + Donnees-20240315/Format Etudiant Public/"
#define CHEMIN_DOSSIER_DONNEES "C:/Users/33611/Dossier_codes/DC-S8/Optimistion-Discrete/Code Opti Tournees Touristiques + Donnees-20240315/Format Etudiant Public/"

#define NOM_FICHIER_LISTE_FICHIER_DONNEES "data.txt"
#define NOM_FICHIER_LISTE_SORTIE "sortie.txt"

#include <iostream>
#include <fstream>
#include <chrono>
#include <algorithm>

#include "Instance.hpp"
#include "Solution.hpp"
#include "Project1\Project1\Heuristique_v1.h"
#include "Project1\Project1\MetaHeuristique.h"
#include "Project1\Project1\MetaHeuristique_Fourmis.h"
using namespace std;

int Resolution(Instance * instance);
 

int main(int argc, const char* argv[])
{
    try
    {
        string s_tmp;
        string s_chemin = CHEMIN_DOSSIER_DONNEES;
        s_chemin.append(NOM_FICHIER_LISTE_FICHIER_DONNEES);

        ifstream fichier(s_chemin.c_str(), std::ios::in); std::ofstream fichier_Sortie_Resume;
        s_chemin = CHEMIN_DOSSIER_DONNEES;
        s_chemin.append(NOM_FICHIER_LISTE_SORTIE);
        ofstream fichier_Sortie(s_chemin.c_str(), std::ios::out | std::ios::app);
        int Nb_Instance = 1;

       

        if (fichier)
        {
            if (fichier_Sortie)
            {
                fichier_Sortie << " Fichier donnEes\t Tps de rEsolution \t Best solution" << endl;
                getline(fichier, s_tmp);
                while (s_tmp != "")
                {
                    Instance* instance = new Instance();
                    chrono::time_point<chrono::system_clock> chrono_start, chrono_end;
                    chrono::duration<double> elapsed;
                    int i_best_solution_score = 0;
                    s_chemin = CHEMIN_DOSSIER_DONNEES;
                    cout << " Resolution de " << s_tmp << endl;
                    s_chemin.append(s_tmp);
                    s_chemin.erase(remove(s_chemin.begin(), s_chemin.end(), '\r'), s_chemin.end());
                    s_chemin.erase(remove(s_chemin.begin(), s_chemin.end(), '\n'), s_chemin.end());

                    instance->chargement_Instance(s_chemin);
                    chrono_start = chrono::system_clock::now();
                    i_best_solution_score = Resolution(instance);
                    cout << " Fin de resolution de " << s_tmp << endl;
                    chrono_end = chrono::system_clock::now();
                    elapsed = chrono_end - chrono_start;
                    //pour l'heuristique
                    fichier_Sortie << s_chemin << "\t" << elapsed.count() << "\t" << i_best_solution_score << endl;
                   
                    //pour la metaheuristique 
                    fichier_Sortie << s_chemin << "\t" << 
                    Nb_Instance++;

                    s_tmp = "";
                    getline(fichier, s_tmp);
                    delete instance;
                }
                fichier_Sortie.close();
            }
            else
            {
                cout << " Erreur lecture des donnees : chemin vers la sortie non valide. " << endl;
            }
            fichier.close();
        }
        else
        {
            cout << " Erreur lecture des donnees : chemin listant l'ensemble des donnees non valide. " << endl;
        }
    }

    catch (string err)
    {
        cout << "Erreur fatale : " << endl;
        cout << err << endl;
    }
    return 0;

}


int Resolution(Instance * instance)
{
    int i_val_Retour_Fct_obj=0;
    Solution* uneSolution;
    vector<int> v_i_tmp ;

    uneSolution = MetaHeuristique_Fourmis::ExtraireSolution(instance);

/*INITIALISATION D'UN SOLUTION EN DUR*/
    //v_i_tmp.clear();
    //uneSolution->v_Id_Hotel_Intermedaire.push_back(2);
    //uneSolution->v_Date_Depart.push_back(0.0);
    //uneSolution->v_Date_Depart.push_back(0.0);
    //v_i_tmp ={0, 2, 5, 9, 14, 21, 28, 20, 27, 35, 42, 36, 29, 22, 30, 31};
    //uneSolution->v_v_Sequence_Id_Par_Jour.push_back(v_i_tmp);
    //v_i_tmp ={24, 32, 40, 33, 25, 19, 26, 34, 41, 47, 52, 56, 59, 61};
    //uneSolution->v_v_Sequence_Id_Par_Jour.push_back(v_i_tmp);
    //uneSolution->i_valeur_fonction_objectif=816;
/* */
    
    uneSolution->Verification_Solution(instance);
    
    i_val_Retour_Fct_obj=uneSolution->i_valeur_fonction_objectif;
    delete uneSolution;
    return i_val_Retour_Fct_obj;
}

