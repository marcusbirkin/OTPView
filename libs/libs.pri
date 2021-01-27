# OTPLib
LIBS += -L$$DESTDIR -lOTPLib

OTPLib_DIR = $$system_path($$PWD/../libs/OTPLib)

CONFIG(release, debug|release): OTPLib_LIBS = $$system_path($${OTPLib_DIR}/build/release)
CONFIG(debug, debug|release): OTPLib_LIBS = $$system_path($${OTPLib_DIR}/build/debug)
QMAKE_PRE_LINK += $$QMAKE_COPY $$shell_quote( $$system_path($${OTPLib_LIBS}/*)) $$shell_quote( $$system_path($$DESTDIR)) $$escape_expand(\\n\\t)

INCLUDEPATH += $${OTPLib_DIR}/src
DEPENDPATH += $${OTPLib_DIR}/src

INCLUDEPATH += $${OTPLib_DIR}
HEADERS += $${OTPLib_DIR}/OTPLib.hpp
