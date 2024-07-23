CC=gcc
CFLAGS=-Wall -Wextra $(shell pkg-config --cflags glib-2.0) $(shell pkg-config --cflags jansson)
LDFLAGS=$(shell pkg-config --libs glib-2.0) $(shell pkg-config --libs json-c)

all: hashtag_search

hashtag_search: hashtag_search.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

text: hashtag_search_text.c
	$(CC) $(CFLAGS) -o $@ $< $(LDFLAGS)

clean:
	rm -f hashtag_search text hashtag_search_text.o
