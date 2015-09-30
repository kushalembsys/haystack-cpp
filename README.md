# README #

**haystack-cpp**, a C++ implementation of the **Haystack** protocol.

For more info please go to the wiki page [wiki/Home](https://bitbucket.org/jasondbriggs/haystack-cpp/wiki/Home)

### What is this repository for? ###

* **haystack-cpp** is a C++ implementation of the **Haystack** protocol. It follows closely the design of the original **haystack-java** implementation while adding some C++ flavor.
You will find included all the basic **Haystack** types, serialization/de-serialization support and a battery of tests for the types and serialization parts.

* Version 0.1

### How do I get set up? ###

* Check out the project
* Install cmake 2.8+ and boost 1.4+
* For Visual Studio, create 'vs' folder in the 'haystack-cpp' root folder, 'cd' to the 'vs' folder and run `cmake ..\`. You will find 'Haystack-cpp.sln' inside that folder if 'cmake' runs successfully. Use Visual Studio to build the solution (or msbuild). You should set as default start app the test-app project. Run the test-app to run the test harness.
* For Linux, create 'gcc' folder in the 'haystack-cpp' root folder, 'cd' to the 'gcc' folder and run 
'cmake ../' and then 'make'. To run the test app type './tests/test_app' from the 'gcc' folder.

If some custom build parameters need to be added you can override them like:

cmake -DBOOST_ROOT=/path/to/boost -DCMAKE_BUILD_TYPE=RELEASE

See cmake docs.

If non standard paths are used for Poco it is recommended to export the 'POCO_ROOT' env variable prior to calling cmake, on Linux something like:
export POCO_ROOT=/path/to/poco;cmake -DBOOST_ROOT=/path/to/boost -DCMAKE_BUILD_TYPE=RELEASE

Consult [PocoConfig.cmake](https://bitbucket.org/jasondbriggs/haystack-cpp/src/master/PocoConfig.cmake?fileviewer=file-view-default) for more details.

* Dependencies: a modern C++ compiler(gcc 4.6+, Visual Studio 10+), cmake 2.8+ and boost 1.4+
* How to run tests: see above.

### Contribution guidelines ###

* Writing tests should be done inside the tests folder, the test framework used is CATCH, please look at the existing test files to see how they are structured.
* Please stick to Cmake and **DON'T** manually configure the cmake generated projects/makefiles.

### Who do I talk to? ###

* Repo owner or admin: Radu Racariu <radur<at>j2inn.com>