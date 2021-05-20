project(test)
cmake_minimum_required(VERSION 3.16)

set( EXECUTABLE_OUTPUT_PATH ../bin)

link_libraries(-lstdshit)



add_executable(dfprof
	process.cc
	profiler.cc
	proflog.cc
)


