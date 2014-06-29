#include "MailBox.h"
#include <assert.h>

#define MAILBOX_SIZE 128

struct Box{
    char *name;
    void *userdata;
    int length;
};

struct MBox{
    pthread_mutex_t mutex;
    int tail;
    struct Box boxes[MAILBOX_SIZE];
};

struct MBox *_box_0;
struct MBox *_box_1;

static void _box_init(struct Box *box){
    box->name = NULL;
    box->userdata = NULL;
    box->length = 0;
}

static void _box_fill(struct Box *box, const char *name, void *data, int len){
    assert(name != NULL);

    int namelen = strlen(name);
    box->name = (char*)malloc((namelen + 1) * sizeof(char*));
    memcpy(box->name, name, namelen);
    box->name[namelen] = '\0';

    if(data != NULL){
        box->userdata = malloc(len);
        memcpy(box->userdata, data, len);
    }

    box->length = len;
}

static void _box_reuse(struct Box *box){
    free(box->name);
    box->name = NULL;

    if(box->userdata != NULL){
        free(box->userdata);
        box->userdata = NULL;
    }

    box->length = 0;
}

static void _mbox_lock(struct MBox *m){
    assert(pthread_mutex_lock(&m->mutex) == 0);
}

static void _mbox_unlock(struct MBox *m){
    assert(pthread_mutex_unlock(&m->mutex) == 0);
}

void mailbox_init(){
    _box_0 = (struct MBox*)malloc(sizeof(*_box_0));
    _box_1 = (struct MBox*)malloc(sizeof(*_box_1));

    pthread_mutex_init(&_box_0->mutex, NULL);
    pthread_mutex_init(&_box_1->mutex, NULL);
    _box_0->tail = -1;
    _box_1->tail = -1;
    for(int i = 0; i < MAILBOX_SIZE; ++i){
        _box_init(_box_0->boxes + i);
        _box_init(_box_1->boxes + i);
    }
}

void mailbox_destroy(){
    // ensure all messages has been sended
    mailbox_dispatch();
    mailbox_dispatch();

    pthread_mutex_destroy(&_box_0->mutex);
    pthread_mutex_destroy(&_box_1->mutex);

    _box_0->tail = -1;
    _box_1->tail = -1;
    for(int i = 0; i < MAILBOX_SIZE; ++i){
        _box_reuse(_box_0->boxes + i);
        _box_reuse(_box_1->boxes + i);
    }

    free(_box_0);
    _box_0 = NULL;
    free(_box_1);
    _box_1 = NULL;
}

void mailbox_send(const char *name, void *data, int len){
    _mbox_lock(_box_0);

    if(_box_0->tail < MAILBOX_SIZE){
        _box_0->tail = _box_0->tail + 1;
        struct Box *b = _box_0->boxes + _box_0->tail;
        _box_fill(b, name, data, len);
    }

    _mbox_unlock(_box_0);
}

static void _dispatch(const struct Box *b){
    (void)b;
}

void mailbox_dispatch(){
    _mbox_lock(_box_0);
    _mbox_lock(_box_1);
    struct MBox *tmp = _box_0;
    _box_0 = _box_1;
    _box_1 = tmp;
    _mbox_unlock(_box_0);
    _mbox_unlock(_box_1);

    for(int i = 0; i <= _box_1->tail; ++i){
        struct Box *b = _box_1->boxes + i;
        _dispatch(b);
    }

    for(int i = 0; i <= _box_1->tail; ++i){
        struct Box *b = _box_1->boxes + i;
        _box_reuse(b);
    }
    _box_1->tail = -1;
}
