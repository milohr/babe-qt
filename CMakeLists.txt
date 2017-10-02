cmake_minimum_required(VERSION 3.0)
project (Babe)

set (KF5_MIN_VERSION "5.24.0")
find_package (ECM ${KF5_MIN_VERSION} CONFIG REQUIRED)
set(CMAKE_MODULE_PATH ${ECM_MODULE_PATH} ${CMAKE_SOURCE_DIR}/cmake/modules)

include(KDEInstallDirs)
include(KDECMakeSettings)
include(KDECompilerSettings NO_POLICY_SCOPE)
include(ECMInstallIcons)
include(FeatureSummary)

set (QT_MIN_VERSION "5.5.0")
# Find the required Qt libraries
find_package (
   Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
   Core
   DBus
   Gui
   Multimedia
   Network
   Sql
   Widgets
   Xml
   WebEngineWidgets
   )

# Find the required KDE Frameworks
find_package (
   KF5 ${KF5_MIN_VERSION} REQUIRED COMPONENTS
   Notifications
   I18n
   Config
   )

find_package (Taglib REQUIRED)
include_directories (${TAGLIB_INCLUDES})

add_subdirectory(data)

set(babe_SRCS
  main.cpp
  mainwindow.cpp
  playlist.cpp
  track.cpp
  settings.cpp
  collectionDB.cpp
  about.cpp
  babetable.cpp
  album.cpp
  albumsview.cpp
  scrolltext.cpp
  playlistsview.cpp
  taginfo.cpp
  infoview.cpp
  mpris2.cpp
  notify.cpp
  youtube.cpp
  metadataForm.cpp
  web_jgm90.cpp
  rabbitview.cpp
  pulpo/pulpo.cpp
  pulpo/webengine.cpp
  pulpo/htmlparser.cpp
  pulpo/services/lastfmService.cpp
  pulpo/services/spotifyService.cpp
  pulpo/services/lyricwikiaService.cpp
  pulpo/services/geniusService.cpp
  playlistform.cpp

)

ki18n_wrap_ui(babe_SRCS
  mainwindow.ui
  settings.ui
  about.ui
  infoview.ui
  metadataForm.ui
  playlistform.ui
)

qt5_add_resources(babe_SRCS data.qrc)

add_definitions (-std=c++14)

# The following define makes your compiler emit warnings if you use
# any feature of Qt which as been marked as deprecated (the exact warnings
# depend on your compiler). Please consult the documentation of the
# deprecated API in order to know how to port your code away from it.
add_definitions (-DQT_DEPRECATED_WARNINGS)

# You can also make your code fail to compile if you use deprecated APIs.
# In order to do so, uncomment the following line.
# You can also select to disable deprecated APIs only up to a certain version of Qt.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0
#add_definitions (-DQT_DISABLE_DEPRECATED_BEFORE=0x060000)

add_executable(babe ${babe_SRCS})

target_link_libraries(babe
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Multimedia
   Qt5::Sql
   Qt5::Network
   Qt5::Xml
   Qt5::DBus
   Qt5::WebEngineWidgets

   KF5::Notifications
   KF5::I18n
   KF5::ConfigCore
   KF5::ConfigGui

   ${TAGLIB_LIBRARIES}
)

install(TARGETS babe ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

install(FILES org.kde.babe.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})

install(PROGRAMS org.kde.babe.desktop DESTINATION ${KDE_INSTALL_APPDIR})

feature_summary (
   WHAT ALL
   INCLUDE_QUIET_PACKAGES
   FATAL_ON_MISSING_REQUIRED_PACKAGES
   )
