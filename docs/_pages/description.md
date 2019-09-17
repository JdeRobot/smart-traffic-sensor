---
permalink: /description/
title: "Description"

sidebar:
  nav: "docs"

#toc: true
#toc_label: "TOC installation"
#toc_icon: "cog"
youTube_id: xDzGYq1Kl-8 
youtube_id: SKO2n4zFbnU
---

Smart-Traffic-Sensor (a.k.a STS) is an advanced vision based vehicles traffic monitoring software, able to gather several traffic stats using a simple stationary camera. STS software starts from [Traffic Monitor](https://github.com/JdeRobot/traffic-monitor),which use old techniques like background detection and 3D models. STS uses deep learning to process traffic flow in real-time. It's a multiplatform system, it supports networks implemented in Keras, Tensorflow and Darknet. In the current version the software is able to:

- Classify the vehicles into seven categories: Motorcycle, Car, Van, Bus, Small-Truck, Truck and Tank-Truck
- Estimate the speed of the vehicles
- Vehicles count
- Detect vehicles with old techniques (3D models and background detection) or deep learning (Darknet, Tensorflow and Keras)
- STS can work in different weather conditions and with different quality cameras.

In the next video you can see STS works with a high quality video:
{% include video id="xDzGYq1Kl-8" provider="youtube" %}

In the next video you can see STS works with a low quality and unfavorable weather conditions video:
{% include video id="SKO2n4zFbnU" provider="youtube" %}
