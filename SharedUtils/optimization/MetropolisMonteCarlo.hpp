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
	double METROPOLIS_TEMPERATURE;
	double ADAPTIVE_TEMPERATURE_SCALAR;
	std::deque<bool> lastFewNegativeTrialResults;
	double latestNegativeAcceptancePct;
	
public:
	int numNegTrialsBeforeAdjustingTemperature;
	int maxStepsToConsiderWhenAdjustingTemperature;
	int maxStepsToConsiderForConvergenceCriteria;
	double desiredNegativeAcceptancePercent;
	
	
	MetropolisMonteCarloAdapter() :
		converged(false),
		numNegTrialsBeforeAdjustingTemperature(9),
		maxStepsToConsiderWhenAdjustingTemperature(11),
		desiredNegativeAcceptancePercent(18.0),
		maxStepsToConsiderForConvergenceCriteria(20),
		totalDeltaTrials(0),
		totalDeltaTrialsNegative(0),
		totalDeltaTrialsNegativeAccepted(0),
		METROPOLIS_TEMPERATURE(1.0),
		ADAPTIVE_TEMPERATURE_SCALAR(1.0) {}
	
	MetropolisMonteCarloAdapter(double initial_temp) :
		converged(false),
		numNegTrialsBeforeAdjustingTemperature(9),
		maxStepsToConsiderWhenAdjustingTemperature(11),
		desiredNegativeAcceptancePercent(18.0),
		maxStepsToConsiderForConvergenceCriteria(20),
		totalDeltaTrials(0),
		totalDeltaTrialsNegative(0),
		totalDeltaTrialsNegativeAccepted(0),
		METROPOLIS_TEMPERATURE(initial_temp),
		ADAPTIVE_TEMPERATURE_SCALAR(1.0) {}
	
	bool IsConverged() const {return converged;}
	
	double getTemperature() const {return (METROPOLIS_TEMPERATURE * ADAPTIVE_TEMPERATURE_SCALAR);}
	
	double getPositivePercent() const {
		return 100.0 * ((double)(totalDeltaTrials - totalDeltaTrialsNegative)) / ((double)totalDeltaTrials);
	}
	double getNegativeAcceptancePercent() const {
		return 100.0 * ((double)totalDeltaTrialsNegativeAccepted) / ((double)totalDeltaTrialsNegative);
	}
	
	void UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console);
};


#endif
