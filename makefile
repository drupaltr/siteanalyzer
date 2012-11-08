main: main.o network.o http_client.o link_parser.o urlqueue.o webgraph.o crawler.o hash.o
	cc -o main -Wall -pedantic main.o network.o http_client.o link_parser.o urlqueue.o webgraph.o crawler.o hash.o -g -lpthread
main.o: main.c 
	cc -o main.o -Wall -pedantic -ansi -c main.c -g
network.o: network.c network.h
	cc -o network.o -Wall -pedantic -ansi -c network.c -g
http_client.o: http_client.c http_client.h
	cc -o http_client.o -Wall -pedantic -ansi -c http_client.c -g
link_parser.o: link_parser.c link_parser.h
	cc -o link_parser.o -Wall -pedantic -ansi -c link_parser.c -g
urlqueue.o: urlqueue.c urlqueue.h
	cc -o urlqueue.o -Wall -pedantic -ansi -c urlqueue.c -g
webgraph.o: webgraph.c webgraph.h
	cc -o webgraph.o -Wall -pedantic -ansi -c webgraph.c -g
crawler.o: crawler.c crawler.h
	cc -o crawler.o -Wall -pedantic -ansi -c crawler.c -g 
hash.o: hash.c hash.h
	cc -o hash.o -Wall -pedantic -ansi -c hash.c -g 
clean: 
	rm -f *.o
