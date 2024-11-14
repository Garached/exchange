# Makefile

# Define sources
USER_SRCS = usuario.c dados.c fracao.c
ADMIN_SRCS = admin.c dados.c fracao.c

# Define the target
TARGET = executavel

# Default rule
all: $(USER_SRCS)
	gcc $(USER_SRCS) -o $(TARGET)

# Check for the --admin flag
ifeq ($(MAKECMDGOALS),admin)
    SRCS = $(ADMIN_SRCS)
else
    SRCS = $(USER_SRCS)
endif

# Target for --admin flag
admin:
	gcc $(SRCS) -o $(TARGET)

# Clean rule
clean:
	rm -f $(TARGET)
