# Makefile for socket examples

CXX=	g++ $(CCFLAGS)
UTIL=	logger.o infinibuf.o socket.o
MSGD=	msgd.o msgserver.o $(UTIL)
MSG=	msg.o client.o $(UTIL)
OBJS =	$(MSGD) $(MSG)

LIBS=

CCFLAGS= -g

all:	msgd msg
run: all
	@echo ./msgd -p 8080 \& ./msg -h localhost -p 8080

msgd: $(MSGD)
	$(CXX) -o msgd $(MSGD) $(LIBS)

msg: $(MSG)
	$(CXX) -o msg $(MSG) $(LIBS)

clean:
	rm -f $(OBJS) $(OBJS:.o=.d)

realclean:
	rm -f $(OBJS) $(OBJS:.o=.d) msgd msg


# These lines ensure that dependencies are handled automatically.
%.d:	%.cc
	$(SHELL) -ec '$(CC) -M $(CPPFLAGS) $< \
		| sed '\''s/\($*\)\.o[ :]*/\1.o $@ : /g'\'' > $@; \
		[ -s $@ ] || rm -f $@'

include	$(OBJS:.o=.d)
