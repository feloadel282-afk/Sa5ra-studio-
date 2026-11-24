cmake_minimum_required(VERSION 3.22)
project(SA5RAStudio VERSION 0.2.0 LANGUAGES CXX)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

include(FetchContent)
FetchContent_Declare(
    JUCE
    GIT_REPOSITORY https://github.com/juce-framework/JUCE.git
    GIT_TAG 8.0.6
)
FetchContent_MakeAvailable(JUCE)

juce_add_gui_app(SA5RAStudio
    PRODUCT_NAME "SA5RA STUDIO"
    COMPANY_NAME "SA5RA Audio"
    BUNDLE_ID "com.sa5ra.audio.studio"
    VERSION "0.2.0"
    ICON_BIG "${CMAKE_CURRENT_SOURCE_DIR}/assets/SA5RAStudio.ico"
)

juce_generate_juce_header(SA5RAStudio)

target_sources(SA5RAStudio PRIVATE
    Source/Main.cpp
    Source/MainComponent.h
    Source/MainComponent.cpp
)

target_compile_definitions(SA5RAStudio PRIVATE
    JUCE_WEB_BROWSER=0
    JUCE_USE_CURL=0
    JUCE_APPLICATION_NAME_STRING="$<TARGET_PROPERTY:SA5RAStudio,JUCE_PRODUCT_NAME>"
    JUCE_APPLICATION_VERSION_STRING="${PROJECT_VERSION}"
)

target_link_libraries(SA5RAStudio PRIVATE
    juce::juce_audio_utils
    juce::juce_audio_formats
    juce::juce_dsp
    juce::juce_gui_extra
    juce::juce_recommended_config_flags
    juce::juce_recommended_lto_flags
    juce::juce_recommended_warning_flags
)

# Keep a predictable install layout for the Windows installer.
install(TARGETS SA5RAStudio
    RUNTIME DESTINATION .
    BUNDLE DESTINATION .
)
