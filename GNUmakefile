# $Id$

.PHONY: clean

PROG= fdm
VERSION= 1.6
DATE= $(shell date +%Y%m%d-%H%M)

DEBUG= 1

PREFIX?= /usr/local

BIN_OWNER= bin
BIN_GROUP= root

CC= gcc

INCDIRS= -I$(PREFIX)/include
LDFLAGS= -L$(PREFIX)/lib

ifeq ($(shell uname),SunOS)
YACC= yacc
YFLAGS= -d
else
YACC= bison
YFLAGS= -dy
endif

INSTALLDIR= install -d
INSTALLBIN= install -g $(BIN_OWNER) -o $(BIN_GROUP) -m 555
INSTALLMAN= install -g $(BIN_OWNER) -o $(BIN_GROUP) -m 444

SRCS= fdm.c \
      attach.c buffer.c cleanup.c command.c connect.c io.c log.c netrc.c \
      child-deliver.c child-fetch.c child.c \
      pcre.c re.c privsep.c replace.c shm-mmap.c strb.c db-tdb.c \
      xmalloc-debug.c xmalloc.c timer.c \
      deliver-add-header.c deliver-drop.c deliver-keep.c deliver-maildir.c \
      deliver-mbox.c deliver-pipe.c deliver-remove-header.c deliver-rewrite.c \
      deliver-smtp.c deliver-stdout.c deliver-tag.c deliver-add-to-cache.c \
      deliver-remove-from-cache.c deliver-write.c deliver-imap.c \
      fetch-imap.c fetch-imappipe.c fetch-maildir.c fetch-nntp.c fetch-pop3.c \
      fetch-pop3pipe.c fetch-stdin.c fetch-mbox.c pop3-common.c imap-common.c \
      mail-state.c mail-time.c mail.c file.c cache-op.c \
      match-all.c match-age.c match-attachment.c match-command.c \
      match-in-cache.c match-matched.c match-regexp.c match-size.c \
      match-string.c match-tagged.c match-unmatched.c match-account.c \
      parent-deliver.c parent-fetch.c \
      lookup.c lookup-passwd.c \
      y.tab.c parse-fn.c lex.c

ifeq ($(shell uname),Darwin)
INCDIRS+= -I/usr/local/include/openssl -Icompat
SRCS+= compat/strtonum.c
DEFS+= -DNO_STRTONUM -DNO_SETRESUID -DNO_SETRESGID -DNO_SETPROCTITLE \
       -DNO_QUEUE_H -DNO_TREE_H
LIBS+= -lresolv -lcrypto
endif

ifneq (, $(filter Linux GNU GNU/%, $(shell uname -s)))
INCDIRS+= -I/usr/include/openssl -Icompat
SRCS+= compat/strlcpy.c compat/strlcat.c compat/strtonum.c
ifeq (, $(filter GNU/k%BSD, $(shell uname -s)))
DEFS+= -DWITH_MREMAP
endif
DEFS+= $(shell getconf LFS_CFLAGS) -D_GNU_SOURCE \
       -DNO_STRLCPY -DNO_STRLCAT -DNO_STRTONUM -DNO_SETPROCTITLE \
       -DNO_QUEUE_H -DNO_TREE_H
LIBS+= -lresolv
# Required for LLONG_MAX and friends
CFLAGS+= -std=c99
endif

OBJS= $(patsubst %.c,%.o,$(SRCS))
CPPFLAGS+= $(DEFS) -I. -I- $(INCDIRS)
ifdef DEBUG
CFLAGS+= -g -ggdb -DDEBUG
LDFLAGS+= -rdynamic
LIBS+= -ldl
DEFS+= -DBUILD="\"$(VERSION) ($(DATE))\""
else
DEFS+= -DBUILD="\"$(VERSION)\""
endif
#CFLAGS+= -pedantic -std=c99
CFLAGS+= -Wno-long-long -Wall -W -Wnested-externs -Wformat=2
CFLAGS+= -Wmissing-prototypes -Wstrict-prototypes -Wmissing-declarations
CFLAGS+= -Wwrite-strings -Wshadow -Wpointer-arith -Wcast-qual -Wsign-compare
CFLAGS+= -Wundef -Wshadow -Wbad-function-cast -Winline -Wcast-align

ifdef PCRE
DEFS+= -DPCRE
LIBS+= -lpcre
endif

LIBS+= -lssl -ltdb -lz

CLEANFILES= $(PROG) y.tab.c y.tab.h $(OBJS) .depend

all: fdm

$(PROG): $(OBJS)
	$(CC) $(LDFLAGS) $(LIBS) -o $@ $+

depend: $(SRCS)
	$(CC) $(CPPFLAGS) -MM $(SRCS) > .depend

y.tab.c y.tab.h: parse.y
	$(YACC) $(YFLAGS) $<

install:
	$(INSTALLDIR) $(DESTDIR)$(PREFIX)/bin
	$(INSTALLBIN) $(PROG) $(DESTDIR)$(PREFIX)/bin/$(PROG)
	$(INSTALLDIR) $(DESTDIR)$(PREFIX)/man/man1
	$(INSTALLMAN) $(PROG).1 $(DESTDIR)$(PREFIX)/man/man1/$(PROG).1
	$(INSTALLDIR) $(DESTDIR)$(PREFIX)/man/man5
	$(INSTALLMAN) $(PROG).conf.5 $(DESTDIR)$(PREFIX)/man/man5/$(PROG).conf.5

clean:
	rm -f $(CLEANFILES)

ifeq ($(wildcard .depend),.depend)
include .depend
endif
