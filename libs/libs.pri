# OTPLib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libs/OTPLib/src/release/ -lOTPLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libs/OTPLib/src/debug/ -lOTPLib
else:unix: LIBS += -L$$OUT_PWD/../libs/OTPLib/src/ -lOTPLib

INCLUDEPATH += $$PWD/../libs/OTPLib/src
DEPENDPATH += $$PWD/../libs/OTPLib/src

INCLUDEPATH += $$PWD/../libs/OTPLib/
HEADERS += $$PWD/../libs/OTPLib/OTPLib.hpp
