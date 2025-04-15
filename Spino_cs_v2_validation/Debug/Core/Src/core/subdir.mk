################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/core/command.c \
../Core/Src/core/control.c \
../Core/Src/core/informationMessage.c \
../Core/Src/core/setup.c 

OBJS += \
./Core/Src/core/command.o \
./Core/Src/core/control.o \
./Core/Src/core/informationMessage.o \
./Core/Src/core/setup.o 

C_DEPS += \
./Core/Src/core/command.d \
./Core/Src/core/control.d \
./Core/Src/core/informationMessage.d \
./Core/Src/core/setup.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/core/%.o Core/Src/core/%.su Core/Src/core/%.cyclo: ../Core/Src/core/%.c Core/Src/core/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L451xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-core

clean-Core-2f-Src-2f-core:
	-$(RM) ./Core/Src/core/command.cyclo ./Core/Src/core/command.d ./Core/Src/core/command.o ./Core/Src/core/command.su ./Core/Src/core/control.cyclo ./Core/Src/core/control.d ./Core/Src/core/control.o ./Core/Src/core/control.su ./Core/Src/core/informationMessage.cyclo ./Core/Src/core/informationMessage.d ./Core/Src/core/informationMessage.o ./Core/Src/core/informationMessage.su ./Core/Src/core/setup.cyclo ./Core/Src/core/setup.d ./Core/Src/core/setup.o ./Core/Src/core/setup.su

.PHONY: clean-Core-2f-Src-2f-core

