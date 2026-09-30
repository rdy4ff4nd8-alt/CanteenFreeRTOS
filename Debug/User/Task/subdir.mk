################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/Task/Alarm_Task.c \
../User/Task/Display_Task.c \
../User/Task/Key_Task.c \
../User/Task/Sensor_Task.c \
../User/Task/UART_Task.c 

OBJS += \
./User/Task/Alarm_Task.o \
./User/Task/Display_Task.o \
./User/Task/Key_Task.o \
./User/Task/Sensor_Task.o \
./User/Task/UART_Task.o 

C_DEPS += \
./User/Task/Alarm_Task.d \
./User/Task/Display_Task.d \
./User/Task/Key_Task.d \
./User/Task/Sensor_Task.d \
./User/Task/UART_Task.d 


# Each subdirectory must supply rules for building sources it contributes
User/Task/%.o User/Task/%.su User/Task/%.cyclo: ../User/Task/%.c User/Task/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/FreeRTOS/Inc" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/BSP" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/Task" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/App" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-Task

clean-User-2f-Task:
	-$(RM) ./User/Task/Alarm_Task.cyclo ./User/Task/Alarm_Task.d ./User/Task/Alarm_Task.o ./User/Task/Alarm_Task.su ./User/Task/Display_Task.cyclo ./User/Task/Display_Task.d ./User/Task/Display_Task.o ./User/Task/Display_Task.su ./User/Task/Key_Task.cyclo ./User/Task/Key_Task.d ./User/Task/Key_Task.o ./User/Task/Key_Task.su ./User/Task/Sensor_Task.cyclo ./User/Task/Sensor_Task.d ./User/Task/Sensor_Task.o ./User/Task/Sensor_Task.su ./User/Task/UART_Task.cyclo ./User/Task/UART_Task.d ./User/Task/UART_Task.o ./User/Task/UART_Task.su

.PHONY: clean-User-2f-Task

