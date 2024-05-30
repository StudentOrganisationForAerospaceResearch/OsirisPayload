################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Components/Debug/DebugTask.cpp 

OBJS += \
./Components/Debug/DebugTask.o 

CPP_DEPS += \
./Components/Debug/DebugTask.d 


# Each subdirectory must supply rules for building sources it contributes
Components/Debug/%.o Components/Debug/%.su Components/Debug/%.cyclo: ../Components/Debug/%.cpp Components/Debug/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++20 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Core" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Drivers" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Core/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Drivers/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Debug" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Debug/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries/embedded-template-library" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries/embedded-template-library/include" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Core/Inc" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components-2f-Debug

clean-Components-2f-Debug:
	-$(RM) ./Components/Debug/DebugTask.cyclo ./Components/Debug/DebugTask.d ./Components/Debug/DebugTask.o ./Components/Debug/DebugTask.su

.PHONY: clean-Components-2f-Debug

