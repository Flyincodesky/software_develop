
# 查找Qt5的Widgets模块（如果在主目录已查找，可省略此行）
find_package(Qt5Widgets REQUIRED)
# 添加子模块
# add_subdirectory(backuper)
# add_subdirectory(pack)
 
# 设置src中的源文件
set(SOURCES
    ${CMAKE_CURRENT_SOURCE_DIR}/filetools.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/backuper/backuper.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/pack/pack.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/fileExplor.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/mainWindow.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/restoreFile.cpp
    ${CMAKE_CURRENT_SOURCE_DIR}/window/backupedView.cpp

)

# 将src中的源文件添加到项目
# target_sources(Backuper PRIVATE ${SOURCES})

# 设置头文件
set(HEADER_FILES
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/AllQtHead.hpp  # include目录下的头文件
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/filetools.hpp 
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/backuper/backuper.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/pack/pack.hpp
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/fileExplor.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/mainWindow.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/restoreFile.hpp   
    ${CMAKE_CURRENT_SOURCE_DIR}/../include/window/backupedView.hpp   


)
 
# 定义可执行文件Backuper，添加源文件和头文件
target_sources(Backuper PRIVATE ${SOURCES} ${HEADER_FILES})

# 包含include目录，以便找到头文件
include_directories(${CMAKE_CURRENT_SOURCE_DIR}/../include)