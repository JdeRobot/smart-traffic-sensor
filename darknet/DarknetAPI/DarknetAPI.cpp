//
// Created by frivas on 25/04/16.
//

#include "DarknetAPI.h"
#include <iostream>
#include "DarknetAPIConversions.h"

//char *voc_names2[] = {"gable_main", "hip_main", "hip_ext", "hipgable_main", "hipgable_ext",
//                              "nested_turret"};
//image voc_labels[6];


char *voc_names2[] = {"None","motorcycle","car","van","bus","truck","small-truck","tank-truck"};
image voc_labels[8];




void addDetection(image& im, int num, float thresh, detection *dets, char **names, image *labels, int classes, DarknetDetections& detections){

    int i,j;
    int classid;
    float prob;
    for(i = 0; i < num; ++i){
        char labelstr[4096] = {0};
        classid = -1;
        for(j = 0; j < classes; ++j){
            if (dets[i].prob[j] > thresh){
                if (classid < 0) {
                    strcat(labelstr, names[j]);
                    classid = j;
		    prob = dets[i].prob[j];
                } else {
                    strcat(labelstr, ", ");
                    strcat(labelstr, names[j]);
                }
                printf("%s: %.0f%%\n", names[j], dets[i].prob[j]*100);
            }
        }
        if(classid >= 0){
            /*int width = pow(prob, 1./2.)*10+1;
            int offset = classid*17 % classes;*/
            box b = dets[i].bbox;

            int left  = (b.x-b.w/2.)*im.w;
            int right = (b.x+b.w/2.)*im.w;
            int top   = (b.y-b.h/2.)*im.h;
            int bot   = (b.y+b.h/2.)*im.h;

            if(left < 0) left = 0;
            if(right > im.w-1) right = im.w-1;
            if(top < 0) top = 0;
            if(bot > im.h-1) bot = im.h-1;

            box detectedbox;

            detectedbox.x = left;
            detectedbox.y = top;
            detectedbox.h = bot - top;
            detectedbox.w = right - left;

//            std::cout<<"---------------------:" << detectedbox.x<<" "<<detectedbox.y<<" "<<detectedbox.w<<" "<<detectedbox.h<<std::endl;
            DarknetDetection detection(detectedbox,classid,prob);

            detections.push_back(detection);


        }
    }
}


DarknetDetections processImageDetection(network *net,image& im, float thresh ){

    float hier_thresh=0.5;


    DarknetDetections detections;

    c_set_batch_network(net, 1);
    srand(2222222);
    clock_t time;
    char buff[256];
    char *input = buff;
    int j;
    float nms = .3;

    image sized = c_letterbox_image(im, net->w, net->h);
    layer l = net->layers[net->n-1];


    float *X = sized.data;
    time=clock();
    c_network_predict(net, X);
    printf("%s: Predicted in %f seconds.\n", input, c_sec(clock()-time));
    int nboxes = 0;
    detection *dets = get_network_boxes(net, im.w, im.h, thresh, hier_thresh, 0, 1, &nboxes);
    /*c_get_region_boxes(l, im.w, im.h, net->w, net->h, thresh, probs, boxes, masks, 0, 0, hier_thresh, 1);*/
    if (l.softmax_tree && nms)
        c_do_nms_obj(dets, nboxes, l.classes, nms);
    else if (nms)
        c_do_nms_sort(dets, nboxes, l.classes, nms);



    addDetection( im, nboxes, thresh, dets, voc_names2, 0, l.classes, detections );
    c_free_image(sized);

    return detections;

}


DarknetDetections processImageDetection(network *net,const  cv::Mat & im, float thresh){
    image imDark= cv_to_image(im);
    auto detection= processImageDetection(net,imDark,thresh);
    c_free_image(imDark);
    return detection;

}



DarknetAPI::DarknetAPI(char *cfgfile, char *weightfile) {
    net = c_parse_network_cfg(cfgfile);
    if (weightfile != nullptr) {
        c_load_weights(net, weightfile);
    }
}
DarknetAPI::~DarknetAPI(){
     c_free_network(net);
}


DarknetDetections DarknetAPI::process(image& im, float thresh ){
    return processImageDetection(this->net,im,thresh);
}

DarknetDetections DarknetAPI::process(const cv::Mat &im, float thresh) {
    image imDark= cv_to_image(im);
    return processImageDetection(this->net,imDark,thresh);
}

std::string DarknetAPI::processToJson(const cv::Mat &im, float thresh) {
    return process(im,thresh).serialize();
}
