execute_process(COMMAND "pip3 install jinja2")
execute_process(COMMAND "pip3 install regex")

function(add_fidl_dependencies target fidl_files generators_path)
    message(STATUS "target is ${target}")
    message(STATUS "fidl_files is ${fidl_files}")
    message(STATUS "generators_path is ${generators_path}")
    foreach(FIDL ${fidl_files})
        message(STATUS "FIDL is ${FIDL}")
        get_filename_component(FIDL_NAME_WITH_EXTENTION ${FIDL} NAME)
        string(REPLACE ".fidl" "" FIDL_NAME ${FIDL_NAME_WITH_EXTENTION})
        message(STATUS "FIDL_NAME is ${FIDL_NAME}")
        message(STATUS "COMMONAPI_GENERATOR is ${COMMONAPI_GENERATOR}")
        message(STATUS "COMMONAPI_DBUS_GENERATOR is ${COMMONAPI_DBUS_GENERATOR}")
        add_custom_command(OUTPUT ${FIDL_NAME}
                DEPENDS ${FIDL}
                COMMAND touch ${FIDL_NAME}
                COMMAND ${COMMONAPI_GENERATOR} --dest ${generators_path} -sk ${FIDL}
                COMMAND ${COMMONAPI_DBUS_GENERATOR} --dest ${generators_path} ${FIDL}
                )
        set(GEN_FIDL_FILES ${GEN_FIDL_FILES} ${FIDL_NAME})
    endforeach()
    message(STATUS "GEN_FIDL_FILES is ${GEN_FIDL_FILES}")
    add_custom_target(${target}_fidl_files_gen
            DEPENDS ${GEN_FIDL_FILES}
            COMMAND echo "Do it ...")
    foreach(FIDL ${fidl_files})
        message(STATUS "ICC_SOURCE_DIR is ${ICC_SOURCE_DIR}")
        message(STATUS "Started generate CommonAPIWrappers")
        message(STATUS "/home/redra/Projects/ICC/src/commonapi/commonapi_tools/commonapi_tools.py")
        message(STATUS "FIDL is ${FIDL}")
        get_filename_component(FIDL_NAME_WITH_EXTENTION ${FIDL} NAME)
        string(REPLACE ".fidl" "" FIDL_NAME ${FIDL_NAME_WITH_EXTENTION})
        message(STATUS "FIDL_NAME is ${FIDL_NAME}")
        add_custom_command(OUTPUT ${FIDL_NAME}CommonAPIWrappers
                DEPENDS ${target}_fidl_files_gen
                COMMAND touch ${FIDL_NAME}CommonAPIWrappers
                COMMAND python3 ${ICC_SOURCE_DIR}/src/commonapi/commonapi_tools/commonapi_tools.py
                ${FIDL}
                ${CMAKE_BINARY_DIR}
                --capi_client ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIClient.hpp.jinja2
                --capi_service ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIService.hpp.jinja2
                )
        set(GEN_FIDL_COMMONAPI_WRAPPER_FILES ${GEN_FIDL_COMMONAPI_WRAPPER_FILES} ${FIDL_NAME}CommonAPIWrappers)
    endforeach()
    message(STATUS "GEN_FIDL_COMMONAPI_WRAPPER_FILES is ${GEN_FIDL_COMMONAPI_WRAPPER_FILES}")
    add_custom_target(${target}_fidl_commonapi_wrappers_gen
            DEPENDS ${GEN_FIDL_COMMONAPI_WRAPPER_FILES}
            COMMAND echo "Do it ...")
    add_dependencies(${target} ${target}_fidl_files_gen ${target}_fidl_commonapi_wrappers_gen)

endfunction(add_fidl_dependencies)