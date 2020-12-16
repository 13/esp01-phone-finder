#!/usr/bin/python
# -*- coding: utf-8 -*-
 
import os
import paho.mqtt.client as mqtt

def on_message(client, userdata, message):
    msg = str(message.payload.decode("utf-8"))
    print("message received:", msg)
    print("message topic:", message.topic)
    if (msg == "ben" or msg == "anna"):
      if (msg == "ben"):
        #pushmsg
      if (msg == "anna"):
        #pushmsg
 
def on_connect(client, userdata, flags, rc):
    client.subscribe('phonefinder/alert')
 
BROKER_ADDRESS = "192.168.22.9"
 
client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
 
client.connect(BROKER_ADDRESS)
 
print("Connected to MQTT Broker: " + BROKER_ADDRESS)
 
client.loop_forever()
