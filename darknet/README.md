![Darknet Logo](http://pjreddie.com/media/files/darknet-black-small.png)

# Darknet #
Darknet is an open source neural network framework written in C and CUDA. It is fast, easy to install, and supports CPU and GPU computation.

For more information see the [Darknet project website](http://pjreddie.com/darknet).

For questions or issues please use the [Google Group](https://groups.google.com/forum/#!forum/darknet).

# Darknet Installation in the computer
I have used Darknet (https://github.com/pjreddie/darknet) and I have adapted it so that we can use it in Jderobot with DarknetApi. You can install it following the instructions below:

```ruby
  Download https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/tree/master/darknet
  cd darknet
  mkdir build && cd build
For GPU users:
  cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DUSE_GPU=ON ..
For CPU users:
  cmake -DCMAKE_INSTALL_PREFIX=/usr/local -DUSE_GPU=OFF ..
  
  make -j4
  sudo make -j4 install
```

## Darknet Training
To train a model with my dataset I use ([Darknet](https://github.com/pjreddie/darknet)). I adapted it so I could install it on my computer and use DarknetApi on smart-traffic-sensor. I have this adaptation in https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez

If you have to use OpenCV, Cuda and GPU, you have to edit the Makefile:

```ruby
  GPU=0
  CUDNN=0
  OPENCV=0
```

To train you can execute make for generating the executable.

```ruby
  cd darknet
  make -j4
```
Here are the steps to follow to train a model with your dataset:

1- We need to generate the label files that Darknet uses. Darknet wants a .txt file for each image with a line for each ground truth object in the image that looks like:

```ruby
  <object-class> <x> <y> <width> <height>
```
Where x, y, width, and height are relative to the image's width and height. In my case I have xml label files because I used labelImg and I saved in this format.To generate these file(.txt) we will run the voc_label.py script in Darknet's scripts/ directory.
To execute this script you need an annotations folder (folder with xml labels), a .txt with the annotations names file (annotations_file.txt) and an output folder. 

Next it shows that it should contain the annotations_file.txt:

```ruby
0006-00000001.xml
0006-00000002.xml
0006-00000003.xml
0006-00000005.xml
0006-00000006.xml
0006-00000007.xml
0006-00000008.xml
...
```
You have to have the next files in the directory darknet/scripts:

![Directory](https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/docs/directory.png)

Now you can generate the .txt files (you get this files in output folder):
```ruby
   python voc_label.py -xml annotations/ -xml_files annotations_file.txt -out labels/
```
You have to copy the labels folder in the darknet/data folder. In the arknet/folder you should have the images and labels folders.

2- Darknet needs one text file with all of the images you want to train on and other with all of the images you want to test. Below is an example of what you should put in both files:

```ruby
   /home/docker/Jessi/Darknet_training/darknet/data/images/0081-00000055.jpg
   /home/docker/Jessi/Darknet_training/darknet/data/images/0081-00000056.jpg
   /home/docker/Jessi/Darknet_training/darknet/data/images/0081-00000057.jpg
   /home/docker/Jessi/Darknet_training/darknet/data/images/0081-00000058.jpg
   /home/docker/Jessi/Darknet_training/darknet/data/images/0081-00000059.jpg
   /home/docker/Jessi/Darknet_training/darknet/data/images/0081-00000060.jpg
```
In my case I have these files in scripts folder.

3- We have to change the cfg/voc.data config file to point to your data:

```ruby
   classes= 8
   train  = /home/docker/Jessi/Darknet_training/darknet/scripts/train.txt
   valid  = /home/docker/Jessi/Darknet_training/darknet/scripts/test.txt
   names = /home/docker/Jessi/Darknet_training/darknet/data/voc.names
   backup = /home/docker/Jessi/Darknet_training/darknet/backup
```
voc.names is a file where the names of the classes that we want to train are indicated. backup is a folder where all the results will be saved. The weights file are saved very 100 iterations during the first 1000 iterations and then every 1000 iterations.  If you need change this you have to edit the line 138 of examples/detector.c (if(i%1000==0 || (i < 1000 && i%100 == 0))).

My voc.names is the next:

```ruby
   None
   motorcycle
   car
   van
   bus
   truck
   small-truck
   tank-truck
```

4- For training we use convolutional weights that are pre-trained on Imagenet. We use weights from the darknet53 model. You can just download the weights for the convolutional layers [here (76 MB)](https://pjreddie.com/media/files/darknet53.conv.74).

5- You have to edit the .cfg file. In my case I used yolov3-voc.cfg. You must modify the following:
 * change line batch to [`batch=64`](https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L6)
  * change line subdivisions to [`subdivisions=16`](https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L7)
  * change line `classes=8` to your number of objects in each of 3 `[yolo]`-layers:
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L611
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L695
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L779
  * change [`filters=39`] to filters=(classes + 5)x3 in the 3 `[convolutional]` before each `[yolo]` layer
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L605
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L689
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L773

  So if `classes=1` then should be `filters=18`. If `classes=2` then write `filters=21`.
  * you need to calculate the anchors. For it,  you have to execute gen_anchor.py:
      ```ruby
  	 python gen_anchor.py -filelist train.txt -output_dir anchors -num_clusters 9
      ```
      You will get the anchors.txt in the anchors folder.With it, you have to change line `anchors` in each of 3 `[yolo]`
      layers:
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L610
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L694
      * https://github.com/RoboticsURJC-students/2018-tfm-Jessica-Fernandez/blob/master/darknet/cfg/yolov3-voc.cfg#L778

6- Now we can train! Run the command:

```ruby
./darknet detector train cfg/voc.data cfg/yolov3-voc.cfg darknet53.conv.74
```

During training, you will see varying indicators of error, and you should stop when no longer decreases 0.XXXXXXX avg:

```ruby
Region Avg IOU: 0.798363, Class: 0.893232, Obj: 0.700808, No Obj: 0.004567, Avg Recall: 1.000000, count: 8 Region Avg IOU: 0.800677, Class: 0.892181, Obj: 0.701590, No Obj: 0.004574, Avg Recall: 1.000000, count: 8

9002: 0.211667, 0.060730 avg, 0.001000 rate, 3.868000 seconds, 576128 images Loaded: 0.000000 seconds
```

* 9002 - iteration number (number of batch)
* 0.060730 avg - average loss (error) - the lower, the better

When you see that average loss 0.xxxxxx avg no longer decreases at many iterations then you should stop training.
