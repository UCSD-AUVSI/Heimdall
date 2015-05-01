#ifndef __METROPOLIS_MONTE_CARLO_HPP___
#define __METROPOLIS_MONTE_CARLO_HPP___

#include <deque>
#include "Optimizer.hpp"


/*
	Seek global optimum stochastically using Markov-Chain-Monte-Carlo (MCMC)
*/
class Optimizer_MCMC : public Optimizer_MainAlgorithm
{
protected:
	double saved_latest_temperature;
public:
	Optimizer_MCMC() : saved_latest_temperature(-1.0) {}
	
	virtual void InitialWarmup(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData);
	virtual void DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData);
};


/*
	This is a tool used by the MCMC optimizer; this is not the optimizer itself
*/
class MetropolisMonteCarloAdapter
{
	bool converged;
	int totalDeltaTrials;
	int totalDeltaTrialsNegative;
	int totalDeltaTrialsNegativeAccepted;
	double ADAPTIVE_TEMPERATURE_SCALAR;
	std::deque<bool> lastFewNegativeTrialResults;
	double latestNegativeAcceptancePct;
	
public:
	int numNegTrialsBeforeAdjustingScalar;
	int maxStepsToConsiderWhenAdjustingScalar;
	int maxStepsToConsiderForConvergenceCriteria;
	double desiredNegativeAcceptancePercent;
	
	
	MetropolisMonteCarloAdapter() :
		converged(false),
		numNegTrialsBeforeAdjustingScalar(9),
		maxStepsToConsiderWhenAdjustingScalar(11),
		desiredNegativeAcceptancePercent(18.0),
		maxStepsToConsiderForConvergenceCriteria(20),
		totalDeltaTrials(0),
		totalDeltaTrialsNegative(0),
		totalDeltaTrialsNegativeAccepted(0),
		ADAPTIVE_TEMPERATURE_SCALAR(1.0) {}
	
	bool IsConverged() const {return converged;}
	
	double getTemperatureScalar() const {return ADAPTIVE_TEMPERATURE_SCALAR;}
	
	double getPositivePercent() const {
		return 100.0 * ((double)(totalDeltaTrials - totalDeltaTrialsNegative)) / ((double)totalDeltaTrials);
	}
	double getNegativeAcceptancePercent() const {
		return 100.0 * ((double)totalDeltaTrialsNegativeAccepted) / ((double)totalDeltaTrialsNegative);
	}
	
	void UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console);
};


#endif
