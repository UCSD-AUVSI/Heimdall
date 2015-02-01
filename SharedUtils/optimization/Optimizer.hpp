#ifndef ____SHARED_UTILS_OPTIMIZER_H_______
#define ____SHARED_UTILS_OPTIMIZER_H_______

#include "OptimizeableModule.hpp"


class Optimizer_MainAlgorithm //the optimizer: SimulatedAnnealing, etc.
{
public:
	Optimizer_Params * savedResultsOfInitialWarmup;
	
	Optimizer_MainAlgorithm() : savedResultsOfInitialWarmup(nullptr) {}
	
	virtual void InitialWarmup(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData) = 0;
	virtual void DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData) = 0;
};


//class Optimizer_Factory


#endif
