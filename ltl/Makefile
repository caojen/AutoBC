CC:=g++
CFLAG:= -Wall -g -std=c++11

OBJDIR:=./obj
INCLUDEDIR:=./include
SRCDIR:=./src

OBJ:=	${OBJDIR}/dict.o ${OBJDIR}/literal.o ${OBJDIR}/ltl.o \
			${OBJDIR}/operator.o ${OBJDIR}/sat.o

HEADER:=${INCLUDEDIR}/dict.hpp ${INCLUDEDIR}/error.hpp \
				${INCLUDEDIR}/literal.hpp ${INCLUDEDIR}/ltl.hpp \
				${INCLUDEDIR}/operator.hpp ${INCLUDEDIR}/sat.hpp

ZSH_RES:=$(shell mkdir -p ${OBJDIR})

all: ${OBJ}

clean:
	rm -rf ./obj

${OBJDIR}/dict.o: ${SRCDIR}/dict.cpp ${HEADER}
	${CC} ${CFLAG} -I${INCLUDEDIR} $< -c -o $@

${OBJDIR}/literal.o: ${SRCDIR}/literal.cpp ${HEADER}
	${CC} ${CFLAG} -I${INCLUDEDIR} $< -c -o $@

${OBJDIR}/ltl.o: ${SRCDIR}/ltl.cpp ${HEADER}
	${CC} ${CFLAG} -I${INCLUDEDIR} $< -c -o $@

${OBJDIR}/operator.o: ${SRCDIR}/operator.cpp ${HEADER}
	${CC} ${CFLAG} -I${INCLUDEDIR} $< -c -o $@

${OBJDIR}/sat.o: ${SRCDIR}/sat.cpp ${HEADER}
	${CC} ${CFLAG} -I${INCLUDEDIR} $< -c -o $@
