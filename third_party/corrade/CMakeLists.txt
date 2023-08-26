FetchContent_Declare(corrade
    GIT_REPOSITORY https://github.com/mosra/corrade.git
    GIT_TAG 183b375
    #[[GIT_TAG v2020.06]]
    GIT_SUBMODULES "")

set(CORRADE_WITH_INTERCONNECT OFF CACHE BOOL "Disable Interconnect library" FORCE)
set(CORRADE_WITH_TESTSUITE OFF CACHE BOOL "Disable TestSuite library" FORCE)
set(CORRADE_BUILD_DEPRECATED OFF CACHE BOOL "Exclude deprecated API in the build" FORCE)

FetchContent_MakeAvailable(corrade)
