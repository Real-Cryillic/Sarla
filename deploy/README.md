Role Name
=========

This role will copy and deploy the Sarla agent to a given inventory.

Requirements
------------

No requirements

Role Variables
--------------

sarla_windows_location: Expected location to copy the binary

sarla_windows_binary: Exact name of the binary in the /files directory

Dependencies
------------

No dependencies

Example Playbook
----------------

    ---
    - hosts: all
      roles:
        - deploy

License
-------

BSD
