cmake_minimum_required(VERSION 3.19)
project(Chatbot-mini-qt LANGUAGES CXX)

#  Thêm Network vào đây
find_package(Qt6 6.5 REQUIRED COMPONENTS Core Widgets Network)

qt_standard_project_setup()

qt_add_executable(Chatbot-mini-qt
    WIN32 MACOSX_BUNDLE
    main.cpp
    mainwindow.cpp
    mainwindow.h
    mainwindow.ui
)

#  Link thêm Qt::Network
target_link_libraries(Chatbot-mini-qt
    PRIVATE
        Qt::Core
        Qt::Widgets
        Qt::Network
)

include(GNUInstallDirs)

install(TARGETS Chatbot-mini-qt
    BUNDLE  DESTINATION .
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
)

qt_generate_deploy_app_script(
    TARGET Chatbot-mini-qt
    OUTPUT_SCRIPT deploy_script
    NO_UNSUPPORTED_PLATFORM_ERROR
)
install(SCRIPT ${deploy_script})
