cmake_minimum_required(VERSION 3.5)
SET(CMAKE_SYSTEM_NAME Generic)

project(automatic-usb-mains-switch C CXX ASM)

SET(DEVICE "attiny45")
SET(FREQ "16500000")

SET(CMAKE_ASM_COMPILER avr-gcc)
SET(CMAKE_C_COMPILER avr-gcc)
SET(CMAKE_CXX_COMPILER avr-g++)

SET(CMAKE_ASM_FLAGS "-mmcu=${DEVICE} -DF_CPU=${FREQ}UL -std=c18 -x assembler-with-cpp")
SET(CMAKE_C_FLAGS "-Os -mmcu=${DEVICE} -DF_CPU=${FREQ}UL -std=c18 -Wl,--gc-sections -Wall -Wextra -pedantic")
SET(CMAKE_CXX_FLAGS "-Os -mmcu=${DEVICE} -DF_CPU=${FREQ}UL -std=c++17 -Wl,--gc-sections -Wall -Wextra -pedantic -fno-exceptions -fno-rtti")

include_directories(usbdrv libs-device src)
SET(LIBRARY_SOURCES
        libs-device/osccal.c libs-device/osccal.h libs-device/osctune.h
        usbdrv/usbdrv.c usbdrv/usbdrvasm.S usbdrv/oddebug.c)
SET(SOURCES src/main.cpp src/usbconfig.h)

ADD_EXECUTABLE(${PROJECT_NAME} ${SOURCES} ${LIBRARY_SOURCES})

ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-objcopy -O ihex -R.eeprom ${CMAKE_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.hex)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-objcopy -O ihex -j .eeprom --set-section-flags=.eeprom="alloc,load" --change-section-lma .eeprom=0 ${CMAKE_BINARY_DIR}/${PROJECT_NAME} ${CMAKE_BINARY_DIR}/${PROJECT_NAME}.eep)
ADD_CUSTOM_COMMAND(TARGET ${PROJECT_NAME} POST_BUILD COMMAND avr-size ${CMAKE_BINARY_DIR}/${PROJECT_NAME} --mcu=${DEVICE} --format=avr)
