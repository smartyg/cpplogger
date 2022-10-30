check_PROGRAMS = test-Logger
test_Logger_SOURCES = test-Logger.cpp
test_Logger_LDADD = @GTEST_LIBS@ @GMOCK_LIBS@ ../libcpplogger.la
test_Logger_LDFLAGS = -pthread
test_Logger_CPPFLAGS = @GTEST_CFLAGS@ @GMOCK_CFLAGS@ -pthread -I../include -I../src

TESTS = test-Logger
