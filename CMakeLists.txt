
#add_definitions(-DMP_32BIT)
set(SRC
        src/xxhash.c
        src/lz4.c
        src/lz4hc.c
        )

add_library(lz4 STATIC ${SRC})
target_include_directories(lz4 PUBLIC inc)

install(TARGETS lz4 EXPORT lz4
        LIBRARY DESTINATION lib
        ARCHIVE DESTINATION lib
        RUNTIME DESTINATION bin
        )
file(GLOB INC_FILES inc/*.h)
install(FILES ${INC_FILES} DESTINATION inc)
export(EXPORT lz4
        FILE "${CMAKE_CURRENT_BINARY_DIR}/cmake/ext/lz4.cmake"
        )

#tests
add_executable(lz4-test-fasttest tests/fasttest.c)
target_link_libraries(lz4-test-fasttest lz4)

add_executable(lz4-test tests/hc9.c)
target_link_libraries(lz4-test lz4)