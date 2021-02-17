CXX = g++      # name of the compiler
CXXFLAGS = -std=c++14 -Wall -MMD    # compiler options
EXEC = vm -lncurses    # name of the executable
OBJECTS = src/main.o src/buffer.o src/controller.o src/cursor.o src/file.o src/keyboard.o src/model.o src/ncurses-display.o src/ncurses-input.o src/ncurses-view.o src/vim.o
DEPENDS = ${OBJECTS:.o=.d}

${EXEC}: ${OBJECTS}
				${CXX} ${OBJECTS} -o ${EXEC}

-include ${DEPENDS}

.PHONY: clean
clean:
				rm ${OBJECTS} ${DEPENDS}
				rm vm
