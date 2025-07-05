#**************************************************************************************************
# raylib Makefile for Windows with subfolder structure (no Unix commands used)
#**************************************************************************************************
.PHONY: all clean print-debug run rebuild
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
# SOURCES AND HEADERS (Automatic subfolder detection)
#=============================================================================
# Get all subdirectories in src
SRC_SUBDIRS := $(shell dir /ad /b $(SRC_DIR) 2>nul)
INCLUDE_SUBDIRS := $(shell dir /ad /b $(INCLUDE_DIR) 2>nul)

# Build source file patterns automatically
SRC := $(wildcard $(SRC_DIR)/*.cpp)
ifneq ($(SRC_SUBDIRS),)
    SRC += $(foreach dir,$(SRC_SUBDIRS),$(wildcard $(SRC_DIR)/$(dir)/*.cpp))
endif

OBJS := $(patsubst $(SRC_DIR)/%.cpp,$(OBJ_DIR)/%.o,$(SRC))
DEPS := $(OBJS:.o=.d)

# Resource file for icon
RESOURCE_FILE := resource.rc
RESOURCE_OBJ := $(OBJ_DIR)/resource.o

# Build include paths automatically
INCLUDE_PATHS := -I. -I$(RAYLIB_PATH)/src -I$(RAYLIB_PATH)/src/external -I$(INCLUDE_DIR)
ifneq ($(INCLUDE_SUBDIRS),)
    INCLUDE_PATHS += $(foreach dir,$(INCLUDE_SUBDIRS),-I$(INCLUDE_DIR)/$(dir))
endif

# Include dependency files (if they exist)
-include $(DEPS)
#=============================================================================
# LINKING
#=============================================================================
LDFLAGS := -L$(RAYLIB_PATH)/src
LDLIBS  := -lraylib -lopengl32 -lgdi32 -lwinmm
#=============================================================================
# BUILD RULES
#=============================================================================
all: $(PROJECT_NAME)

# Compile source files with dependency generation
$(OBJ_DIR)/%.o: $(SRC_DIR)/%.cpp
	@if not exist "$(dir $@)" mkdir "$(dir $@)"
	$(CC) -c $< -o $@ $(CFLAGS) $(INCLUDE_PATHS) -D$(PLATFORM) -MMD -MP -MF $(OBJ_DIR)/$*.d

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

# Force rebuild everything
rebuild: clean all
#=============================================================================
# RUN
#=============================================================================
run: $(PROJECT_NAME)
	@echo Starting game...
	@.\$(PROJECT_NAME).exe

#=============================================================================
# DEBUG
#=============================================================================
print-debug:
	@echo Found source directories:
	@echo $(SRC_SUBDIRS)
	@echo.
	@echo Found include directories:
	@echo $(INCLUDE_SUBDIRS)
	@echo.
	@echo Found source files:
	@echo $(SRC)
	@echo.
	@echo Will compile into:
	@echo $(OBJS)
	@echo.
	@echo Include paths:
	@echo $(INCLUDE_PATHS)