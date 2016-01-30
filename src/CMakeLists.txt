set(include_dir ${PROJECT_SOURCE_DIR}/src)

include_directories("${include_dir}")

set(HEADER_FILES ${include_dir}/sipster/sipster.h ${include_dir}/sipster/log.h ${include_dir}/sipster/sip.h ${include_dir}/sipster/sip_headers.h)

set(sources sipster.cpp log.cpp sip.cpp utils.cpp utils.h sip_headers.cpp)

# Make a shared Lib
add_library (sipster SHARED ${sources} ${HEADER_FILES})
#add_library (sipster STATIC ${sources})

