from flask import *
from pymongo import MongoClient
from base64 import b64decode

import common.authentication as auth

app = Flask(__name__)

client = MongoClient('localhost', 27017)

db = client.sarla
agents = db.agents

@app.route('/api/register', methods = ('POST', 'GET'))
def register():
    if request.method == 'POST':
        host = ""
        user = ""
        pid = ""
        name = ""
        arch = ""
        key = ""
        id = ""
        agents.insert_one({
            'host':host,
            'user':user,
            'pid':pid,
            'name':name,
            'arch':arch,
            'key':key,
            'id':id
        })
        return redirect(url_for('register'))

    enum_agents = agents.find()
    return render_template('register.html', agents=enum_agents)

@app.route('/api/negotiate', methods = ('POST', 'GET'))
def negotiate():
    if request.method == 'POST':
        data = str(request.data)
        data = data.lstrip("b\'")
        data = data.rstrip("\\r\\n\\r\\n\\x00\\x00\'")

        plaintext = str(b64decode(data))
        plaintext = plaintext.rstrip("\'")
        plaintext = plaintext.lstrip("b\'0:")

        print(plaintext)

        if plaintext == auth.keyword:
            key = auth.generate_key(plaintext)

            agents.insert_one({
                'key':key
            })
            
            return key
    
    return render_template('index.html')

@app.route('/')
def index():
    return render_template('index.html')

if __name__ == "__main__":
    app.run(debug=True, host='0.0.0.0')