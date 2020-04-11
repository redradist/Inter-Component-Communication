function(add_fidl_sources TARGET FIDL_FILES SERVICE_OR_CLIENT)
    message(STATUS "TARGET is ${TARGET}")
    message(STATUS "FIDL_FILES is ${FIDL_FILES}")
    message(STATUS "SERVICE_OR_CLIENT is ${SERVICE_OR_CLIENT}")
    set(COMMONAPI_GENERATED_FILES "")
    foreach(FIDL ${FIDL_FILES})
        message(STATUS "FIDL is ${FIDL}")
        get_filename_component(FIDL_NAME_WITH_EXTENTION ${FIDL} NAME)
        string(REPLACE ".fidl" "" FIDL_NAME ${FIDL_NAME_WITH_EXTENTION})
        message(STATUS "FIDL_NAME is ${FIDL_NAME}")
        execute_process(COMMAND python3 ${ICC_SOURCE_DIR}/src/icc/interprocess/commonapi/interface_hierarchical_path.py
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
            string(REPLACE "." "" INTERFACE_NAME ${INTERFACE_NAME})
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
        endforeach()
    endforeach()
    set_source_files_properties(${COMMONAPI_GENERATED_FILES} PROPERTIES GENERATED TRUE)
    message(STATUS "COMMONAPI_GENERATED_FILES is ${COMMONAPI_GENERATED_FILES}")
    target_sources(${TARGET} PRIVATE ${COMMONAPI_GENERATED_FILES})

endfunction(add_fidl_sources)