#ifndef __CC_MAILBOX__
#define __CC_MAILBOX__

#include <iostream>

void mailbox_init();
void mailbox_destroy();
void mailbox_dispatch();
// thread safe
void mailbox_send(const char *name, void *data, int len);

#endif /* defined(__CC_MAILBOX__) */
