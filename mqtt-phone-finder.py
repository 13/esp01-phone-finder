#!/usr/bin/python
# -*- coding: utf-8 -*-
 
import os
import paho.mqtt.client as mqtt
from pushbullet import Pushbullet

def on_message(client, userdata, message):
    msg = str(message.payload.decode("utf-8"))
    print("message received:", msg)
    print("message topic:", message.topic)
    if (msg == "ben" or msg == "anna"):
      if (msg == "ben"):
        pb = Pushbullet("v1V61d2ptmCVqCECciR0pwP3FdSILZ5SiKujz3mA55KM0")
      if (msg == "anna"):
        pb = Pushbullet("o.RcirsF9VRtJpuOuJFYJNtdPkAmz1GQJ6")
      push = pb.push_note("p1", "phonealert")
 
def on_connect(client, userdata, flags, rc):
    client.subscribe('phonefinder/alert')
 
BROKER_ADDRESS = "192.168.22.9"
 
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
 
client.connect(BROKER_ADDRESS)
 
print("Connected to MQTT Broker: " + BROKER_ADDRESS)
 
client.loop_forever()
