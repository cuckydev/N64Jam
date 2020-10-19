# Defines
include $(ROOT)/usr/include/make/PRdefs

# Directories
LIB = $(ROOT)/usr/lib
LPR = $(LIB)/PR
INC = $(ROOT)/usr/include
NUSYSINCDIR  = /usr/include/n64/nusys
NUSYSLIBDIR  = /usr/lib/n64/nusys

# Compilation flags
LCDEFS =  -DF3DEX_GBI_2
LCINCS =  -I. -I$(NUSYSINCDIR) -I$(ROOT)/usr/include/PR
LCOPTS =  -G 0
LDIRT =   $(APP)
LDFLAGS = $(MKDEPOPT) -L$(LIB) -L$(NUSYSLIBDIR) -s -lnusys -lultra -L$(N64_LIBGCCDIR) -lgcc -lnustd
OPTIMIZER = -O2

# Target directory
APP =     jam.out
TARGETS = jam.n64

# Source files
CODEFILES = \
	main.c \
	mem.c \
	math_util.c \
	render.c \
	input.c \
	game.c \
	map.c \
	object.c \
	obj_player.c \
	gm_game.c

# Output objects
CODEOBJECTS = $(CODEFILES:.c=.o)  $(NUSYSLIBDIR)/nusys.o

DATAFILES   = 
DATAOBJECTS = $(DATAFILES:.c=.o)

CODESEGMENT = codesegment.o

OBJECTS = $(CODESEGMENT) $(DATAOBJECTS)

# Compilation
default:        $(TARGETS)

include $(COMMONRULES)

$(CODESEGMENT):	$(CODEOBJECTS) Makefile
		$(LD) -o $(CODESEGMENT) -r $(CODEOBJECTS) $(LDFLAGS)

$(TARGETS):	$(OBJECTS)
		$(MAKEROM) spec -s 9 -I$(NUSYSINCDIR) -r $(TARGETS) -e $(APP)
		makemask $(TARGETS)
