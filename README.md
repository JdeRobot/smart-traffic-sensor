# traffic-monitor
smart-traffic-senosr is an advanced vision based vehicles traffic monitoring software, able to gather several traffic stats using a simple stationary camera.
STS sofware has been initiated as part of my [Ph.D thesis](http://rua.ua.es/dspace/handle/10045/68987#.Wa-sQzgxSGg.facebook). STS is able to process traffic
flow on multiple lanes simultaneously in real-time. In the current version the software is able to:
* Classify the vehicles into five categories: Motorcycle, Car, Van, Bus and Truck
* Estimate the speed of the vehicles
* Vehicles count

### 2. Installation

git clone https://github.com/JdeRobot/smart-traffic-sensor

#### 2.1 Requiered Dependencies

sudo apt-get install libopencv-dev libcairomm-1.0-dev libgtkmm-3.0-dev libgsl-dev

Note: Software was tested with Opencv 3.1 FFMPEG support must be enabled.

#### 2.3 Build

     cd traffic-monitor
     mkdir build
     cd build
     cmake ../src/
     make -j

### 3 Configuration

Traffic monitor uses two configuration files: **trafficmonitor.cfg** and **camera.cfg**. The first one is the main configuration file and
it contains a set of key=value per each line. This file is read during the initialization and is updated each time the user saves
the program configuration (Save button).

The second one is the camera configuration/calibration file and uses the following syntax (metric values are in millimeter):

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

The above values must be updated to fit the input video geometry. This file can be done by using the semi-automated calibration tool provided
by the traffic-monitor software. For this the user has to enable the **Auto calibration** option and use the calibration dialog
(View > Show Camera Calibration). [Following video](https://www.youtube.com/watch?v=_JoWBaWCNTk) explains the calibration process.

### 4 Usage

    cd build
    ln -s ../cfg/trafficmonitor.cfg trafficmonitor.cfg
    ./trafficmonitor path_to_video_file
    ./trafficmonitor ../traffic-videos/video-0042-o-4.MPG (i.e)

### 5 traffic-monitor dataset

As part of this work we created a database with more than 100 traffic videos. This dataset is available for [download](http://jderobot.org/store/trafficmonitor-dataset/) for
acaedmic non-comercial use. Video's naming uses the following syntax: **video-#id-[o|i]-#lanes.ext** where o/i stands for outgoing and ingoing videos. Stats of the different
videos ara available in the [excel sheet](doc/traffic-monitor-video-database.xlsx). If you find this dataset useful please consider citing the traffic-monitor [paper](#related-papers).

### 6 License

  Traffic monitor is released under a GPLv3 license. For a closed-source version of this software for commercial use, please contact the author(s).
