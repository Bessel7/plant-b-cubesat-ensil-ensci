################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../Core/Src/mailboxMode/mailbox.c \
../Core/Src/mailboxMode/modeMailbox.c 

OBJS += \
./Core/Src/mailboxMode/mailbox.o \
./Core/Src/mailboxMode/modeMailbox.o 

C_DEPS += \
./Core/Src/mailboxMode/mailbox.d \
./Core/Src/mailboxMode/modeMailbox.d 


# Each subdirectory must supply rules for building sources it contributes
Core/Src/mailboxMode/%.o Core/Src/mailboxMode/%.su Core/Src/mailboxMode/%.cyclo: ../Core/Src/mailboxMode/%.c Core/Src/mailboxMode/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m4 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32L451xx -c -I../Core/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc -I../Drivers/STM32L4xx_HAL_Driver/Inc/Legacy -I../Drivers/CMSIS/Device/ST/STM32L4xx/Include -I../Drivers/CMSIS/Include -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Core-2f-Src-2f-mailboxMode

clean-Core-2f-Src-2f-mailboxMode:
	-$(RM) ./Core/Src/mailboxMode/mailbox.cyclo ./Core/Src/mailboxMode/mailbox.d ./Core/Src/mailboxMode/mailbox.o ./Core/Src/mailboxMode/mailbox.su ./Core/Src/mailboxMode/modeMailbox.cyclo ./Core/Src/mailboxMode/modeMailbox.d ./Core/Src/mailboxMode/modeMailbox.o ./Core/Src/mailboxMode/modeMailbox.su

.PHONY: clean-Core-2f-Src-2f-mailboxMode

