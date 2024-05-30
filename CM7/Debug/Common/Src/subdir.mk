################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (12.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c 

C_DEPS += \
./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.d 

OBJS += \
./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o 


# Each subdirectory must supply rules for building sources it contributes
Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o: /Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.c Common/Src/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m7 -std=gnu11 -g3 -DDEBUG -DCORE_CM7 -DUSE_HAL_DRIVER -DSTM32H747xx -DUSE_FULL_LL_DRIVER -c -I../Core/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc -I../../Drivers/STM32H7xx_HAL_Driver/Inc/Legacy -I../../Drivers/CMSIS/Device/ST/STM32H7xx/Include -I../../Drivers/CMSIS/Include -I../../Middlewares/Third_Party/FreeRTOS/Source/include -I../../Middlewares/Third_Party/FreeRTOS/Source/CMSIS_RTOS -I../../Middlewares/Third_Party/FreeRTOS/Source/portable/GCC/ARM_CM4F -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Core" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Drivers" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Libraries" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Core/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++/Drivers/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Cube++" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Debug" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Debug/Inc" -I"/Users/cjchan/Desktop/SOAR/Payload/OsirisPayload/CM7/Components/Core/Inc" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfpu=fpv5-d16 -mfloat-abi=hard -mthumb -o "$@"

clean: clean-Common-2f-Src

clean-Common-2f-Src:
	-$(RM) ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.cyclo ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.d ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.o ./Common/Src/system_stm32h7xx_dualcore_boot_cm4_cm7.su

.PHONY: clean-Common-2f-Src

