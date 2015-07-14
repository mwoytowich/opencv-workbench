#include <iostream>
#include <fstream>
#include <string>
#include <algorithm>

#include <opencv_workbench/syllo/syllo.h>

#include <opencv_workbench/rapidxml/rapidxml.hpp>
#include <opencv_workbench/rapidxml/rapidxml_utils.hpp>
#include <opencv_workbench/rapidxml/rapidxml_print.hpp>
//#include <opencv_workbench/rapidxml/rapidxml_iterators.hpp>

#include <opencv_workbench/plot/Plot.h>

#include "AnnotationParser.h"

using std::cout;
using std::endl;

using namespace rapidxml;

AnnotationParser::AnnotationParser()
{
     
}

void AnnotationParser::reset()
{
     video_filename_ = "";
     xml_filename_ = "";
     dir_ = "";
     basename_ = "";
     
     frames.clear();
}

void AnnotationParser::clear()
{
     frames.clear();
}

int AnnotationParser::CheckForFile(std::string video_file, 
                                    AnnotationParser::AnnotateType_t ann_type)
{
     video_filename_ = video_file;
     
     int lastindex = video_file.find_last_of("."); 
     xml_filename_ = video_file.substr(0, lastindex);
     
     if (ann_type == hand) {
          xml_filename_ += ".truth.xml";
     } else if (ann_type == track) {
          xml_filename_ += ".tracks.xml";
     } else {
          cout << "ERROR: Invalid annotation type" << endl;
     }         
     
     dir_ = fs::path(video_file).parent_path();
     basename_ = fs::path(video_file).filename();

     if ( !boost::filesystem::exists( xml_filename_ ) ) {
          if (ann_type == hand) {
               cout << "Hand ";
          } else if (ann_type == track) {
               cout << "Track ";
          }
          cout << "annotation file doesn't exist yet." << endl;
          return 1;
     } else {
          if (ann_type == hand) {
               cout << "Hand ";
          } else if (ann_type == track) {
               cout << "Track ";
          }
          cout << "annotation file found." << endl;
          return this->ParseFile(xml_filename_);
     }
}

void AnnotationParser::write_annotation()
{
     this->write_header();
}

void AnnotationParser::write_header()
{     
     xml_document<> doc;
     xml_node<> *root_node = doc.allocate_node(node_element, "annotation");
     doc.append_node(root_node);

     xml_node<> *folder_node = doc.allocate_node(node_element, "folder", dir_.c_str());
     root_node->append_node(folder_node);
     
     xml_node<> *filename_node = doc.allocate_node(node_element, "filename", basename_.c_str());
     root_node->append_node(filename_node);

     xml_node<> *type_node = doc.allocate_node(node_element, "type", "video");
     root_node->append_node(type_node);

     xml_node<> *size_node = doc.allocate_node(node_element, "size");
     root_node->append_node(size_node);

     ///////////////////////////
     /// Width, height, depth
     ///////////////////////////
     std::string width = syllo::int2str(width_);
     xml_node<> *width_node = doc.allocate_node(node_element, "width", width.c_str());
     size_node->append_node(width_node);

     std::string height = syllo::int2str(height_);
     xml_node<> *height_node = doc.allocate_node(node_element, "height", height.c_str());
     size_node->append_node(height_node);

     std::string depth = syllo::int2str(depth_);
     xml_node<> *depth_node = doc.allocate_node(node_element, "depth", depth.c_str());
     size_node->append_node(depth_node);

     // Number of frames
     std::string number_of_frames = syllo::int2str(number_of_frames_);
     xml_node<> *num_frames_node = doc.allocate_node(node_element, "number_of_frames", number_of_frames.c_str());
     root_node->append_node(num_frames_node);

     // Number of Annotated frames
     std::string number_of_annotated_frames = syllo::int2str(frames.size());    
     xml_node<> *num_annotated_frames_node = doc.allocate_node(node_element, "number_of_annotated_frames", number_of_annotated_frames.c_str());
     root_node->append_node(num_annotated_frames_node);
     
     // Frames
     xml_node<> *frames_node = doc.allocate_node(node_element, "frames");
     root_node->append_node(frames_node);

     // Loop through each frame
     std::map<int,Frame>::iterator frame_it = frames.begin();
     for(; frame_it != frames.end(); frame_it++) {         
          
          // "frame"
          xml_node<> *frame_node = doc.allocate_node(node_element, "frame");
          frames_node->append_node(frame_node);

          // "frame_number"
          char * frame_number = doc.allocate_string(syllo::int2str(frame_it->second.frame_number()).c_str());
          xml_node<> *frame_number_node = doc.allocate_node(node_element, "frame_number", frame_number);
          frame_node->append_node(frame_number_node);         
          
          // Loop through each object          
          std::map<std::string, Object>::iterator object_it;
          object_it = frame_it->second.objects.begin();
          for (; object_it != frame_it->second.objects.end(); object_it++) {

               // "object" node
               xml_node<> *object_node = doc.allocate_node(node_element, "object");
               frame_node->append_node(object_node);
               
               // "Name" node               
               char * name = doc.allocate_string(object_it->second.name().c_str());
               xml_node<> *name_node = doc.allocate_node(node_element, "name", name);
               object_node->append_node(name_node);

               // "ID" node               
               char * id = doc.allocate_string(syllo::int2str(object_it->second.id()).c_str());
               xml_node<> *id_node = doc.allocate_node(node_element, "ID", id);
               object_node->append_node(id_node);

               // "Age" node               
               char * age = doc.allocate_string(syllo::int2str(object_it->second.age()).c_str());
               xml_node<> *age_node = doc.allocate_node(node_element, "age", age);
               object_node->append_node(age_node);

               // "pose" node               
               char * pose = doc.allocate_string("Unspecified");
               xml_node<> *pose_node = doc.allocate_node(node_element, "pose", pose);
               object_node->append_node(pose_node);

               // "truncated" node               
               char * truncated = doc.allocate_string("0");
               xml_node<> *truncated_node = doc.allocate_node(node_element, "truncated", truncated);
               object_node->append_node(truncated_node);

               // "difficult" node               
               char * difficult = doc.allocate_string("0");
               xml_node<> *difficult_node = doc.allocate_node(node_element, "difficult", difficult);
               object_node->append_node(difficult_node);

               // "bndbox" node               
               xml_node<> *bndbox_node = doc.allocate_node(node_element, "bndbox");
               object_node->append_node(bndbox_node);
               
               // "xmin" node               
               char * xmin = doc.allocate_string(syllo::int2str(object_it->second.bbox.xmin()).c_str());
               xml_node<> *xmin_node = doc.allocate_node(node_element, "xmin", xmin);
               bndbox_node->append_node(xmin_node);

               // "ymin" node               
               char * ymin = doc.allocate_string(syllo::int2str(object_it->second.bbox.ymin()).c_str());
               xml_node<> *ymin_node = doc.allocate_node(node_element, "ymin", ymin);
               bndbox_node->append_node(ymin_node);

               // "xmax" node               
               char * xmax = doc.allocate_string(syllo::int2str(object_it->second.bbox.xmax()).c_str());
               xml_node<> *xmax_node = doc.allocate_node(node_element, "xmax", xmax);
               bndbox_node->append_node(xmax_node);

               // "ymax" node               
               char * ymax = doc.allocate_string(syllo::int2str(object_it->second.bbox.ymax()).c_str());
               xml_node<> *ymax_node = doc.allocate_node(node_element, "ymax", ymax);
               bndbox_node->append_node(ymax_node);                              
          }
     }
     
     // Prints the document to screen
     //std::cout << doc;
    
     std::ofstream outfile;
     outfile.open(xml_filename_.c_str());          
     outfile << doc;
     outfile.close();
}

int AnnotationParser::ParseFile(std::string file)
{
     xml_filename_ = file;

     rapidxml::file<> xmlFile(file.c_str());
     rapidxml::xml_document<> doc;
     doc.parse<0>(xmlFile.data());

     // First node should be named <annotation>
     if (strcmp(doc.first_node()->name(), "annotation") != 0) {
          cout << "Invalid xml file: annotation" << endl;
          return -1;
     }

     // Find <frames>
     xml_node<> * frames_node = doc.first_node()->first_node("frames");
     if (frames_node == 0) {
          return -1;          
     }
     
     // Loop through all frames
     xml_node<> *frame_node = frames_node->first_node("frame");
     do {
          if (frame_node == 0) {
               cout << "Missing frame node" << endl;
               return -1;
          }
          
          xml_node<> *frame_number_node = frame_node->first_node("frame_number");
          if (frame_number_node == 0) {               
               cout << "Missing frame number" << endl;
               return -1;
          }
                              
          int frame_number = syllo::str2int(frame_number_node->value());
                    
          Frame frame;
          frame.set_frame_number(frame_number);
          
          // Loop through objects in this frame.
          xml_node<> *object_node = frame_node->first_node("object");
          do {
               if (object_node == 0) {
                    //cout << "Missing an object node" << endl;
                    break;
               }               
               
               std::string object_name = object_node->first_node("name")->value();
               
               Object object;
               object.set_name(object_name);
               
               xml_node<> *box = object_node->first_node("bndbox");
               if (box == 0) {
                    cout << "Missing bounding box" << endl;
                    return -1;
               }
               
               int xmin, ymin, xmax, ymax;               
               xmin = syllo::str2int(box->first_node("xmin")->value());
               ymin = syllo::str2int(box->first_node("ymin")->value());
               xmax = syllo::str2int(box->first_node("xmax")->value());
               ymax = syllo::str2int(box->first_node("ymax")->value());

               object.bbox = BoundingBox(xmin,xmax,ymin,ymax);
               
               frame.objects[object_name] = object;
               
          }while ((object_node = object_node->next_sibling()) != 0 );
          
          frames[frame_number] = frame;
          
     }while ((frame_node = frame_node->next_sibling()) != 0 );
     
     return 0;
}

void AnnotationParser::print()
{
     std::map<int,Frame>::iterator frame_it = frames.begin();
     for(; frame_it != frames.end(); frame_it++) {
          cout << "-----------------------------" << endl;
          cout << "Frame Number: " << frame_it->second.frame_number() << endl; 
          cout << "---------" << endl;
          cout << "Objects: " << endl;
          
          std::map<std::string, Object>::iterator object_it;
          object_it = frame_it->second.objects.begin();
          for (; object_it != frame_it->second.objects.end(); object_it++) {
               cout << "\t Name: " << object_it->second.name() << endl;
               cout << "\t Bbox: (" << object_it->second.bbox.xmin() << ","
                    << object_it->second.bbox.ymin() << ","
                    << object_it->second.bbox.xmax() << ","
                    << object_it->second.bbox.ymax() << ")" << endl << endl;
          }
     }
}

bool AnnotationParser::export_roi()
{
     if (video_filename_ == "") {
          cout << "No annotation file" << endl;
          return false;
     }
     
     // First determine size of movie we will export
     // Currently, find average height and width of all video frames
     int width_sum = 0, height_sum = 0;
     int count = 0;
     
     // Assumes only single ROI per frame
     std::map<int,Frame>::iterator frame_it = frames.begin();
     for(; frame_it != frames.end(); frame_it++) {
          std::map<std::string, Object>::iterator object_it;
          object_it = frame_it->second.objects.begin();
          for (; object_it != frame_it->second.objects.end(); object_it++) {
               width_sum += object_it->second.bbox.width();
               height_sum += object_it->second.bbox.height();
               count++;
          }
     }
     
     double width_avg = (double)width_sum / (double)count;
     double height_avg = (double)height_sum / (double)count;
     
     // New Video File to Write
     int lastindex = video_filename_.find_last_of("."); 
     std::string roi_fn = video_filename_.substr(0, lastindex) + 
          std::string("_roi.avi");
     cout << "Output: " << roi_fn << endl;
     
     cv::Size size(width_avg, height_avg); // height width?
     cv::VideoWriter out(roi_fn, CV_FOURCC('M','J','P','G'), 23, size, true);
     if (!out.isOpened()) {
          cout << "Failed to Export ROI" << endl;
          return false;
     }

     // Open original video file
     cv::VideoCapture in;
     in.open(video_filename_);
     if (!in.isOpened()) {
          cout << "Failed to open input video: Export ROI" << endl;
          return false;
     }
     
     // Find the centroid of each annotated frame, export the average
     // ROI size
     for(frame_it = frames.begin(); frame_it != frames.end(); frame_it++) {
          std::map<std::string, Object>::iterator object_it;
          object_it = frame_it->second.objects.begin();
          for (; object_it != frame_it->second.objects.end(); object_it++) {
               cv::Mat img;
               in.set(CV_CAP_PROP_POS_FRAMES, frame_it->second.frame_number());
               in.read(img);
               cv::Rect rect = object_it->second.bbox.ForceBox(width_avg, height_avg);
               cv::Mat roi(img, rect);
               out.write(roi);
          }
     }         
     cout << "Export ROI Complete" << endl;
     return true;
}

std::vector<std::string> AnnotationParser::track_names()
{
     // Get list of all track IDs
     std::vector<std::string> names;
     std::map<int,Frame>::iterator it_frame = frames.begin();
     for (; it_frame != frames.end(); it_frame++) {
          Frame frame = it_frame->second;
          
          // Loop through all objects in each frame
          std::map<std::string, Object>::iterator it_obj = frame.objects.begin();
          for (; it_obj != frame.objects.end(); it_obj++) {
               // Add the track name to the names vector only if it doesn't
               // already exist in the names vector
               if (std::find(names.begin(), names.end(), 
                             it_obj->first) == names.end()) {
                    names.push_back(it_obj->first);
               }
               //IDs[it_obj->first] = it_obj->first;
          }          
     }
     return names;
}

void AnnotationParser::plot_tracks(std::vector<std::string> &names)
{
     std::map<std::string, std::vector<cv::Point> > points;     
     
     // Loop through all frames, plotting tracks that match the user's input
     std::map<int,Frame>::iterator it_frame = frames.begin();
     for (; it_frame != frames.end(); it_frame++) {
          Frame frame = it_frame->second;
          
          // Loop through all objects in each frame
          std::map<std::string, Object>::iterator it_obj = frame.objects.begin();
          for (; it_obj != frame.objects.end(); it_obj++) {
               
               // Does this object name match any of the IDs we care about?
               if (std::find(names.begin(), names.end(), 
                             it_obj->first) != names.end()) {

                    // Push the point onto the appropriate points vector
                    points[it_obj->first].push_back(it_obj->second.bbox.centroid());
                    break;
               }     
          }          
     }

     // Plot the tracks;
     std::vector< std::vector<cv::Point> > vectors;
     const std::string title = "Tracks";
     std::vector<std::string> labels;
     std::vector<std::string> styles;

     std::map<std::string, std::vector<cv::Point> >::iterator it_points;
     for (it_points = points.begin(); it_points != points.end(); it_points++) {
          vectors.push_back(it_points->second);
          labels.push_back(it_points->first);
          //styles.push_back("points");
          //styles.push_back("lines");
          styles.push_back("linespoints");
     }
     
     //vectors.push_back(points);
     //labels.push_back("temp num");
     //styles.push_back("points");     
     
     syllo::Plot plot;
     //plot.gnuplot_test();
     plot.plot(vectors, title, labels, styles);
}

void AnnotationParser::write_gnuplot_data()
{
     //TODO, future work, maybe.
}

void AnnotationParser::score_detector(AnnotationParser &truth, 
                                   std::vector<std::string> &names)
{
     int TPs = 0, TNs = 0, FPs = 0, FNs = 0;

     // Starting with frame number 0, increment through the truth and detector
     // frames until both truth a detector frames are all processed.
     std::map<int,Frame>::const_iterator it_tru_frame = truth.frames.begin();
     std::map<int,Frame>::iterator it_detect_frame = frames.begin();
     for (int i = 0; it_tru_frame != truth.frames.end() && 
               it_detect_frame != frames.end(); i++) {
          
          // Does the truth vector have an entry for this frame?
          bool tru_frame_exists = false;
          Frame tru_frame;          
          if (it_tru_frame != truth.frames.end() && it_tru_frame->first == i) {
               // Grab the truth frame
               tru_frame = it_tru_frame->second;
               tru_frame_exists = true;
               // Increment the frame pointer
               it_tru_frame++;
          }

          // Does the detector have an entry for this frame?
          bool detect_frame_exists = false;
          Frame detect_frame;
          if (it_detect_frame != frames.end() && it_detect_frame->first == i) {
               // Grab the detector's frame
               detect_frame = it_detect_frame->second;
               detect_frame_exists = true;
               // Increment the frame pointer
               it_detect_frame++;
          }

          if (!tru_frame_exists && !detect_frame_exists) {
               // Neither a true frame or detected frame exists.
               // Thus, the detector didn't incorrectly detect an object.
               TNs++;
          } else if (!tru_frame_exists && detect_frame_exists) {
               // If a truth frame doesn't exist, but a detected frame exists
               // determine if the detected frame labelled an object we care
               // about
               std::vector<std::string>::iterator it_name = names.begin();
               for (; it_name != names.end(); it_name++) {
                    // Does this object exist in the detector frame?
                    Object detected_obj;
                    bool detect_obj_exists = false;
                    detect_obj_exists = detect_frame.contains_object(it_name->c_str(), 
                                                                     detected_obj);
                    if (detect_obj_exists) {
                         // The detector labelled an object, but it doesn't 
                         // exist in a truth frame.
                         FPs++;                         
                    } else {
                         // The object doesn't exist in the sequence of truth
                         // frames. Also, it doesn't exist in the detector
                         // frame.
                         TNs++;
                    }
               }               
          } else if (tru_frame_exists && !detect_frame_exists) {
               // If a truth frame exists, but a detector didn't detect
               // anything. We need to check to see if the truth frame has an 
               // object that we care about that the detector missed.
               std::vector<std::string>::iterator it_name = names.begin();
               for (; it_name != names.end(); it_name++) {
                    // Does this object exist in the truth frame?
                    Object tru_obj;
                    bool tru_obj_exists = false;
                    tru_obj_exists = tru_frame.contains_object(it_name->c_str(), 
                                                                     tru_obj);
                    if (tru_obj_exists) {
                         // The object exists in the truth frame, but doesn't
                         // exist in the detector frame. False Negative.
                         FNs++;
                    } else {
                         // The object doesn't exist in the truth frame and it
                         // doesn't exist in the detector frame. True Negative.
                         TNs++;
                    }
               }               
          } else if (tru_frame_exists && detect_frame_exists) {
               // Both a truth frame and a detector frame exists. Loop through 
               // all object names we care about and count stats.

               // Loop through the "names" vector, which contains names of
               // objects we care about scoring the detector on
               std::vector<std::string>::iterator it_name = names.begin();
               for (; it_name != names.end(); it_name++) {
                    // Search for this named object in ground truth frame
                    Object tru_obj;
                    bool tru_obj_exists;
                    tru_obj_exists = tru_frame.contains_object(it_name->c_str(), 
                                                               tru_obj);
                   
                    // Does this object exist in the detector frame?
                    Object detected_obj;
                    bool obj_exists;
                    obj_exists = detect_frame.contains_object(it_name->c_str(), 
                                                       detected_obj);
                    
                    if (!tru_obj_exists && !obj_exists) {
                         // If the object name doesn't exist in the truth frame
                         // or the detector frame, it is a True Negative
                         TNs++;                         
                    } else if (!tru_obj_exists && obj_exists) {
                         // If the object doesn't exist in the truth frame, 
                         // but exists in the detector frame, it is a false
                         // positive
                         FPs++;
                    } else if (tru_obj_exists && !obj_exists) {
                         // If the object exists in the truth frame, but it 
                         // doesn't exist in the detector frame, it is a false
                         // negative
                         FNs++;
                    } else if (tru_obj_exists && obj_exists) {
                         // If the object exists in both the truth frame and
                         // the detector frame, we have to determine if the
                         // detector's placement of the object's centroid is
                         // within the bounding box of the truth frame.
                         bool contains;
                         contains = tru_obj.bbox.contains(detected_obj.bbox.centroid());
                         if (contains) {
                              // The detected object's centroid is within the
                              // bounding box of the truth frame's object.
                              TPs++;
                         } else {
                              // The detected object's centroid is outside of
                              // the bounding box of the truth frame's object.
                              FPs++;
                         }
                    }
               }
          }
     }
         
     syllo::fill_line("+");
     cout << "True Positives: " << TPs << endl;
     cout << "True Negatives: " << TNs << endl;
     cout << "False Positives: " << FPs << endl;
     cout << "False Negatives: " << FNs << endl;
     syllo::fill_line("+");
}
