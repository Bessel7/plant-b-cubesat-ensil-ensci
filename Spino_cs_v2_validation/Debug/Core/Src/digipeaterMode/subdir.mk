################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/digipeaterMode/ModeDigipeater.c 

OBJS += \
./Core/Src/digipeaterMode/ModeDigipeater.o 

C_DEPS += \
./Core/Src/digipeaterMode/ModeDigipeater.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/digipeaterMode/%.o Core/Src/digipeaterMode/%.su Core/Src/digipeaterMode/%.cyclo: ../Core/Src/digipeaterMode/%.c Core/Src/digipeaterMode/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L451xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-digipeaterMode

clean-Core-2f-Src-2f-digipeaterMode:
	-$(RM) ./Core/Src/digipeaterMode/ModeDigipeater.cyclo ./Core/Src/digipeaterMode/ModeDigipeater.d ./Core/Src/digipeaterMode/ModeDigipeater.o ./Core/Src/digipeaterMode/ModeDigipeater.su

.PHONY: clean-Core-2f-Src-2f-digipeaterMode

