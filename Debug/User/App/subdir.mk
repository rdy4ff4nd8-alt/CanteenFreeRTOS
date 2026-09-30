################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/App/Data_App.c 

OBJS += \
./User/App/Data_App.o 

C_DEPS += \
./User/App/Data_App.d 


# Each subdirectory must supply rules for building sources it contributes
User/App/%.o User/App/%.su User/App/%.cyclo: ../User/App/%.c User/App/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/FreeRTOS/Inc" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/BSP" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/Task" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/App" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-App

clean-User-2f-App:
	-$(RM) ./User/App/Data_App.cyclo ./User/App/Data_App.d ./User/App/Data_App.o ./User/App/Data_App.su

.PHONY: clean-User-2f-App

