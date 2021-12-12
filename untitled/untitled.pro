QT       += core gui opengl

greaterThan(QT_MAJOR_VERSION, 4): QT += widgets

CONFIG += c++17
CONFIG += qtestlib

QMAKE_CXXFLAGS+= -fopenmp
QMAKE_LFLAGS +=  -fopenmp

# You can make your code fail to compile if it uses deprecated APIs.
# In order to do so, uncomment the following line.
#DEFINES += QT_DISABLE_DEPRECATED_BEFORE=0x060000    # disables all the APIs deprecated before Qt 6.0.0

SOURCES += \
    MyQTWidgets/Spoiler.cpp \
    isosurface.cpp \
    library/BufferLayout.cpp \
    library/Camera.cpp \
    library/ErrorHandler.cpp \
    library/IBO.cpp \
    library/Model.cpp \
    library/Shader.cpp \
    library/Texture.cpp \
    library/VAO.cpp \
    library/VBO.cpp \
    library/Vertex.cpp \
    library/stb.cpp \
    main.cpp \
    mainwindow.cpp \
    myglwindow.cpp

HEADERS += \
    MyQTWidgets/Spoiler.h \
    RawModel.h \
    WindowState.h \
    isosurface.h \
    library/Camera.h \
    library/ErrorHandler.h \
    library/KHR/khrplatform.h \
    library/MyLibrary.h \
    library/math_utils.h \
    library/stb/stb_image.h \
    mainwindow.h \
    myglwindow.h

FORMS += \
    mainwindow.ui

LIBS += -lGLEW -lglut -lm -lpthread

#INCLUDEPATH += /usr/include/

# Default rules for deployment.
qnx: target.path = /tmp/$${TARGET}/bin
else: unix:!android: target.path = /opt/$${TARGET}/bin
!isEmpty(target.path): INSTALLS += target

DISTFILES += \
    library/ReadMe.md \
    library/shaders/shader.fs \
    library/shaders/shader.gs \
    library/shaders/shader.vs \
    models/bostonteapot_256x256x178_uint8.raw \
    models/coordinate.off \
    models/cube.off \
    models/pyramid.off \
    models/sphere.off \
    models/square.off \
    models/world.off \
    shaders/commonShader.fs \
    shaders/commonShader.vs \
    shaders/mainShader.fs \
    shaders/mainShader.vs
