broception
==========
A network visualizer using bro's broccoli library and my soapybrain engine.  Notice I did not say traffic.


Pull down the repo

```
git clone --recursive https://github.com/thegreatpissant/broception.git
```


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
cd broception/src
bro -i <your_interface> broconn.bro
```

Then run broception from the build/src dir

```
cd broception/build/src
./broception
```

Refresh your web page
