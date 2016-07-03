Welcome to litestomp
===================

 Build Status
----
Linux Build Status: [![Linux Build Status](https://travis-ci.org/orpiske/litestomp.svg?branch=master)](https://travis-ci.org/orpiske/litestomp) 

Introduction:
----

This is a fork of the old litestomp library, modified for simplicity and 
reusability. This library implements a simple STOMP, a text oriented messaging
protocol that can be used to talk to message brokers such as Apace ActiveMQ.

This library supports the version 1.2 of the STOMP protocol as documented by: 
https://stomp.github.io/stomp-specification-1.2.html.

Project documentation, as well as examples, can be found here: 
http://orpiske.net/files/litestomp/html/index.html.

Please feel free to contribute to make this library better.


Dependencies:
----

Runtime/Compilation:
* cmake
* gcc or clang
* qpid-proton-c-devel
* apr-devel 
* apr-util-devel 