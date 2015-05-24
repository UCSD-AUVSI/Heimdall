/*
* SimulatedAnnealing
* Author: Jason Bunk
* Web page: http://sites.google.com/site/jasonbunk
* 
* Copyright (c) 2015 Jason Bunk
* Licensed under the Apache License, Version 2.0 (the "License");
* you may not use this file except in compliance with the License.
* You may obtain a copy of the License at
* http://www.apache.org/licenses/LICENSE-2.0
* 
* Unless required by applicable law or agreed to in writing, software
* distributed under the License is distributed on an "AS IS" BASIS,
* WITHOUT WARRANTIES OR CONDITIONS OF ANY KIND, either express or implied.
* See the License for the specific language governing permissions and
* limitations under the License.
*/
#include "SimulatedAnnealing.hpp"
#include "MetropolisMonteCarlo.hpp"
#include <iostream>
using std::cout; using std::endl;
#include <math.h>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include <thread>
#include <chrono>
#include <deque>
#include <fstream>
/*
	tries to maximize the fitness score
	
	"Simulated Annealing": step size is proportional to temperature
	1. Start at high temperature
	2. Keep sampling until the negative-step rate becomes high (perhaps > 75% ?)
	3. This means you're in a generally minimal region
	4. Now decrease the temperature
	5. Goto step 2
	
	Quit when temperature is low enough that the step sizes are tiny enough that
		the results (the fitness score) no longer changes (much) with each step
*/



void SimulatedAnnealing::InitialWarmup(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData)
{
	cout<<"~~~~~~~~ SimulatedAnnealing::InitialWarmup(), doing "<<samples_warmup<<" iterations"<<endl;
	
	const int numInitialSamples = samples_warmup;
	int thisLoopIterNum = 0;
	
	assert(givenModule != nullptr);
	assert(givenData != nullptr);
	RNG_rand_r myRNG;
	
	const bool MoreConsoleOutput = false;
	
	std::vector<double> testedScores(numInitialSamples);
	std::vector<Optimizer_Params*> testedParams(numInitialSamples, nullptr);
	for(int ii=0; ii<numInitialSamples; ii++) {
		testedParams[ii] = givenModule->CreateNewParams();
		testedParams[ii]->GenerateNewArgs(-1.0, true);
	}
	Optimizer_Optimizee_Output* output = givenModule->CreateOutput();
	Optimizer_ResultsStats* latest_results = givenModule->CreateResultsStats();
	
//--------------------------------------------------------------------------------------------
	
	while((++thisLoopIterNum) <= numInitialSamples) {
		const int ii = (thisLoopIterNum - 1);
		const bool isFirstLoop = (thisLoopIterNum == 1);
		const bool isLastLoop = (thisLoopIterNum == numInitialSamples);
		
		output->clear();
		if(global__key_has_been_pressed || isLastLoop) {
			cout<<endl<<"======================================== "<<thisLoopIterNum<<" experiments done!"<<endl;
		} else {
			cout<<"=============================== starting experiment number "<<thisLoopIterNum<<"  (warmup)"<<endl;
		}
		
		givenModule->ReceivedUpdatedArgs(testedParams[ii]);
		givenModule->ProcessData(givenData, output);
		
		// Check output
		latest_results = output->CalculateResults();
		
		latest_results->Print(cout, false);
		if(isFirstLoop) {
			latest_results->Print(cout, true);
		}
		testedScores[ii] = latest_results->CalculateFitnessScore();
		
		cout<<"THESE PARAMS: ";
		testedParams[ii]->Print(cout);
		cout<<endl<<"THIS SCORE: "<<testedScores[ii]<<endl;
		
		std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	double highestScoreSeen = -1e12;
	int highestScoreIdx = -1;
	for(int ii=0; ii<numInitialSamples; ii++) {
		if(testedScores[ii] > highestScoreSeen) {
			highestScoreSeen = testedScores[ii];
			highestScoreIdx = ii;
		}
	}
	for(int ii=0; ii<numInitialSamples; ii++) {
		if(ii == highestScoreIdx) {
			saved_best_params_from_warmup = testedParams[ii];
		} else {
			delete testedParams[ii];
		}
	}
	cout<<"highest score seen in the warmup: "<<highestScoreSeen<<endl;
	saved_best_params_from_warmup->Print(cout);
	testedParams.clear();
	
	cout<<"changes in energy:"<<endl;
	cout<<"======================================================================"<<endl;
	
	double averageChangesInEnergy = 0.0;
	double numPtsDifferences = 0.0;
	for(int ii=1; ii<numInitialSamples; ii++) {
		for(int jj=0; jj<ii; jj++) {
			numPtsDifferences += 1.0;
			averageChangesInEnergy += fabs(testedScores[ii] - testedScores[jj]);
			
			cout<<fabs(testedScores[ii] - testedScores[jj])<<endl;
		}
	}
	averageChangesInEnergy /= numPtsDifferences;
	warmedUpTemperature = (averageChangesInEnergy / 1.9); //aim for a negative step acceptance of about 25 %
	
	cout<<"======================================================================"<<endl;
	cout<<"averageChangesInEnergy == "<<averageChangesInEnergy<<endl;
	cout<<"warmedUpTemperature == "<<warmedUpTemperature<<endl;
}



void SimulatedAnnealing::DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData)
{
	cout<<"~~~~~~~~ SimulatedAnnealing::DoPostWarmupLoops()"<<endl;
	
	double StepScalar = 0.45; //to start with, can cover up to 45% of the space in one step, i.e. step size +/- 22.5%
	const double StepReductionAmountPerDrop = 0.75; //the step size at any level is: StepScalar*(StepReductionAmountPerDrop^(num drops))
												//thus this affects the cooling rate
	const double OriginalStepScalar = StepScalar; //to find proportional temperature drop
	double CurrentTemperature = warmedUpTemperature;
	
	assert(givenModule != nullptr);
	assert(givenData != nullptr);
	EnableKeyPressToExit(false);
	int maxLoopsBeforeQuitting = 200000000; //use convergence criteria (i.e. user keypress), not num loops
	RNG_rand_r myRNG;
	
	std::ofstream all_scores_history_file;
	int testfilenameidx = 0;
	while(check_if_file_exists(std::string("simulated_annealing_history_")+to_istring(testfilenameidx)+std::string(".txt")) == true) {
        testfilenameidx++;
    }
    all_scores_history_file.open(std::string("simulated_annealing_history_")+to_istring(testfilenameidx)+std::string(".txt"));
	
	const bool MoreConsoleOutput = false;
	bool firstLoop = true;
	bool thisIsTheLastLoop = false;
	int numLoopsDone = 0;
	int numDrops = 0;
	
	const int num_best_seen_results = 100;
	std::vector<double> best_seen_scores;
	std::vector<Optimizer_Params*> best_seen_params;
	std::vector<Optimizer_ResultsStats*> best_seen_results;
	
	const int thisdrop_num_top_results_to_consider = 3;
	std::vector<double> thisdrop_top_scores;
	std::vector<Optimizer_Params*> thisdrop_top_params;
	std::vector<Optimizer_ResultsStats*> thisdrop_top_results;
	
	Optimizer_Params* previous_params = givenModule->CreateNewParams();
	Optimizer_Params* latest_params = saved_best_params_from_warmup;
	double previous_score, latest_score, scoreDelta;
	
	Optimizer_Optimizee_Output* output = givenModule->CreateOutput();
	Optimizer_ResultsStats* latest_results = givenModule->CreateResultsStats();
	
	//keep track of ratio +dE vs. -dE to see if we are approximately at equilibrium
	//immediately after a step change, there should be more +dE than -dE
	const int num_recent_scoredeltas_to_consider = 47;
	std::deque<double> recent_scoredeltas;
	
	//this helps ensure that the negative acceptance rate remains reasonable,
	//since we might not be able to assume that the step size (temperature) is directly proportional to average energy change <|dE|>
	//in fact we can just leave this as the probability temperature, and let it adapt rather than make assumptions about proportionality
	SimulatedAnnealingAdapter negtrials_adjuster;
	negtrials_adjuster.desiredNegativeAcceptancePercent = 25.0;
	negtrials_adjuster.numNegTrialsBeforeAdjustingScalar = 18;
//--------------------------------------------------------------------------------------------
	
	
	while((--maxLoopsBeforeQuitting) >= 0) {
		output->clear();
		if(global__key_has_been_pressed || maxLoopsBeforeQuitting == 0) {
			cout<<endl<<"======================================== "<<numLoopsDone<<" experiments done!"<<endl;
			cout      <<"                      calculating results with final parameters..."<<endl;
			maxLoopsBeforeQuitting = 0;
			thisIsTheLastLoop = true;
		} else {
			cout<<"=============================== starting experiment number "<<(++numLoopsDone)<<", so far there have been "<<numDrops<<" drops"<<endl;
			if(firstLoop == false) {
				previous_params->CopyFromOther(latest_params);
				
				double stepwidthcalculated = StepScalar;
				stepwidthcalculated *= pow(StepReductionAmountPerDrop, ((double)numDrops));
				CurrentTemperature = warmedUpTemperature * (stepwidthcalculated / OriginalStepScalar); //proportional drop in temperature
				latest_params->GenerateNewArgs(stepwidthcalculated, true);
			}
		}
		
		givenModule->ReceivedUpdatedArgs(latest_params);
		givenModule->ProcessData(givenData, output);
		
		// Check output
		latest_results = output->CalculateResults();
		previous_score = latest_score;
		latest_score = latest_results->CalculateFitnessScore();
		
		//print to history file
		all_scores_history_file<<"score: "<<latest_score<<", numLoopsDone: "<<numLoopsDone<<", numDrops: "<<numDrops<<", tempscalar: "<<negtrials_adjuster.getTemperatureScalar()<<", recentNegAcceptRate: "<<negtrials_adjuster.getLatestNegAcceptRate()<<endl;
		latest_results->Print(all_scores_history_file, false);
		latest_params->Print(all_scores_history_file);
		//print to console
		latest_results->Print(cout, false);
		cout << "THIS SCORE: "<<latest_score<<endl;
		
		//---------------------------------------------------
        if(thisdrop_top_params.size() == thisdrop_num_top_results_to_consider) {
            double worstscore = 1000000.0;
            int worstscoreidx = -1;
            for(int ii=0; ii<thisdrop_num_top_results_to_consider; ii++) {
                if(thisdrop_top_scores[ii] < worstscore) {
                    worstscore = thisdrop_top_scores[ii];
                    worstscoreidx = ii;
                }
            }
            if(latest_score > worstscore) {
                delete thisdrop_top_params[worstscoreidx];
                delete thisdrop_top_results[worstscoreidx];
                thisdrop_top_params.erase(thisdrop_top_params.begin() + worstscoreidx);
                thisdrop_top_results.erase(thisdrop_top_results.begin() + worstscoreidx);
                thisdrop_top_scores.erase(thisdrop_top_scores.begin() + worstscoreidx);
            }
        }
		if(thisdrop_top_scores.size() < thisdrop_num_top_results_to_consider) {
            thisdrop_top_params.push_back(givenModule->CreateNewParams());
            thisdrop_top_params.back()->CopyFromOther(latest_params);
            thisdrop_top_results.push_back(givenModule->CreateResultsStats());
            thisdrop_top_results.back()->CopyFromOther(latest_results);
            thisdrop_top_scores.push_back(latest_score);
		}
        if(best_seen_params.size() == num_best_seen_results) {
            double worstscore = 1000000.0;
            int worstscoreidx = -1;
            for(int ii=0; ii<num_best_seen_results; ii++) {
                if(best_seen_scores[ii] < worstscore) {
                    worstscore = best_seen_scores[ii];
                    worstscoreidx = ii;
                }
            }
            if(latest_score > worstscore) {
                delete best_seen_params[worstscoreidx];
                delete best_seen_results[worstscoreidx];
                best_seen_params.erase(best_seen_params.begin() + worstscoreidx);
                best_seen_results.erase(best_seen_results.begin() + worstscoreidx);
                best_seen_scores.erase(best_seen_scores.begin() + worstscoreidx);
            }
        }
        if(best_seen_params.size() < num_best_seen_results) {
            best_seen_params.push_back(givenModule->CreateNewParams());
            best_seen_params.back()->CopyFromOther(latest_params);
            best_seen_results.push_back(givenModule->CreateResultsStats());
            best_seen_results.back()->CopyFromOther(latest_results);
            best_seen_scores.push_back(latest_score);
        }
		//---------------------------------------------------
		
		if(firstLoop) {
			firstLoop = false;
		} else {
			scoreDelta = (latest_score - previous_score);
			
			//check if step was accepted (probabilistic)
			bool accepted = ((scoreDelta >= 0.0) || myRNG.rand_double(0.0,1.0) < exp(scoreDelta/(CurrentTemperature*negtrials_adjuster.getTemperatureScalar())));
			
			//if reject, step back; otherwise keep
			if(accepted == false) {
				latest_params->CopyFromOther(previous_params);
			}
			
			//adapt negative acceptance rate to be reasonable
			negtrials_adjuster.UpdateTrials(scoreDelta, accepted, MoreConsoleOutput);
			
			//check if we should drop the step size
			if(recent_scoredeltas.size() >= num_recent_scoredeltas_to_consider) {
				recent_scoredeltas.pop_front();
			}
			recent_scoredeltas.push_back(scoreDelta);
			if(recent_scoredeltas.size() >= num_recent_scoredeltas_to_consider) {
				int numPositiveDeltaScores = 0;
				int numNegativeDeltaScores = 0;
				for(int ii=0; ii<num_recent_scoredeltas_to_consider; ii++) {
					if(recent_scoredeltas[ii] > 0.0) {
						numPositiveDeltaScores++;
					} else if(recent_scoredeltas[ii] < 0.0) {
						numNegativeDeltaScores++;
					}
				}
				cout<<"numPositiveDeltaScores == "<<numPositiveDeltaScores<<", numNegativeDeltaScores == "<<numNegativeDeltaScores<<endl;
				if(numNegativeDeltaScores >= numPositiveDeltaScores) {
					cout<<"DROPPING THE STEP SIZE AND TEMPERATURE LEVEL"<<endl;
					numDrops++;
					recent_scoredeltas.clear(); //force it to do a bunch more iterations before considering a drop again
					int randomScoreToRewindTo = myRNG.rand_int(0, thisdrop_num_top_results_to_consider - 1);
					latest_score = thisdrop_top_scores[randomScoreToRewindTo];
					latest_params->CopyFromOther(thisdrop_top_params[randomScoreToRewindTo]);
					latest_results->CopyFromOther(thisdrop_top_results[randomScoreToRewindTo]);
					for(int ii=0; ii<thisdrop_num_top_results_to_consider; ii++) {
                        delete thisdrop_top_params[ii];
                        delete thisdrop_top_results[ii];
					}
					thisdrop_top_scores.clear();
					thisdrop_top_params.clear();
					thisdrop_top_results.clear();
					if(numDrops >= max_num_drops) {
                        maxLoopsBeforeQuitting = 1; //force the end of the optimization at the next run
					}
				}
			}
		}
		
		if(thisIsTheLastLoop) {
			cout<<"------------------------------"<<endl;
			
			std::ofstream outf;
			outf.open(std::string("simulated_annealing_results_")+to_istring(testfilenameidx)+std::string(".txt"));
			for(int ii=0; ii<best_seen_params.size(); ii++) {
				outf<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
				outf<<"score: "<<best_seen_scores[ii]<<", ";
				best_seen_results[ii]->Print(outf, false);
				outf<<endl<<"params: ";
				best_seen_params[ii]->Print(outf);
				outf<<endl<<"results: ";
				best_seen_results[ii]->Print(outf, true);
				outf<<endl;
			}
			outf.close();
			
			//output->SaveToDisk(); //save most recent crop results
		}
		
		cout<< (thisIsTheLastLoop ? "FINAL PARAMETERS: " : "PARAMS SO FAR: ");
		latest_params->Print(cout);
		cout<<endl;
		
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	all_scores_history_file.close();
}






void SimulatedAnnealingAdapter::UpdateTrials(double new_score_delta, bool trial_was_accepted, bool print_more_to_console)
{
	if(new_score_delta < 0.0) {
		//save latest results
		lastFewNegativeTrialResults.push_back(trial_was_accepted);
		
		if((int)lastFewNegativeTrialResults.size() >= numNegTrialsBeforeAdjustingScalar) {
			//update scalar?
			latestNegativeAcceptancePct = 0.0;
			for(int ii=0; ii<lastFewNegativeTrialResults.size(); ii++)
				if(lastFewNegativeTrialResults[ii])
					latestNegativeAcceptancePct += 100.0;
			latestNegativeAcceptancePct /= ((double)lastFewNegativeTrialResults.size());
			double bdelta = 0.01*(latestNegativeAcceptancePct - desiredNegativeAcceptancePercent);
			ADAPTIVE_TEMPERATURE_SCALAR /= pow(1.0+bdelta,0.9);
			if (ADAPTIVE_TEMPERATURE_SCALAR < 0.00000000001) {
				ADAPTIVE_TEMPERATURE_SCALAR = 0.00000000001;
			}
			//this reduces the overshooting effect of the controller
			int numToPop = (numNegTrialsBeforeAdjustingScalar / 2);
			for(int ii=0; ii<numToPop; ii++) {
                lastFewNegativeTrialResults.pop_front();
			}
        }
	}
	cout<<"recent -delta accept pct: "<<RoundDoubleToInt(latestNegativeAcceptancePct);
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




