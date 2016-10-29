################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../gizwits/gizwits_product.c \
../gizwits/gizwits_protocol.c 

OBJS += \
./gizwits/gizwits_product.o \
./gizwits/gizwits_protocol.o 

C_DEPS += \
./gizwits/gizwits_product.d \
./gizwits/gizwits_protocol.d 


# Each subdirectory must supply rules for building sources it contributes
gizwits/%.o: ../gizwits/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: Cross ARM C Compiler'
	arm-none-eabi-gcc -mcpu=cortex-m0plus -mthumb -O0 -fmessage-length=0 -fsigned-char -ffunction-sections -fdata-sections -Wall  -g3 -D"CPU_MKL46Z256VLH4" -I../CMSIS -I../gizwits -I../hal_key -I../board -I../drivers -I../startup -I../utilities -std=gnu99 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$@" -c -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


