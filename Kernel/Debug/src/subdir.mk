################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../src/funciones_kernel.c \
../src/loader.c \
../src/main.c \
../src/planificador.c 

OBJS += \
./src/funciones_kernel.o \
./src/loader.o \
./src/main.o \
./src/planificador.o 

C_DEPS += \
./src/funciones_kernel.d \
./src/loader.d \
./src/main.d \
./src/planificador.d 


# Each subdirectory must supply rules for building sources it contributes
src/%.o: ../src/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/utnso/tp-2014-2c-los-metatecla/MiniGral_lib" -I"/home/utnso/tp-2014-2c-los-metatecla/ansisop-panel" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


