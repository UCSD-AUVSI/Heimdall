#ifndef __METROPOLIS_MONTE_CARLO_HPP___
#define __METROPOLIS_MONTE_CARLO_HPP___

#include <deque>


class AdaptiveMetropolisBeta
{
	int totalDeltaTrials;
	int totalDeltaTrialsNegative;
	int totalDeltaTrialsNegativeAccepted;
	double METROPOLIS_BETA_CONST_MUST_BE_POSITIVE;
	std::deque<bool> lastFewNegativeTrialResults;
	double latestNegativeAcceptancePct;
	
public:
	int numNegTrialsBeforeAdjustingBeta;
	int maxStepsToConsiderWhenAdjustingBeta;
	double desiredNegativeAcceptancePercent;
	
	
	AdaptiveMetropolisBeta() :
		numNegTrialsBeforeAdjustingBeta(9),
		maxStepsToConsiderWhenAdjustingBeta(12),
		desiredNegativeAcceptancePercent(18.0),
		totalDeltaTrials(0),
		totalDeltaTrialsNegative(0),
		totalDeltaTrialsNegativeAccepted(0),
		METROPOLIS_BETA_CONST_MUST_BE_POSITIVE(2.0) {}
	
	AdaptiveMetropolisBeta(double initial_beta) :
		numNegTrialsBeforeAdjustingBeta(9),
		maxStepsToConsiderWhenAdjustingBeta(12),
		desiredNegativeAcceptancePercent(18.0),
		totalDeltaTrials(0),
		totalDeltaTrialsNegative(0),
		totalDeltaTrialsNegativeAccepted(0),
		METROPOLIS_BETA_CONST_MUST_BE_POSITIVE(initial_beta) {}
	
	
	double getPositiveBeta() {return METROPOLIS_BETA_CONST_MUST_BE_POSITIVE;}
	
	double getPositivePercent() {
		return 100.0 * ((double)(totalDeltaTrials - totalDeltaTrialsNegative)) / ((double)totalDeltaTrials);
	}
	double getNegativeAcceptancePercent() {
		return 100.0 * ((double)totalDeltaTrialsNegativeAccepted) / ((double)totalDeltaTrialsNegative);
	}
	
	void UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console);
};


#endif
