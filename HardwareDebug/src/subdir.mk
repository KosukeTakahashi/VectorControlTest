################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
CPP_SRCS += \
../src/VectorControlTest.cpp \
../src/cpwm.cpp \
../src/dacvtr.cpp \
../src/sensors.cpp \
../src/vecctrl.cpp 

COMPILER_OBJS += \
src/VectorControlTest.obj \
src/cpwm.obj \
src/dacvtr.obj \
src/sensors.obj \
src/vecctrl.obj 

CPP_DEPS += \
src/VectorControlTest.d \
src/cpwm.d \
src/dacvtr.d \
src/sensors.d \
src/vecctrl.d 

# Each subdirectory must supply rules for building sources it contributes
src/%.obj: ../src/%.cpp 
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	@echo src\cppDepSubCommand.tmp=
	@sed -e "s/^/ /" "src\cppDepSubCommand.tmp"
	ccrx -subcommand="src\cppDepSubCommand.tmp" -output=dep="$(@:%.obj=%.d)"  -MT="$(@:%.d=%.obj)"  -MT="$(@:%.obj=%.d)" "$<"
	@echo src\cppSubCommand.tmp=
	@sed -e "s/^/ /" "src\cppSubCommand.tmp"
	ccrx -subcommand="src\cppSubCommand.tmp" "$<"
	@echo 'Finished Scanning and building: $<'
	@echo.

