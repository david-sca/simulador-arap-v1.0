################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/fd-net-device/model/fd-net-device.cc 

CC_DEPS += \
./src/fd-net-device/model/fd-net-device.d 

OBJS += \
./src/fd-net-device/model/fd-net-device.o 


# Each subdirectory must supply rules for building sources it contributes
src/fd-net-device/model/%.o: ../src/fd-net-device/model/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


