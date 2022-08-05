#----------------------------------------------------------------------------
# Copyright Persistant Studios, SARL. All Rights Reserved.
# https://www.popcornfx.com/terms-and-conditions/
#----------------------------------------------------------------------------

if (PK_O3DE_MAJOR_VERSION GREATER_EQUAL 2205)
    add_compile_definitions(O3DE_DEV)
    set(FREETYPE_3RDPARTY 3rdParty::Freetype)
    o3de_pal_dir(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} "${gem_restricted_path}" "${gem_path}" "${gem_name}")
else ()
    set(FREETYPE_3RDPARTY 3rdParty::freetype)
    ly_get_list_relative_pal_filename(pal_dir ${CMAKE_CURRENT_LIST_DIR}/Platform/${PAL_PLATFORM_NAME} ${gem_restricted_path} ${gem_path} ${gem_name})
endif ()

# Now that we have the platform abstraction layer (PAL) folder for this folder, thats where we will find the
# traits for this platform. Traits for a platform are defines for things like whether or not something in this gem
# is supported by this platform.
include(${pal_dir}/PAL_${PAL_PLATFORM_NAME_LOWERCASE}.cmake)

# Add the DemoRoom.Static target
# Note: We include the common files and the platform specific files which are set in demoroom_common_files.cmake
ly_add_target(
    NAME DemoRoom.Static STATIC
    NAMESPACE Gem
    FILES_CMAKE
        demoroom_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PUBLIC
            ${FREETYPE_3RDPARTY}
            AZ::AzCore
            AZ::AzFramework
            Gem::AtomLyIntegration_CommonFeatures.Static
            Gem::PhysX.Static
            Gem::AtomFont
)

# Here add DemoRoom target, it depends on the DemoRoom.Static
ly_add_target(
    NAME DemoRoom ${PAL_TRAIT_MONOLITHIC_DRIVEN_MODULE_TYPE}
    NAMESPACE Gem
    FILES_CMAKE
        demoroom_shared_files.cmake
    INCLUDE_DIRECTORIES
        PUBLIC
            Include
        PRIVATE
            Source
    BUILD_DEPENDENCIES
        PRIVATE
            Gem::DemoRoom.Static
)

# By default, we will specify that the above target DemoRoom would be used by
# Client and Server type targets when this gem is enabled.  If you don't want it
# active in Clients or Servers by default, delete one of both of the following lines:
ly_create_alias(NAME DemoRoom.Clients NAMESPACE Gem TARGETS Gem::DemoRoom)
ly_create_alias(NAME DemoRoom.Servers NAMESPACE Gem TARGETS Gem::DemoRoom)

# If we are on a host platform, we want to add the host tools targets like the DemoRoom.Editor target which
# will also depend on DemoRoom.Static
if(PAL_TRAIT_BUILD_HOST_TOOLS)
    ly_add_target(
        NAME DemoRoom.Editor.Static STATIC
        NAMESPACE Gem
        AUTOMOC
        AUTORCC
        FILES_CMAKE
            demoroom_editor_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                AZ::AzToolsFramework
                Gem::DemoRoom.Static
    )

    ly_add_target(
        NAME DemoRoom.Editor GEM_MODULE
        NAMESPACE Gem
        AUTOMOC
        FILES_CMAKE
            demoroom_editor_shared_files.cmake
        INCLUDE_DIRECTORIES
            PRIVATE
                Source
            PUBLIC
                Include
        BUILD_DEPENDENCIES
            PUBLIC
                Gem::DemoRoom.Editor.Static
    )

    # By default, we will specify that the above target DemoRoom would be used by
    # Tool and Builder type targets when this gem is enabled.  If you don't want it
    # active in Tools or Builders by default, delete one of both of the following lines:
    ly_create_alias(NAME DemoRoom.Tools    NAMESPACE Gem TARGETS Gem::DemoRoom.Editor)
    ly_create_alias(NAME DemoRoom.Builders NAMESPACE Gem TARGETS Gem::DemoRoom.Editor)


endif()

################################################################################
# Tests
################################################################################
# See if globally, tests are supported
if(PAL_TRAIT_BUILD_TESTS_SUPPORTED)
    # We globally support tests, see if we support tests on this platform for DemoRoom.Static
    if(PAL_TRAIT_SHAPEEXAMPLE_TEST_SUPPORTED)
        # We support DemoRoom.Tests on this platform, add DemoRoom.Tests target which depends on DemoRoom.Static
        ly_add_target(
            NAME DemoRoom.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
            NAMESPACE Gem
            FILES_CMAKE
                demoroom_files.cmake
                demoroom_tests_files.cmake
            INCLUDE_DIRECTORIES
                PRIVATE
                    Tests
                    Source
            BUILD_DEPENDENCIES
                PRIVATE
                    AZ::AzTest
                    AZ::AzFramework
                    Gem::DemoRoom.Static
        )

        # Add DemoRoom.Tests to googletest
        ly_add_googletest(
            NAME Gem::DemoRoom.Tests
        )
    endif()

    # If we are a host platform we want to add tools test like editor tests here
    if(PAL_TRAIT_BUILD_HOST_TOOLS)
        # We are a host platform, see if Editor tests are supported on this platform
        if(PAL_TRAIT_SHAPEEXAMPLE_EDITOR_TEST_SUPPORTED)
            # We support DemoRoom.Editor.Tests on this platform, add DemoRoom.Editor.Tests target which depends on DemoRoom.Editor
            ly_add_target(
                NAME DemoRoom.Editor.Tests ${PAL_TRAIT_TEST_TARGET_TYPE}
                NAMESPACE Gem
                FILES_CMAKE
                    demoroom_editor_tests_files.cmake
                INCLUDE_DIRECTORIES
                    PRIVATE
                        Tests
                        Source
                BUILD_DEPENDENCIES
                    PRIVATE
                        AZ::AzTest
                        Gem::DemoRoom.Editor
            )

            # Add DemoRoom.Editor.Tests to googletest
            ly_add_googletest(
                NAME Gem::DemoRoom.Editor.Tests
            )
        endif()
    endif()
endif()
