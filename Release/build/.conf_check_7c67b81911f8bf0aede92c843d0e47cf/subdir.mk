################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../build/.conf_check_7c67b81911f8bf0aede92c843d0e47cf/test.cpp 

OBJS += \
./build/.conf_check_7c67b81911f8bf0aede92c843d0e47cf/test.o 

CPP_DEPS += \
./build/.conf_check_7c67b81911f8bf0aede92c843d0e47cf/test.d 


# Each subdirectory must supply rules for building sources it contributes
build/.conf_check_7c67b81911f8bf0aede92c843d0e47cf/%.o: ../build/.conf_check_7c67b81911f8bf0aede92c843d0e47cf/%.cpp
	@echo 'Building file: $<'
	@echo 'Invoking: Cross G++ Compiler'
	g++ -O3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


