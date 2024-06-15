# Introduction
  **msync** is a c++ template based **prue header** library which does synchronization between different messages of different types. A so called message is just an ordinary c++ object.
To support different requirements on synchronization,  several concepts are invovled, which are 'Synchronizer', 'Policy', 'Storage'. We will explain these concepts in details in the following
sections. Before that, lets take a glance of the top view of msync:   
<img src="https://github.com/minrui-hust/msync/blob/master/pic/top.png" alt="drawing" width="400"/>

# Synchronizer
  Synchronizer is the top module which handle message synchronization, it accepts stamped messages at different time and synchronizes messages with the (almost) same stamp, then it call the registered callback, which take
the synchronized messages as parameters. There are two kinks of synchronizers, 'SynchronizerMasterSlave' and 'SynchronizerMinInterval'.    
  An 'SynchronizerMasterSlave' treats messages differently, one message is specified as master message, and others are slaves. Message synchronization only happens at the master message's stamp, when an master message arrives with stamp accompanied, synchronizer will try get slave messages from slave policies, if all slave messages are collected successfully, then callback will be called. On the other hand, if some slave message failed to peek, depends on the configuration, if this message is optional, then callback will be called as normal, with unavailable message flaged, otherwise, callback wont be called and master message will be cached, waiting for the available slave messages.
