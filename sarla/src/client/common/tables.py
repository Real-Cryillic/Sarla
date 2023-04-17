from terminaltables import SingleTable

def create_table(data):
    table_data = [['Agent ID', 'Username', 'Hostname', 'Process ID', 'Process Name', 'System Arch', 'Key', 'Last Check-In']]

    for key in data:
        row = []
        for subkey, values in key.items():
            if (subkey == "_id"):
                pass
            else: 
                row.append(values)

        order = [3, 7, 2, 6, 5, 0, 4, 1]
        row = [row[i] for i in order]

        table_data.append(row)
    
    return SingleTable(table_data)
        