from sarla.server.src.authorization import generate_id, generate_key

keyword = "boondoggle"

format_dict = {
    'id': '',
    'keyword': '',
    'key': '',
    'user': '',
    'host': '',
    'pid': '',
    'name': '',
    'domain': '',
    'build': ''
}


def register(data, dictionary):
    job_dictionary = format_dict
    agent_information = data.split(',')
    agent_id = generate_id()

    print(data)

    job_dictionary['id'] = agent_id
    job_dictionary['user'] = agent_information[0]
    job_dictionary['host'] = agent_information[1]
    job_dictionary['pid'] = agent_information[2]
    job_dictionary['build'] = agent_information[3]
    job_dictionary['name'] = agent_information[4]
    job_dictionary['domain'] = agent_information[5]
    job_dictionary['arch'] = agent_information[6]
    job_dictionary['product'] = agent_information[7]
    job_dictionary['keyword'] = agent_information[8]

    print(job_dictionary['keyword'])

    if job_dictionary['keyword'] == keyword:
        key = generate_key(job_dictionary['keyword'])
        job_dictionary['key'] = key

        dictionary[job_dictionary['id']] = job_dictionary

        print(job_dictionary)
        return key
    else:
        return False
