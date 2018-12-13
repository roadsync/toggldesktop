add_definitions(-DAPP_VERSION="${TOGGL_VERSION}")

if(TOGGL_PRODUCTION_BUILD)
    add_definitions(
        -DAPP_ENVIRONMENT="production"
        -DTOGGL_PRODUCTION_BUILD=1
    )
else()
    add_definitions(-DAPP_ENVIRONMENT="development")
endif()

if(TOGGL_ALLOW_UPDATE_CHECK)
    add_definitions(-DTOGGL_ALLOW_UPDATE_CHECK=1)
endif()

# And include dirs
include_directories(
    ${TOGGLDESKTOPLIBRARY_INCLUDE_DIR}
    ${OAUTH_INCLUDE_DIR}
    ${BUGSNAG_INCLUDE_DIR}
    ${QXT_INCLUDE_DIRS}
)

# TogglDesktop sources
set(BINARY_SOURCE_FILES
    aboutdialog.cpp
    autocompleteview.cpp
    clickablelabel.cpp
    colorpicker.cpp
    countryview.cpp
    errorviewcontroller.cpp
    feedbackdialog.cpp
    genericview.cpp
    idlenotificationdialog.cpp
    loginwidget.cpp
    main.cpp
    mainwindowcontroller.cpp
    overlaywidget.cpp
    preferencesdialog.cpp
    settingsview.cpp
    singleapplication.cpp
    timeentrycellwidget.cpp
    timeentryeditorwidget.cpp
    timeentrylistwidget.cpp
    timeentryview.cpp
    timerwidget.cpp
    toggl.cpp

    # Resources have to be listed, .ui files don't
    Resources.qrc
)

# Set up compilation targets
add_executable(TogglDesktop ${BINARY_SOURCE_FILES})

# And list the dependencies between them (and system libraries)
target_link_libraries(TogglDesktop PRIVATE
    TogglDesktopLibrary
    Qt5::Widgets Qt5::WebEngine
    OAuth2
    ${QXT_LIBRARIES}
    Bugsnag 
    PocoCrypto PocoDataSQLite PocoNetSSL
    -lX11 -lXss
)

configure_file(${PROJECT_SOURCE_DIR}/src/ssl/cacert.pem cacert.pem COPYONLY)