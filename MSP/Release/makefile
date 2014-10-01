################################################################################
# Automatically-generated file. Do not edit!
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include src/subdir.mk
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
all: MSP

dependents:
	-cd /home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib/Debug && $(MAKE) all
	-cd /home/utnso/workspace/ansisop-panel/Debug && $(MAKE) all

# Tool invocations
MSP: $(OBJS) $(USER_OBJS) /home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib/Debug/libMiniGral_lib.so /home/utnso/workspace/ansisop-panel/Debug/libansisop-panel.so
	@echo 'Building target: $@'
	@echo 'Invoking: GCC C Linker'
	gcc -L"/home/utnso/workspace/ansisop-panel/Debug" -L"/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib/Debug" -o "MSP" $(OBJS) $(USER_OBJS) $(LIBS)
	@echo 'Finished building target: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) $(OBJS)$(C_DEPS)$(EXECUTABLES) MSP
	-@echo ' '

.PHONY: all clean dependents
.SECONDARY:
/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib/Debug/libMiniGral_lib.so:
/home/utnso/workspace/ansisop-panel/Debug/libansisop-panel.so:

-include ../makefile.targets
