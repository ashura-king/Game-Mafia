#**************************************************************************************************
# raylib Makefile for Windows with subfolder structure (no Unix commands used)
#**************************************************************************************************

.PHONY: all clean print-debug

#=============================================================================
# USER CONFIGURATION
#=============================================================================
PROJECT_NAME       ?= game
RAYLIB_PATH        ?= C:/raylib/raylib
COMPILER_PATH      ?= C:/raylib/w64devkit/bin
PLATFORM           ?= PLATFORM_DESKTOP
BUILD_MODE         ?= RELEASE
RAYLIB_LIBTYPE     ?= STATIC

SRC_DIR            := src
OBJ_DIR            := obj
INCLUDE_DIR        := includes

#=============================================================================
# PLATFORM DETECTION
#=============================================================================
PLATFORM_OS := WINDOWS
export PATH := $(COMPILER_PATH):$(PATH)

#=============================================================================
# COMPILER & FLAGS
#=============================================================================
CC      := g++
CFLAGS  := -Wall -std=c++17 -D_DEFAULT_SOURCE -Wno-missing-braces

ifeq ($(BUILD_MODE),DEBUG)
    CFLAGS += -g -O0
else
    CFLAGS += -s -O1
endif

#=============================================================================
# SOURCES AND HEADERS (Windows safe, no Unix find)
#=============================================================================
SRC := $(wildcard $(SRC_DIR)/*.cpp) \
       $(wildcard $(SRC_DIR)/Bots/*.cpp) \
       $(wildcard $(SRC_DIR)/Buttons/*.cpp) \
       $(wildcard $(SRC_DIR)/Layer/*.cpp)

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))

# Resource file for icon
RESOURCE_FILE := resource.rc
RESOURCE_OBJ := $(OBJ_DIR)/resource.o

# Remove the problematic OBJDIRS lines - directories will be created during compilation

INCLUDE_PATHS := -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external
INCLUDE_PATHS += -I$(INCLUDE_DIR) -I$(INCLUDE_DIR)/Bots -I$(INCLUDE_DIR)/Buttons -I$(INCLUDE_DIR)/Layer

#=============================================================================
# LINKING
#=============================================================================
LDFLAGS := -L$(RAYLIB_PATH)/src
LDLIBS := -lraylib -lopengl32 -lgdi32 -lwinmm

#=============================================================================
# BUILD RULES
#=============================================================================
all: $(PROJECT_NAME)

$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM)

# Compile resource file
$(RESOURCE_OBJ): $(RESOURCE_FILE)
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	windres $(RESOURCE_FILE) -O coff -o $(RESOURCE_OBJ)

$(PROJECT_NAME): $(OBJS) $(RESOURCE_OBJ)
	$(CC) -o $(PROJECT_NAME).exe $(OBJS) $(RESOURCE_OBJ) $(CFLAGS) $(INCLUDE_PATHS) $(LDFLAGS) $(LDLIBS) -D$(PLATFORM)

#=============================================================================
# CLEAN
#=============================================================================
clean:
	@if exist $(OBJ_DIR) rmdir /s /q $(OBJ_DIR)
	@if exist $(PROJECT_NAME).exe del $(PROJECT_NAME).exe
	@echo Cleaning done

#=============================================================================
# DEBUG
#=============================================================================
print-debug:
	@echo Found source files:
	@echo $(SRC)
	@echo
	@echo Will compile into:
	@echo $(OBJS)