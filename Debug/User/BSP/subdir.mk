################################################################################
# Automatically-generated file. Do not edit!
# Toolchain: GNU Tools for STM32 (13.3.rel1)
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../User/BSP/Buzzer.c \
../User/BSP/DHT22.c \
../User/BSP/Delay_us.c \
../User/BSP/Key.c \
../User/BSP/LED.c \
../User/BSP/MyADC.c \
../User/BSP/OLED.c \
../User/BSP/Sensor.c 

OBJS += \
./User/BSP/Buzzer.o \
./User/BSP/DHT22.o \
./User/BSP/Delay_us.o \
./User/BSP/Key.o \
./User/BSP/LED.o \
./User/BSP/MyADC.o \
./User/BSP/OLED.o \
./User/BSP/Sensor.o 

C_DEPS += \
./User/BSP/Buzzer.d \
./User/BSP/DHT22.d \
./User/BSP/Delay_us.d \
./User/BSP/Key.d \
./User/BSP/LED.d \
./User/BSP/MyADC.d \
./User/BSP/OLED.d \
./User/BSP/Sensor.d 


# Each subdirectory must supply rules for building sources it contributes
User/BSP/%.o User/BSP/%.su User/BSP/%.cyclo: ../User/BSP/%.c User/BSP/subdir.mk
	arm-none-eabi-gcc "$<" -mcpu=cortex-m3 -std=gnu11 -g3 -DDEBUG -DUSE_HAL_DRIVER -DSTM32F103xB -c -I../Core/Inc -I../Drivers/STM32F1xx_HAL_Driver/Inc/Legacy -I../Drivers/STM32F1xx_HAL_Driver/Inc -I../Drivers/CMSIS/Device/ST/STM32F1xx/Include -I../Drivers/CMSIS/Include -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/FreeRTOS/Inc" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/BSP" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/Task" -I"C:/ST/STM32CubeIDE_1.19.0/STM32CubeIDE/workspace/HotelFreeRTOS/User/App" -O0 -ffunction-sections -fdata-sections -Wall -fstack-usage -fcyclomatic-complexity -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" --specs=nano.specs -mfloat-abi=soft -mthumb -o "$@"

clean: clean-User-2f-BSP

clean-User-2f-BSP:
	-$(RM) ./User/BSP/Buzzer.cyclo ./User/BSP/Buzzer.d ./User/BSP/Buzzer.o ./User/BSP/Buzzer.su ./User/BSP/DHT22.cyclo ./User/BSP/DHT22.d ./User/BSP/DHT22.o ./User/BSP/DHT22.su ./User/BSP/Delay_us.cyclo ./User/BSP/Delay_us.d ./User/BSP/Delay_us.o ./User/BSP/Delay_us.su ./User/BSP/Key.cyclo ./User/BSP/Key.d ./User/BSP/Key.o ./User/BSP/Key.su ./User/BSP/LED.cyclo ./User/BSP/LED.d ./User/BSP/LED.o ./User/BSP/LED.su ./User/BSP/MyADC.cyclo ./User/BSP/MyADC.d ./User/BSP/MyADC.o ./User/BSP/MyADC.su ./User/BSP/OLED.cyclo ./User/BSP/OLED.d ./User/BSP/OLED.o ./User/BSP/OLED.su ./User/BSP/Sensor.cyclo ./User/BSP/Sensor.d ./User/BSP/Sensor.o ./User/BSP/Sensor.su

.PHONY: clean-User-2f-BSP

