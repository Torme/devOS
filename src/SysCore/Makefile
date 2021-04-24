FILE_PATH		= Kernel

LIB_PATH 		= Lib

GCC					= g++

RM					= rm -f

AR					= ar rc

NAME 				= devOS

NAME_LIB		= libdevOS.a

# CXXFLAGS		+= -Wextra -Werror -Wall
CXXFLAGS		+= -IInclude

LIB 				= -ldevOS \
							-L.

SRC 				= ${FILE_PATH}/main.cpp \
							${FILE_PATH}/DebugDisplay.cpp

OBJ 				= ${SRC:.cpp=.o}

SRC_LIB			= ${LIB_PATH}/string.cpp \
							${LIB_PATH}/cstd.cpp

OBJ_LIB			= ${SRC_LIB:.cpp=.o}

all: lib ${NAME} ${NAME}.debug

lib: ${OBJ_LIB}
	${AR} ${NAME_LIB} ${OBJ_LIB}

${NAME}: ${OBJ}
	${GCC} -o ${NAME} ${OBJ} ${LIB} ${CXXFLAGS}

${NAME}.debug:
	${GCC} -o ${NAME}.debug ${SRC} ${LIB} ${CXXFLAGS} -g

clean:
	${RM} ${OBJ}

fclean: clean
	${RM} ${NAME} ${NAME}.debug ${NAME_LIB}

re: fclean all

.PHONY: all lib ${NAME} ${NAME}.debug clean re
