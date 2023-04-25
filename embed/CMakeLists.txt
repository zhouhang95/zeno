add_executable(zenoembedtest main.cpp include/cobalt.h include/run.h)
target_include_directories(zenoembedtest PRIVATE include)
target_link_libraries(zenoembedtest PRIVATE zeno)
