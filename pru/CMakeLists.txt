add_custom_target(led_pru
    COMMAND ${CMAKE_COMMAND} -E copy_directory
    "${CMAKE_CURRENT_LIST_DIR}"
    "~/cmpt433/public/pru/led_pru"
    COMMENT "Copying PRU files to public NFS directory"
)

add_dependencies(led_pru led_pru_shared)

add_custom_target(led_pru_shared
    COMMAND ${CMAKE_COMMAND} -E copy
    "${CMAKE_SOURCE_DIR}/hal/include/hal/pru_shared.h"
    "~/cmpt433/public/pru/led_pru/pru_shared.h"
    COMMENT "Copying shared PRU file to NFS directory"
)