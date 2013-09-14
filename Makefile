# Makefile for socket examples

CXX=			g++ $(CCFLAGS)
MSGD=			msgd.o server.o infinibuf.o
MSGD_CLIENT=		msgdclient.o client.o infinibuf.o
OBJS =			$(MSGD) $(MSGD_CLIENT)

LIBS=

CCFLAGS= -g

all:	msgd msgdclient
run: all
	@echo ./msgd -p 8080 \& ./msgdclient -h localhost -p 8080

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
