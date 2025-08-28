add_executable(qtdocxtemplate_tests test_main.cpp)
target_link_libraries(qtdocxtemplate_tests PRIVATE QtDocxTemplate::core)
add_test(NAME qtdocxtemplate_smoke COMMAND qtdocxtemplate_tests)

add_executable(package_test package_test.cpp)
target_include_directories(package_test PRIVATE ${CMAKE_SOURCE_DIR}/src)
target_link_libraries(package_test PRIVATE QtDocxTemplate::core Qt6::Core)
add_test(NAME qtdocxtemplate_package COMMAND package_test)
