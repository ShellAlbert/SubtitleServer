################################################################################
# Automatically-generated file. Do not edit!
################################################################################

# Add inputs and outputs from these tool invocations to the build variables 
C_SRCS += \
../rle32/zrle32.c 

OBJS += \
./rle32/zrle32.o 

C_DEPS += \
./rle32/zrle32.d 


# Each subdirectory must supply rules for building sources it contributes
rle32/%.o: ../rle32/%.c
	@echo 'Building file: $<'
	@echo 'Invoking: GCC C Compiler'
	arm-linux-gcc -I/home/shell.albert/project/subtitles/libs/libfreetype2/include/freetype2 -I"/home/shell.albert/project/subtitles/zsubtitles_server/rle32" -I"/home/shell.albert/project/subtitles/zsubtitles_server/free_type" -I"/home/shell.albert/project/subtitles/zsubtitles_server/ringbuffer" -I"/home/shell.albert/project/subtitles/zsubtitles_server/screen_simulator" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_downloader" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_parser" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_receiver" -I"/home/shell.albert/project/subtitles/zsubtitles_server/ringbuffer" -I"/home/shell.albert/project/subtitles/zsubtitles_server/thread_receiver" -I/home/shell.albert/project/subtitles/libs/libzlib/include -I/home/shell.albert/project/subtitles/libs/libpng/include -I/home/shell.albert/project/subtitles/libs/libpng/include/libpng12 -I/home/shell.albert/project/subtitles/libs/libxml2/include/libxml2/ -I"/home/shell.albert/project/subtitles/zsubtitles_server/inc" -O0 -g3 -Wall -c -fmessage-length=0 -MMD -MP -MF"$(@:%.o=%.d)" -MT"$(@:%.o=%.d)" -o "$@" "$<"
	@echo 'Finished building: $<'
	@echo ' '


