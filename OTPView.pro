TEMPLATE = subdirs
SUBDIRS += OTPView OTPLib

OTPLib.subdir  = libs/OTPLib
OTPView.subdir = app

OTPView.depends = OTPLib
