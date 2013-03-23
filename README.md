selfdestructing-cpp
===================

This is a small C++ "artwork" - classes that deliberately crash on object construction.

The goal is to create a DSL that is readable in English.

The project is meant to be a work in progress, since there are lots of possibilites of use and configuration of such classes.

Use?
----

A the primary use is (self-)education. Other uses may include debugging legacy code, class policy enforcement, and perhaps something else I cannot think of.

Test: see selfdestructing.cpp

````cpp
struct TestNumberCrash : public crashes::on<2>::copies {};
````
- crashes on 2 total copies of the originally created object
- doesn't crash on 2 total instances of the class



````cpp
struct TestCopyNrCrash : public crashes::after<2>::copies {};
````
- crashes on any second copy of the original object
- doesn't crash on 2 total instances of the class 



````cpp
struct TestTotalNrCrash : public crashes::on_total<3,TestTotalNrCrash>::instances {};
````
- crashes on 2 total live instances of the class
- doesn't crash on any creation if the total amount of instances is below 3

````cpp
struct TestAfterTotalNrCrash : public crashes::after_total<2,TestAfterTotalNrCrash>::instances {};
````
- crashes after any 2 instantiations of the class

License
=======
Author: Dmitry Ledentsov

MIT License (http://www.opensource.org/licenses/mit-license.php)
