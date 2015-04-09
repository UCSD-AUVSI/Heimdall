--------------------------------------------------
libraries required to build PythonOCR:
--------------------------------------------------

OpenCV

Python
Python-OpenCV
Boost-Python (none of the rest of Boost; just Boost-Python)


---------------------------------------------
How to add new PythonOCR modules:
---------------------------------------------
For example, pretend your module is named "COOLOCR"

1. Create a folder for your Python module in this directory ("Heimdall/Recognition/OCR/PythonOCR/COOLOCR")

2. In the COOLOCR folder, create a subfolder called "Python", where you should place your Python code
		There will need to be at least one file named "main.py" with a function named "doOCR"

3. Edit the "Heimdall/Recognition/OCR/PythonOCR/CMakeLists.txt" file to make sure there is a line that says:
		add_subdirectory(COOLOCR)

4. Create a new "CMakeLists.txt" file in the COOLOCR folder with the following line:
		file(COPY Python DESTINATION ${CMAKE_BINARY_DIR}/Recognition/OCR/PythonOCR/COOLOCR)

