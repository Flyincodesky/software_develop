# 指定CMake的最低版本要求为3.5。
cmake_minimum_required(VERSION 3.5)
#定义项目名称为 Backuper 指定语言为 C++。
project(Backuper LANGUAGES CXX)
# 设定编译的c标准
set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON) # 设置是否严格要求指定的 C++ 标准（如 C++17）  ON(开)==是
set(CMAKE_CXX_EXTENSIONS OFF)  # 控制是否允许编译器使用与指定 C++ 标准的扩展。 保证代码可移植性

# 查找Qt5的Widgets模块
find_package(Qt5Widgets REQUIRED)

set(CMAKE_AUTOMOC ON) # 启用自动处理 MOC
set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTORCC ON)




# 定义一个可执行文件，命名为 Backuper, 并指定源文件 main.cpp           定义可执行文件，编译src/CMakeLists.txt中的源文件
add_executable(Backuper main.cpp)

# 添加子目录src
add_subdirectory(src)

# 链接Qt库
target_link_libraries(Backuper Qt5::Widgets)

