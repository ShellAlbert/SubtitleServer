################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include thread_receiver/subdir.mk
-include thread_parser/subdir.mk
-include thread_downloader/subdir.mk
-include src/subdir.mk
-include screen_simulator/subdir.mk
-include rle32/subdir.mk
-include ringbuffer/subdir.mk
-include free_type/subdir.mk
-include subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

# Add inputs and outputs from these tool invocations to the build variables 

# All Target
all: zsubtitles_server

# Tool invocations
zsubtitles_server: $(OBJS) $(USER_OBJS)
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc  -o "zsubtitles_server" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) zsubtitles_server
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:

-include ../makefile.targets
