#include "MetropolisMonteCarlo.hpp"
#include <iostream>
using std::cout; using std::endl;
#include <math.h>
#include "SharedUtils/SharedUtils.hpp"


void AdaptiveMetropolisBeta::UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console)
{
	totalDeltaTrials++;
	if(new_score_delta < 0.0) {
		totalDeltaTrialsNegative++;
		if(trial_was_accepted) {
			totalDeltaTrialsNegativeAccepted++;
		}
		if(lastFewNegativeTrialResults.size() == maxStepsToConsiderWhenAdjustingBeta) {
			lastFewNegativeTrialResults.pop_front();
		}
		lastFewNegativeTrialResults.push_back(trial_was_accepted);
		
		//update beta?
		if(totalDeltaTrialsNegative >= numNegTrialsBeforeAdjustingBeta) {
			latestNegativeAcceptancePct = 0.0;
			for(int ii=0; ii<lastFewNegativeTrialResults.size(); ii++)
				if(lastFewNegativeTrialResults[ii])
					latestNegativeAcceptancePct += 100.0;
			latestNegativeAcceptancePct /= (double)(lastFewNegativeTrialResults.size());
			double bdelta = 0.8*0.01*(latestNegativeAcceptancePct - desiredNegativeAcceptancePercent);
			METROPOLIS_BETA_CONST_MUST_BE_POSITIVE *= pow(1.0+bdelta, 1.0);
			if(METROPOLIS_BETA_CONST_MUST_BE_POSITIVE < 0.0000000001) {
				METROPOLIS_BETA_CONST_MUST_BE_POSITIVE = 0.0000000001;
			}
		}
	}
	cout<<"neg trials: "<<totalDeltaTrialsNegative<<",  +delta pct: "<<RoundDoubleToInt(getPositivePercent())<<",  recent -delta accept pct: "<<RoundDoubleToInt(latestNegativeAcceptancePct);
	if(new_score_delta < 0.0) {
		cout<<",  metropolis beta: "<<METROPOLIS_BETA_CONST_MUST_BE_POSITIVE;
	}
	cout<<endl;
	
	if(print_more_to_console) {
		if(new_score_delta < 0.0) {
			cout << "NEGATIVE STEP: SCORE DELTA: "<<new_score_delta<<(trial_was_accepted?" ACCEPTED~~~~~~~~~~~~~~********":" REJECTED~~~~~~~~~~~~~~")<<endl;
		} else {
			cout << "POSITIVE STEP: SCORE DELTA: "<<new_score_delta<<(trial_was_accepted?" ACCEPTED":" REJECTED")<<endl;
		}
	}
}
