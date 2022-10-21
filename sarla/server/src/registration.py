from base64 import b64decode

def register(data):
    registration_unformatted = str(b64decode(data))
    registration_unformatted = registration_unformatted[2:len(registration_unformatted)]
    registration_unformatted = registration_unformatted[:-1]
    agent_information = registration_unformatted.split(',')

    job = []

    for x in range(len(agent_information)):
        job.append(agent_information[x])

    job[0] = "Username: " + job[0]
    job[1] = "Hostname: " + job[1]
    job[2] = "Process ID: " + job[2]
    job[3] = "Version: " + job[3]

    print("\n")

    for x in range(len(job)):
        print(job[x])

    return job