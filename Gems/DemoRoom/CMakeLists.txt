#----------------------------------------------------------------------------
# Copyright Persistant Studios, SARL. All Rights Reserved.
# https://www.popcornfx.com/terms-and-conditions/
#----------------------------------------------------------------------------

#List of supported O3DE versions, uncomment one of the line to force the version
#set(PK_O3DE_ENGINE_VERSION "2210.0")
#set(PK_O3DE_ENGINE_VERSION "2205.0")
#set(PK_O3DE_ENGINE_VERSION "2111.2")
#set(PK_O3DE_ENGINE_VERSION "2111.1")

#Latest supported O3DE version
set(PK_O3DE_ENGINE_VERSION_LATEST "2210.0")

if (${PK_O3DE_ENGINE_VERSION})
    set(o3de_build_number ${PK_O3DE_ENGINE_VERSION})
else()
    ly_file_read(${LY_ROOT_FOLDER}/engine.json manifest_json_data)

    # Use the engine.json file version to determine the version and build fields
    string(JSON file_version ERROR_VARIABLE manifest_json_error GET ${manifest_json_data} "file_version")
    if(file_version GREATER_EQUAL 2)
        set(o3de_build_number_key "build")
        set(o3de_display_version_key "display_version")
        set(o3de_version_key "version")
    else()
        set(o3de_build_number_key "O3DEBuildNumber")
        set(o3de_display_version_key "O3DEVersion")
        set(o3de_version_key "O3DEVersion")
    endif()

    string(JSON o3de_display_version ERROR_VARIABLE manifest_json_error GET ${manifest_json_data} ${o3de_display_version_key})

    # During development the engine display version is not used
    if (o3de_display_version STREQUAL "00.00" OR o3de_display_version STREQUAL "0.1.0.0")
        set(PK_O3DE_DEVELOPMENT ON)
        set(o3de_build_number ${PK_O3DE_ENGINE_VERSION_LATEST})
    else ()
        string(JSON o3de_build_number ERROR_VARIABLE manifest_json_error GET ${manifest_json_data} ${o3de_build_number_key})
        if (manifest_json_error OR o3de_build_number STREQUAL "" OR o3de_build_number STREQUAL "0")
            message(FATAL_ERROR "Invalid ${o3de_build_number_key} or ${o3de_display_version_key} in 'engine.json'. Please set PK_O3DE_ENGINE_VERSION in the PopcornFX CMakeLists.txt.")
        endif()
    endif()
endif()

string(REPLACE "." ";" o3de_version_list ${o3de_build_number})
list(GET o3de_version_list 0 PK_O3DE_MAJOR_VERSION)
list(GET o3de_version_list 1 PK_O3DE_PATCH_VERSION)

if (${PK_O3DE_DEVELOPMENT})
    MATH(EXPR PK_O3DE_MAJOR_VERSION "${PK_O3DE_MAJOR_VERSION}+1")
endif()

set(gem_path ${CMAKE_CURRENT_LIST_DIR})
set(gem_json ${gem_path}/gem.json)
o3de_restricted_path(${gem_json} gem_restricted_path gem_parent_relative_path)

if (PK_O3DE_MAJOR_VERSION GREATER_EQUAL 2205)
    o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" "${gem_path}" "${gem_parent_relative_path}")
else ()
    o3de_read_json_key(gem_name ${gem_json} "gem_name")
    ly_get_list_relative_pal_filename(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" ${gem_path} ${gem_name})
endif ()

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# project cmake for this platform.
include(${pal_dir}/${PAL_PLATFORM_NAME_LOWERCASE}_gem.cmake)

add_subdirectory(Code)
