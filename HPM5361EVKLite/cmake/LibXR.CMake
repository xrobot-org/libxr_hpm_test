set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

get_filename_component(
    LIBXR_HPM_TEST_ROOT
    "${CMAKE_CURRENT_LIST_DIR}/../.."
    ABSOLUTE
)
set(
    LIBXR_DIR
    "${LIBXR_HPM_TEST_ROOT}/libxr"
    CACHE PATH
    "Path to LibXR source tree"
)

if(NOT EXISTS "${LIBXR_DIR}/CMakeLists.txt")
    message(FATAL_ERROR "LibXR not found. Please set LIBXR_DIR correctly: ${LIBXR_DIR}")
endif()

set(LIBXR_SYSTEM None)
set(LIBXR_DRIVER hpm)
set(LIBXR_NO_EIGEN True)

add_subdirectory("${LIBXR_DIR}" "${CMAKE_CURRENT_BINARY_DIR}/libxr")

if(DEFINED HPM_SDK_LIB_ITF AND TARGET ${HPM_SDK_LIB_ITF})
    target_link_libraries(xr PUBLIC ${HPM_SDK_LIB_ITF})
endif()

if(TARGET app)
    target_link_libraries(app PUBLIC xr)
endif()

if(DEFINED APP_ELF_NAME AND TARGET ${APP_ELF_NAME})
    target_link_libraries(${APP_ELF_NAME} xr)
endif()

if(TARGET xr)
    set_source_files_properties(
        "${LIBXR_DIR}/src/core/libxr_mem.cpp"
        TARGET_DIRECTORY xr
        PROPERTIES COMPILE_OPTIONS -Wno-undef
    )
endif()
