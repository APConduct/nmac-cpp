add_executable(pattern_matching_test test_pattern_matching.cpp)

target_link_libraries(pattern_matching_test
        PRIVATE
        nmac
)

