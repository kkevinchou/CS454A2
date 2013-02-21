CXX = g++					# compiler
CXXFLAGS = -g -Wall -Wno-unused-label -MMD -DTYPE="${TYPE}" # compiler flags
MAKEFILE_NAME = ${firstword ${MAKEFILE_LIST}}	# makefile name

OBJECTS1 = stringServer.o
EXEC1 = stringServer				# 1st executable name

OBJECTS2 = stringClient.o
EXEC2 = stringClient				# 2st executable name


OBJECTS = ${OBJECTS1} ${OBJECTS2} 				# all object files
DEPENDS = ${OBJECTS:.o=.d}			# substitute ".o" with ".d"
EXECS = ${EXEC1} ${EXEC2} 				# all executables

#############################################################

.PHONY : all clean

all : ${EXECS}					# build all executables

${EXEC1} : ${OBJECTS1}				# link step 1st executable
	${CXX} $^ -o $@

${EXEC2} : ${OBJECTS2}				# link step 1st executable
	${CXX} $^ -o $@

#############################################################

${OBJECTS} : ${MAKEFILE_NAME}			# OPTIONAL : changes to this file => recompile

-include ${DEPENDS}				# include *.d files containing program dependences

clean :						# remove files that can be regenerated
	rm -f *.d *.o ${EXECS}
