################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/MSP.c \
../src/funcionesMSP.c 

OBJS += \
./src/MSP.o \
./src/funcionesMSP.o 

C_DEPS += \
./src/MSP.d \
./src/funcionesMSP.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-2c-los-metatecla/ansisop-panel" -I"/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


