#pragma once
#include <unordered_map>
#include <map>
#include <algorithm>    
#include "..\..\Instance.hpp"
#include "..\..\Solution.hpp"
#include "Heuristique_v1.h"

#define HOTEL_OBSOLETE -2

#define POI_OBSOLETE false
#define POI_NON_OBSOLETE true

using namespace std;

class MetaHeuristique
{
private:
	Instance* instance;
	Solution* solution;
	
	vector<int> pi_POI;
	vector<vector<int>> ppi_POI_par_Jour; // id_Jour : POI accessible entre hotel du debut de journee et hotel de fin de journee

public :
	MetaHeuristique(Instance* instanceParam);
	static Solution* ExtraireSolution(Instance* instanceParam);
	void Solution();
};