if(NOT DEFINED QT_SECRET_SOURCE_DIR)
    message(FATAL_ERROR "QT_SECRET_SOURCE_DIR is required")
endif()

set(_quasar_targets_file "${QT_SECRET_SOURCE_DIR}/src/mini-gmp/CMake/QuasarAppCITargets.cmake")

if(NOT EXISTS "${_quasar_targets_file}")
    message(FATAL_ERROR "Qt-Secret QuasarAppCITargets.cmake was not found")
endif()

file(READ "${_quasar_targets_file}" _quasar_targets)

string(REPLACE "if(TARGET test)" "if(TARGET qt_secret_test)" _quasar_targets "${_quasar_targets}")
string(REPLACE "endif(TARGET test)" "endif(TARGET qt_secret_test)" _quasar_targets "${_quasar_targets}")
string(REPLACE "the test target already created!" "the qt_secret_test target already created!" _quasar_targets "${_quasar_targets}")
string(REPLACE "ADD_CUSTOM_TARGET(\n        test\n" "ADD_CUSTOM_TARGET(\n        qt_secret_test\n" _quasar_targets "${_quasar_targets}")
string(REPLACE "add_dependencies(test " "add_dependencies(qt_secret_test " _quasar_targets "${_quasar_targets}")

file(WRITE "${_quasar_targets_file}" "${_quasar_targets}")
