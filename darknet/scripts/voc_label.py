import xml.etree.ElementTree as ET
import pickle
import os
from os import listdir
from os.path import join
import argparse

classes = ["None", "motorcycle", "car", "van","bus", "truck", "small-truck", "tank-truck"]

def convert(size, box):
    dw = 1./(size[0])
    dh = 1./(size[1])
    x = (box[0] + box[1])/2.0 - 1
    y = (box[2] + box[3])/2.0 - 1
    w = box[1] - box[0]
    h = box[3] - box[2]
    x = x*dw
    w = w*dw
    y = y*dh
    h = h*dh
    return (x,y,w,h)

def convert_annotation(xml_dir, out_dir,image_id):
    xml_file = xml_dir+image_id
    in_file = open(xml_file)
    #image_id contain the xml names, so you have to eliminate the extension (3 character)
    label_file = out_dir+image_id[0:len(image_id)-3-1]+'.txt'
    out_file = open(label_file, 'w')
    tree=ET.parse(in_file)
    root = tree.getroot()
    size = root.find('size')
    w = int(size.find('width').text)
    h = int(size.find('height').text)

    for obj in root.iter('object'):
        difficult = obj.find('difficult').text
        cls = obj.find('name').text
        if cls not in classes or int(difficult) == 1:
            continue
        cls_id = classes.index(cls)
        xmlbox = obj.find('bndbox')
        b = (float(xmlbox.find('xmin').text), float(xmlbox.find('xmax').text), float(xmlbox.find('ymin').text), float(xmlbox.find('ymax').text))
        bb = convert((w,h), b)
        out_file.write(str(cls_id) + " " + " ".join([str(a) for a in bb]) + '\n')

    

def main():
    parser = argparse.ArgumentParser()
    parser.add_argument("-xml", help="Relative location of xml files directory", required=True)
    parser.add_argument("-xml_files", help="Names of xml files", required=True)
    parser.add_argument("-out", help="Relative location of output txt files directory", default="out")
        
    args = parser.parse_args()
    xml_dir = os.path.join(os.path.dirname(os.path.realpath('__file__')), args.xml)

    if not os.path.exists(xml_dir):
        print("Provide the correct folder for xml files.")
        sys.exit()

    out_dir = os.path.join(os.path.dirname(os.path.realpath('__file__')), args.out)
    if not os.path.exists(out_dir):
        os.makedirs(out_dir)

    xml_files = os.path.join(os.path.dirname(os.path.realpath('__file__')), args.xml_files)
    if not os.path.exists(xml_files):
        os.makedirs(xml_files)

    image_ids = open(xml_files).read().strip().split()
	
    for image_id in image_ids:
        convert_annotation(xml_dir, out_dir,image_id)
    

if __name__ == "__main__":
    main()
