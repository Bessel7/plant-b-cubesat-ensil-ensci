################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/experimentalMode/experimentalMode.c 

OBJS += \
./Core/Src/experimentalMode/experimentalMode.o 

C_DEPS += \
./Core/Src/experimentalMode/experimentalMode.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/experimentalMode/%.o Core/Src/experimentalMode/%.su Core/Src/experimentalMode/%.cyclo: ../Core/Src/experimentalMode/%.c Core/Src/experimentalMode/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L451xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-experimentalMode

clean-Core-2f-Src-2f-experimentalMode:
	-$(RM) ./Core/Src/experimentalMode/experimentalMode.cyclo ./Core/Src/experimentalMode/experimentalMode.d ./Core/Src/experimentalMode/experimentalMode.o ./Core/Src/experimentalMode/experimentalMode.su

.PHONY: clean-Core-2f-Src-2f-experimentalMode

