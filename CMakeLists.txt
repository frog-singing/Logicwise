# Logicwise/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(Logicwise_Repository LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL) #VERSION 3.21+ 特性
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()

# --------------------------------------------------------------------------------

function(logicwise_setup_testing)
    message(STATUS "[Logicwise] Build testing: ON")

    enable_testing()

    # 使用 GLOB_RECURSE 下探到 test/ 所有子目录
    # CONFIGURE_DEPENDS 确保在添加新文件时，CMake 会自动重新扫描
    file(GLOB_RECURSE TEST_SOURCES CONFIGURE_DEPENDS "${CMAKE_CURRENT_SOURCE_DIR}/test/*.cpp")

    foreach(test_file ${TEST_SOURCES})
        # 1. 获取相对路径，用于生成唯一的测试名称，防止重名冲突
        file(RELATIVE_PATH rel_path "${CMAKE_CURRENT_SOURCE_DIR}/test" ${test_file})
        string(REPLACE ".cpp" "" test_name_raw ${rel_path})

        # 将路径分隔符替换为下划线，让可执行文件名更规范
        string(REPLACE "/" "_" test_name ${test_name_raw})
        
        # 为了防冲突，给可执行文件加个前缀
        set(target_name "test_${test_name}")

        # 2. 创建测试可执行文件
        add_executable(${target_name} ${test_file})

        # 3. 链接库
        target_link_libraries(${target_name} PRIVATE logicwise::logicwise)

        # 4. 注册到 CTest 框架
        add_test(NAME ${target_name} COMMAND ${target_name})

        # 5. 添加 IDE 分组
        set_target_properties(${target_name} PROPERTIES FOLDER "Test")
    endforeach()
endfunction()

# --------------------------------------------------------------------------------

include(FetchContent)

# 定义一个变量，优先查找本地 external 目录
set(_manipond_local_path "${CMAKE_CURRENT_SOURCE_DIR}/external/Manipond")

# 只有当本地确实有 CMakeLists.txt 时，才使用本地仓库
if(EXISTS "${_manipond_local_path}/CMakeLists.txt")
    set(FETCHCONTENT_SOURCE_DIR_MANIPOND "${_manipond_local_path}" CACHE PATH "Force local Manipond")
    message(STATUS "[Logicwise] Using local external dependency Manipond from external/Manipond")
else()
    unset(FETCHCONTENT_SOURCE_DIR_MANIPOND CACHE)
    message(STATUS "[Logicwise] External dependency Manipond not found at external/Manipond, fetching from GitHub")
endif()

FetchContent_Declare(manipond

    # 关闭静默模式，让 Git 的下载进度直接刷在控制台上
    FETCHCONTENT_QUIET OFF

    GIT_REPOSITORY  "https://github.com/frog-singing/Manipond.git"
    GIT_TAG         "v1.0.0" # GIT_TAG 向后贪婪匹配参数，因此不要在此之后添加其他参数
)

FetchContent_MakeAvailable(manipond)

# --------------------------------------------------------------------------------

if(TARGET logicwise::logicwise)
	message(STATUS "[Logicwise] Using existing logicwise::logicwise target.")
else()
    # 定义一个接口库（Header-only 专用）
    add_library(logicwise_library INTERFACE)

    # 设置一个库别名
	add_library(logicwise::logicwise ALIAS logicwise_library)

    # 强制要求 C++20 特性传递
    target_compile_features(logicwise_library INTERFACE cxx_std_20)

    # 关闭编译器特定的扩展，确保代码的纯度和可移植性
	set_target_properties(logicwise_library PROPERTIES CXX_EXTENSIONS OFF)

    # 为当前库设置包含路径，并且任何链接此库的目标都将自动获得此路径
    target_include_directories(logicwise_library INTERFACE

		$<BUILD_INTERFACE:${CMAKE_CURRENT_SOURCE_DIR}/library>
		$<INSTALL_INTERFACE:include>
    )

    # 定义头文件实体列表
    set(logicwise_header_file

        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise.h"
        
        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise/rangewise.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise/mode.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise/arrangement.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise/wrapper.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise/container.h"
        "${CMAKE_CURRENT_SOURCE_DIR}/library/logicwise/relation.h"
    )

    # 将接口头文件列表注入目标，使得IDE可见。不影响编译
    target_sources(logicwise_library INTERFACE ${logicwise_header_file})

    # 设置 Source Group，在 IDE 中更整洁
    source_group(
        TREE "${CMAKE_CURRENT_SOURCE_DIR}/library"
        PREFIX "Header File"
        FILES ${logicwise_header_file}
    )

    # 链接 Manipond 仓库中的 exosuit 库
	if(TARGET manipond::exosuit)
		target_link_libraries(logicwise_library INTERFACE manipond::exosuit)
	else()
		message(FATAL_ERROR "[Logicwise] External dependency manipond::exosuit is missing!")
	endif()

    # 链接 Manipond 仓库中的 meta 库
	if(TARGET manipond::meta)
		target_link_libraries(logicwise_library INTERFACE manipond::meta)
	else()
		message(FATAL_ERROR "[Logicwise] External dependency manipond::meta is missing!")
	endif()

    message(STATUS "[Logicwise] Created logicwise::logicwise target.")
endif()

# --------------------------------------------------------------------------------

option(LOGICWISE_BUILD_TESTING "Build testing for Logicwise" ${PROJECT_IS_TOP_LEVEL})

if(LOGICWISE_BUILD_TESTING)
    logicwise_setup_testing()
endif()
