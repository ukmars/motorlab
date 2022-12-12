#!/usr/bin/env python3
# -*- coding:utf-8 -*-
###
# Project: <<project>>
# File:    motorlab-dashboard.py
# File Created: Thursday, 8th December 2022 10:25:57 pm
# Author: Peter Harrison 
# -----
# Last Modified: Sunday, 11th December 2022 9:39:15 am
# -----
# Copyright 2022 - 2022 Peter Harrison, Micromouseonline
# -----
# Licence:
# MIT License
# 
# Copyright (c) 2022 Peter Harrison
# 
# Permission is hereby granted, free of charge, to any person obtaining a copy of
# this software and associated documentation files (the "Software"), to deal in
# the Software without restriction, including without limitation the rights to
# use, copy, modify, merge, publish, distribute, sublicense, and/or sell copies
# of the Software, and to permit persons to whom the Software is furnished to do
# so, subject to the following conditions:
# 
# The above copyright notice and this permission notice shall be included in all
# copies or substantial portions of the Software.
# 
# THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND, EXPRESS OR
# IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF MERCHANTABILITY,
# FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. IN NO EVENT SHALL THE
# AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY CLAIM, DAMAGES OR OTHER
# LIABILITY, WHETHER IN AN ACTION OF CONTRACT, TORT OR OTHERWISE, ARISING FROM,
# OUT OF OR IN CONNECTION WITH THE SOFTWARE OR THE USE OR OTHER DEALINGS IN THE
# SOFTWARE.
###

# see here for pyqtgraph tutorial
#    https://www.pythonguis.com/tutorials/plotting-pyqtgraph/
import os
import sys
import time
import numpy as np

from serial import Serial
import serial.tools.list_ports

from PyQt5.QtGui import (QFont, QColor)
from PyQt5.QtCore import (Qt,QThread,pyqtSignal,pyqtSlot)
from PyQt5.QtWidgets import (
    QApplication, QMainWindow,
    QStatusBar,QAction,
    QDesktopWidget,
    QSpacerItem, QMessageBox, QSizePolicy,
    QWidget, QGroupBox, QFrame,
    QSpinBox, QDoubleSpinBox,
    QGridLayout,QHBoxLayout,QVBoxLayout,
    QPlainTextEdit, QPushButton,
    QRadioButton, QLabel)

import pyqtgraph as pg

if hasattr(Qt, 'AA_EnableHighDpiScaling'):
    QApplication.setAttribute(Qt.AA_EnableHighDpiScaling, True)

if hasattr(Qt, 'AA_UseHighDpiPixmaps'):
    QApplication.setAttribute(Qt.AA_UseHighDpiPixmaps, True)

# App versioning
APP_VERSION = '0.1.0'
# App name
APP_NAME = 'UKMARS MotorLab'

palette = ("#101418", "#c00000", "#c000c0", "#c06000", "#00c000", "#0072c3", "#6fdc8c", "#d2a106")

# Define the resource path
RESOURCE_PATH = 'res'
if hasattr(sys, '_MEIPASS'):
    RESOURCE_PATH = os.path.join(sys._MEIPASS, RESOURCE_PATH)
    os.environ['PATH'] = sys._MEIPASS + '\;' + os.environ.get('PATH', '')
USB_CON_IMG = os.path.join(RESOURCE_PATH, 'plug-connect.png')
USB_DCON_IMG = os.path.join(RESOURCE_PATH, 'plug-disconnect.png')

FULL_CONTROL = 0
NO_FF = 1
ONLY_FF = 2


class DataChannel:
    '''
    Encapsulates a single-dimensional numpy array holding
    data for a plot.
    Normally used just for y-axis values, the DataChannel may
    also be used for the x-axis on x-y plots.

    The only real reason for the extra class is to make the
    semantics of shifting a value into the array a little
    easier to read.  Probably fails at that but . . . hey!
    '''

    def __init__(self, points):
        self.points = points
        self.data_ = np.zeros(shape=self.points, dtype=float)

    def add_new_value(self, value):
        self.data_[:-1] = self.data_[1:]
        self.data_[-1] = value

    def data(self):
        return self.data_


hline_style = 'border: 2px solid gray'


class Dashboard(QMainWindow):
    usb_dis = pyqtSignal()
    usb_con = pyqtSignal(str, str)
    message = pyqtSignal(str, str)
    dialog = pyqtSignal(str, str, str)

    def __init__(self):
        super().__init__()

        self.processing = None
        self.device = None
        self.data = []
        self.parameters = {}
        self.move_mode = FULL_CONTROL

        self.nChannels = 8
        self.nPoints = 400
        self.telemetry = [DataChannel(self.nPoints) for i in range(self.nChannels)]
        self.plots = [pg.PlotItem for i in range(self.nChannels)]
        self.initUI()

    def initUI(self):
        pg.setConfigOption('background', pg.mkColor(25, 50, 75))
        pg.setConfigOption('background', palette[0])
        pg.setConfigOption('foreground', 'y')
        pg.setConfigOptions(antialias=True)
        styles = {'color': 'cyan', 'font-size': '13px', 'bottom_margin': '50px'}
        hline_style = {'border-style': 'solid', 'border-color': 'green', 'bottom_margin': '50px'}

        self.output_plot = pg.PlotWidget()
        self.output_plot.setYRange(-1, 7)
        self.output_plot.setXRange(0, 1000)
        self.output_plot.disableAutoRange()
        self.output_plot.setTitle("<span style=\"color:cyan;\">Controller outputs</span>")
        self.output_plot.setLabel('left', 'Volts', **styles)
        self.output_plot.setLabel('bottom', 'time (ms)', **styles)
        self.output_plot.addLegend(offset=(-5, 20))
        self.output_plot.showGrid(x=True, y=True)
        ax = self.output_plot.getPlotItem().getAxis('left')
        ax.setWidth(60)

        self.motion_plot = pg.PlotWidget()
        self.motion_plot.setYRange(-100, 3900)
        self.motion_plot.setXRange(0, 1000)
        self.motion_plot.disableAutoRange()
        self.motion_plot.setTitle("<span style=\"color:cyan;\">Motion</span>")
        self.motion_plot.setLabel('left', 'Speed (deg/s)', **styles)
        self.motion_plot.setLabel('bottom', 'time (ms)', **styles)
        self.motion_plot.showGrid(x=True, y=True)
        ax = self.motion_plot.getPlotItem().getAxis('left')
        ax.setWidth(60)
        vb = self.motion_plot.getPlotItem().getViewBox()
        w = vb.viewRect().width()
        self.motion_plot.addLegend(offset=(-5, 20))

        # Serial port connection indication
        self.text_box = QPlainTextEdit()
        self.text_box.setFont(QFont('mono'))
        self.text_box.setLineWrapMode(QPlainTextEdit.NoWrap)

        self.label_biasff = QLabel("Bias FF")
        self.spin_biasff = QSpinBox()

        #### SETTINGS GROUP ##########################################
        self.settings_grid = QGridLayout()
        self.settings_grid.setContentsMargins(0, 0, 0, 0)
        self.spin_biasff = self.double_spinbox("biasFF", 0.0, 0.4, 0.01, 2)
        self.spin_speedff = self.double_spinbox("speedFF", 0.0, 0.001, 0.00001, 5)
        self.spin_accff = self.double_spinbox("accFF", 0.0, 0.001, 0.00001, 5)
        self.spin_zeta = self.double_spinbox("zeta", 0.0, 2.0, 0.005, 3)
        self.spin_td = self.double_spinbox("Td", 0.0, 1.00, 0.01, 3)
        self.spin_kp = self.double_spinbox("KP", 0.0, 8.0, 0.001, 4)
        self.spin_kd = self.double_spinbox("KD", 0.0, 2.0, 0.0001, 4)

        self.spin_biasff.valueChanged.connect(self.parameter_change)
        self.spin_speedff.valueChanged.connect(self.parameter_change)
        self.spin_accff.valueChanged.connect(self.parameter_change)
        self.spin_zeta.valueChanged.connect(self.parameter_change)
        self.spin_td.valueChanged.connect(self.parameter_change)
        self.spin_kp.valueChanged.connect(self.parameter_change)
        self.spin_kd.valueChanged.connect(self.parameter_change)

        self.settings_grid.addWidget(self.spin_biasff, 0, 1, 1, 1)
        self.settings_grid.addWidget(self.spin_speedff, 1, 1, 1, 1)
        self.settings_grid.addWidget(self.spin_accff, 2, 1, 1, 1)
        self.settings_grid.addWidget(self.spin_zeta, 3, 1, 1, 1)
        self.settings_grid.addWidget(self.spin_kp, 5, 1, 1, 1)
        self.settings_grid.addWidget(self.spin_td, 4, 1, 1, 1)
        self.settings_grid.addWidget(self.spin_kd, 6, 1, 1, 1)

        self.lbl_biasff = QLabel(" Bias FF:")
        self.lbl_biasff.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_speedff = QLabel("Speed FF:")
        self.lbl_speedff.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_accff = QLabel("  Acc FF:")
        self.lbl_accff.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_zeta = QLabel("Damping Ratio:")
        self.lbl_zeta.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_td = QLabel("Settling Time:")
        self.lbl_td.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_kp = QLabel("Kp:")
        self.lbl_kp.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_kd = QLabel("Kd:")
        self.lbl_kd.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_km = QLabel("Km:")
        self.lbl_km.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_km_val = QLabel("0.000")
        self.lbl_tm = QLabel("Tm:")
        self.lbl_tm.setAlignment(Qt.AlignRight | Qt.AlignTrailing | Qt.AlignVCenter)
        self.lbl_tm_val = QLabel("0.000")

        self.settings_grid.addWidget(self.lbl_biasff, 0, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_kp, 5, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_speedff, 1, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_accff, 2, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_zeta, 3, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_td, 4, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_kd, 6, 0, 1, 1)

        self.settings_grid.addWidget(self.lbl_km, 7, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_km_val, 7, 1, 1, 1)
        self.settings_grid.addWidget(self.lbl_tm, 8, 0, 1, 1)
        self.settings_grid.addWidget(self.lbl_tm_val, 8, 1, 1, 1)

        self.btn_read_settings = QPushButton('READ', self)
        self.btn_read_settings.clicked.connect(self.read_settings)
        self.btn_read_settings.setEnabled(False)

        self.btn_write_settings = QPushButton('WRITE', self)
        self.btn_write_settings.clicked.connect(self.write_settings)
        self.btn_write_settings.setEnabled(False)

        self.btn_reset_settings = QPushButton('RESET', self)
        self.btn_reset_settings.clicked.connect(self.reset_settings)
        self.btn_reset_settings.setEnabled(False)
  
        settings_button_layout = QHBoxLayout()
        settings_button_layout.addWidget(self.btn_read_settings)
        settings_button_layout.addWidget(self.btn_write_settings)
        settings_button_layout.addWidget(self.btn_reset_settings)

        self.settings_grid.addLayout(settings_button_layout, 9, 0, 1, 2)

        settings_group = QGroupBox()
        settings_group.setObjectName("SettingsGroup")
        settings_group.setStyleSheet("QGroupBox#SettingsGroup { border: 1px solid gray;}")
        settings_group.setLayout(self.settings_grid)

        #### BUTTONS LAYOUT ##################################
        self.btn_id = QPushButton('ID', self)
        self.btn_id.clicked.connect(self.get_id)
        self.btn_id.setEnabled(False)

        self.btn_move = QPushButton('MOVE', self)
        self.btn_move.clicked.connect(self.send_move)
        self.btn_move.setEnabled(False)

        self.btn_reset = QPushButton('RESET', self)
        self.btn_reset.clicked.connect(self.target_reset)

        button_layout = QHBoxLayout()
        button_layout.addWidget(self.btn_id)
        button_layout.addWidget(self.btn_move)
        button_layout.addWidget(self.btn_reset)

        #####################################################
        option_layout = QHBoxLayout()
        self.rb_full = QRadioButton("Full Control")
        self.rb_full.mode = FULL_CONTROL
        self.rb_full.toggled.connect(self.option_select)
        option_layout.addWidget(self.rb_full)

        self.rb_noff = QRadioButton("No FF")
        self.rb_noff.mode = NO_FF
        self.rb_noff.toggled.connect(self.option_select)
        option_layout.addWidget(self.rb_noff)

        self.rb_onlyff = QRadioButton("Only FF")
        self.rb_onlyff.mode = ONLY_FF
        self.rb_onlyff.toggled.connect(self.option_select)
        option_layout.addWidget(self.rb_onlyff)
        self.rb_onlyff.setChecked(True)
        
        

        #####################################################

        self.side_bar = QWidget()
        side_layout = QVBoxLayout()
        side_layout.addWidget(self.text_box)
        side_layout.addWidget(self.HLine())
        side_layout.addLayout(option_layout)
        side_layout.addLayout(button_layout)
        side_layout.addWidget(settings_group)
        self.side_bar.setLayout(side_layout)
        self.side_bar.setFixedWidth(300)

        #### STATUS BAR ####################################
        # Add the statusbar into a toolbar
        # self.tool_bar = self.addToolBar('Main')
        self.status_bar = QStatusBar()
        self.lbl_con_status = QLabel()
        self.status_bar.addPermanentWidget(self.lbl_con_status)
        self.setStatusBar(self.status_bar)
        self.show_connection_message('warning', 'Searching')

        #### DATA PLOTS ####################################
        plot_layout = QVBoxLayout()
        plot_layout.addWidget(self.output_plot)
        plot_layout.addWidget(self.motion_plot)

        center_layout = QHBoxLayout()
        center_layout.addLayout(plot_layout)
        center_layout.addWidget(self.side_bar)

        # Vertical app layout
        main_vbox = QVBoxLayout()
        main_vbox.addLayout(center_layout)
        # main_vbox.addLayout(button_layout)

        # Wrap the layout into a widget
        main_widget = QWidget()
        main_widget.setLayout(main_vbox)

        # Add menubar items
        menubar = self.menuBar()
        file_menu = menubar.addMenu('Help')
        # App info item
        app_info = QAction('About MotorLab', self)
        app_info.triggered.connect(self.about)
        file_menu.addAction(app_info)

        # Main window style, layout and position

        self.setWindowTitle("UKMARS MotorLab Dashboard")
        self.setCentralWidget(main_widget)
        self.setMinimumSize(960, 720)
        self.resize(960, 720)
        self.show()
        self.center_window()
        # To lose focus on the textedit field
        self.setFocus()

    ### ^^^^^^ Set up UI ^^^^^^ ###################################################################

    def HLine(self):
        line = QFrame()
        line.setFrameShape(QFrame.HLine)
        # line.setFrameShadow(QFrame.Sunken)
        line.setStyleSheet(hline_style)
        return line

    def double_spinbox(self, name, min, max, step, decimals=5):
        spin = QDoubleSpinBox()
        spin.setMinimum(min)
        spin.setMaximum(max)
        spin.setSingleStep(step)
        spin.setDecimals(decimals)
        spin.setObjectName(name)
        return spin

    def set_safely(self, widget: QWidget, value):
        '''Locks a widget while updating its value
        '''
        widget.blockSignals(True)
        widget.setValue(value)
        widget.blockSignals(False)

    def parameter_change(self):
        spinner = self.sender()
        if spinner.objectName() == "zeta" or spinner.objectName() == "Td":
            tm = self.parameters["Tm"]
            km = self.parameters["Km"]
            z = self.spin_zeta.value()
            td = self.spin_td.value()
            td = max(td,0.005)
            z = max(z,0.005)
            kp = 16 * tm / km / z / z / td / td
            kd = (8 * tm - td) / td / km
            self.set_safely(self.spin_kp, kp)
            self.set_safely(self.spin_kd, kd)
        elif spinner.objectName() == "KD":
            # the reverse calculations are harder
            pass
        elif spinner.objectName() == "KP":
            # the reverse calculations are harder
            pass

    def option_select(self):
        radioButton = self.sender()
        if radioButton.isChecked():
            self.move_mode = radioButton.mode

    def center_window(self):
        qr = self.frameGeometry()
        cp = QDesktopWidget().availableGeometry().center()
        qr.moveCenter(cp)
        self.move(qr.topLeft())

    # @pyqtSlot(str, str)
    def show_connection_message(self, type, message, timeout=0):
        if type == 'error':
            style = 'color: #E33;'
        elif type == 'warning':
            style = 'color: #e12;'
        elif type == 'info':
            style = 'color: #050;'
        else:  # Unrecognized message type
            return

        self.lbl_con_status.setStyleSheet(style)
        self.lbl_con_status.setText(message)

    @pyqtSlot(str,str)
    def usb_connect(self, data, hwid):
        # self.lbl_con_status.setText('CONNECTED')
        self.device = data
        try:
            self.serial = Serial(self.device, baudrate=115200, timeout=0.25)
        except serial.SerialException as e:
            sys.stderr.write(f'could not open port {self.device}: {e}\n')
            self.device = None
            # device = ask_for_port()
        if self.device:
            self.show_connection_message('info', F'CONNECTED : {self.device} [{hwid}]')
            self.device_init()
            self.read_settings()
            self.btn_read_settings.setEnabled(True)
            self.btn_write_settings.setEnabled(True)
            self.btn_reset_settings.setEnabled(True)
            self.btn_move.setEnabled(True)
            self.btn_id.setEnabled(True)

    @pyqtSlot()
    def usb_disconnect(self):
        self.device = None
        self.show_connection_message('warning', 'DISCONNECTED')

    def closeEvent(self, unused_event):  # on window close event
        if not self.device == None:
            self.serial.close()

    @pyqtSlot(str, str, str)
    def show_dialog(self, title, message, details):
        msg_box = QMessageBox()
        msg_box.setDetailedText(details)
        msg_box.setWindowTitle('Message')
        msg_box.setTextFormat(Qt.RichText)
        msg_box.setStandardButtons(QMessageBox.Close)
        msg_box.setText(title)
        msg_box.setInformativeText('<font face=Arial>' + message + '</font>')
        horizontalSpacer = QSpacerItem(550, 0, QSizePolicy.Minimum, QSizePolicy.Expanding);
        layout = msg_box.layout();
        layout.addItem(horizontalSpacer, layout.rowCount(), 0, 1, layout.columnCount());
        msg_box.exec_()

    ##################################################################

    def plot(self, x, y, plot_widget, plotname, color, line_style=Qt.SolidLine):

        pen = pg.mkPen(color=color, width=3, style=line_style)
        plot_widget.plot(x, y, name=plotname, pen=pen)

    def log_data(self):
        for line in self.data:
            if len(line) > 0:
                self.text_box.appendPlainText(line)
            self.text_box.update()
            QApplication.processEvents()  # nasty way to make it look faster

    def log_message(self, message):
        self.text_box.appendPlainText(message)
        self.text_box.update()
        QApplication.processEvents()  # nasty way to make it look faster

    def clear_data(self):
        self.data = []

    def get_response(self):
        time.sleep(0.25)  # let the arduino boot
        data = []
        done = False
        while not done:
            line = self.serial.readline().decode('ascii').strip()
            if line == '>':
                done = True
                continue
            data.append(line)
        return data

    def write(self, message):
        ''' send a message and throw away the response'''
        if not self.device:
            return
        self.serial.flushInput()
        self.serial.write(message.encode('ascii'))

    def query(self, message):
        ''' send a message and return the response'''
        if not self.device:
            return
        self.serial.flushInput()
        self.serial.write(message.encode('ascii'))
        return self.get_response()

    #############################################################################

    def device_init(self):
        # time.sleep(0.5) # let the arduino boot
        self.data = self.get_response()
        self.log_data()
        self.write('echo off\n')
        self.get_response()

    def update_parameters(self):
        self.set_safely(self.spin_biasff,self.parameters['biasFF'])
        self.set_safely(self.spin_speedff,self.parameters['speedFF'])
        self.set_safely(self.spin_accff,self.parameters['accFF'])
        self.set_safely(self.spin_zeta,self.parameters['zeta'])
        self.set_safely(self.spin_td,self.parameters['Td'])
        self.set_safely(self.spin_kp,self.parameters['KP'])
        self.set_safely(self.spin_kd,self.parameters['KD'])
        self.lbl_km_val.setText(F"{self.parameters['Km']}")
        self.lbl_tm_val.setText(F"{self.parameters['Tm']}")

    def write_parameters(self):
        for spinner in self.findChildren(QDoubleSpinBox):
            pass
        for key in self.parameters:
            spinner = self.findChild(QDoubleSpinBox, key)
            value = self.parameters[key]
            if spinner:
                value = spinner.value()
            msg = F"{key} = {value}\n"
            # print(msg)
            self.write(msg)

    def target_reset(self):
        self.output_plot.clear()
        self.motion_plot.clear()
        self.log_message('Device Reset')
        self.serial.setDTR(0)
        time.sleep(0.1)
        self.serial.setDTR(1)
        self.log_message('----------------')
        self.device_init()
        self.read_settings()

    def read_settings(self):
        if not self.device:
            return
        self.log_message('Read Settings')
        self.data = self.query("$\n")
        self.log_data()
        for line in self.data:
            parts = line.split(' ')
            self.parameters[parts[0]] = float(parts[2])
        self.update_parameters()
        self.log_message('----------------')

    def write_settings(self):
        if not self.device:
            return
        self.log_message('Save Settings')
        self.write_parameters()
        self.data = self.query("$\n")
        self.log_data()
        self.log_message('----------------')

   
    def reset_settings(self):
        if not self.device:
            return
        self.log_message('Default Settings')
        self.data = self.query("#\n")
        self.log_message('----------------')
        self.read_settings()

    # Button clicked event
    def get_id(self):
        if not self.device:
            return
        pdata = []
        vdata = []
        comments = []
        headings = []
        self.data = self.query(F'volts\n')
        self.log_data()
        # search the data for the (first) header line
        cols = 0
        for line in self.data:
            if line[0] == '$':
                headings = line[1:].split(' ')
                cols = len(headings)
                # print(F'found {cols} columns {headings}')
                break
        d = [[] for i in range(cols)]
        # "$time volts speed"
        t = []
        for line in self.data:
            if len(line) == 0:
                continue
            if line[0] == '#':
                comments.append(line)
                continue
            if line[0] == '$':
                # headings.append(line[1:])
                continue
            parts = line.split(' ')
            # print(line, parts)
            for i in range(len(parts)):
                try:
                    f = float(parts[i])
                except:
                    print(F'Unable to convert value {parts[i]}')
                    f = d[-1]
                d[i].append(f)
        self.output_plot.clear()
        self.plot(d[0], d[1], self.output_plot, headings[1], palette[3], Qt.SolidLine)
        # self.output_plot.enableAutoRange()
        # self.output_plot.disableAutoRange()
        self.output_plot.setYRange(-1, 7)

        self.motion_plot.clear()
        # self.plot(d[0],d[3],self.motion_plot, "Set Speed",palette[4],Qt.DotLine)
        self.plot(d[0], d[2], self.motion_plot, headings[2], palette[5])
        self.motion_plot.enableAutoRange()
        self.output_plot.setXLink(self.motion_plot)
        for line in comments:
            self.text_box.appendPlainText(line)
        # for line in headings:
        #     self.text_box.appendPlainText(line)
        self.text_box.appendPlainText(F'{headings}')

    def send_move(self):
        if not self.device:
            return
        self.data = self.query(F'MOVE {self.move_mode}\n')
        self.log_data()
        d = [[] for i in range(self.nChannels)]
        # "$time set_pos robot_pos set_speed robot_speed ctrl_volts ff_volts, motor_volts"
        pdata = []
        vdata = []
        comments = []
        headings = []
        t = []
        for line in self.data:
            if line[0] == '#':
                comments.append(line)
                continue
            if line[0] == '$':
                headings.append(line[1:])
                continue
            parts = line.split(' ')
            for i in range(len(parts)):
                try:
                    f = float(parts[i])
                except:
                    print(F'Unable to convert value {parts[i]}')
                    f = d[-1]
                d[i].append(f)
        for i in range(len(d[0])):
            d[5][i] = min(d[5][i], 6.0)
            d[5][i] = max(d[5][i], -6.0)

        self.output_plot.clear()
        if self.move_mode == NO_FF:
            style = Qt.DotLine
        else:
            style = Qt.SolidLine
        self.plot(d[0], d[6], self.output_plot, "FF Volts", palette[1], style)
        if self.move_mode == ONLY_FF:
            style = Qt.DotLine
        else:
            style = Qt.SolidLine
        self.plot(d[0], d[5], self.output_plot, "CTRL Volts", palette[2], style)
        self.plot(d[0], d[7], self.output_plot, "Motor Volts", palette[3], Qt.SolidLine)
        self.output_plot.enableAutoRange()
        self.output_plot.setYRange(-7, 7)

        self.motion_plot.clear()
        self.plot(d[0], d[3], self.motion_plot, "Set Speed", palette[4], Qt.DotLine)
        self.plot(d[0], d[4], self.motion_plot, "Robot Speed", palette[5])
        self.motion_plot.enableAutoRange()
        self.output_plot.setXLink(self.motion_plot)
        for line in comments:
            self.text_box.appendPlainText(line)
        for line in headings:
            self.text_box.appendPlainText(line)

    def about(self, event):
        # Show app info dialog
        self.show_dialog('App info', '<br>' +
                         'UKMARS Motorlab Dashboard<br>', '')


class PortUpdate(QThread):
    '''
    This thread runs constantly and monitors the connected serial
    ports once per second.
    If there is no device currently connected, and it finds
    a device form the accepted list, it will attempt to connect
    the application to that device.
    This is crude but makes possible auto-connect and re-connect
    after a loss.
    The application also uses it as a way to reset the target by
    closing the port. Shortly after, the connection will be remade
    and, because it is an Arduino, it will reset.
    '''

    # update serialport status
    def run(self):
        while True:
            # Wait for a second to pass
            time.sleep(1.0)
            port = None
            # Scan the serial ports 
            for p in serial.tools.list_ports.comports():
                # 1A86:7523 is a CH340 Serial converter - QinHeng Electronics
                # 10C4:80A9 is a CP210x UART bridge - SiLabs adaptor
                # 2341:0058 is the Arduino Nano Every
                # print(p.hwid)
                if '1A86:' in p.hwid or '10C4:' in p.hwid or '2341:' in p.hwid:
                    port = p.device
                    break

            # When specific vendor ID was found AND it's a new port
            if port and port != app_window.device:
                msg = F'{p} {p.hwid} {p.manufacturer} {p.description} {p.device}'
                # print(msg)
                app_window.usb_con.emit(port, p.hwid)
            elif not port:
                app_window.usb_dis.emit()


if __name__ == "__main__":
    app = QApplication(sys.argv)

    # Create the app main window
    app_window = Dashboard()
    # Connect signals to slots
    app_window.dialog.connect(app_window.show_dialog)
    app_window.usb_con.connect(app_window.usb_connect)
    app_window.usb_dis.connect(app_window.usb_disconnect)
    # window.message.connect(window.show_message)

    # Start port update thread
    port_update = PortUpdate()
    port_update.start()

    # Launch application
    sys.exit(app.exec())
