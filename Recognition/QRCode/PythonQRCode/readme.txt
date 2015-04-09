--------------------------------------------------
libraries required to build PythonQRCode:
--------------------------------------------------

OpenCV

Python
Python-OpenCV
Boost-Python (none of the rest of Boost; just Boost-Python)


---------------------------------------------
How to add new PythonQRCode modules:
---------------------------------------------
For example, pretend your module is named "COOLQRC"

1. Create a folder for your Python module in this directory ("Heimdall/Recognition/QRCode/PythonQRCode/COOLQRC")

2. In the COOLQRC folder, create a subfolder called "Python", where you should place your Python code
		There will need to be at least one file named "main.py" with a function named "tryQRCodeRead"

3. Edit the "Heimdall/Recognition/QRCode/PythonQRCode/CMakeLists.txt" file to make sure there is a line that says:
		add_subdirectory(COOLQRC)

4. Create a new "CMakeLists.txt" file in the COOLQRC folder with the following line:
		file(COPY Python DESTINATION ${CMAKE_BINARY_DIR}/Recognition/QRCode/PythonQRCode/COOLQRC)

