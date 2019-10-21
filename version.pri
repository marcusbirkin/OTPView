## Version defines

GIT_COMMAND = git --git-dir $$shell_quote($$PWD/.git) --work-tree $$shell_quote($$PWD)
GIT_VERSION = $$system($$GIT_COMMAND describe --always --tags)
GIT_DATE_DAY = $$system($$GIT_COMMAND show -s --date=format:\"%a\" --format=\"%cd\" $$GIT_VERSION)
GIT_DATE_DATE = $$system($$GIT_COMMAND show -s --date=format:\"%d\" --format=\"%cd\" $$GIT_VERSION)
GIT_DATE_MONTH = $$system($$GIT_COMMAND show -s --date=format:\"%b\" --format=\"%cd\" $$GIT_VERSION)
GIT_DATE_YEAR = $$system($$GIT_COMMAND show -s --date=format:\"%Y\" --format=\"%cd\" $$GIT_VERSION)
GIT_TAG = $$system($$GIT_COMMAND describe --abbrev=0 --always --tags)
GIT_SHA1 = $$system($$GIT_COMMAND rev-parse --short HEAD)

DEFINES += GIT_CURRENT_SHA1=\\\"$$GIT_VERSION\\\"
DEFINES += GIT_DATE_DAY=\\\"$$GIT_DATE_DAY\\\"
DEFINES += GIT_DATE_DATE=\\\"$$GIT_DATE_DATE\\\"
DEFINES += GIT_DATE_MONTH=\\\"$$GIT_DATE_MONTH\\\"
DEFINES += GIT_DATE_YEAR=\\\"$$GIT_DATE_YEAR\\\"
