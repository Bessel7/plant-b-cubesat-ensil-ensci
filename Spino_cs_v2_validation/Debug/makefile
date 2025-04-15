################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

-include ../makefile.init

RM := rm -rf

# All of the sources participating in the build are defined here
-include sources.mk
-include Drivers/STM32L4xx_HAL_Driver/Src/subdir.mk
-include Core/Startup/subdir.mk
-include Core/Src/surveyMode/subdir.mk
-include Core/Src/payloadMode/subdir.mk
-include Core/Src/mailboxMode/subdir.mk
-include Core/Src/logMngt/subdir.mk
-include Core/Src/experimentalMode/subdir.mk
-include Core/Src/dropMsgMngt/subdir.mk
-include Core/Src/drivers/subdir.mk
-include Core/Src/digipeaterMode/subdir.mk
-include Core/Src/core/subdir.mk
-include Core/Src/ax25/subdir.mk
-include Core/Src/SpinoDriver/subdir.mk
-include Core/Src/subdir.mk
-include objects.mk

ifneq ($(MAKECMDGOALS),clean)
ifneq ($(strip $(S_DEPS)),)
-include $(S_DEPS)
endif
ifneq ($(strip $(S_UPPER_DEPS)),)
-include $(S_UPPER_DEPS)
endif
ifneq ($(strip $(C_DEPS)),)
-include $(C_DEPS)
endif
endif

-include ../makefile.defs

OPTIONAL_TOOL_DEPS := \
$(wildcard ../makefile.defs) \
$(wildcard ../makefile.init) \
$(wildcard ../makefile.targets) \


BUILD_ARTIFACT_NAME := Spino_cs_v2_validation
BUILD_ARTIFACT_EXTENSION := elf
BUILD_ARTIFACT_PREFIX :=
BUILD_ARTIFACT := $(BUILD_ARTIFACT_PREFIX)$(BUILD_ARTIFACT_NAME)$(if $(BUILD_ARTIFACT_EXTENSION),.$(BUILD_ARTIFACT_EXTENSION),)

# Add inputs and outputs from these tool invocations to the build variables 
EXECUTABLES += \
Spino_cs_v2_validation.elf \

MAP_FILES += \
Spino_cs_v2_validation.map \

SIZE_OUTPUT += \
default.size.stdout \

OBJDUMP_LIST += \
Spino_cs_v2_validation.list \


# All Target
all: main-build

# Main-build Target
main-build: Spino_cs_v2_validation.elf secondary-outputs

# Tool invocations
Spino_cs_v2_validation.elf Spino_cs_v2_validation.map: $(OBJS) $(USER_OBJS) /home/bessel/STM32CubeIDE/workspace_1.17.1/Spino_cs_v2_validation/STM32L451RETX_FLASH.ld makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-gcc -o "Spino_cs_v2_validation.elf" @"objects.list" $(USER_OBJS) $(LIBS) -mcpu=cortex-m4 -T"/home/bessel/STM32CubeIDE/workspace_1.17.1/Spino_cs_v2_validation/STM32L451RETX_FLASH.ld" --specs=nosys.specs -Wl,-Map="Spino_cs_v2_validation.map" -Wl,--gc-sections -static --specs=nano.specs -mfpu=fpv4-sp-d16 -mfloat-abi=hard -mthumb -u _printf_float -Wl,--start-group -lc -lm -Wl,--end-group
	@echo 'Finished building target: $@'
	@echo ' '

default.size.stdout: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-size  $(EXECUTABLES)
	@echo 'Finished building: $@'
	@echo ' '

Spino_cs_v2_validation.list: $(EXECUTABLES) makefile objects.list $(OPTIONAL_TOOL_DEPS)
	arm-none-eabi-objdump -h -S $(EXECUTABLES) > "Spino_cs_v2_validation.list"
	@echo 'Finished building: $@'
	@echo ' '

# Other Targets
clean:
	-$(RM) Spino_cs_v2_validation.elf Spino_cs_v2_validation.list Spino_cs_v2_validation.map default.size.stdout
	-@echo ' '

secondary-outputs: $(SIZE_OUTPUT) $(OBJDUMP_LIST)

fail-specified-linker-script-missing:
	@echo 'Error: Cannot find the specified linker script. Check the linker settings in the build configuration.'
	@exit 2

warn-no-linker-script-specified:
	@echo 'Warning: No linker script specified. Check the linker settings in the build configuration.'

.PHONY: all clean dependents main-build fail-specified-linker-script-missing warn-no-linker-script-specified

-include ../makefile.targets
