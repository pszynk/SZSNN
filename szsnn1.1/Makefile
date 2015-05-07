# Redefine the global variables
# Compiler
CXX 		= g++

# Preprocessor flags
CPPFLAGS 	=

# Compiler flags
CXXFLAGS 	= -g -Wall -fmessage-length=0

# Linker flags
LDFLAGS 	=

## FOLDERS ##

# Src folder
SRCFOLDER	:= src

# Obj folder
OBJFOLDER	:= obj

# Bin folder
BINFOLDER	:= bin

# Read sources list
SOURCES 	:= $(wildcard $(SRCFOLDER)/*.cpp)

# Generate objects list
#OBJS 		:= $(patsubst %.cpp, %.o, $(SOURCES))
OBJS		:= $(addprefix $(OBJFOLDER)/,$(notdir $(SOURCES:.cpp=.o)))


# A list of all needed special libraries 
LIBS 		:= OpenCL
LIBS 		:= $(addprefix -l,$(LIBS))

# A list of paths to special libraries
INCLIBS		:= ${AMDAPPSDKROOT}/lib/x86_64
INCLIBS		:= $(addprefix -L,$(INCLIBS))
LDFLAGS 	+= $(INCLIBS)

# A list of all directories for including
#INCDIRS 	:= /usr/local/cuda/include
INCDIRS 	:= ${AMDAPPSDKROOT}/include
INCDIRS 	:= $(addprefix -I,$(INCDIRS))

# A list of all macros
MACROS		:= SZSNN_NEURON_MODEL_IDX=0
MACROS		:= $(addprefix -D,$(MACROS))

all: release

# Rule for compiling .cpp source files to .o object files
#.cpp.o:
$(OBJFOLDER)/%.o : $(SRCFOLDER)/%.cpp
	$(CXX) $(CPPFLAGS) $(CXXFLAGS) $(MACROS) $(INCDIRS) -c $< -o $@

# Cleanup
clean:
	rm -fv $(OBJFOLDER)/*.o
	rm -fv Makefile.deps
	
cleanest: clean
	rm -fv $(BINFOLDER)/release
	rm -fv $(BINFOLDER)/debug

# Generate dependencies for all source files using -MM switch (-M lists all deps, including a lot of the system headers)
deps:
	$(CXX) -MM $(INCDIRS) $(SOURCES)  > Makefile.deps
	sed 's/\(^.*\.o:\)/$(OBJFOLDER)\/\1/' < Makefile.deps > temp.deps
	mv temp.deps Makefile.deps

# Effectively, the default build target ('all' redirects here)
# Append compiler code optimalization
release: CXXFLAGS += -O2

# Building depends on generated dependencies and all .o (object) files
release: deps $(OBJS)
	$(CXX) $(CXXFLAGS) $(MACROS) $(LDFLAGS) $(OBJS) $(LIBS) -o $(BINFOLDER)/$@

# Debugging target, append max possible level (3rd) of adding 
# debugging symbols to the output program
debug: CXXFLAGS += -g3
debug: MACROS	+= -DSZSNN_DEBUG=1
debug: deps $(OBJS)
	$(CXX) $(CXXFLAGS) $(MACROS) $(LDFLAGS) $(OBJS) $(LIBS) -o $(BINFOLDER)/$@

echo:
	@echo CXX: $(CXX)
	@echo CPPFLAGS: $(CPPFLAGS)
	@echo CXXFLAGS: $(CXXFLAGS)
	@echo MACROS:	$(MACROS)
	@echo LDFLAGS: $(LDFLAGS)
	@echo SOURCES: $(SOURCES)
	@echo OBJS: $(OBJS)
	@echo LIBS: $(LIBS)

# Force all targets
.PHONY: all clean deps release debug echo cleanest

# Include (ie. insert here literally) all the generated dependencies
# '-' prevents make from showing errors if the file doesn't exist (or is not readable)
-include Makefile.deps
