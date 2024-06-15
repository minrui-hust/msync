# Introduction
**msync** is a c++ template based **prue header** library which does synchronization between different messages of different types. A so called message is just an ordinary c++ object.
To support different requirements on synchronization,  several concepts are invovled, which are 'Synchronizer', 'Policy', 'Storage'. We will explain these concepts in details in the following
sections. Before that, lets take a glance of the top view of msync:
