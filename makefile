CC				= g++
OBJPAGER 		= libvm_pager.o
OBJAPP			= libvm_app.o -ldl
FLAG    		= -Wall -Werror
VERSION			= -std=c++11
DEBUG 			= -DDEBUG

EXECUTABLE		= proj3

TEST			= test-spec.2.cc
TESTSAM			= test-sam7.10.cc

TESTALBERT 		= test-albert1.8.cc
TESTHOWIE		= test-file-backed-10.2.cc


UNITTEST 		= unit-test1.cc

PAGER  			= vm_pager.cc
PAGEROUT		= vm_pager


pager:
	$(CC) $(PAGER) $(OBJPAGER) $(FLAG) $(VERSION) -o $(PAGEROUT)

debug: 
	$(CC) $(PAGER) $(OBJPAGER) $(VERSION) $(DEBUG) -o $(PAGEROUT)

spec: 
	$(CC) $(TEST) $(OBJAPP) $(VERSION) $(DEBUG)	-o spec

albert:
	$(CC) $(TESTALBERT) $(OBJAPP) $(VERSION) -o albert
unit:
	$(CC) $(UNITTEST) $(OBJAPP) $(VERSION) -o unit

albert-debug:
	$(CC) $(TESTALBERT) $(OBJAPP) $(VERSION) $(DEBUG) -o albert

sam:
	$(CC) $(TESTSAM) $(OBJAPP) $(VERSION) -o sam

howie:
	$(CC) $(TESTHOWIE) $(OBJAPP) $(VERSION) -o howie
