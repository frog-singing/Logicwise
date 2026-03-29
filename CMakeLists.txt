# Logicwise/CMakeLists.txt
cmake_minimum_required(VERSION 3.21)
project(Logicwise_Repository LANGUAGES CXX)

if(PROJECT_IS_TOP_LEVEL) #VERSION 3.21+ 特性
    set(CMAKE_CXX_STANDARD 20)
    set(CMAKE_CXX_STANDARD_REQUIRED ON)
    set(CMAKE_CXX_EXTENSIONS OFF)
endif()


# 找到 external 文件夹下的 Manipond 仓库
if(EXISTS "${CMAKE_CURRENT_SOURCE_DIR}/external/Manipond/CMakeLists.txt")
    add_subdirectory(external/Manipond)
else()
    message(FATAL_ERROR "[Logicwise] External dependency Manipond NOT found at external/Manipond!")
endif()


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

    # 把接口头文件放在这里，使得IDE可见。不影响编译
    target_sources(logicwise_library INTERFACE


    )

    # 链接 Manipond 仓库中的库
	if(TARGET manipond::meta)
		target_link_libraries(logicwise_library INTERFACE manipond::meta)
	else()
		message(FATAL_ERROR "[Logicwise] External dependency manipond::meta is missing!")
	endif()

    message(STATUS "[Logicwise] Created logicwise::logicwise target.")
endif()
