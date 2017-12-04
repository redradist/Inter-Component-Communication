function(add_fidl_dependencies TARGET FIDL_FILES SERVICE_OR_CLIENT GENERATOR_PATH)
    message(STATUS "COMMONAPI_GENERATOR is ${COMMONAPI_GENERATOR}")
    message(STATUS "COMMONAPI_DBUS_GENERATOR is ${COMMONAPI_DBUS_GENERATOR}")
    message(STATUS "TARGET is ${TARGET}")
    message(STATUS "FIDL_FILES is ${FIDL_FILES}")
    message(STATUS "GENERATOR_PATH is ${GENERATOR_PATH}")
    set(COMMONAPI_GENERATED_FILES "")
    set(COMMONAPI_WRAPPER_GENERATED_FILES "")
    foreach(FIDL ${FIDL_FILES})
        message(STATUS "FIDL is ${FIDL}")
        get_filename_component(FIDL_NAME_WITH_EXTENTION ${FIDL} NAME)
        string(REPLACE ".fidl" "" FIDL_NAME ${FIDL_NAME_WITH_EXTENTION})
        message(STATUS "FIDL_NAME is ${FIDL_NAME}")
        execute_process(COMMAND python3 ${ICC_SOURCE_DIR}/src/commonapi/commonapi_tools/commonapi_hierarchy_name.py
                                ${FIDL}
                        OUTPUT_VARIABLE HIERARCHIC_PATH)
        string(REPLACE "\r" "" HIERARCHIC_PATH ${HIERARCHIC_PATH})
        string(REPLACE "\n" "" HIERARCHIC_PATH ${HIERARCHIC_PATH})
        message(STATUS "HIERARCHIC_PATH is ${HIERARCHIC_PATH}")
        if(${SERVICE_OR_CLIENT} EQUAL "Service")
            set(COMMONAPI_GENERATED_FILES
                ${COMMONAPI_GENERATED_FILES}
                "${CMAKE_BINARY_DIR}/${HIERARCHIC_PATH}/${FIDL_NAME}DBusStubAdapter.cpp"
                "${CMAKE_BINARY_DIR}/${HIERARCHIC_PATH}/${FIDL_NAME}StubDefault.cpp"
                "${CMAKE_BINARY_DIR}/${HIERARCHIC_PATH}/${FIDL_NAME}DBusDeployment.cpp")
        else(${SERVICE_OR_CLIENT} EQUAL "Service")
            set(COMMONAPI_GENERATED_FILES
                ${COMMONAPI_GENERATED_FILES}
                "${CMAKE_BINARY_DIR}/${HIERARCHIC_PATH}/${FIDL_NAME}DBusProxy.cpp"
                "${CMAKE_BINARY_DIR}/${HIERARCHIC_PATH}/${FIDL_NAME}DBusDeployment.cpp")
        endif(${SERVICE_OR_CLIENT} EQUAL "Service")
        set(COMMONAPI_WRAPPER_GENERATED_FILES
            ${COMMONAPI_WRAPPER_GENERATED_FILES}
            ${GENERATOR_PATH}/${FIDL_NAME}Client.hpp
            ${GENERATOR_PATH}/${FIDL_NAME}Service.hpp)
        add_custom_command(
            OUTPUT ${GENERATOR_PATH}/${FIDL_NAME}Client.hpp
            OUTPUT ${GENERATOR_PATH}/${FIDL_NAME}Service.hpp
            DEPENDS ${FIDL}
            COMMAND touch ${FIDL_NAME}CommonAPIWrappers
            COMMAND python3 ${ICC_SOURCE_DIR}/src/commonapi/commonapi_tools/commonapi_tools.py
            ${FIDL}
            ${GENERATOR_PATH}
            --capi_client ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIClient.hpp.jinja2
            --capi_service ${ICC_SOURCE_DIR}/src/commonapi/templates/CommonAPIService.hpp.jinja2
        )
    endforeach()
    set_source_files_properties(${COMMONAPI_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    set_source_files_properties(${COMMONAPI_WRAPPER_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    message(STATUS "COMMONAPI_GENERATED_FILES is ${COMMONAPI_GENERATED_FILES}")
    message(STATUS "COMMONAPI_WRAPPER_GENERATED_FILES is ${COMMONAPI_WRAPPER_GENERATED_FILES}")
    target_sources(${TARGET} PRIVATE ${COMMONAPI_GENERATED_FILES})
    target_sources(${TARGET} PRIVATE ${COMMONAPI_WRAPPER_GENERATED_FILES})
    add_custom_command(
        OUTPUT ${COMMONAPI_GENERATED_FILES}
        DEPENDS ${FIDL_FILES}
        COMMAND ${COMMONAPI_GENERATOR} --dest ${GENERATOR_PATH} -sk ${FIDL_FILES}
        COMMAND ${COMMONAPI_DBUS_GENERATOR} --dest ${GENERATOR_PATH} ${FIDL_FILES}
    )
    add_custom_target(
        ${TARGET}_commonapi_wrappers_gen
        DEPENDS ${COMMONAPI_GENERATED_FILES}
        DEPENDS ${COMMONAPI_WRAPPER_GENERATED_FILES}
        COMMAND echo "Generation of CommonAPI wrappers is finished")
    add_dependencies(${TARGET} ${TARGET}_commonapi_wrappers_gen)

endfunction(add_fidl_dependencies)