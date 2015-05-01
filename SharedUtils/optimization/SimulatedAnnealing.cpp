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
			cout<<"=============================== starting experiment number "<<thisLoopIterNum<<endl;
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
	if(thisLoopIterNum > numInitialSamples){thisLoopIterNum = numInitialSamples;}
	
	double highestScoreSeen = -1e12;
	int highestScoreIdx = -1;
	for(int ii=0; ii<thisLoopIterNum; ii++) {
		if(testedScores[ii] > highestScoreSeen) {
			highestScoreSeen = testedScores[ii];
			highestScoreIdx = ii;
		}
	}
	
	cout<<"changes in energy:"<<endl;
	cout<<"======================================================================"<<endl;
	
	double averageChangesInEnergy = 0.0;
	double numPtsDifferences = 0.0;
	for(int ii=1; ii<thisLoopIterNum; ii++) {
		for(int jj=0; jj<ii; jj++) {
			numPtsDifferences += 1.0;
			averageChangesInEnergy += fabs(testedScores[ii] - testedScores[jj]);
			
			cout<<fabs(testedScores[ii] - testedScores[jj])<<endl;
		}
	}
	averageChangesInEnergy /= numPtsDifferences;
	warmedUpTemperature = (averageChangesInEnergy / 1.2); //negative step acceptance of about 30 %
	
	cout<<"======================================================================"<<endl;
	cout<<"averageChangesInEnergy == "<<averageChangesInEnergy<<endl;
	cout<<"warmedUpTemperature == "<<warmedUpTemperature<<endl;
}



void SimulatedAnnealing::DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData)
{
	cout<<"~~~~~~~~ SimulatedAnnealing::DoPostWarmupLoops()"<<endl;
	
	double StepScalar = 0.50; //to start with, can cover up to 50% of the space in one step, i.e. step size +/- 25%
	double StepReductionAmountPerDrop = 0.75; //the step size at any level is: StepScalar*(StepReductionAmountPerDrop^(num drops))
												//thus this affects the cooling rate
	
	assert(givenModule != nullptr);
	assert(givenData != nullptr);
	EnableKeyPressToExit(false);
	int maxLoopsBeforeQuitting = 200000000; //use convergence criteria (i.e. user keypress), not num loops
	RNG_rand_r myRNG;
	
	std::ofstream all_scores_history_file;
	all_scores_history_file.open("simulated_annealing_history.txt");
	
	const bool MoreConsoleOutput = false;
	bool firstLoop = true;
	bool thisIsTheLastLoop = false;
	int numLoopsDone = 0;
	int numDrops = 0;
	
	const int num_best_seen_results = 100;
	std::vector<double> best_seen_scores;
	std::vector<Optimizer_Params*> best_seen_params;
	std::vector<Optimizer_ResultsStats*> best_seen_results;
	
	Optimizer_Params* previous_params = givenModule->CreateNewParams();
	Optimizer_Params* latest_params = givenModule->CreateNewParams();
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
	MetropolisMonteCarloAdapter negtrials_adjuster;
	negtrials_adjuster.desiredNegativeAcceptancePercent = 30.0;
	negtrials_adjuster.numNegTrialsBeforeAdjustingScalar = 20;
	negtrials_adjuster.maxStepsToConsiderWhenAdjustingScalar = 20;
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
				latest_params->GenerateNewArgs(stepwidthcalculated, true);
			}
		}
		
		givenModule->ReceivedUpdatedArgs(latest_params);
		givenModule->ProcessData(givenData, output);
		
		// Check output
		latest_results = output->CalculateResults();
		
		//print to history file
		all_scores_history_file<<"score: "<<latest_score<<", numLoopsDone: "<<numLoopsDone<<", numDrops: "<<numDrops<<", tempscalar: "<<negtrials_adjuster.getTemperatureScalar()<<endl;
		latest_results->Print(all_scores_history_file, false);
		//print to console
		latest_results->Print(cout, false);
		previous_score = latest_score;
		latest_score = latest_results->CalculateFitnessScore();
		cout << "THIS SCORE: "<<latest_score<<endl;
		
		if(firstLoop) {
			firstLoop = false;
		} else {
			scoreDelta = (latest_score - previous_score);
			
			//check if step was accepted (probabilistic)
			bool accepted = ((scoreDelta >= 0.0) || myRNG.rand_double(0.0,1.0) < exp(scoreDelta/(warmedUpTemperature*negtrials_adjuster.getTemperatureScalar())));
			
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
				}
			}
			
			if(best_seen_params.size() == num_best_seen_results) {
				double worstscore = 100000.0;
				int worstscoreidx = 0;
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
		}
		
		if(thisIsTheLastLoop) {
			cout<<"------------------------------"<<endl;
			
			std::ofstream outf;
			outf.open("simulated_annealing_results.txt");
			for(int ii=0; ii<best_seen_params.size(); ii++) {
				outf<<"~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~~"<<endl;
				outf<<"score: "<<best_seen_scores[ii]<<endl;
				outf<<"params: ";
				best_seen_params[ii]->Print(outf);
				outf<<endl<<"results: ";
				best_seen_results[ii]->Print(outf, true);
				outf<<endl;
			}
			outf.close();
			
			output->SaveToDisk(); //save most recent results
		}
		
		cout<< (thisIsTheLastLoop ? "FINAL PARAMETERS: " : "PARAMS SO FAR: ");
		latest_params->Print(cout);
		cout<<endl;
		
        std::this_thread::sleep_for(std::chrono::milliseconds(1));
	}
	
	all_scores_history_file.close();
}






