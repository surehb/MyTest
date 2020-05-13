Play Book
====================

Install dependencies
^^^^^^^^^^^^^^^^^^^^

Install the following dependencies.
::
  1. yum install cmake3 autoconf libtool


Build
^^^^^^^^^

Submodules
::
  1. cd MyTest
  2. git submodule init
  3. git submodule update

Build gRPC
::
  1. cd thirdparty/grpc/
  2. git checkout tags/v1.13.1
  3. git submodule update --init
  4. mkdir -p cmake/build
  5. cd cmake/build
  6. cmake3 ../..
  7. make

Build g3log
::
  1. cd ../../../g3log
  2. git checkout tags/1.3.2
  3. mkdir build
  4. cd build
  5. cmake3 .. -DG3_SHARED_LIB=OFF -DADD_FATAL_EXAMPLE=OFF
  6. make

Build MyTest
::
  1. cd ../../..
  2. cmake3 .
  3. make
