include("UserPathToQt5Install.config")

cmake_minimum_required(VERSION 2.8.9)

project(JPTest)

# ///////////////////////////////////
# ---- DEVELOPER-EDITED SECTION ----
# ///////////////////////////////////
# Developer: List additional project files here as needed. For those 
# familiar with Qt, this file basically replaces the functionality of
# the Qt project file (.pro)

set (
	JPTest_QT_MODULES 
	# ------------ ADDITIONAL MODULES HERE ------------
	Widgets
)
set (
	JPTest_SOURCES 
	# ------------ ADDITIONAL SOURCES HERE ------------
	src/main.cpp 
	src/jptmainwindow.cpp
)
set (
	JPTest_FORMS 
	# ------------ ADDITIONAL FORMS HERE ------------
	src/jptmainwindow.ui
)
# ///////////////////////////////////

# Set output to bin folder
set(CMAKE_RUNTIME_OUTPUT_DIRECTORY ${CMAKE_BINARY_DIR}/../bin)

# Tell CMake to run moc when necessary:
set(CMAKE_AUTOMOC ON)
set(CMAKE_INCLUDE_CURRENT_DIR ON)
# (since moc files are generated in the binary dir, tell CMake
# to always look for includes there)

# Find widgets package (Widgets finds its own dependencies)
find_package(Qt5Widgets REQUIRED)

# Generate ui_[name].h files from Qt [name].ui files
QT5_WRAP_UI(JPTest_FORMS_HDRS ${JPTest_FORMS})

# Tell CMake to create the JPTest executable
add_executable(JPTest ${JPTest_SOURCES} ${JPTest_FORMS_HDRS})

# Use the Qt modules specified above
QT5_USE_MODULES(JPTest ${JPTest_QT_MODULES})