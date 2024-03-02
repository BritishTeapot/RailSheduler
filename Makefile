SHELL = /bin/sh

final_target: build_folder
	g++ -O2 src/main.cpp src/route.cpp src/track_graph.cpp -o build/scheduler

debug: clean build_folder
	g++ -Wall -Werror -Wextra -pedantic -g src/main.cpp src/route.cpp src/track_graph.cpp -o build/scheduler

build_folder:
	if [ ! -d "build" ]; then mkdir build; fi

clean:
	rm -r build
