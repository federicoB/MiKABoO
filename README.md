Phase 1 (Process/Thread/Messages control blocks) test execution
![phase1](https://user-images.githubusercontent.com/15829877/55736970-a8461b00-5a24-11e9-98d9-55b54024aa28.gif)
Phase 2 (Scheduling, interrupts, deadlock detection and SSI) test execution
![phase2x4](https://user-images.githubusercontent.com/15829877/55737018-bbf18180-5a24-11e9-9efe-cac58c7e6405.gif)
<img src="https://www.unibo.it/en/logo-unibo.png" data-canonical-src="https://www.unibo.it/en/logo-unibo.png" width="100" />
### MiKABoO Project 
A kernel for the ARMv7 processor with message passing architecture.

Developed as operating system course project at University of Bologna during year 2016/2017.

A little implementation guide can be found [here](https://github.com/federicoB/MiKABoO/blob/master/documentation/implementation-guide.pdf).

Professor [Renzo Davoli](http://www.cs.unibo.it/~renzo/) provided test files which are compiled with the project.

How to compile
---------------
A CMakeLists.txt is provided with the source code.  
Running `cmake .` in the same directory will automatically create a makefile then running `make` will build the test binaries using the cross compiler toolchain.  

The binaries `test1` and `test2` will be generated in `./dist/`.

The GNU ARM Embedded Toolchain and cmake are compiling dependencies.

How to run
---------------
Test binaries must be loaded and run on the [uARM emulator](https://github.com/mellotanica/uARM) as described in its documentation.

Authors
---------------
Riccardo Maffei <riccardo.maffei@studio.unibo.it>  
Teresa Signati <teresa.signati@studio.unibo.it>  
Federico Bertani <federico.bertani@studio.unibo.it>  
Oleksandr Poddubnyy <oleksandr.poddubnyy@studio.unibo.it>  

Contains some test scripts from Professor Renzo Davoli and a list.h subset from the linux kernel.

License
---------------
This software is released under the GNU GPL v2.0 license.  
This project contains a subset of list.h from the linux kernel released under GPL v2.0.  
A copy of the license is provided in the LICENSE file.  
>Copyright (C) 2017  Riccardo Maffei, Teresa Signati, Federico Bertani, Oleksandr Poddubnyy.

>This program is free software: you can redistribute it and/or modify
>it under the terms of the GNU General Public License as published by
>the Free Software Foundation, either version 2 of the License, or
>(at your option) any later version.

>This program is distributed in the hope that it will be useful,
>but WITHOUT ANY WARRANTY; without even the implied warranty of
>MERCHANTABILITY or FITNESS FOR A PARTICULAR PURPOSE.  See the
>GNU General Public License for more details.

>You should have received a copy of the GNU General Public License
>along with this program.  If not, see <http://www.gnu.org/licenses/>.
