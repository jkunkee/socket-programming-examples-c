# Makefile for socket examples

CXX=			g++ $(CCFLAGS)
MSGD=			msgd.o server.o
MSGD_CLIENT=		msgdclient.o client.o
OBJS =			$(MSGD) $(MSGD_CLIENT) 

LIBS=

CCFLAGS= -g

all:	msgd msgdclient

msgd:$(MSGD)
	$(CXX) -o msgd $(MSGD) $(LIBS)

msgdclient:$(MSGD_CLIENT)
	$(CXX) -o msgdclient $(MSGD_CLIENT) $(LIBS)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) msgd msgdclient


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
