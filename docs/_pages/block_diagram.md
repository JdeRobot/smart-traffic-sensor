---
title: Block Diagram
permalink: /block_diagram/

collection: portfolio
entries_layout: grid

classes: wide

sidebar:
  nav: "docs"
---

Smart-Traffic-Sensor consists of two parts:

1. Detection and Classification: the different vehicles are detected with Deep Learning. In this case, three different Deep Learning platforms can be used (Tensorflow, Keras and Darknet). Although the model that has obtained better results is the one implemented with Darknet.

2. Monitoring: STS uses vehicle association based on spatial proximity. In case there are problematic vehicles, KLT (Kanade-Lucas-Tomasi) is used

In the next image you can see a block diagram about Smart-Traffic-Sensor:

<img src="/assets/images/cover/diagramabloques.PNG" width="50%" height="30%">



