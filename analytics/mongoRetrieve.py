from pymongo import MongoClient
import json


client = MongoClient("mongodb+srv://swqms:Swqms123@nodered.5rouyap.mongodb.net/swqms?retryWrites=true&w=majority&appName=nodeRed")
db = client["swqms"]
collection = db["swqms"]


data = list(collection.find())

for document in data:
    document.pop('_id', None)  # Removed MongoDB internal _id field
    for key, value in document.items():
        if hasattr(value, 'isoformat'):  # converted timestamp to iso format
            document[key] = value.isoformat()


with open('analytics/collected_data.json', 'w') as f:
    json.dump(data, f, indent=4)

print("Data exported successfully to collected_data.json!")
