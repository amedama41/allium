allium
======

Experimental OpenFlow library for C++11

*This code is undocumented, poorly tested, and constantly changing*


Install
-------

This library can be used in the header only.
But some header files need to be generated from `openflow.h` in advance.

```
cd ofparser
make run
```

Features
--------

### Controller decoration

Controller decoration is a technique to inject commonly used functions,
such as setting the table miss flow entry, into your controller.
This is based on a static version of the decorator design pattern.

See `examples/decorator_controller`

