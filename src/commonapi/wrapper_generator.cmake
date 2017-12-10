function(add_wrapper_dependencies TARGET FIDL_FILES SERVICE_OR_CLIENT IS_LOGGED GENERATOR_PATH)
    message(STATUS "COMMONAPI_GENERATOR is ${COMMONAPI_GENERATOR}")
    message(STATUS "COMMONAPI_DBUS_GENERATOR is ${COMMONAPI_DBUS_GENERATOR}")
    message(STATUS "TARGET is ${TARGET}")
    message(STATUS "FIDL_FILES is ${FIDL_FILES}")
    message(STATUS "SERVICE_OR_CLIENT is ${SERVICE_OR_CLIENT}")
    message(STATUS "GENERATOR_PATH is ${GENERATOR_PATH}")
    set(COMMONAPI_GENERATED_FILES "")
    set(COMMONAPI_WRAPPER_GENERATED_FILES "")
    foreach(FIDL ${FIDL_FILES})
        message(STATUS "FIDL is ${FIDL}")
        get_filename_component(FIDL_NAME_WITH_EXTENTION ${FIDL} NAME)
        string(REPLACE ".fidl" "" FIDL_NAME ${FIDL_NAME_WITH_EXTENTION})
        message(STATUS "FIDL_NAME is ${FIDL_NAME}")
        execute_process(COMMAND python3 ${ICC_SOURCE_DIR}/src/commonapi/interface_hierarchical_path.py
                                ${FIDL}
                        OUTPUT_VARIABLE HIERARCHIC_PATHS)
        string(REGEX MATCHALL "([^;]*);"
               INTERFACE_HIERARCHIC_PATHS "${HIERARCHIC_PATHS}")
        foreach(INTERFACE_HIERARCHIC_PATH ${INTERFACE_HIERARCHIC_PATHS})
            string(REPLACE "\r" "" INTERFACE_HIERARCHIC_PATH ${INTERFACE_HIERARCHIC_PATH})
            string(REPLACE "\n" "" INTERFACE_HIERARCHIC_PATH ${INTERFACE_HIERARCHIC_PATH})
            message(STATUS "INTERFACE_HIERARCHIC_PATH is ${INTERFACE_HIERARCHIC_PATH}")
            string(REGEX MATCH "/([^/]*)$"
                   INTERFACE_NAME "${INTERFACE_HIERARCHIC_PATH}")
            string(REPLACE "/" "" INTERFACE_NAME ${INTERFACE_NAME})
            message(STATUS "INTERFACE_NAME is ${INTERFACE_NAME}")
            if(${SERVICE_OR_CLIENT} STREQUAL "Service")
                set(COMMONAPI_GENERATED_FILES
                    ${COMMONAPI_GENERATED_FILES}
                    "${CMAKE_BINARY_DIR}/${INTERFACE_HIERARCHIC_PATH}DBusStubAdapter.cpp"
                    "${CMAKE_BINARY_DIR}/${INTERFACE_HIERARCHIC_PATH}StubDefault.cpp"
                    "${CMAKE_BINARY_DIR}/${INTERFACE_HIERARCHIC_PATH}DBusDeployment.cpp")
            else(${SERVICE_OR_CLIENT} STREQUAL "Service")
                set(COMMONAPI_GENERATED_FILES
                    ${COMMONAPI_GENERATED_FILES}
                    "${CMAKE_BINARY_DIR}/${INTERFACE_HIERARCHIC_PATH}DBusProxy.cpp"
                    "${CMAKE_BINARY_DIR}/${INTERFACE_HIERARCHIC_PATH}DBusDeployment.cpp")
            endif(${SERVICE_OR_CLIENT} STREQUAL "Service")
            set(COMMONAPI_WRAPPER_GENERATED_FILES
                ${COMMONAPI_WRAPPER_GENERATED_FILES}
                ${GENERATOR_PATH}/${INTERFACE_NAME}Client.hpp
                ${GENERATOR_PATH}/${INTERFACE_NAME}Service.hpp)
            if (${IS_LOGGED})
                message(STATUS "Logged wrappers was choosed")
                add_custom_command(
                        OUTPUT ${GENERATOR_PATH}/${INTERFACE_NAME}Client.hpp
                        OUTPUT ${GENERATOR_PATH}/${INTERFACE_NAME}Service.hpp
                        DEPENDS ${FIDL}
                        COMMAND touch ${INTERFACE_NAME}CommonAPIWrappers
                        COMMAND python3 ${ICC_SOURCE_DIR}/src/commonapi/commonapi_tools/wrapper_generator.py
                        ${FIDL}
                        ${GENERATOR_PATH}
                        --capi_client ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIClientLogged.hpp.jinja2
                        --capi_service ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIServiceLogged.hpp.jinja2
                )
            else()
                message(STATUS "Simple wrappers was choosed")
                add_custom_command(
                        OUTPUT ${GENERATOR_PATH}/${INTERFACE_NAME}Client.hpp
                        OUTPUT ${GENERATOR_PATH}/${INTERFACE_NAME}Service.hpp
                        DEPENDS ${FIDL}
                        COMMAND touch ${INTERFACE_NAME}CommonAPIWrappers
                        COMMAND python3 ${ICC_SOURCE_DIR}/src/commonapi/commonapi_tools/wrapper_generator.py
                        ${FIDL}
                        ${GENERATOR_PATH}
                        --capi_client ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIClient.hpp.jinja2
                        --capi_service ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIService.hpp.jinja2
                )
            endif ()

        endforeach()
    endforeach()
    set_source_files_properties(${COMMONAPI_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    set_source_files_properties(${COMMONAPI_WRAPPER_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    message(STATUS "COMMONAPI_GENERATED_FILES is ${COMMONAPI_GENERATED_FILES}")
    message(STATUS "COMMONAPI_WRAPPER_GENERATED_FILES is ${COMMONAPI_WRAPPER_GENERATED_FILES}")

    set(FILTERED_COMMONAPI_GENERATED_FILES "")
    foreach(COMMONAPI_GENERATED_FILE ${COMMONAPI_GENERATED_FILES})
        if(NOT TARGET ${COMMONAPI_GENERATED_FILE})
            set(FILTERED_COMMONAPI_GENERATED_FILES
                    ${FILTERED_COMMONAPI_GENERATED_FILES}
                    ${COMMONAPI_GENERATED_FILE})
        endif()
    endforeach()
    if(FILTERED_COMMONAPI_GENERATED_FILES)
        add_custom_command(
                OUTPUT ${FILTERED_COMMONAPI_GENERATED_FILES}
                DEPENDS ${FIDL_FILES}
                COMMAND ${COMMONAPI_GENERATOR} --dest ${GENERATOR_PATH} -sk ${FIDL_FILES}
                COMMAND ${COMMONAPI_DBUS_GENERATOR} --dest ${GENERATOR_PATH} ${FIDL_FILES}
        )
    endif()
    set_source_files_properties(${FILTERED_COMMONAPI_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    message(STATUS "FILTERED_COMMONAPI_GENERATED_FILES is ${FILTERED_COMMONAPI_GENERATED_FILES}")
    target_sources(${TARGET} PRIVATE ${FILTERED_COMMONAPI_GENERATED_FILES})
    target_sources(${TARGET} PRIVATE ${COMMONAPI_WRAPPER_GENERATED_FILES})
    add_custom_target(
        ${TARGET}_commonapi_wrappers_gen
        #DEPENDS ${COMMONAPI_GENERATED_FILES}
        DEPENDS ${COMMONAPI_WRAPPER_GENERATED_FILES}
        COMMAND echo "Generation of CommonAPI wrappers is finished")
    add_dependencies(${TARGET} ${TARGET}_commonapi_wrappers_gen)

endfunction(add_wrapper_dependencies)