---------------------------------------------
libraries required to build PythonSaliency:
---------------------------------------------

OpenCV

Python
Python-OpenCV
Boost-Python (none of the rest of Boost; just Boost-Python)


---------------------------------------------
How to add new PythonSaliency modules:
---------------------------------------------
For example, pretend your module is named "COOLSALIENCY"

1. Create a folder for your Python module in this directory ("Heimdall/Saliency/PythonSaliency/COOLSALIENCY")

2. In the COOLSALIENCY folder, create a subfolder called "Python", where you should place your Python code

3. Edit the "Heimdall/Saliency/PythonSaliency/CMakeLists.txt" file to make sure there is a line that says:
		add_subdirectory(COOLSALIENCY)

4. Create a new "CMakeLists.txt" file in the COOLSALIENCY folder with the following line:
		file(COPY Python DESTINATION ${CMAKE_BINARY_DIR}/Saliency/PythonSaliency/COOLSALIENCY)

