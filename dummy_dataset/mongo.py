from pymongo import MongoClient
from bson.objectid import ObjectId
import json
from datetime import datetime
import time
import random
import os

# 1. Connect to your MongoDB server
client = MongoClient("mongodb+srv://swqms:Swqms123@nodered.5rouyap.mongodb.net/swqms?retryWrites=true&w=majority&appName=nodeRed")

# 2. Choose your database and collection
db = client["swqms"]  # Database
collection = db["swqms"]  # Collection

# 3. Load the JSON data
with open("reservoir_sensor_data_ESP32_000110.json", "r") as f:
    sensor_data = json.load(f)

# 4. Convert timestamp and create a custom ObjectId with real time
for doc in sensor_data:
    if isinstance(doc["timestamp"], str):
        dt = datetime.strptime(doc["timestamp"], "%Y-%m-%dT%H:%M:%S")
        doc["timestamp"] = dt
    else:
        dt = doc["timestamp"]

    unix_seconds = int(time.mktime(dt.timetuple()))
    rand_bytes = os.urandom(8)
    custom_id = ObjectId(bytes([unix_seconds >> 24 & 0xFF,
                                unix_seconds >> 16 & 0xFF,
                                unix_seconds >> 8 & 0xFF,
                                unix_seconds & 0xFF]) + rand_bytes)
    doc["_id"] = custom_id


# 5. Insert the documents
collection.insert_many(sensor_data)

print("✅ Data inserted with custom ObjectIds based on timestamp.")
