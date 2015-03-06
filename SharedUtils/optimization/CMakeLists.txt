set(optimization_experiments_source_files
	experimentUtils.cpp
	experimentUtils.hpp
	MetropolisMonteCarlo.cpp
	MetropolisMonteCarlo.hpp
	SimulatedAnnealing.cpp
	SimulatedAnnealing.hpp
	)

add_library(optimization ${optimization_experiments_source_files})

target_link_libraries(optimization sharedutils)

