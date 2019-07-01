set(include_dir ${PROJECT_SOURCE_DIR}/src)
set(CMAKE_LIBRARY_OUTPUT_DIRECTORY "${PROJECT_SOURCE_DIR}/build/Debug")
include_directories("${include_dir}" "${PROJECT_SOURCE_DIR}/deps/include")

set(HEADER_FILES ${include_dir}/sipster/sipster.h ${include_dir}/sipster/log.h ${include_dir}/sipster/sip.h ${include_dir}/sipster/sip_headers.h ${include_dir}/sipster/sdp.h
${include_dir}/sipster/sip_enums.h isaac/standard.h isaac/rand.h ${PROJECT_SOURCE_DIR}/deps/include/uuid.h sipster/sdp_enums.h sdp_private.h sipster/base.h)

set(sources sipster.cpp log.cpp sip.cpp utils.cpp sipster/utils.h sip_headers_private.h sip_headers.cpp sdp.cpp isaac/rand.c sipster/sdp_enums.h sdp_private.h sipster/sdp_headers.h sdp_headers.cpp base.cpp)

# Make a shared Lib
add_library (sipster SHARED ${sources} ${HEADER_FILES})
target_link_libraries(sipster uv ${PROJECT_SOURCE_DIR}/deps/libs/linux_x64/libuuid.so)
#add_library (sipster STATIC ${sources})
