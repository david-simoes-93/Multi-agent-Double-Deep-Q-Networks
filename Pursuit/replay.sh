#!/bin/tcsh

echo "*****************************************************************"
echo "* Pursuit Domain - University of Amsterdam, The Netherlands     *"
echo "* Created by Jelle Kok                                          *"
echo "* Copyright 2002.  All rights reserved.                         *"
echo "*****************************************************************"

set log_dir      = "logplayer/src"
set logplayer    = "$log_dir/pursuit_logplayer"
set monitor_dir  =  "monitor/src"
set monitor      =  "$monitor_dir/pursuit_monitor"
set monitor_conf =  "$monitor_dir/monitor.conf"

{$logplayer} $1 &                   # start logplayer with the supplied file
sleep 4 
{$monitor} -conf {$monitor_conf} &  # start the monitor for visualization


