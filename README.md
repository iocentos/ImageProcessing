Parallel Image Processing using C++
===================================

The project explores the advantages of multithreading in image processing while comparing between three different implementations. Several image processing functions have been implemented and are available in each implementation for comparison. 

Timers are used in all implementations in order to accurately measure the difference of implementations. At the end of each execution execution times are printed to the standard output.

Serial implementaion
--------------------

In order to see the advantages of multithreading, the first execution mode is serial where a single operation is applied to a number of images and the processing is performed on a single thread. 

In order to compile the serial implementation run the following command
```sh
$ make -f sefialMakefile
```


OpenMP implementation
---------------------

The second implementation uses the OpenMP libraries available for C/C++. OpenMP uses compiler directives in order to split the work on the available processors.

In order to compile the openMP implementation run the following command
```sh
$ make -f openmpMakefile
```

CilkPlus implementation
-----------------------

The third implementation uses the CilkPlus compiler which adds simple language extensions to the C and C++languages to express task and data parallelism.

In order to compile the CilkPlus implementation run the following command
```sh
$ make -f cilkplusMakefile
```

Dependencies
------------

All three implementations depend on the following libraries in order to be able to open and manipulate images in different formats. The libraries need to be in the $INCLUDE_PATH and the $LD_LIBRARY_PATH.

- libtiff
- libpng
- libjpeg

For the OpenMP implementation the OpenMP library needs to be installed on the system and finally for the CilkPlus implementation the CilkPlus compiler needs to be installed.

Execution
---------

Depending on the implementation, three different executables will be generated, serial, openmp and cilkplus.

The following image processing functions are provided for comparison.
- reverse color
- adjust brightness
- adjust contrast
- convert RGB to Grey
- convert Grey to RGB
- convert RGB to Black and White
- convert RGB to Sepia
- rotate 
- scale
- blur

The more intense the processing is the more the advantages of multithreading can be seen. For example, in order to reverse the colors of an image, all that is required is to invert the value of individual pixels which is not very computationaly intensive. On the other hand, trying to blur an image is more intensive thus running it on multiple cores can yield better execution time.

Upon execution, all implementation require an input folder with images and an ouput folder where the processed images will be stored. Furthermore, the operation to performed and further arguments required by the specific operation.

Here are a few examples.

Rotate all images in the input folder to the left
```sh
$ ./program input/folder output/folder -rotate -left
```
Blur all images in the input folder by 90%
```sh
$ ./program input/folder output/folder -blur 90
```
Convert all images to black and white
```sh
$ ./program input/folder output/folder -rgb2bw 0
```
