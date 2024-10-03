CFLAGS ?= -DNDEBUG -O3 -Wall -Wextra -pedantic -std=c99
LDLIBS = -lm
SOURCES := main.c renderer.c microui.c
OBJECTS := $(SOURCES:%.c=%.o)
DEPS := $(SOURCES:%.c=%.d)
CFLAGS += -MMD

main: $(OBJECTS)

-include $(DEPS)

ifeq ($(OS),Windows_NT)
	LDLIBS += -lgdi32
else
	UNAME_S := $(shell uname -s)
	ifeq ($(UNAME_S),Darwin)
		LDLIBS += -framework Cocoa
	else
		LDLIBS += -lX11
	endif
endif

clean:
	rm -f main $(OBJECTS) $(DEPS)

.PHONY: clean
