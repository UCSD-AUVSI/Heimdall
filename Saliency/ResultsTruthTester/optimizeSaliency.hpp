#ifndef __OPTIMIZE_SALIENCY_HPP___
#define __OPTIMIZE_SALIENCY_HPP___

#include "SaliencyExperimentResults.hpp"
#include "SharedUtils/SharedUtils_RNG.hpp"
#include "Saliency/GenericSaliencyImplementation.hpp"
class OptimizingSystemState_Saliency;


double CalculateSaliencyFitnessScore(const SaliencyExperimentResults & results, int allowed_false_positives);


void StartOptimizationLoop(GenericSaliencyImplementation* themodule,
							double initial_guess_for_temperature_beta,
							OptimizingSystemState_Saliency* initial_state,
							OptimizingSystemState_Saliency* later_states,
							std::vector<std::vector<unsigned char>*> & compressed_images,
							std::vector<std::string> & image_original_filenames,
							int maxLoopsBeforeQuitting,
							std::string truth_filename,
							std::string folder_to_save_output_if_desired);


class OptimizingSystemState_Saliency
{
public:
	SaliencyExperimentResults results;
	RNG_rand_r myRNG;
	std::vector<double> OptimizedArgs;
	std::vector<double> OptimizedArgsUpdateSteps;
	double score;
	
	virtual void InitArgs() = 0;
	virtual void ConstrainArgs() = 0;
	
	
	OptimizingSystemState_Saliency() :
		results(SaliencyExperimentResults()),
		myRNG(RNG_rand_r()),
		score(0.0)
		{}
	
	OptimizingSystemState_Saliency(const OptimizingSystemState_Saliency & other) :
		results(other.results),
		myRNG(RNG_rand_r()),
		score(other.score)
		{GetArgsFrom(other);}
	
	void GenerateNewArgs();
	void GetArgsFrom(const OptimizingSystemState_Saliency & other);
};


#endif
