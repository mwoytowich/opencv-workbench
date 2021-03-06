#include <iostream>
#include <fstream>
#include <string>
#include <stdio.h>
#include <time.h>

// OpenCV headers
#include <cv.h>
#include <highgui.h>

#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <opencv_workbench/utils/Stream.h>
#include <opencv_workbench/syllo/syllo.h>

#include <opencv_workbench/utils/AnnotationParser.h>
#include <opencv_workbench/plot/Plot.h>

#include <opencv_workbench/detector/Detector.h>
#include <opencv_workbench/plugin_manager/PluginManager.h>

using std::cout;
using std::endl;

PluginManager<Detector, Detector_maker_t> plugin_manager_;

int main(int argc, char *argv[])
{    
     //int aflag = 0;
     //int bflag = 0;
     //char *cvalue = NULL;
     //int index;
     int hide_window_flag = 0;
     int step_flag = 0;
     int c;
     //std::string video_filename = "/home/syllogismrxs/Documents/Thesis/data/sonar-avi/2014-01-24-ME-sonar-only-diver/2014_01_24_15_24_06.avi";
     std::string video_filename = "/home/syllogismrxs/Documents/Thesis/data/NEEMO/neemo-sonar/2015_07_29-Walking-Habitat/2015_07_29_13_28_17-hab-diver-fish.son";
     std::string plugin_name = "relative_detector";
     std::string xml_output_dir = "";
     //int xml_output_dir_flag = 0;
  
     while ((c = getopt (argc, argv, "abc:f:hp:o:s")) != -1) {
          switch (c) {
          //case 'a':
          //     aflag = 1;
          //     break;
          //case 'b':
          //     bflag = 1;
          //     break;
          //case 'o':
          //     xml_output_dir_flag = 1;
          //     xml_output_dir = std::string(optarg);
          //     break;
          case 'h':
               hide_window_flag = 1;
               break;
          case 'p':
               plugin_name = std::string(optarg);
               break;
          case 'f':
               video_filename = std::string(optarg);
               break;
          case 's':
               step_flag = 1;
               break;
          case '?':
               if (optopt == 'c') {
                    fprintf (stderr, "Option -%c requires an argument.\n", optopt);
               } else if (optopt == 'f') {
                    fprintf (stderr, "Option -%c requires a video filename as an argument.\n", optopt);
               } else if (optopt == 'o') {
                    fprintf (stderr, "Option -%c requires an output directory as an argument.\n", optopt);
               } else if (optopt == 'p') {
                    fprintf (stderr, "Option -%c requires a plugin name as an argument.\n", optopt);
               } else if (isprint (optopt)) {
                    fprintf (stderr, "Unknown option `-%c'.\n", optopt);
               } else {
                    fprintf (stderr,
                             "Unknown option character `\\x%x'.\n",
                             optopt);
               }
               return 1;
          default:
               abort ();
          }
     }
          
     syllo::fill_line("=");
     cout << "Annotated Metrics" << endl;
     syllo::fill_line("=");
     
     syllo::Stream stream;
     syllo::Status status = stream.open(video_filename);
     
     if (status != syllo::Success) {
          cout << "Failed to open: " << video_filename << endl;
          return -1;
     }

     // Setup Hand Annotated Parser (Truth)
     //bool hand_ann_found = true;
     AnnotationParser parser_truth;
     int retcode = parser_truth.CheckForFile(video_filename, AnnotationParser::hand);
     if (retcode != 0) {
          cout << "Error parsing hand annotated file." << endl;
          //hand_ann_found = false;          
     }

     //// Setup Annotation Parser_Tracks
     //AnnotationParser parser_tracks;
     //parser_tracks.CheckForFile(video_filename, AnnotationParser::track);
     //parser_tracks.clear();
     //parser_tracks.set_width(stream.width());
     //parser_tracks.set_height(stream.height());     
     //parser_tracks.set_depth(3);
     //parser_tracks.set_type("video");
     //parser_tracks.set_number_of_frames(stream.get_frame_count());
     //parser_tracks.set_plugin_name(plugin_name);
     //
     //if (xml_output_dir_flag == 1) {
     //     parser_tracks.set_xml_output_dir(xml_output_dir);
     //}
     //
     //// Load the Bridge shared library (based on yml file)
     //retcode = plugin_manager_.search_for_plugins("OPENCV_WORKBENCH_PLUGIN_PATH");
     //if (retcode != 0) {
     //     cout << "Failed to find plugins." << endl;
     //     return -1;
     //}
     //
     //retcode = plugin_manager_.open_library(plugin_name);
     //if (retcode != 0) {
     //     cout << "Unable to open library: " << plugin_name << endl;
     //     
     //     return -1;
     //} else {
     //     cout << "Using Bridge Library: " << plugin_name << endl;
     //}
     //
     //Detector * detector_;     
     //detector_ = plugin_manager_.object();
     //detector_->set_stream(&stream);
     //detector_->print();       
     //
     //if (hide_window_flag) {
     //     detector_->hide_windows(true);
     //} else {
     //     detector_->hide_windows(false);
     //}
     
     cv::Point2f prev_point;
     cv::Mat original;
     double prev_bearing = 0;
     double bearing_total = 0;
     bool start = false;
     int frame_number = 0;
     while (stream.read(original)) {
          cv::Mat img = original.clone();
          
          if (parser_truth.frames.count(frame_number) > 0) {
               if (parser_truth.frames[frame_number].objects.count("point:1") > 0) {
                    cv::Rect rect = parser_truth.frames[frame_number].objects["point:1"].bbox().rectangle();
                    cv::Point pt(cvRound((rect.tl().x + rect.br().x)/2.0), 
                                cvRound((rect.tl().y + rect.br().y)/2.0));
                    
                    cv::circle(img, pt, 1, cv::Scalar(0,0,0), 1, 8, 0);
                    cv::circle(img, pt, 5, cv::Scalar(255,255,255), 1, 8, 0);
                    
                    //double range, bearing;
                    double bearing;
                    //range = stream.pixel_range(pt.y, pt.x);
                    bearing = stream.pixel_bearing(pt.y, pt.x);
                    
                    if (start) {
                         double bearing_diff = std::abs(prev_bearing - bearing);
                         cout << "Bearing Diff: " <<  bearing_diff << endl;
                         bearing_total += bearing_diff;
                         cout << "Bearing Total: " << bearing_total << endl;
                    }
                    
                    start = true;                    
                    prev_bearing = bearing;
                    //cv::Point2f point;
                    //point.x = range*sin(bearing);
                    //point.y = range*cos(bearing);

                    //cv::Point2f diff;
                    //diff = point - prev_point;
                    
                    //prev_point = point;
               }
          } 
          
          cv::imshow("Point", img);
          
          
          // Pass the frame to the detector plugin
          //detector_->set_frame(frame_number, original);

          // Get track list from detector
          //std::vector<wb::Entity> tracks = detector_->tracks();
          
          //// Put all track data in parser for saving and
          //// Draw estimated diver locations on original image
          //Frame frame;
          //frame.set_frame_number(frame_number);
          //std::vector<wb::Entity>::iterator it = tracks.begin();
          //for (; it != tracks.end(); it++) {
          //     cv::Point point = it->centroid();
          //     
          //     frame.objects[it->name()] = *it;
          //
          //     if (it->type() == wb::Entity::Diver) {
          //          // If this is a diver type, mark it on the original image
          //          int radius = 3;
          //          cv::circle(original, cv::Point(point.x,point.y), 
          //                     radius, cv::Scalar(0,0,0), 2, 8, 0);
          //     }
          //}     
          //// Save frame to parser
          //parser_tracks.frames[frame_number] = frame;

          if (!hide_window_flag) { 
               
               if (step_flag) {
                    int key = cv::waitKey(0);    
                    if (key == 'q' || key == 1048689) { // 'q' key
                         cout << "Ending early." << endl;
                         break;
                    } else if (key == 'p' || key == 1048688) {
                         step_flag = 0;
                    }     
               } else {
                    int key = cv::waitKey(1);               
                    if (key == 'q' || key == 1048689) {
                         cout << "Ending early." << endl;
                         break;
                    } else if (key == 'p' || key == 1048688) {
                         step_flag = 1;
                    } 
               }

          }                    
          frame_number++;
     }          
     cout << "Done Processing." << endl;
     return 0;
}
