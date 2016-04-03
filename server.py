
import paho.mqtt.client as mqtt

def make_topic(topic):
        client.publish(topic,"Online")

def on_connect(client, userdata, flags, rc):
        print("Connected with result code" + str(rc))
        client.subscribe("hello/world",0)

def on_message(client, userdata, msg):
        print(msg.topic + " " + str(msg.payload))
        message = str(msg.payload)
        if(message=="led"):
                print("LED yaaa")

client = mqtt.Client()
client.on_connect = on_connect
client.on_message = on_message
client.connect('192.168.43.222',1883,60)

make_topic("hello/world")
make_topic("hello/world2")


client.loop_forever()



