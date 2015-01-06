--------------------------------------------------
libraries required to build PythonVerif:
--------------------------------------------------

OpenCV

Python
Python-OpenCV
Boost-Python (none of the rest of Boost; just Boost-Python)


---------------------------------------------
How to add new PythonVerif modules:
---------------------------------------------
For example, pretend your module is named "COOLPVERIF"

1. Create a folder for your Python module in this directory ("Heimdall/Verification/PythonVerif/COOLPVERIF")

2. In the COOLPVERIF folder, create a subfolder called "Python", where you should place your Python code
		There will need to be at least one file named "main.py" with a function named "doVerif"

3. Edit the "Heimdall/Verification/PythonVerif/CMakeLists.txt" file to make sure there is a line that says:
		add_subdirectory(COOLPVERIF)

4. Create a new "CMakeLists.txt" file in the COOLPVERIF folder with the following line:
		file(COPY Python DESTINATION ${CMAKE_BINARY_DIR}/Verification/PythonVerif/COOLPVERIF)

