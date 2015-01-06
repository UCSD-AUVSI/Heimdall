#ifndef __PYTHON_UTILS_SHARED_HPP___
#define __PYTHON_UTILS_SHARED_HPP___

#include <string>
#include <vector>
#include <boost/python.hpp>


void AddPathToPythonSys(std::string path);


template<class T>
boost::python::list std_vector_to_py_list(const std::vector<T>& v)
{
	boost::python::list l;
	typename std::vector<T>::const_iterator it;
	for (it = v.begin(); it != v.end(); ++it)
		l.append(*it);   
	return l;  
}


bool PrepareForPythonStuff();



/*
	Use when C++ might try to run multiple Python interpreters
*/
struct aquire_py_GIL
{
	PyGILState_STATE state;
	aquire_py_GIL() {
		state = PyGILState_Ensure();
	}

	~aquire_py_GIL() {
		PyGILState_Release(state);
	}
};

/*
	Use this when a Python script needs to call some external C++ utility; put it in the C++ utility function
*/
struct release_py_GIL
{
	PyThreadState *state;
	release_py_GIL() {
		state = PyEval_SaveThread();
	}
	~release_py_GIL() {
		PyEval_RestoreThread(state);
	}
};


#endif
