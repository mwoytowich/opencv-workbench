/****************************************************************************
 **
 ** Copyright (C) 2013 Digia Plc and/or its subsidiary(-ies).
 ** Contact: http://www.qt-project.org/legal
 **
 ** This file is part of the examples of the Qt Toolkit.
 **
 ** $QT_BEGIN_LICENSE:BSD$
 ** You may use this file under the terms of the BSD license as follows:
 **
 ** "Redistribution and use in source and binary forms, with or without
 ** modification, are permitted provided that the following conditions are
 ** met:
 **   * Redistributions of source code must retain the above copyright
 **     notice, this list of conditions and the following disclaimer.
 **   * Redistributions in binary form must reproduce the above copyright
 **     notice, this list of conditions and the following disclaimer in
 **     the documentation and/or other materials provided with the
 **     distribution.
 **   * Neither the name of Digia Plc and its Subsidiary(-ies) nor the names
 **     of its contributors may be used to endorse or promote products derived
 **     from this software without specific prior written permission.
 **
 **
 ** THIS SOFTWARE IS PROVIDED BY THE COPYRIGHT HOLDERS AND CONTRIBUTORS
 ** "AS IS" AND ANY EXPRESS OR IMPLIED WARRANTIES, INCLUDING, BUT NOT
 ** LIMITED TO, THE IMPLIED WARRANTIES OF MERCHANTABILITY AND FITNESS FOR
 ** A PARTICULAR PURPOSE ARE DISCLAIMED. IN NO EVENT SHALL THE COPYRIGHT
 ** OWNER OR CONTRIBUTORS BE LIABLE FOR ANY DIRECT, INDIRECT, INCIDENTAL,
 ** SPECIAL, EXEMPLARY, OR CONSEQUENTIAL DAMAGES (INCLUDING, BUT NOT
 ** LIMITED TO, PROCUREMENT OF SUBSTITUTE GOODS OR SERVICES; LOSS OF USE,
 ** DATA, OR PROFITS; OR BUSINESS INTERRUPTION) HOWEVER CAUSED AND ON ANY
 ** THEORY OF LIABILITY, WHETHER IN CONTRACT, STRICT LIABILITY, OR TORT
 ** (INCLUDING NEGLIGENCE OR OTHERWISE) ARISING IN ANY WAY OUT OF THE USE
 ** OF THIS SOFTWARE, EVEN IF ADVISED OF THE POSSIBILITY OF SUCH DAMAGE."
 **
 ** $QT_END_LICENSE$
 **
 ****************************************************************************/

#ifndef ANNOTATE_H
#define ANNOTATE_H

// OpenCV headers
#include <cv.h>
//#include <highgui.h>

#include <opencv2/imgproc/imgproc.hpp>
#include <opencv2/contrib/contrib.hpp>
#include <opencv2/core/core.hpp>
#include <opencv2/highgui/highgui.hpp>

#include <video_window/ui_VideoWindow.h>

#include <opencv_workbench/utils/Stream.h>
#include <opencv_workbench/utils/Frame.h>
#include <opencv_workbench/wb/Entity.h>
#include <opencv_workbench/utils/AnnotationParser.h>
#include <opencv_workbench/gui/cut/cut.h>
#include <opencv_workbench/gui/video_window/VideoWindow.h>
#include <opencv_workbench/gui/object_name/ObjectName.h>

#include <QTextStream>

class Annotate : public VideoWindow
{
     Q_OBJECT

     public:
     
     Annotate(VideoWindow *parent = 0);
     void export_roi();
     QString str2qstr(std::string str);
          
protected:     
     
     virtual void before_next_frame();
     virtual void before_display(cv::Mat &img);
     virtual void on_open();     
     virtual void on_mouseMoved(QMouseEvent * event);

     QMutex mutex_;

     std::map<std::string, wb::Entity> *objects_;
     int current_frame_;
     
     QDialog * object_name_dialog_;
     ObjectName *object_name_;

     int distance(QPoint p1, QPoint p2);
     bool nearby(QPoint p1, QPoint p2, int threshold);
     bool inside(QPoint p, QPoint p1, QPoint p2);
     
     bool point_present_;
     QPoint pt_;
     bool edit_pt_;
     
     bool box_present_;
     
     QPoint pt1_;
     QPoint pt2_;

     QPoint curr_pos_;

     bool box_selected_;
     std::map<std::string, wb::Entity>::iterator it_selected_;
     
     int near_thresh;
     bool edit_pt1_;
     bool edit_pt2_;
     bool edit_box_;

     QPoint pt1_drag_offset_;
     QPoint pt2_drag_offset_;             

     AnnotationParser parser_;

     bool edit_enabled_;          
     bool display_labels_;
     
private:
     
private slots:
     void mousePressed(QMouseEvent * event);
     void mouseReleased(QMouseEvent * event);     
     void save_annotation();
     void erase_box();
     void edit_enabled();     
     void display_labels();
};

#endif
