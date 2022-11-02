from sarla.server.src.registration import register


def test_agent_registration():
    # setup
    data = 'VGVzdFVzZXIsVGVzdC1Cb3gsMTIzNCwxMjM0LGJvb25kb2dnbGU='
    expected = {
        "username": "TestUser",
        "hostname": "Test-Box",
        "process id": '1234',
        "version": '1234',
        "key": "boondoggle"
    }

    # invoke
    actual = register(data)
    actual.pop('key_hash')
    actual.pop('agent_id')

    # analyze
    assert actual == expected
