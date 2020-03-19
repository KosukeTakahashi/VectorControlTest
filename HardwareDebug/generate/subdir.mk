################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables
C_SRCS += \
../generate/dbsct.c \
../generate/hwsetup.c \
../generate/intprg.c \
../generate/resetprg.c \
../generate/sbrk.c \
../generate/vecttbl.c 

COMPILER_OBJS += \
generate/dbsct.obj \
generate/hwsetup.obj \
generate/intprg.obj \
generate/resetprg.obj \
generate/sbrk.obj \
generate/vecttbl.obj 

C_DEPS += \
generate/dbsct.d \
generate/hwsetup.d \
generate/intprg.d \
generate/resetprg.d \
generate/sbrk.d \
generate/vecttbl.d 

# Each subdirectory must supply rules for building sources it contributes
generate/%.obj: ../generate/%.c 
	@echo 'Scanning and building file: $<'
	@echo 'Invoking: Scanner and Compiler'
	@echo generate\cDepSubCommand.tmp=
	@sed -e "s/^/ /" "generate\cDepSubCommand.tmp"
	ccrx -subcommand="generate\cDepSubCommand.tmp" -output=dep="$(@:%.obj=%.d)"  -MT="$(@:%.d=%.obj)"  -MT="$(@:%.obj=%.d)" "$<"
	@echo generate\cSubCommand.tmp=
	@sed -e "s/^/ /" "generate\cSubCommand.tmp"
	ccrx -subcommand="generate\cSubCommand.tmp" "$<"
	@echo 'Finished Scanning and building: $<'
	@echo.

