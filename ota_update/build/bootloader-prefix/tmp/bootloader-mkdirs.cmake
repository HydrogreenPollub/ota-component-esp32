# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "/home/matqlorens/esp/v5.2.1/esp-idf/components/bootloader/subproject"
  "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader"
  "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix"
  "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix/tmp"
  "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix/src/bootloader-stamp"
  "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix/src"
  "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "/home/matqlorens/Projekty/ota-component-esp32/ota_update/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
