# CppND-System-Monitor

Completed code for System Monitor Project in the Object Oriented Programming Course of the [Udacity C++ Nanodegree Program](https://www.udacity.com/course/c-plus-plus-nanodegree--nd213). 


![System Monitor](images/monitor.png)


## ncurses
[ncurses](https://www.gnu.org/software/ncurses/) is a library that facilitates text-based graphical output in the terminal. This project relies on ncurses for display output.

install ncurses within your own Linux environment: `sudo apt install libncurses5-dev libncursesw5-dev`

## Make
This project uses [Make](https://www.gnu.org/software/make/). The Makefile has four targets:
* `build` compiles the source code and generates an executable
* `format` applies [ClangFormat](https://clang.llvm.org/docs/ClangFormat.html) to style the source code
* `debug` compiles the source code and generates an executable, including debugging symbols
* `clean` deletes the `build/` directory, including all of the build artifacts

## Instructions

1. Clone the entire repo as explained [HERE](https://github.com/eugen-schaefer/UdacityNanoDegree-CPP/blob/master/README.md)
2. change directory to UdacityNanoDegree-CPP/SystemMonitor/
3. Make a build directory in SystemMonitor: `mkdir build && cd build`
4. Compile: `cmake .. && make`
3. Run the resulting executable: `./monitor`
![Starting System Monitor](images/starting_monitor.png)
