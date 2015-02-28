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
#include <iostream>
using std::cout; using std::endl;
#include <math.h>
#include "SharedUtils/SharedUtils.hpp"
#include "SharedUtils/EnableKeyPressToExit.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include <thread>
#include <chrono>
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
	cout<<"~~~~~~~~ SimulatedAnnealing::InitialWarmup()"<<endl;
	
	const int numInitialSamples = 50;
	int thisLoopIterNum = 0;
	
	assert(givenModule != nullptr);
	assert(givenData != nullptr);
	EnableKeyPressToExit(false);
	RNG_rand_r myRNG;
	
	const bool MoreConsoleOutput = false;
	
	std::vector<double> testedScores(numInitialSamples);
	std::vector<Optimizer_Params*> testedParams(numInitialSamples, nullptr);
	for(int ii=0; ii<numInitialSamples; ii++) {
		testedParams[ii] = givenModule->CreateNewParams();
		testedParams[ii]->GenerateNewArgs(-1.0, true);
	}
	Optimizer_ResultsStats* latest_results = givenModule->CreateResultsStats();
	Optimizer_Optimizee_Output* output = givenModule->CreateOutput();
	
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
	
	cout<<"======================================================================"<<endl;
	cout<<"averageChangesInEnergy == "<<averageChangesInEnergy<<endl;
	
	warmedUpTemperature = (averageChangesInEnergy / 0.7);
}



void SimulatedAnnealing::DoPostWarmupLoops(Optimizer_Optimizee * givenModule, Optimizer_SourceData * givenData)
{
	cout<<"~~~~~~~~ SimulatedAnnealing::DoPostWarmupLoops()"<<endl;
	
	cout<<"todo"<<endl;
}






