# Build the app, using the HAL

include_directories(include)
file(GLOB MY_SOURCES "src/*.c")
add_executable(pentris ${MY_SOURCES})

# Make use of the HAL library
target_link_libraries(pentris LINK_PRIVATE hal pthread)

add_dependencies(pentris led_pru)

# Copy executable to final location (change `hello_world` to project name as needed)
add_custom_command(TARGET pentris POST_BUILD 
    COMMAND "${CMAKE_COMMAND}" -E copy 
    "$<TARGET_FILE:pentris>"
    "~/cmpt433/public/myApps/pentris" 
    COMMENT "Copying ARM executable to public NFS directory")
