################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CC_SRCS += \
../src/dsr/examples/dsr.cc 

CC_DEPS += \
./src/dsr/examples/dsr.d 

OBJS += \
./src/dsr/examples/dsr.o 


# Each subdirectory must supply rules for building sources it contributes
src/dsr/examples/%.o: ../src/dsr/examples/%.cc
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


