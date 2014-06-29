Prototype for thread safe message queue
=======

thread safe message queue (or mail box)

`mailbox_send` is thread safe, call it in any thread you want, and call `mailbox_dispatch` in main thread.


TODO
====
associate mail box with thread, so you can use one mail box per thread.
