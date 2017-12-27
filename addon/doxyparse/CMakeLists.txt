if (build_parse)

add_definitions(-std=c++11)

set(EXTRA_LIBS "-lyaml-cpp")

# configvalues.h
add_custom_command(
    COMMAND ${PYTHON_EXECUTABLE} ${CMAKE_SOURCE_DIR}/src/configgen.py -maph ${CMAKE_SOURCE_DIR}/src/config.xml > ${GENERATED_SRC}/configvalues.h
    DEPENDS ${CMAKE_SOURCE_DIR}/src/config.xml ${CMAKE_SOURCE_DIR}/src/configgen.py
    OUTPUT ${GENERATED_SRC}/configvalues.h
)
set_source_files_properties(${GENERATED_SRC}/configvalues.h PROPERTIES GENERATED 1)

find_package(Iconv)

include_directories(
	${CMAKE_SOURCE_DIR}/src
	${GENERATED_SRC}
	${CMAKE_SOURCE_DIR}/qtools
	${ICONV_INCLUDE_DIR}
	${CLANG_INCLUDEDIR}
)

add_executable(doxyparse
main.cpp
doxyparse.cpp
doxyparseconfig.cpp
doxyparseinput.cpp
doxyparseinterface.cpp
doxyparser.cpp
doxyparseresults.cpp
)

target_link_libraries(doxyparse
_doxygen
qtools
md5
doxycfg
vhdlparser
${ICONV_LIBRARIES}
${CMAKE_THREAD_LIBS_INIT}
${SQLITE3_LIBRARIES}
${EXTRA_LIBS}
${CLANG_LIBS}
)
set (CMAKE_CXX_FLAGS "-lyaml-cpp")

install(TARGETS doxyparse DESTINATION bin)

endif()
