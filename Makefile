CPP = g++

INCLUDE = 
LIBS =
C_ARGS = -g -Wall -D_FILE_OFFSET_BITS=64 -D__STDC_FORMAT_MACROS $(INCLUDE) 

BINARY = test
test_dep = test.o profanity_filter.o
test_lib = 

ALL_OBJS = $(foreach d,$(BINARY),$($(d)_dep))

%.o : %.cpp
	$(CPP) $(C_ARGS) -c  $< -o $(patsubst %.cpp,%.o,$<)
%.o : %.cc
	$(CPP) $(C_ARGS) -c  $< -o $(patsubst %.cc,%.o,$<)
%.o : %.c
	$(CPP) $(C_ARGS) -c  $< -o $(patsubst %.c,%.o,$<)
	
all : $(BINARY)

$(BINARY) : $(ALL_OBJS)
	@echo "now building:" $@
	@echo "dep:" $($@_dep)
	rm -f $@
	$(CPP) $(C_ARGS) -o $@  $($@_dep) $(LIBS) $($@_lib)

clean:
	rm -f $(ALL_OBJS) $(BINARY)

print:
	@echo "print all vars"
	@echo "all objs:" $(ALL_OBJS)
