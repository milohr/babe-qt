cmake_minimum_required (VERSION 2.8.12)
project (Babe)

find_package (ECM 0.0.11 REQUIRED NO_MODULE)
set (
   CMAKE_MODULE_PATH
   ${ECM_MODULE_PATH}
   ${ECM_KDE_MODULE_DIR}
   ${CMAKE_CURRENT_SOURCE_DIR}/cmake/modules
   )

include (KDEInstallDirs)
include (KDECMakeSettings)
include (KDECompilerSettings NO_POLICY_SCOPE)
# include (ECMInstallIcons)
# include (ECMMarkAsTest)
# include (ECMMarkNonGuiExecutable)
# include (ECMOptionalAddSubdirectory)
# include (ECMQtDeclareLoggingCategory)
include (FeatureSummary)
include (CheckIncludeFiles)



# Add the build directories to the incudes
set (CMAKE_INCLUDE_CURRENT_DIR ON)

# Run MOC for everything that needs it
set (CMAKE_AUTOMOC ON)

set (QT_MIN_VERSION "5.5.0")
set (KF5_MIN_VERSION "5.24.0")

# Find the required Qt libraries
find_package (
   Qt5 ${QT_MIN_VERSION} CONFIG REQUIRED COMPONENTS
   Core
   Gui
   Widgets
   Multimedia
   Sql
   Network
   Xml
   DBus
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

set (
   babe_SRCS
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
   artwork.cpp
   infoview.cpp
   lyrics.cpp
   mpris2.cpp
   notify.cpp
   youtube.cpp
   metadataForm.cpp
   )

   
install(FILES org.kde.babe.appdata.xml DESTINATION ${KDE_INSTALL_METAINFODIR})
# install_it.path = $$OUT_PWD
# install_it.files = data/*
#
# unix:!macx: LIBS += -L$$PWD/3rdparty/taglib-1.9.1/taglib/ -ltag
# INCLUDEPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers
# DEPENDPATH += $$PWD/3rdparty/taglib-1.9.1/taglib/Headers


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

ki18n_wrap_ui (
   babe_SRCS
   mainwindow.ui
   settings.ui
   about.ui
   infoview.ui
   metadataForm.ui
   )

qt5_add_resources (
   babe_SRCS
   data.qrc
   )

add_executable (
   babe
   ${babe_SRCS}
   )

install(TARGETS babe ${KDE_INSTALL_TARGETS_DEFAULT_ARGS})

target_link_libraries (
   babe
   Qt5::Core
   Qt5::Gui
   Qt5::Widgets
   Qt5::Multimedia
   Qt5::Sql
   Qt5::Network
   Qt5::Xml
   Qt5::DBus

   KF5::Notifications
   KF5::I18n
   KF5::ConfigCore
   KF5::ConfigGui

   ${TAGLIB_LIBRARIES}
   )

feature_summary (
   WHAT ALL
   INCLUDE_QUIET_PACKAGES
   FATAL_ON_MISSING_REQUIRED_PACKAGES
   )
