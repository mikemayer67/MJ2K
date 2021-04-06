TARGET = j2k_parser

SRCS = $(shell find . -name '*.cc')
OBJS = $(SRCS:.cc=.o)
DEPS = $(OBJS:.o=.d)

INSTDIR = $(abspath $(dir $(abspath ..))/bin)

target: $(TARGET)

$(TARGET): $(OBJS)
	g++ -g $(OBJS) -o $@

.cc.o: %cc
	g++ -MMD -MP -I./src -c $<

clean:
	@rm -f *.o
	@rm -f *.d
	@rm -f $(TARGET)

fresh: clean target


install: $(TARGET) $(INSTDIR)
	@mv $(TARGET) $(INSTDIR)

uninstall:
	@rm $(INSTDIR)/$(TARGET)

$(INSTDIR):
	@mkdir -p $(INSTDIR)

-include $(DEPS)
