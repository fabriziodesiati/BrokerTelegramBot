# /* ========================================================================
#  * PLANETEK SPACE KIT
#  * (C) 2016 by PLANETEK ITALIA
#  *
#  * Author: Fabrizio De Siati
#  * Date  : 2016-03-23
#  * ======================================================================== */

function(get_version CM_TYPE BASELINE WD)
  string(SUBSTRING ${CMAKE_HOST_SYSTEM} 0 3 HS)
if(${HS} STREQUAL "Win")
  execute_process(COMMAND ${WD}/getversiontag-${CM_TYPE}.bat
      WORKING_DIRECTORY ${WD}
      OUTPUT_VARIABLE CM_VERSION
      OUTPUT_STRIP_TRAILING_WHITESPACE)

  if ("${CM_VERSION}" STREQUAL "Unversioned directory")
  	set(CM_VERSION "0")
  endif()

  set(PLAIN_VERSION "${CM_VERSION}" PARENT_SCOPE)
  set(SOURCE_VERSION "${BASELINE}-${CM_VERSION}" PARENT_SCOPE)
  message("(WIN) VERSION=" ${CM_VERSION})
else()
  execute_process(COMMAND ${WD}/getversiontag-${CM_TYPE}
    	WORKING_DIRECTORY ${WD}
    	OUTPUT_VARIABLE CM_VERSION
    	OUTPUT_STRIP_TRAILING_WHITESPACE)

  if ("${CM_VERSION}" STREQUAL "Unversioned directory")
  	set(CM_VERSION "0")
  endif()

  set(PLAIN_VERSION "${CM_VERSION}" PARENT_SCOPE)
  set(SOURCE_VERSION "${BASELINE}-${CM_VERSION}" PARENT_SCOPE)
  message("(POX) VERSION=" ${CM_VERSION})
endif()
endfunction(get_version)
