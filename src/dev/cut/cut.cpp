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

#include <QtGui>
#include <QResource>
#include <QSettings>

#include "cut.h"

using std::cout;
using std::endl;

CutForm::CutForm(QWidget *parent)
     : QWidget(parent)
{
     ui.setupUi(this);

     readSettings();

     min_ = 0;
     max_ = 0;

     //cout << parent->parent->fps_ << endl;
     connect(ui.cut_save_button, SIGNAL(released()), this, SLOT(emit_export()));
}

void CutForm::set_min_max(int min, int max)
{
     min_ = min;
     max_ = max;

     ui.start_frame_spinbox->setRange(min_,max_);
     ui.end_frame_spinbox->setRange(min_,max_);
}

void CutForm::emit_export()
{
     if (ui.start_frame_spinbox->value() > ui.end_frame_spinbox->value()) {
          QMessageBox::critical(this,tr("Error"), tr("Start frame number is larger than end frame number."));
     } else if (ui.start_frame_spinbox->value() < min_) {
          QMessageBox::critical(this,tr("Error"), tr("You selected a frame number less than the minimum!"));
     } else if (ui.end_frame_spinbox->value() > max_) {
          QMessageBox::critical(this,tr("Error"), tr("You selected a frame number more than the maximum!"));
     } else {
          filename_ = "";

          // Define the file type strings
          QString str_avi = "AVI (*.avi)";
          QString str_mpg = "Motion JPEG (*.MPG)";
          QString str_mp4 = "MP4 (*.MP4)";

          // Build the file type filter
          QString str_filter = str_avi + QString(" ;; ") + str_mpg + QString(" ;; ") + str_mp4;

          // Create the dialog window
          QFileDialog * save_diag = new QFileDialog(this, tr("Save file"), QDir::homePath(), tr(str_filter.toStdString().c_str()));
          save_diag->setAcceptMode(QFileDialog::AcceptSave);

          bool file_selected = false;
          if (save_diag->exec()) {
               file_selected = true;
               // Get the selected file
               filename_ = save_diag->selectedFiles()[0];

               // Add the appropriate extension
               QString ext_select = save_diag->selectedNameFilter();               
               if ( ext_select == str_avi) {
                    filename_ += ".avi";
               } else if (ext_select == str_mpg) {                    
                    filename_ += ".mpg";
               } else if (ext_select == str_mp4) {
                    filename_ += ".mp4";
               } else {
                    filename_ += ".avi";
               }                              
          }              
          delete save_diag;
          
          // If a filename was successfully selected and generated
          // Emit a signal to the main window to export the video
          if (file_selected) {
               emit export_video();
          }
     }
}

int CutForm::get_start_frame()
{
     return ui.start_frame_spinbox->value();
}

int CutForm::get_end_frame()
{
     return ui.end_frame_spinbox->value();
}

QString CutForm::get_filename()
{
     return filename_;
}
     

void CutForm::writeSettings()
{
     QSettings settings;

     //// Window size and position
     //settings.beginGroup("MainWindow");
     //settings.setValue("size", size());
     //settings.setValue("pos", pos());
     //
     //// Previously opened directory
     //settings.setValue("prev_open_path", prev_open_path_);
     //
     //settings.endGroup();
}

void CutForm::readSettings()
{
     //QSettings settings;
     //
     //settings.beginGroup("MainWindow");
     //
     //// Window size
     //resize(settings.value("size", QSize(400, 400)).toSize());
     //move(settings.value("pos", QPoint(200, 200)).toPoint());
     //
     //// Previously opened directory
     //prev_open_path_ = settings.value("prev_open_path").toString();
     //
     //settings.endGroup();
}
