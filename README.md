selfdestructing-cpp
===================

This is a small C++ "artwork" - classes that deliberately crash on object construction.

The goal is to create a policy DSL that is readable in English. The project is a work in progress, since there are lots of possibilites of use and configuration of such classes, considering the richness of the recent C++ standard.

Use?
----

A the primary use is (self-)education. Other uses may include debugging legacy code, class policy enforcement, and perhaps something else I cannot think of.

Test: see selfdestructing.cpp

````cpp
struct TestNumberCrash : public crashes::on<2>::copies {};
````
- crashes on 2 total copies of the originally created object
- doesn't crash on 2 total instances of the class

---


````cpp
struct TestCopyNrCrash : public crashes::after<2>::copies {};
````
- crashes on any second copy of the original object
- doesn't crash on 2 total instances of the class 

---

````cpp
struct TestTotalNrCrash : public crashes::on_total<3,TestTotalNrCrash>::instances {};
````
- crashes on instantiation of an object if 2 objects are alive
- doesn't crash on any creation if the total amount of instances is below 2

---

````cpp
struct TestAfterTotalNrCrash : public crashes::after_total<3,TestAfterTotalNrCrash>::instances {};
````
- crashes after a third object instantiation of the class

---

Singular form alias is also available, i.e. `crashes::on<1>::copy`.

There's no thread-safe policy yet.

License
=======
Author: Dmitry Ledentsov

MIT License (http://www.opensource.org/licenses/mit-license.php)
