#----------------------------------------------------------------------------
# Copyright Persistant Studios, SARL. All Rights Reserved.
# https://www.popcornfx.com/terms-and-conditions/
#----------------------------------------------------------------------------

set(gem_path ${CMAKE_CURRENT_LIST_DIR})
set(gem_json ${gem_path}/gem.json)
o3de_restricted_path(${gem_json} gem_restricted_path gem_parent_relative_path)

# Currently we are in the O3DEPopcornFXExamples/Code folder: ${CMAKE_CURRENT_LIST_DIR}
# Get the platform specific folder ${pal_dir} for the current folder: ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME}
# Note: ly_get_list_relative_pal_filename will take care of the details for us, as this may be a restricted platform
#       in which case it will see if that platform is present here or in the restricted folder.
#       i.e. It could here : O3DEPopcornFXExamples/Code/Platform/<platorm_name>  or
#            <restricted_folder>/<platform_name>/O3DEPopcornFXExamples/Code
o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} ${gem_restricted_path} ${gem_path} ${gem_parent_relative_path})

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this project
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Now that we have loaded our project traits for this platform, see if this project is even supported on this platform.
# If its not supported we just return after including the unsupported.
if(NOT PAL_TRAIT_O3DEPOPCORNFXEXAMPLES_SUPPORTED)
    return()
endif()

# We are on a supported platform, so add the O3DEPopcornFXExamples target
# Note: We include the common files and the platform specific files which are set in o3depopcornfxexamples_files.cmake and
# in ${pal_dir}/o3depopcornfxexamples_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
ly_add_target(
    NAME O3DEPopcornFXExamples.Static STATIC
    NAMESPACE Gem
    FILES_CMAKE
        o3depopcornfxexamples_files.cmake
        ${pal_dir}/o3depopcornfxexamples_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
    BUILD_DEPENDENCIES
        PRIVATE
            AZ::AzGameFramework
            Gem::Atom_AtomBridge.Static
            Gem::DemoRoom
)

ly_add_target(
    NAME O3DEPopcornFXExamples ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        o3depopcornfxexamples_shared_files.cmake
        ${pal_dir}/o3depopcornfxexamples_shared_${PAL_PLATFORM_NAME_LOWERCASE}_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
    BUILD_DEPENDENCIES
        PRIVATE
            Gem::O3DEPopcornFXExamples.Static
            AZ::AzCore
)

# if enabled, O3DEPopcornFXExamples is used by all kinds of applications
ly_create_alias(NAME O3DEPopcornFXExamples.Builders NAMESPACE Gem TARGETS Gem::O3DEPopcornFXExamples)
ly_create_alias(NAME O3DEPopcornFXExamples.Tools    NAMESPACE Gem TARGETS Gem::O3DEPopcornFXExamples)
ly_create_alias(NAME O3DEPopcornFXExamples.Clients  NAMESPACE Gem TARGETS Gem::O3DEPopcornFXExamples)
ly_create_alias(NAME O3DEPopcornFXExamples.Servers  NAMESPACE Gem TARGETS Gem::O3DEPopcornFXExamples)

################################################################################
# Gem dependencies
################################################################################

# Enable the specified list of gems from GEM_FILE or GEMS list for this specific project:
ly_enable_gems(PROJECT_NAME O3DEPopcornFXExamples GEM_FILE enabled_gems.cmake)
