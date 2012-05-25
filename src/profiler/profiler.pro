######################################################################
# Automatically generated by qmake (2.01a) ?? ???. 11 00:15:23 2011
######################################################################

TEMPLATE = app
TARGET = profiler 
DEPENDPATH += .
INCLUDEPATH += . ../http_requests/inc/ ../common/inc/ ../json/inc/
LIBS += -lqjson

QT += network 
QT -= gui
CONFIG += debug network thread
QMAKE_CXXFLAGS += -g3

# Input
HEADERS += ProfilerThread.h LoginThread.h TrackThread.h LoadTagsThread.h \
../http_requests/inc/DefaultQuery.h \
../http_requests/inc/WriteTagQuery.h \
../http_requests/inc/LoadTagsQuery.h \
../http_requests/inc/LoginQuery.h \
../common/inc/User.h \
../common/inc/Channel.h \
../common/inc/TimeSlot.h \
../common/inc/DataMarks.h \
../common/inc/User.h \
../common/inc/defines.h \
../common/inc/ErrnoTypes.h \
../common/inc/SettingsStorage.h \
../json/inc/LoginRequestJSON.h \
../json/inc/LoginResponseJSON.h \
../json/inc/WriteTagRequestJSON.h \
../json/inc/WriteTagResponseJSON.h \
../json/inc/LoadTagsRequestJSON.h \
../json/inc/LoadTagsResponseJSON.h \
../json/inc/JsonUser.h \
../json/inc/JsonDataMark.h \
../json/inc/JsonChannel.h \
../json/inc/JsonSerializer.h \
../json/inc/DefaultResponseJSON.h 

SOURCES += LoginThread.cpp main.cpp TrackThread.cpp ProfilerThread.cpp LoadTagsThread.cpp \
../common/src/User.cpp \
../common/src/Channel.cpp \
../common/src/TimeSlot.cpp \
../common/src/DataMarks.cpp \
../common/src/defines.cpp \
../common/src/SettingsStorage.cpp \
../http_requests/src/DefaultQuery.cpp \
../http_requests/src/WriteTagQuery.cpp \
../http_requests/src/LoginQuery.cpp \
../http_requests/src/LoadTagsQuery.cpp \
../json/src/LoginRequestJSON.cpp \
../json/src/LoginResponseJSON.cpp \
../json/src/WriteTagRequestJSON.cpp \
../json/src/WriteTagResponseJSON.cpp \
../json/src/LoadTagsRequestJSON.cpp \
../json/src/LoadTagsResponseJSON.cpp \
../json/src/JsonChannel.cpp \
../json/src/JsonDataMark.cpp \
../json/src/JsonSerializer.cpp \
../json/src/JsonUser.cpp \
../json/src/DefaultResponseJSON.cpp 
