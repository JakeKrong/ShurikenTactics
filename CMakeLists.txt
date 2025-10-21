# CMakeList.txt : Top-level CMake project file, do global configuration and include sub-projects here.

cmake_minimum_required (VERSION 3.8)
project("ShurikenTactics" VERSION 0.1.0)

#Fetch SFML
include(FetchContent)
FetchContent_Declare(
	sfml
	GIT_REPOSITORY https://github.com/SFML/SFML.git
	GIT_TAG 3.0.0
)
FetchContent_MakeAvailable(sfml)

# Enable Hot Reload for MSVC compilers if supported.
if (POLICY CMP0141)
  cmake_policy(SET CMP0141 NEW)
  set(CMAKE_MSVC_DEBUG_INFORMATION_FORMAT "$<IF:$<AND:$<C_COMPILER_ID:MSVC>,$<CXX_COMPILER_ID:MSVC>>,$<$<CONFIG:Debug,RelWithDebInfo>:EditAndContinue>,$<$<CONFIG:Debug,RelWithDebInfo>:ProgramDatabase>>")
endif()

# Include sub-projects.
add_subdirectory ("ShurikenTactics")


				