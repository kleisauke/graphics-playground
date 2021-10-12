FetchContent_Declare(corrade
    GIT_REPOSITORY https://github.com/mosra/corrade.git
    GIT_TAG bb626d6
    #[[GIT_TAG v2020.06]]
    GIT_SUBMODULES "")

set(WITH_INTERCONNECT OFF CACHE BOOL "Disable Interconnect library" FORCE)
set(WITH_TESTSUITE OFF CACHE BOOL "Disable TestSuite library" FORCE)
set(BUILD_DEPRECATED OFF CACHE BOOL "Exclude deprecated API in the build" FORCE)

FetchContent_MakeAvailable(corrade)
