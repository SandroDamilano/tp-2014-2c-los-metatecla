################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/libs/funcionesCPU.c \
../src/libs/operaciones.c 

OBJS += \
./src/libs/funcionesCPU.o \
./src/libs/operaciones.o 

C_DEPS += \
./src/libs/funcionesCPU.d \
./src/libs/operaciones.d 


# Each subdirectory must supply rules for building sources it contributes
src/libs/%.o: ../src/libs/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


