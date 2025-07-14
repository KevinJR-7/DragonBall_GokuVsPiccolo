cmake_minimum_required(VERSION 3.16)

project(Final_Info_II LANGUAGES CXX)

set(CMAKE_AUTOUIC ON)
set(CMAKE_AUTOMOC ON)
set(CMAKE_AUTORCC ON)

set(CMAKE_CXX_STANDARD 17)
set(CMAKE_CXX_STANDARD_REQUIRED ON)

find_package(QT NAMES Qt6 Qt5 REQUIRED COMPONENTS Core Widgets Multimedia)
find_package(Qt${QT_VERSION_MAJOR} REQUIRED COMPONENTS Core Widgets Multimedia)


add_executable(Final_Info_II
  main.cpp
  game.h game.cpp
  game.ui
  personaje.h personaje.cpp
  goku.h goku.cpp
  habilidad.h habilidad.cpp
  kamehameha.h kamehameha.cpp
  blastb.h blastb.cpp
  Sprites.qrc

  piccolo.h piccolo.cpp
  rayo.h rayo.cpp
  mainmenu.h mainmenu.cpp
  gravityblast.h gravityblast.cpp
  kick.h kick.cpp


)
target_link_libraries(Final_Info_II
    Qt${QT_VERSION_MAJOR}::Core
    Qt${QT_VERSION_MAJOR}::Widgets
    Qt${QT_VERSION_MAJOR}::Multimedia
)


include(GNUInstallDirs)
install(TARGETS Final_Info_II
    LIBRARY DESTINATION ${CMAKE_INSTALL_LIBDIR}
    RUNTIME DESTINATION ${CMAKE_INSTALL_BINDIR}
)
