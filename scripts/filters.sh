filter_cmake_build_type()
{
    case ${BUILD_TYPE} in
        release|*)
            BUILD_TYPE="release"
            CMAKE_BUILD_TYPE="Release"
            ;;
        debug)
            CMAKE_BUILD_TYPE="Debug"
            ;;
    esac
}