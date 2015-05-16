#ifndef ____SIMULATED_ANNEALING_MCMC_HPP_______
#define ____SIMULATED_ANNEALING_MCMC_HPP_______

#include "Optimizer.hpp"
#include <deque>

class SimulatedAnnealing : public Optimizer_MainAlgorithm
{
	/*
		P(dE) = min(1,exp(dE/T)) when we want the maximum E (so dE > 0 steps are desired)
		
		There are two effects affecting the accept rate:
		1. step size -- affects fraction of yes vs. no
		2. temperature -- affects fraction of no's that are accepted
		
		idea: step size and temperature both decrease over time as it converges to global optimum
		
		...negative-accept-rate might stay roughly the same as |dE| decreases with |dstep| ??? but this would likely be nonlinear
				perhaps keep a constant-max-range step width at a given temperature,
					but have probability go like exp(dE/(c*T)) where c adapts to keep negative accept rate around ~40% ?
					hopefully wouldn't have to adapt too much... especially since, what happens at the next temperature drop?
		
		negative-accept-rates:
		25 % means |-dE|/T0 ~ 1.39
		30 % means |-dE|/T0 ~ 1.20
		36.7%means |-dE|/T0 = 1.00
		40 % means |-dE|/T0 ~ 0.91
		50 % means |-dE|/T0 ~ 0.69
		67 % means |-dE|/T0 ~ 0.40
		75 % means |-dE|/T0 ~ 0.29
		80 % means |-dE|/T0 ~ 0.22
		
		Perhaps...
		Pick some (~30-40?) initial sample points completely at random and find the average change in energy <|dE|> between them
		Then start from the best of those initial points,
		with a max-step size of like 50% the space width and an initial temperature T0 ~ (<|dE|>/1.2) for ~30% negative accept rate
		Step size scales down linearly from there, so (T/T0) ~ (dstep/dstep0)
		
		Finally... rate of cooling is another parameter
		How many steps to take before the temperature is dropped?
		S.A. is based on analogy with thermodynamics, so cooling should be slow enough that system is always NEAR EQUILIBRIUM
		i.e. at each temperature decrease step, expect an initial decrease in energy, followed by fluctuations about new equilibrium
		Determining whether system is at equilibrium requires low-pass filter like moving average to track -slope- of dE/dt,
		or looking at the ratio (num +dE)/(num |-dE|) with a number of recent steps and seeing when it is approx. <= 1
	*/
	
	double warmedUpTemperature;
	Optimizer_Params* saved_best_params_from_warmup;
	
public:
	int samples_warmup;
	int max_num_drops;
	
	virtual void InitialWarmup(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData);
	virtual void DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData);
	
	SimulatedAnnealing() : Optimizer_MainAlgorithm(), samples_warmup(50), max_num_drops(50), warmedUpTemperature(1.0), saved_best_params_from_warmup(nullptr) {}
};


/*
	This is a tool used by the MCMC optimizer; this is not the optimizer itself
*/
class SimulatedAnnealingAdapter
{
	double ADAPTIVE_TEMPERATURE_SCALAR;
	std::deque<bool> lastFewNegativeTrialResults;
	double latestNegativeAcceptancePct;
public:
	int numNegTrialsBeforeAdjustingScalar;
	double desiredNegativeAcceptancePercent;
	
	
	SimulatedAnnealingAdapter() :
		numNegTrialsBeforeAdjustingScalar(9),
		desiredNegativeAcceptancePercent(18.0),
		ADAPTIVE_TEMPERATURE_SCALAR(1.0) {}
	
	double getLatestNegAcceptRate() const {return latestNegativeAcceptancePct;}
	double getTemperatureScalar() const {return ADAPTIVE_TEMPERATURE_SCALAR;}
	
	void UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console);
};


#endif
