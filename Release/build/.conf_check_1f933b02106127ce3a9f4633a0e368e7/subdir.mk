################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../build/.conf_check_1f933b02106127ce3a9f4633a0e368e7/test.cpp 

OBJS += \
./build/.conf_check_1f933b02106127ce3a9f4633a0e368e7/test.o 

CPP_DEPS += \
./build/.conf_check_1f933b02106127ce3a9f4633a0e368e7/test.d 


# Each subdirectory must supply rules for building sources it contributes
build/.conf_check_1f933b02106127ce3a9f4633a0e368e7/%.o: ../build/.conf_check_1f933b02106127ce3a9f4633a0e368e7/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


