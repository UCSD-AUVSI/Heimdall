
-------------------------
In this folder:
-------------------------
	
	Builds a shared library that can be imported in Python
	to access Heimdall's C++ implementation of Kmeans++,
	in which you can give a mask for clustering.
	
	
	Use in Python with the following code:
		
		import pykmeansppcpplib
		RETURNEDTUPLE = pykmeansppcpplib.ClusterKmeansPPwithMask(AAAARRRGS)
		
		# AAAARRRGS are:
		#   filteredCropImage
		#   maskForClustering
		#   k_num_cores
		#   num_lloyd_iterations
		#   num_kmeanspp_iterations
		
		"""
		"num_lloyd_iterations" is the number of KMEANS iterations within each KMEANS step until
					that step is considered converged... good numbers are at least 6 to 10
					higher numbers will (may) produce (slightly) more accurate clusters
	
		"num_kmeanspp_iterations" is the number of times KMEANS is called until we decide
						to stop and pick the result that had the lowest potential...
						should probably do about 15 to 20
						higher numbers will produce more consistent (and slightly better) results
		"""
		
		# RETURNEDTUPLE is:
		#    (DrawnClusters, ClusterColors, ClusterMasks)











