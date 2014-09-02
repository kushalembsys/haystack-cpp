# README #

haystack-cpp, a C++ implementation of the haystack protocol.

### What is this repository for? ###

* haystack-cpp is a C++ implementation of the haystack protocol. It follows closely the design of the original haystack-java implementation while adding some C++ flavor.
You will find included all the basic **haystack** types, serialization/de-serialization support and a battery of tests for the types and serialization parts.

* Version 0.1

### How do I get set up? ###

* Check out the project
* Install cmake 2.8+ and boost 1.4+
* For Visual Studio, create 'vs' folder in the 'haystack-cpp' root folder, 'cd' to the 'vs' folder and run `cmake ..\`. You will find 'Haystack-cpp.sln' inside that folder if 'cmake' runs successfully. Use Visual Studio to build the solution (or msbuild). You should set as default start app the test-app project. Run the test-app to run the test harness.
* For Linux, create 'gcc' folder in the 'haystack-cpp' root folder, 'cd' to the 'gcc' folder and run 
'cmake ../' and then 'make'. To run the test app type './tests/test_app' from the 'gcc' folder.

* Dependencies: a modern C++ compiler(gcc 4.6+, Visual Studio 10+), cmake 2.8+ and boost 1.4+
* How to run tests: see above.

### Contribution guidelines ###

* Writing tests should be done inside the tests folder, the test framework used is CATCH, please look at the existing test files to see how they are structured.

### Who do I talk to? ###

* Repo owner or admin: Radu Racariu <radur@j2inn.com>