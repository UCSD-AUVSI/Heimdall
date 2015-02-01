#ifndef ____SHARED_UTILS_OPTIMIZEABLE_MODULE_H_______
#define ____SHARED_UTILS_OPTIMIZEABLE_MODULE_H_______

#include <iostream>

class Optimizer_ResultsStats; //first declaration, used in _Output


/*----------------------------------------------------
	1. Data holder - e.g. images
*/
class Optimizer_SourceData
{};


/*----------------------------------------------------
	2. Parameters (arguments) for module
*/
class Optimizer_Params
{
public:
	virtual void CopyFromOther(Optimizer_Params const*const other) = 0;
	
	virtual void InitArgs() = 0;
	virtual void GenerateNewArgs(double arg) = 0;
	virtual void Print(std::ostream & printHere) = 0;
};


/*----------------------------------------------------
	3. Output data holder - e.g. cropped images, shape names, colors, etc.
*/
class Optimizer_Optimizee_Output
{
public:
	virtual void clear() = 0;
	
	virtual Optimizer_ResultsStats * CalculateResults() = 0;
	virtual void SaveToDisk() = 0;
};


/*----------------------------------------------------
	4. Results stats holder - how good was the output?
*/
class Optimizer_ResultsStats
{
public:
	virtual void CopyFromOther(Optimizer_ResultsStats const*const other) = 0;
	
	virtual double CalculateFitnessScore() = 0;
	virtual void Print(std::ostream & printHere, bool more_detailed) = 0;
};


/*----------------------------------------------------
	5. the module being optimized (Saliency, Segmentation, etc...)
*/
class Optimizer_Optimizee
{
public:
	virtual Optimizer_Params * CreateNewParams() = 0;
	virtual Optimizer_Optimizee_Output * CreateOutput() = 0;
	virtual Optimizer_ResultsStats * CreateResultsStats() = 0;
	
	virtual void ReceivedUpdatedArgs(Optimizer_Params const*const newArgs) = 0;
	virtual void ProcessData(Optimizer_SourceData * givenData, Optimizer_Optimizee_Output * returnedOutput) = 0;
};


#endif
