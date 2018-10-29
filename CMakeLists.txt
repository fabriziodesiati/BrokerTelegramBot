# /* ========================================================================
#  * Author: Fabrizio De Siati
#  * Date  : 2018-10-16
#  * ======================================================================== */

# Configure minimum required version for CMake
cmake_minimum_required(VERSION 3.2)

# Declare project name and languages
project(BrokerTelegramBot)

# Declare name of main target (don't use '-' in names)
set(MAINTGT BrokerTelegramBot)
set(VERSION_VAR SPACEKIT_APP_VERSION)
set(COMPILE_DEF HAVE_CONFIG_H)
set(WEXCEPTIONS 0 CACHE STRING "Enable exceptions") # Enable Exceptions
set(WRTTI "-frtti" CACHE STRING "Enable RTTI")      # Enable RTTI
set(ISA "-mavx" CACHE STRING "Instruction set")
set(WARN_POL -Wall -Wno-missing-braces)

# Configure basic behaviors on include directories
set(CMAKE_MODULE_PATH "${${MAINTGT}_SOURCE_DIR}/cmake/modules")
set(CMAKE_INCLUDE_CURRENT_DIR ON)
set(CMAKE_INCLUDE_CURRENT_DIR_IN_INTERFACE ON)
set(CMAKE_EXPORT_COMPILE_COMMANDS ON)

# Define version of this project
set(APP_VMAJOR 1 CACHE STRING "Major application version")
set(APP_VMINOR 0 CACHE STRING "Minor application version")
set(APP_VMICRO 0 CACHE STRING "Micro application version")
set(APP_VPATCH 0 CACHE STRING "Patch application version")
set(${VERSION_VAR}
  "${APP_VMAJOR}.${APP_VMINOR}.${APP_VMICRO}"
  CACHE STRING "Application version string")

# Include specific helpers
include(GenerateExportHeader)
include(GetVersion)

# Load current baseline from current directory.
file(STRINGS ${CMAKE_SOURCE_DIR}/baselinetag BASELINE)

# Determine baseline tag.
get_version("git" ${BASELINE} ${${MAINTGT}_SOURCE_DIR})
set(APP_SOFTWARE_RELEASE ${SOURCE_VERSION})
set(APP_PLAIN_RELEASE ${PLAIN_VERSION})

# Define main project folder
set(MAIN_PRJ_FOLDER "${${MAINTGT}_SOURCE_DIR}")

# Help cmake to find Qt
if    (MSVC)
  set(CMAKE_PREFIX_PATH "C:/Qt/Qt5.11.1/5.11.1/msvc2017_64" CACHE PATH "Qt helper")
else  (MSVC)
  set(CMAKE_PREFIX_PATH "/opt/Qt/5.11.1/gcc_64" CACHE PATH "Qt helper")
endif (MSVC)

# Qt5 setup directives
set(CMAKE_AUTOMOC ON)
find_package(      Qt5Core REQUIRED)
find_package(       Qt5Gui REQUIRED)
find_package(   Qt5Network REQUIRED)
find_package(       Qt5Sql REQUIRED)
find_package(Qt5WebSockets REQUIRED)
find_package(   Qt5Widgets REQUIRED)

# Setup variables: source and include paths
set(SRC_PATH "src")
set(INC_PATH "inc")
set(UIF_PATH "ui")
set(QTTELEGRAMBOT_PATH "QtTelegramBot")

# Resources.
set(RES ${MAIN_PRJ_FOLDER}/${MAINTGT}.qrc)

# Update the central variable containing all external libraries
set(LIBS
  Qt5::Core
  Qt5::Gui
  Qt5::Network
  Qt5::Sql
  Qt5::WebSockets
  Qt5::Widgets
)

# Update the central variable containing all external includes
set(INCS
)

# Update the central variable containing all definitions
set(DEFS
)

# Update the central variable containing library directories
set(LIBDIRS
)

# Special tag for external libraries
#link_directories(${LIBDIRS})

# Setup variable containing source files
set(SOURCES
  #QTTELEGRAMBOT
  ${QTTELEGRAMBOT_PATH}/qttelegrambot.cpp
  ${QTTELEGRAMBOT_PATH}/networking.cpp
  ${QTTELEGRAMBOT_PATH}/types/message.cpp
  ${QTTELEGRAMBOT_PATH}/types/update.cpp
  ${QTTELEGRAMBOT_PATH}/types/chat.cpp
  ${QTTELEGRAMBOT_PATH}/types/user.cpp
  ${QTTELEGRAMBOT_PATH}/types/document.cpp
  ${QTTELEGRAMBOT_PATH}/types/photosize.cpp
  ${QTTELEGRAMBOT_PATH}/types/audio.cpp
  ${QTTELEGRAMBOT_PATH}/types/sticker.cpp
  ${QTTELEGRAMBOT_PATH}/types/video.cpp
  ${QTTELEGRAMBOT_PATH}/types/voice.cpp
  ${QTTELEGRAMBOT_PATH}/types/contact.cpp
  ${QTTELEGRAMBOT_PATH}/types/location.cpp
  ${QTTELEGRAMBOT_PATH}/types/callbackquery.cpp
  #APP
  ${SRC_PATH}/app_main.cpp
  ${SRC_PATH}/app_broker_binary.cpp
  ${SRC_PATH}/app_configuration.cpp
  ${SRC_PATH}/app_database.cpp  
  ${SRC_PATH}/app_model.cpp
  ${SRC_PATH}/app_telegram_bot.cpp
)

# Setup variable containing include files
set(INCLUDES
  #QTTELEGRAMBOT
  ${QTTELEGRAMBOT_PATH}/qttelegrambot.h
  ${QTTELEGRAMBOT_PATH}/networking.h
  ${QTTELEGRAMBOT_PATH}/types/message.h
  ${QTTELEGRAMBOT_PATH}/types/update.h
  ${QTTELEGRAMBOT_PATH}/types/chat.h
  ${QTTELEGRAMBOT_PATH}/types/user.h
  ${QTTELEGRAMBOT_PATH}/types/file.h
  ${QTTELEGRAMBOT_PATH}/types/document.h
  ${QTTELEGRAMBOT_PATH}/types/photosize.h
  ${QTTELEGRAMBOT_PATH}/types/audio.h
  ${QTTELEGRAMBOT_PATH}/types/sticker.h
  ${QTTELEGRAMBOT_PATH}/types/video.h
  ${QTTELEGRAMBOT_PATH}/types/voice.h
  ${QTTELEGRAMBOT_PATH}/types/contact.h
  ${QTTELEGRAMBOT_PATH}/types/location.h
  ${QTTELEGRAMBOT_PATH}/types/reply/genericreply.h
  ${QTTELEGRAMBOT_PATH}/types/reply/replykeyboardmarkup.h
  ${QTTELEGRAMBOT_PATH}/types/reply/replykeyboardhide.h
  ${QTTELEGRAMBOT_PATH}/types/reply/forcereply.h
  ${QTTELEGRAMBOT_PATH}/types/callbackquery.h
  ${QTTELEGRAMBOT_PATH}/types/reply/inlinekeyboardmarkup.h
  ${QTTELEGRAMBOT_PATH}/types/reply/replykeyboardremove.h
  #APP
  ${INC_PATH}/app_priv.h
  ${INC_PATH}/app_broker_binary.h
  ${INC_PATH}/app_configuration.h
  ${INC_PATH}/app_database.h
  ${INC_PATH}/app_model.h
  ${INC_PATH}/app_telegram_bot.h
)
# Setup variable containing UI files
set(GUIS
  ${UIF_PATH}/wdgmain.ui
  ${UIF_PATH}/wdgcentral.ui
)

# Prepare resources and user interface files
qt5_add_resources(RES_OUT ${RES})
qt5_wrap_ui(GUIS_H ${GUIS})

# Setup specific source code macros.
set(         APP_SYSTEM_NAME ${CMAKE_SYSTEM_NAME} CACHE STRING "Name")
set(                APP_TAGS "NONE" CACHE STRING "Tags")
set(     APP_LOGGING_ENABLED "1" CACHE STRING "Enable heavy logging.")

# Setup Windows specific stuff
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
    SET(RES_FILE "${MAINTGT}.rc")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Setup compile options
if (NOT ${WEXCEPTIONS})
  if     ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(COMPILE_OPT ${WARN_POL} -fno-exceptions ${WRTTI} ${ISA})
    set(APP_TAGS "NO-EX/NO-TI/${ISA}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
    set(COMPILE_OPT ${WARN_POL} -fno-exceptions ${WRTTI} ${ISA})
    set(APP_TAGS "NO-EX/NO-TI/${ISA}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(COMPILE_OPT /GR- /Oi)
    set(APP_TAGS "NO-TI/${ISA}")
  endif ()
else ()
  if     ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "GNU")
    set(COMPILE_OPT ${WARN_POL} ${WRTTI} ${ISA})
    set(APP_TAGS "NO-TI/${ISA}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" MATCHES ".*Clang")
    set(COMPILE_OPT ${WARN_POL} ${WRTTI} ${ISA})
    set(APP_TAGS "NO-TI/${ISA}")
  elseif ("${CMAKE_CXX_COMPILER_ID}" STREQUAL "MSVC")
    set(COMPILE_OPT /GR- /Oi)
    set(APP_TAGS "NO-TI/${ISA}")
  endif ()
endif ()
message("TAGS=${APP_TAGS}")

#
# Build configuration include file.
#
CONFIGURE_FILE(
  ${CMAKE_SOURCE_DIR}/config.h.cmake
  ${CMAKE_BINARY_DIR}/${MAINTGT}_config.h)

# Add target for main
add_executable(${MAINTGT} ${SOURCES} ${INCLUDES} ${GUIS_H} ${RES_OUT} ${RES_FILE})
set_property(TARGET ${MAINTGT} PROPERTY VERSION ${${VERSION_VAR}})
set_property(TARGET ${MAINTGT} PROPERTY SOVERSION ${APP_VMAJOR})
set_property(TARGET ${MAINTGT} PROPERTY
  INTERFACE_${MAINTGT}_MAJOR_VERSION ${APP_VMAJOR})
set_property(TARGET ${MAINTGT} APPEND PROPERTY
  COMPATIBLE_INTERFACE_STRING ${MAINTGT}_MAJOR_VERSION
)

# Setup include paths for target
target_include_directories(${MAINTGT} PRIVATE ${INC_PATH} ${IPTH} ${INCS})
target_compile_definitions(${MAINTGT} PRIVATE ${COMPILE_DEF} ${DEFS})
target_compile_options(${MAINTGT} PRIVATE ${COMPILE_OPT})
target_link_libraries(${MAINTGT} ${LIBS} ${LINKING_OPT})
set_property(TARGET ${MAINTGT} PROPERTY CXX_STANDARD 11)

# Setup executable deployment on Windows
if(${CMAKE_SYSTEM_NAME} MATCHES "Windows")
  set_target_properties(${MAINTGT} PROPERTIES
    LINK_FLAGS_DEBUG "/SUBSYSTEM:CONSOLE")
  set_target_properties(${MAINTGT} PROPERTIES
    RELWITHDEBINFO "/SUBSYSTEM:CONSOLE")
  set_target_properties(${MAINTGT} PROPERTIES
    LINK_FLAGS_RELEASE "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
  set_target_properties(${MAINTGT} PROPERTIES
    MINSIZEREL "/SUBSYSTEM:WINDOWS /ENTRY:mainCRTStartup")
endif(${CMAKE_SYSTEM_NAME} MATCHES "Windows")

# Installation of targets
install(TARGETS ${MAINTGT} EXPORT ${MAINTGT}Targets
  LIBRARY DESTINATION lib
  RUNTIME DESTINATION bin
  ARCHIVE DESTINATION lib)
install(DIRECTORY etc/ DESTINATION etc DIRECTORY_PERMISSIONS
  OWNER_WRITE OWNER_READ OWNER_EXECUTE
  GROUP_WRITE GROUP_READ GROUP_EXECUTE
  WORLD_WRITE WORLD_READ WORLD_EXECUTE)
install(DIRECTORY db/ DESTINATION db DIRECTORY_PERMISSIONS
  OWNER_WRITE OWNER_READ OWNER_EXECUTE
  GROUP_WRITE GROUP_READ GROUP_EXECUTE
  WORLD_WRITE WORLD_READ WORLD_EXECUTE)