SHELL = /bin/sh

final_target: build_folder
	g++ -O2 src/main.cpp src/route.cpp src/track_graph.cpp -o build/scheduler

build_folder:
	if [ ! -d "build" ]; then mkdir build; fi
