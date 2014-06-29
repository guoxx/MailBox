CC=clang

lib : MailBox.cpp
	$(CC) -shared -lpthread $< -o libmailbox.so

all : lib
clean :
	rm libmailbox.so

.Phony : all, clean

