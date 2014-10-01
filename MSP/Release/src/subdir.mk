################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/ConsolaMSP.c \
../src/MSP.c \
../src/funcionesMSP.c 

OBJS += \
./src/ConsolaMSP.o \
./src/MSP.o \
./src/funcionesMSP.o 

C_DEPS += \
./src/ConsolaMSP.d \
./src/MSP.d \
./src/funcionesMSP.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/workspace/ansisop-panel" -I"/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib" -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


