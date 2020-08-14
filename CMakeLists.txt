cmake_minimum_required( VERSION 2.8 )

project(GauFreqScaler)
message(status "Project: GauFreqScaler")

include_directories(
${PROJECT_SOURCE_DIR}/include
)

set(SRC
${PROJECT_SOURCE_DIR}/src/GauFreqScaler.c
)

set(CMAKE_RUNTIME_OUTPUT_DIRECTORY 
${PROJECT_SOURCE_DIR}/bin
)

add_executable(
${PROJECT_NAME} ${SRC}
)

# uncomment the lines below if you want to delete temporary files
add_definitions(
-D DEL_TMP_OUT_FILES
)

