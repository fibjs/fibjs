filter_cmake_build_type()
{
    case ${BUILD_TYPE} in
        clean)
            ;;
        debug)
            CMAKE_BUILD_TYPE="Debug"
            ;;
        release|*)
            BUILD_TYPE="release"
            CMAKE_BUILD_TYPE="Release"
            ;;
    esac
}