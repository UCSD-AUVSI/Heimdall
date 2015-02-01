#include "SimulatedAnnealing.hpp"
#include <iostream>
using std::cout; using std::endl;
#include <math.h>
#include "SharedUtils/SharedUtils.hpp"

/*
	"Simulated Annealing": step size is proportional to temperature
	1. Start at high temperature
	2. Keep sampling until the negative-step rate becomes high (perhaps > 75% ?)
	3. This means you're in a generally minimal region
	4. Now decrease the temperature
	5. Goto step 2
	
	Quit when temperature is low enough that the step sizes are tiny enough that
		the results (the fitness score) no longer changes (much) with each step
*/



void SimulatedAnnealing::InitialWarmup(Optimizer_Optimizee * givenModule, Optimizer_Data * givenData)
{
	const int numInitialSamples = 40;
}

void SimulatedAnnealing::DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_Data * givenData)
{
}
