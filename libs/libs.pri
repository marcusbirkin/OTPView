# OTPLib
win32:CONFIG(release, debug|release): LIBS += -L$$OUT_PWD/../libs/OTPLib/release/ -lOTPLib
else:win32:CONFIG(debug, debug|release): LIBS += -L$$OUT_PWD/../libs/OTPLib/debug/ -lOTPLib
else:unix: LIBS += -L$$OUT_PWD/../libs/OTPLib/ -lOTPLib

INCLUDEPATH += $$PWD/../libs/OTPLib
DEPENDPATH += $$PWD/../libs/OTPLib
