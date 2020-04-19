IDIR =.
CC=gcc
CFLAGS=-I$(IDIR)

DEFINES=#-DVMEM64

DEBUG_OPT=-g -Wall $(DEFINES) -DDEBUG
RELEASE_OPT=-O2 -s -Wall $(DEFINES)

DEBUG_ODIR=debug_obj
RELEASE_ODIR=release_obj

OUTPUT_DIR=lib

DEBUG_LIB=$(OUTPUT_DIR)/VMem_Debug.a
RELEASE_LIB=$(OUTPUT_DIR)/VMem_Release.a

_DEPS = InternalHeap.hpp VMemCriticalSection.hpp BasicCoalesceHeap.hpp VMemDefs.hpp LargeHeap.hpp VMemHashMap.hpp BasicFSA.hpp PageHeap.hpp VMemHeap.hpp Bitfield.hpp VMemMemProStats.hpp TrailGuard.hpp CoalesceHeap.hpp VMem_PCH.hpp VirtualMem.hpp VMemStats.hpp FSAHeap.hpp VMemSys.hpp VMemAlloc.hpp FSA.hpp VMemCore.hpp VMemThread.hpp

DEPS = $(patsubst %,$(IDIR)/%,$(_DEPS))

_OBJ = BasicCoalesceHeap.o LargeHeap.o BasicFSA.o PageHeap.o VMemHeap.o Bitfield.o TrailGuard.o CoalesceHeap.o VMem_PCH.o VirtualMem.o FSA.o  FSAHeap.o VMemAlloc.o VMemSys.o VMemCore.o VMemThread.o InternalHeap.o

DEBUG_OBJ = $(patsubst %,$(DEBUG_ODIR)/%,$(_OBJ))
RELEASE_OBJ = $(patsubst %,$(RELEASE_ODIR)/%,$(_OBJ))

# all

.PHONY: all
all: $(DEBUG_LIB) $(RELEASE_LIB)

$(OUTPUT_DIR):
	mkdir -p $(OUTPUT_DIR)

# debug

$(DEBUG_LIB): $(DEBUG_OBJ) | $(OUTPUT_DIR)
	ar rs $@ $^

$(DEBUG_ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(DEBUG_OPT)

$(DEBUG_OBJ): | $(DEBUG_ODIR)

$(DEBUG_ODIR):
	mkdir -p $(DEBUG_ODIR)

# release

$(RELEASE_LIB): $(RELEASE_OBJ) | $(OUTPUT_DIR)
	ar rs $@ $^

$(RELEASE_ODIR)/%.o: %.cpp $(DEPS)
	$(CC) -c -o $@ $< $(CFLAGS) $(RELEASE_OPT)

$(RELEASE_OBJ): | $(RELEASE_ODIR)

$(RELEASE_ODIR):
	mkdir -p $(RELEASE_ODIR)

# clean

.PHONY: clean
clean:
	rm -f $(DEBUG_LIB) $(DEBUG_ODIR)/*.o *~ $(INCDIR)/*~
	rm -f $(RELEASE_LIB) $(RELEASE_ODIR)/*.o *~ $(INCDIR)/*~  


