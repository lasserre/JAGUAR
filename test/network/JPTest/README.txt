JPTest Project README
Caleb Stewart
1/16/14
----------------------

Directions:
1. Create file named UserPathToQt5Install.config 
2. Copy following 2 lines into the file:
	# Set path
	set (CMAKE_PREFIX_PATH "/Users/caleb/Qt5.0.0/5.0.0/clang_64")
3. Modify "/Users/caleb/..." to reflect the path to a Qt5 install on your machine
4. Build sublime project with "JPTest - cmake" system selected to generate makefile
5. Build with "JPTest - make" to generate target
6. Thereafter, if sources or qt forms are added, modify CMakeLists.txt file accordingly 
(per instructions in the file). Cmake will need to be run again, followed by make. 
If no new files are added, only make needs to be run.