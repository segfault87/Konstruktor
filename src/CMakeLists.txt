include_directories(
    . 
    ../3rdparty
)

subdirs(libldr)
subdirs(renderer)
subdirs(app)

if(COMPILE_TESTS)
subdirs(tests)
endif(COMPILE_TESTS)


