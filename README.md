# smart-traffic-sensor
**smart-traffic-senosr** (a.k.a STS) is an advanced vision based vehicles traffic monitoring software, able to gather several traffic stats using a simple stationary camera by using
deep-learning techniques. STS is able to process traffic flow on multiple lanes simultaneously in real-time. In the current version the software is able to:
* Classify the vehicles into five categories: Motorcycle, Car, Van, Bus and Truck
* Estimate the speed of the vehicles
* Vehicles count

STS currently supports Linux operating systems. Below are the steps for installing the software.

### Requiered Dependencies

Smart-Traffic-Sensor needs the next dependencies:

1. OpenCV 3.2 with contrib
2. Cuda
3. Tensorflow
    ```bash
    # CPU
    sudo pip install tensorflow

    # GPU
    sudo pip install tensorflow-gpu
    ```

4. Keras
    ```bash
    sudo pip install keras
    ```
5. Darknet. It is an open source neural network framework written in C. Darknet supports both GPU and CPU builds. You can install it following the instructions below:

    ```bash
      git clone https://github.com/JdeRobot/darknet
      cd darknet
      mkdir build && cd build
    #For GPU users:
      cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DUSE_GPU=ON ..
    #For CPU users:
      cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DUSE_GPU=OFF ..

      make -j4
      sudo make -j4 install
    ```

   In case of errors related with rapidjson headers rapidjson-dev must be installed

6. gtkmm 3.0:
    ```bash
    sudo apt-get install libopencv-dev libcairomm-1.0-dev libgtkmm-3.0-dev libgsl-dev
    ```



### Build


```bash
   git clone https://github.com/JdeRobot/smart-traffic-sensor
   cd smart-traffic-sensor
   mkdir build
   cd build
   cmake ../src/
   make -j
```

### Configuration
Traffic monitor uses two configuration files: trafficmonitor.cfg and camera.cfg. The first one is the main configuration file and it contains a set of key=value per each line. This file is read during the initialization and is updated each time the user saves the program configuration (Save button).

The second one is the camera configuration/calibration file and uses the following syntax (metric values are in millimeter):

```bash
    #extrinsics, position
    positionX 0
    positionY 0
    positionZ 10302
    positionH 1

    #orientation
    FOApositionX -476
    FOApositionY 50577
    FOApositionZ 0
    FOApositionH 1
    roll 0

    #Intrensics
    fx 1143
    fy 1143
    skew 0
    u0 288
    v0 360
    columns 720
    rows 576
```
The above values must be updated to fit the input video geometry. This file can be done by using the semi-automated calibration tool provided by the smart-traffic-sensor software. For this the user has to enable the Auto calibration option and use the calibration dialog (View > Show Camera Calibration). [Following video](https://www.youtube.com/watch?v=_JoWBaWCNTk) explains the calibration process.

### Usage

```bash
   cd build
   ln -s ../cfg/trafficmonitor.cfg trafficmonitor.cfg
   export LD_LIBRARY_PATH=$LD_LIBRARY_PATH:/usr/local/cuda-9.2/lib64
   ./trafficmonitor path_to_video_file
   ./trafficmonitor ../traffic-videos/video-0042-o-4.MPG (i.e)
```
