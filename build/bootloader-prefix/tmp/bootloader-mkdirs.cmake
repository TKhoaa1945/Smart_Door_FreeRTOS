# Distributed under the OSI-approved BSD 3-Clause License.  See accompanying
# file Copyright.txt or https://cmake.org/licensing for details.

cmake_minimum_required(VERSION 3.5)

file(MAKE_DIRECTORY
  "C:/Users/haoha/esp/v5.2.3/esp-idf/components/bootloader/subproject"
  "D:/Documents/htn/Smart_Door/build/bootloader"
  "D:/Documents/htn/Smart_Door/build/bootloader-prefix"
  "D:/Documents/htn/Smart_Door/build/bootloader-prefix/tmp"
  "D:/Documents/htn/Smart_Door/build/bootloader-prefix/src/bootloader-stamp"
  "D:/Documents/htn/Smart_Door/build/bootloader-prefix/src"
  "D:/Documents/htn/Smart_Door/build/bootloader-prefix/src/bootloader-stamp"
)

set(configSubDirs )
foreach(subDir IN LISTS configSubDirs)
    file(MAKE_DIRECTORY "D:/Documents/htn/Smart_Door/build/bootloader-prefix/src/bootloader-stamp/${subDir}")
endforeach()
if(cfgdir)
  file(MAKE_DIRECTORY "D:/Documents/htn/Smart_Door/build/bootloader-prefix/src/bootloader-stamp${cfgdir}") # cfgdir has leading slash
endif()
