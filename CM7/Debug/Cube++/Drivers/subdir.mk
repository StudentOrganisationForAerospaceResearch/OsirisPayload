################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
CPP_SRCS += \
../Cube++/Drivers/UARTDriver.cpp 

OBJS += \
./Cube++/Drivers/UARTDriver.o 

CPP_DEPS += \
./Cube++/Drivers/UARTDriver.d 


# Each subdirectory must supply rules for building sources it contributes
Cube++/Drivers/%.o Cube++/Drivers/%.su Cube++/Drivers/%.cyclo: ../Cube++/Drivers/%.cpp Cube++/Drivers/subdir.mk
	arm-none-eabi-g++ "$<" -mcpu=cortex-m7 -std=gnu++14 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Core" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Drivers" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Libraries" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Core/Inc" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++/Drivers/Inc" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Cube++" -I"C:/Users/shiva/Documents/SOAR2/Payload/OsirisPayload/CM7/Components" -O0 -ffunction-sections -fdata-sections -fno-exceptions -fno-rtti -fno-use-cxa-atexit -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Cube-2b--2b--2f-Drivers

clean-Cube-2b--2b--2f-Drivers:
	-$(RM) ./Cube++/Drivers/UARTDriver.cyclo ./Cube++/Drivers/UARTDriver.d ./Cube++/Drivers/UARTDriver.o ./Cube++/Drivers/UARTDriver.su

.PHONY: clean-Cube-2b--2b--2f-Drivers

