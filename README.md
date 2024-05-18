## Table of contents
* [About the project](#About-the-project)
* [Technologies](#Technologies)
* [Dataset file format](#Dataset-file-format)
* [Setup](#Setup)
* [Tips](#Tips)
## About the project
Simple genetic algorithm solving 0/1 knapsack problem with possibility of changing parameters + simple brute force. 
## Technologies
Project is created with C standard library.
## Dataset file format:
* 'number of items' 'max weigth'
* 'value of item1' 'weigth of item1'
* ...
* 'value of itemN' 'weigth of itemN'
## Setup
Simple Makefile is included. To run with Bash:
```
$ sudo apt-get install make   # to install make
$ make main                   # to compile and link
$ ./main                      # to run a program

$ make clean                  # to clean .obj files
```
Might need some workarounds to run on Windows.(Input/Output)
## Tips
* For bigger datasets changing parameters might be essential.
* Such as number of generations, number of individuals in generation, mutation rate and parameter used in generating random individual(x in rand() % x), pretty much defines how often '1' will appear in an individual.

Project made to see how hard it is do something AI related using C  :)
