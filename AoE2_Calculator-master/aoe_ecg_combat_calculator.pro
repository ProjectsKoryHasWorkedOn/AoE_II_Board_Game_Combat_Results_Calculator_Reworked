QT       += core gui
# Updated QT to a newer version (6.5.2)


# Set icon
RC_ICONS = swords_clashing.ico


greaterThan(QT_MAJOR_VERSION, 4): QT += widgets core multimedia

CONFIG += c++17

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    aboutwindow.cpp \
    aliases.cpp \
    backend/combatCalculator.cpp \
    backend/entity.cpp \
    backend/fileImporter.cpp \
    backend/modifiersCalculator.cpp \
    backend/run_game.cpp \
    cross-window_palette.cpp \
    csv/csv_count_with_text.cpp \
    csv/csv_entity.cpp \
    csv/csv_player_medieval_age.cpp \
    csv/csv_player_name.cpp \
    csv/entities.cpp \
    csv/player_medieval_ages.cpp \
    csv/player_names.cpp \
    file_paths.cpp \
    main.cpp \
    mainwindow.cpp \
    soundEffects.cpp \
    stream_buffer.cpp \
    uicoloring.cpp

HEADERS += \
    aboutwindow.h \
    aliases.h \
    backend/combatCalculator.h \
    backend/entity.h \
    backend/fileImporter.h \
    backend/modifiersCalculator.h \
    backend/run_game.h \
    cross-window_palette.h \
    csv/csv_count_with_text.hpp \
    csv/csv_entity.h \
    csv/csv_event.h \
    csv/csv_player_medieval_ages.h \
    csv/csv_player_name.h \
    csv/csv_technology.h \
    csv/entities.h \
    csv/events.h \
    csv/modifiers.h \
    csv/player_medieval_ages.h \
    csv/player_names.h \
    csv/technologies.h \
    file_paths.h \
    mainwindow.h \
    mainwindow.hpp \
    player.h \
    soundEffects.h \
    stream_buffer.h \
    uicoloring.h

FORMS += \
    aboutwindow.ui \
    mainwindow.ui

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    CMakeLists.txt

RESOURCES +=
