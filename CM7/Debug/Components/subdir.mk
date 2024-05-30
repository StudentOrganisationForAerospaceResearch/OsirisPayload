################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Components/main_system.cpp 

OBJS += \
./Components/main_system.o 

CPP_DEPS += \
./Components/main_system.d 


# Each subdirectory must supply rules for building sources it contributes
Components/%.o Components/%.su Components/%.cyclo: ../Components/%.cpp Components/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Core" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Drivers" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Libraries" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Core/Inc" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Drivers/Inc" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Components" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Components

clean-Components:
	-$(RM) ./Components/main_system.cyclo ./Components/main_system.d ./Components/main_system.o ./Components/main_system.su

.PHONY: clean-Components
