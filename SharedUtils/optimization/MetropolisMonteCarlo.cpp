#include "MetropolisMonteCarlo.hpp"
#include <iostream>
using std::cout; using std::endl;
#include <math.h>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include <thread>
#include <chrono>

/*
	Tries to maximize the fitness score
*/


void Optimizer_MCMC::InitialWarmup(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData)
{
	cout<<"~~~~~~~~ Optimizer_MCMC::InitialWarmup()"<<endl;
	
	assert(givenModule != nullptr);
	assert(givenData != nullptr);
	EnableKeyPressToExit(false);
	int maxLoopsBeforeQuitting = 200000000; //use convergence criteria, not num loops
	RNG_rand_r myRNG;
	
	const bool MoreConsoleOutput = false;
	bool firstLoop = true;
	bool thisIsTheLastLoop = false;
	double firstScoreSaved = 0.0;
	int numLoops = 0;
	
	Optimizer_Params* last_best_params = givenModule->CreateNewParams();
	Optimizer_Params* previous_params = givenModule->CreateNewParams();
	Optimizer_Params* latest_params = givenModule->CreateNewParams();
	
	double previous_score, latest_score, last_best_score, scoreDelta;
	
	Optimizer_ResultsStats* previous_results = givenModule->CreateResultsStats();
	Optimizer_ResultsStats* latest_results = givenModule->CreateResultsStats();
	Optimizer_ResultsStats* last_best_results = givenModule->CreateResultsStats();
	
	Optimizer_Optimizee_Output* output = givenModule->CreateOutput();
	
	MetropolisMonteCarloAdapter negtrials_adjuster;
	negtrials_adjuster.desiredNegativeAcceptancePercent = 33.3;
	negtrials_adjuster.numNegTrialsBeforeAdjustingScalar = 6;
	negtrials_adjuster.maxStepsToConsiderWhenAdjustingScalar = 10;
	negtrials_adjuster.maxStepsToConsiderForConvergenceCriteria = 30;
//--------------------------------------------------------------------------------------------
	
	
	while((--maxLoopsBeforeQuitting) >= 0) {
		output->clear();
		if(global__key_has_been_pressed || maxLoopsBeforeQuitting == 0) {
			cout<<endl<<"======================================== "<<numLoops<<" experiments done!"<<endl;
			cout      <<"                      calculating results with final parameters..."<<endl;
			maxLoopsBeforeQuitting = 0;
			thisIsTheLastLoop = true;
		} else {
			cout<<"=============================== starting experiment number "<<(++numLoops)<<endl;
			if(firstLoop == false) {
				previous_params->CopyFromOther(latest_params);
				latest_params->GenerateNewArgs(-1.0, false);
										//step size is fixed to what was suggested by the algorithm
										//use Simulated Annealing if you want an adaptive step size
			}
		}
		
		givenModule->ReceivedUpdatedArgs(latest_params);
		
		givenModule->ProcessData(givenData, output);
		
		// Check output
		previous_results->CopyFromOther(latest_results);
		latest_results = output->CalculateResults();
		
		latest_results->Print(cout, false);
		
		if(firstLoop) {
			latest_results->Print(cout, true);
		}
		
		previous_score = latest_score;
		latest_score = latest_results->CalculateFitnessScore();
		
		cout << "THIS SCORE: "<<latest_score<<endl;
		
		if(firstLoop) {
			firstLoop = false;
			last_best_score = firstScoreSaved = latest_score;
		} else {
			scoreDelta = (latest_score - previous_score);
			
			//for this MCMC the temperature is entirely determined by the adapter, while the step size is fixed
			bool accepted = ((scoreDelta >= 0.0) || myRNG.rand_double(0.0,1.0) < exp(scoreDelta/negtrials_adjuster.getTemperatureScalar()));
			
			negtrials_adjuster.UpdateTrials(scoreDelta, accepted, MoreConsoleOutput);
			
			if(accepted == false) {
				latest_params->CopyFromOther(previous_params);
			} else {
				last_best_results->CopyFromOther(latest_results);
				last_best_params->CopyFromOther(latest_params);
				last_best_score = latest_score;
			}
			
			if(negtrials_adjuster.IsConverged()) {
				maxLoopsBeforeQuitting = 1; //very next loop will be the last!
			}
		}
		
		if(thisIsTheLastLoop) {
			scoreDelta = (latest_score - firstScoreSaved);
			cout<<"------------------------------"<<endl;
			last_best_results->Print(cout, true);
			cout<<"ORIGINAL PARAMETERS' SCORE: "<<firstScoreSaved<<endl;
			cout<<"FINAL PARAMETERS' SCORE:    "<<last_best_score<<endl;
			cout<<"DELTA SCORE (FINAL-ORIGINAL) == "<<scoreDelta<<endl;
			output->SaveToDisk();
			latest_params->CopyFromOther(last_best_params); //just for this last print
		}
		
		cout<< (thisIsTheLastLoop ? "FINAL PARAMETERS: " : "PARAMS SO FAR: ");
		latest_params->Print(cout);
		cout<<endl;
		
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
}

void Optimizer_MCMC::DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData)
{
	cout << "Optimizer_MCMC::DoPostWarmupLoops() -- TODO:\n\"adaptive_temp_scalar\" should have converged, so take samples" << endl;
}

void MetropolisMonteCarloAdapter::UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console)
{
	/*
		To use this as a MCMC method:
		Run until the AdaptiveTemperature converges at the desired "negative-accept-rate",
			which signifies the end of the "warmup" phase.
		Then start sampling, and keep sampling until you quit...
		Once you quit, average all saved samples,
		because (if it warmed up properly) the MCMC should have been fluctuating around the minimum.
		
		SimulatedAnnealing is (probably) an improvement upon this algorithm...
			it should use this at each stage after dropping the temperature
	*/
	
	
	totalDeltaTrials++;
	if(new_score_delta < 0.0) {
		//save latest results
		totalDeltaTrialsNegative++;
		if(trial_was_accepted) {
			totalDeltaTrialsNegativeAccepted++;
		}
		if(lastFewNegativeTrialResults.size() == maxStepsToConsiderForConvergenceCriteria) {
			lastFewNegativeTrialResults.pop_front();
		}
		lastFewNegativeTrialResults.push_back(trial_was_accepted);
		
		if(totalDeltaTrialsNegative >= numNegTrialsBeforeAdjustingScalar) {
			//update scalar?
			latestNegativeAcceptancePct = 0.0;
			int loopmax = MIN(lastFewNegativeTrialResults.size(), maxStepsToConsiderWhenAdjustingScalar);
			for(int ii=0; ii<loopmax; ii++)
				if(lastFewNegativeTrialResults[ii])
					latestNegativeAcceptancePct += 100.0;
			latestNegativeAcceptancePct /= (double)loopmax;
			double bdelta = 0.8*0.01*(latestNegativeAcceptancePct - desiredNegativeAcceptancePercent);
			ADAPTIVE_TEMPERATURE_SCALAR /= (1.0+bdelta);//pow(1.0+bdelta,1.0);
			if (ADAPTIVE_TEMPERATURE_SCALAR < 0.00000000001) {
				ADAPTIVE_TEMPERATURE_SCALAR = 0.00000000001;
			}
			//converged (stable)?
			double convergenceNegAccPct = 0.0;
			loopmax = lastFewNegativeTrialResults.size();
			if(loopmax == maxStepsToConsiderForConvergenceCriteria) {
				for(int ii=0; ii<loopmax; ii++)
					if(lastFewNegativeTrialResults[ii])
						convergenceNegAccPct += 100.0;
				convergenceNegAccPct /= (double)loopmax;
				//how close you can get to the desired % depends on the precision, i.e. how many steps are considered
				if(fabs(convergenceNegAccPct - desiredNegativeAcceptancePercent) < (0.000000001+50.0/((double)maxStepsToConsiderForConvergenceCriteria))) {
					cout<<"ADAPTIVE-MCMC HAS CONVERGED TO BEST PROBABILITY-TEMPERATURE SCALAR"<<endl;
					converged = true;
				}
			}
		} else {
			converged = false;
		}
	}
	cout<<"neg trials: "<<totalDeltaTrialsNegative<<",  +delta pct: "<<RoundDoubleToInt(getPositivePercent())<<",  recent -delta accept pct: "<<RoundDoubleToInt(latestNegativeAcceptancePct);
	if(new_score_delta < 0.0) {
		cout<<",  adaptive temperature scalar: "<<ADAPTIVE_TEMPERATURE_SCALAR;
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

