from terminaltables import SingleTable

def create_table(data):
    table_data = [['Agent ID', 'Username', 'Hostname', 'Process ID', 'Process Name', 'System Arch']]

    for key in data:
        row = []
        for subkey, values in key.items():
            if (subkey == "_id"):
                pass
            else: 
                row.append(values)
        table_data.append(row)
    
    return SingleTable(table_data)
        