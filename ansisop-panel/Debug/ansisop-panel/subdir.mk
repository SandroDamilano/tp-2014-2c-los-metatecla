################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../ansisop-panel/cpu.c \
../ansisop-panel/kernel.c \
../ansisop-panel/panel.c 

OBJS += \
./ansisop-panel/cpu.o \
./ansisop-panel/kernel.o \
./ansisop-panel/panel.o 

C_DEPS += \
./ansisop-panel/cpu.d \
./ansisop-panel/kernel.d \
./ansisop-panel/panel.d 


# Each subdirectory must supply rules for building sources it contributes
ansisop-panel/%.o: ../ansisop-panel/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O0 -g3 -Wall -c -fmessage-length=0 -fPIC -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


