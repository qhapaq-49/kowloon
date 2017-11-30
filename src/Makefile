# Makefile for Kowloon

CC = g++
FLAG = -std=c++14 -Ofast -Wall -Wextra -Wcast-qual -Wno-unused-function -Wno-sign-compare -Wno-unused-value -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter -fno-exceptions -fno-rtti -pedantic -Wno-long-long -msse4.2 -D__STDC_CONSTANT_MACROS -fopenmp
#FLAG = -std=c++14 -O0 -g -Wall -Wextra -Wcast-qual -Wno-unused-function -Wno-sign-compare -Wno-unused-value -Wno-unused-label -Wno-unused-variable -Wno-unused-but-set-variable -Wno-unused-parameter -fno-exceptions -fno-rtti -pedantic -Wno-long-long -msse4.2 -D__STDC_CONSTANT_MACROS -fopenmp
TARGET = ../kowloon_uec.exe

SOURCES = main.cpp connection_uec.cpp tournament_uec.cpp position.cpp agent_kowloon_uec.cpp testagent.cpp changer_uecdef.cpp searcher_uecdef.cpp strategy.cpp movepick.cpp eval.cpp translation.cpp learner.cpp changer_kowloon.cpp mate.cpp
OBJDIR = obj
OBJECTS  = $(addprefix $(OBJDIR)/, $(SOURCES:.cpp=.o))
DEPENDS  = $(OBJECTS:.o=.d)

$(TARGET): $(OBJECTS)
	$(CC) -o $@ $^ $(FLAG)

$(OBJDIR)/%.o: %.cpp
	@[ -d $(OBJDIR) ] || mkdir -p $(OBJDIR)
	$(CC) $(FLAG) -o $@ -c $<

all:
	$(MAKE) FLAG='$(FLAG)' $(TARGET)

clean:
	rm -f $(OBJECTS) $(DEPENDS) $(TARGET)

-include $(DEPENDS)

