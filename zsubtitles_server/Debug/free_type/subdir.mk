################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../free_type/zfree_type.c 

OBJS += \
./free_type/zfree_type.o 

C_DEPS += \
./free_type/zfree_type.d 


# Each subdirectory must supply rules for building sources it contributes
free_type/%.o: ../free_type/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	gcc -I"/home/shell.albert/project/subtitles/zsubtitles_server/inc" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_downloader" -I"/home/shell.albert/project/subtitles/zsubtitles_server/rle32" -I"/home/shell.albert/project/subtitles/zsubtitles_server/free_type" -I"/home/shell.albert/project/subtitles/zsubtitles_server/screen_simulator" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_parser" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_receiver" -I"/home/shell.albert/project/subtitles/zsubtitles_server/ringbuffer" -I"/home/shell.albert/project/subtitles/zsubtitles_server/inc" -I/usr/include/freetype2 -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


