broception
==========
A network visualizer using bro's broccoli library and my soapybrain engine.  Notice I did not say traffic.


git clone --recursive https://github.com/thegreatpissant/broception.git


This set of commands will set up the env after the build

```
cd broception
mkdir build
cd build
cmake ../
make
cd src
ln -s  ../../aux/soapybrain/shaders shaders
```

Now run bro with the src/broconn.bro script

```
bro -i <your_interface> broconn.bro
```

Then run broception from the build/src dir

```
./broception
```
