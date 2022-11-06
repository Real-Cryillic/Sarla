from sarla.server.src.handler import process_agent


def test_agent_registration():
    data = 'cmVnaXN0ZXI6VGVzdFVzZXIsVGVzdC1Cb3gsMTIzNCwxMjM0LGJvb25kb2dnbGU='
    dictionary = dict()
    expected = {
        "keyword": "boondoggle",
        "user": "TestUser",
        "host": "Test-Box",
        "pid": '1234',
        "build": '1234',
    }
    process_agent(data, dictionary)
    for key in dictionary:
        dictionary[key].pop('id')
        dictionary[key].pop('key')
        print(dictionary[key])
        print(expected)
        assert expected == dictionary[key]