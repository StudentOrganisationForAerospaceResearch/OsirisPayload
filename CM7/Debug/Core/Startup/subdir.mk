################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
S_SRCS += \
../Core/Startup/startup_stm32h747igtx.s 

S_DEPS += \
./Core/Startup/startup_stm32h747igtx.d 

OBJS += \
./Core/Startup/startup_stm32h747igtx.o 


# Each subdirectory must supply rules for building sources it contributes
Core/Startup/%.o: ../Core/Startup/%.s Core/Startup/subdir.mk
	arm-none-eabi-gcc -mcpu=cortex-m7 -g3 -DDEBUG -c -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Core" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Drivers" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Core/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Drivers/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Debug" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Debug/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries/embedded-template-library" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries/embedded-template-library/include" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Core/Inc" -x assembler-with-cpp -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@" "$<"

clean: clean-Core-2f-Startup

clean-Core-2f-Startup:
	-$(RM) ./Core/Startup/startup_stm32h747igtx.d ./Core/Startup/startup_stm32h747igtx.o

.PHONY: clean-Core-2f-Startup

