# OTPLib
LIBS += -L$$DESTDIR -lOTPLib

OTPLib_DIR = $$PWD/../libs/OTPLib

CONFIG(release, debug|release): OTPLib_LIBS = $${OTPLib_DIR}/build/release
CONFIG(debug, debug|release): OTPLib_LIBS = $${OTPLib_DIR}/build/debug
QMAKE_PRE_LINK += $$QMAKE_COPY $${OTPLib_LIBS}/* $$DESTDIR $$escape_expand(\\n\\t)

INCLUDEPATH += $${OTPLib_DIR}/src
DEPENDPATH += $${OTPLib_DIR}/src

INCLUDEPATH += $${OTPLib_DIR}
HEADERS += $${OTPLib_DIR}/OTPLib.hpp
