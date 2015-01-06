--------------------------------------------------
libraries required to build PythonColorClassifier:
--------------------------------------------------

OpenCV

Python
Python-OpenCV
Boost-Python (none of the rest of Boost; just Boost-Python)


---------------------------------------------
How to add new PythonColorClassifier modules:
---------------------------------------------
For example, pretend your module is named "COOLCC"

1. Create a folder for your Python module in this directory ("Heimdall/Recognition/ColorClassifier/PythonColorClassifier/COOLCC")

2. In the COOLCC folder, create a subfolder called "Python", where you should place your Python code
		There will need to be at least one file named "main.py" with a function named "doColorClassification"

3. Edit the "Heimdall/Recognition/ColorClassifier/PythonColorClassifier/CMakeLists.txt" file to make sure there is a line that says:
		add_subdirectory(COOLCC)

4. Create a new "CMakeLists.txt" file in the COOLCC folder with the following line:
		file(COPY Python DESTINATION ${CMAKE_BINARY_DIR}/Recognition/ColorClassifier/PythonColorClassifier/COOLCC)

