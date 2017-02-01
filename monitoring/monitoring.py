#!/usr/bin/python

import sys
from datetime import datetime
import psutil
import serial
from gettext import c2py
from time import sleep
import sensors
import subprocess

from os import ttyname

serial_com = None
DISK_PATH = "/"
CPU_TEMP_PATH = '/sys/class/thermal/thermal_zone0/temp'
last_time = ""



def send_command(command):
    global serial_com
    serial_com.write(command)
    serial_com.flush()
    response = serial_com.readline()
    count = 10
    while count > 0:
        if "OK" in response:
            break
        response = serial_com.readline()
        sleep(0.01)
        count -= 1
    serial_com.readlines()
    serial_com.flush()


def set_date(date_time):
    global serial_com
    request = 'UPDATE /rtc/date?value=%s\r\n' % date_time.strftime('%Y-%m-%d')
    send_command(request)


def set_time(date_time):
    global serial_com, last_time
    request = 'UPDATE /rtc/time?value=%s\r\n' % date_time.strftime('%H:%M:%S')
    if last_time != request:
        last_time = request
        send_command(request)


def set_max():
    global serial_com

    disk_max = int(psutil.disk_usage(DISK_PATH).total / (1024.*1024.))
    request = 'UPDATE /disk/max?value=%d\r\n' % disk_max
    send_command(request)

    ram_max = int(psutil.virtual_memory().total / (1024.*1024.))
    request = 'UPDATE /ram/max?value=%d\r\n' % ram_max
    send_command(request)


def get_sensors():
    cpu_fan = 0
    gpu_temp = 0
    case_fan = 0
    case_temp = 0

#    try:
    for chip in sensors.iter_detected_chips():
        if str(chip) == 'it8772-isa-0a30':
            for feature in chip:
                if str(feature.label) == 'fan2':
                    case_fan = int(feature.get_value())
                elif str(feature.label) == 'fan3':
                    cpu_fan = int(feature.get_value())
                elif str(feature.label) == 'temp2':
                    case_temp = int(feature.get_value())
#        elif str(chip) == 'radeon-pci-0100':
#            for feature in chip:
#                if str(feature.label) == 'temp1':
#                    gpu_temp = int(feature.get_value())
    #finally:
    #    sensors.cleanup()
    proc = subprocess.Popen(['aticonfig', '--odgt'], stdout=subprocess.PIPE)
    gpu_temp = int(float(proc.stdout.read().split("Temperature - ")[1].split(" C")[0]))

    proc = subprocess.Popen(["aticonfig --pplib-cmd 'get fanspeed 0'"], shell=True, stdout=subprocess.PIPE)
    gpu_fan = int(float(proc.stdout.read().split("Result: Fan Speed: ")[1].split("%")[0]))
    return cpu_fan, gpu_temp, case_fan, case_temp, gpu_fan


last_time_c = None
last_time_rx = None
last_time_tx = None


def bandwith():
    global last_time_c, last_time_rx, last_time_tx
    if last_time_c is None:
        last_time_c = datetime.now()
        tmp = psutil.network_io_counters(pernic=True)['eth0']
        last_time_rx = tmp.bytes_recv
        last_time_tx = tmp.bytes_sent

    new_time = datetime.now()
    diff = (new_time - last_time_c)
    delta = (diff.days * 86400000) + (diff.seconds * 1000) + (diff.microseconds / 1000)
    if delta >= 1000.: #ms
        eth0 = psutil.network_io_counters(pernic=True)['eth0']
        kb_tx = (eth0.bytes_sent - last_time_tx) / delta;
        kb_rx = (eth0.bytes_recv - last_time_rx) / delta;
        last_time_c = new_time
        last_time_rx = eth0.bytes_recv
        last_time_tx = eth0.bytes_sent
        return kb_tx, kb_rx
    else:
        return None, None


def start(tty_serial):
    global serial_com
    serial_com = serial.Serial(tty_serial, 115200, timeout=0)
    set_date(datetime.now())
    sleep(0.1)
    set_max()
    sleep(0.1)
    disk_count = 10
    psutil.cpu_percent(interval=None, percpu=False)
    sleep(0.1)
    while 1:
        cpu_pct = psutil.cpu_percent(interval=None, percpu=False)
        cpu_global = int(cpu_pct)
        request = 'UPDATE /cpu/usage?value=%d\r\n' % cpu_global
        send_command(request)
        cpu_temp = int(subprocess.check_output(['cat', CPU_TEMP_PATH]))/1000
        request = 'UPDATE /cpu/temp?value=%d\r\n' % int(cpu_temp)
        send_command(request)

        set_time(datetime.now())

        disk_count -= 1
        if disk_count == 0:
            disk_free = int(psutil.disk_usage(DISK_PATH).free / (1024.*1024.))
            request = 'UPDATE /disk/usage?value=%d\r\n' % disk_free
            send_command(request)
            disk_count = 25

        ram_free = int(psutil.virtual_memory().free / (1024.*1024.))
        request = 'UPDATE /ram/usage?value=%d\r\n' % ram_free
        send_command(request)

        cpu_fan, gpu_temp, case_fan, case_temp, gpu_fan= get_sensors()
        request = 'UPDATE /cpu/fan?value=%d\r\n' % cpu_fan
        send_command(request)
        request = 'UPDATE /gpu/temp?value=%d\r\n' % gpu_temp
        send_command(request)
        request = 'UPDATE /case/fan?value=%d\r\n' % case_fan
        send_command(request)
        request = 'UPDATE /case/temp?value=%d\r\n' % case_temp
        send_command(request)
        request = 'UPDATE /gpu/fan?value=%d\r\n' % gpu_fan
        send_command(request)

        tx, rx = bandwith()
        if tx is not None and rx is not None:
            request = 'UPDATE /eth/up?value=%d\r\n' % tx
            send_command(request)
            request = 'UPDATE /eth/down?value=%d\r\n' % rx
            send_command(request)

        sleep(0.1)


def main( argv):
    global serial_com
    comList = []
    # print command line arguments
    if len(argv) < 1:
        comList = ['/dev/ttyUSB0', '/dev/ttyUSB1', '/dev/ttyUSB2', '/dev/ttyUSB3']
    else:
        comList = [argv[0]]
    sensors.init()
    get_sensors()
#    start('/dev/ttyUSB0')
    while 1:
        for com in comList:
            try:
                start(com)
            except:
                sleep(5)

    sensors.cleanup()

if __name__ == "__main__":
    main(sys.argv[1:])
