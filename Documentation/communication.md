The communication between baalue and the different nodes (baalued)
==================================================================

The simple device managment protocol is implemented in libbaalue (https://github.com/tjohann/libbaalue.git). The focus here is on the user interaction/interface.

Most topic below represents a button in the main window of baalue.

The common workflow is first to search for nodes, connect to a specific node(s) and then *tbd*/halt/reboot.


search
------

First we have to search for devices/nodes. To to this a broadcast request is send in your network (for example 192.168.0.0/24 -> 192.168.0.255). To reduce the network traffic you can give a ip range via configuration file /etc/baalue.conf or ~/.config/baalue/baalue.conf.

Example:

	FIRST_IP = 192.168.0.80
	LAST_IP = 192.168.0.90

A click on the *Search for nodes* button creates a thread which shows the available devices/nodes in a seperate window. The hostnames and ip's will be (permanently) stored in ~/.config/baalue/nodes.conf. They will be available/active until you do a *Search for nodes* again. If the configuration in your network is stable there's no need to do it again, so after a reboot the connect button is click-able.


connect
-------

A click on the *Connect node(s)* opens a new window where you can select to which devices/nodes you want to connect. With every selected device/node a new thread with a new window for this specific device/node will be created. If you close the window the connection will be closed.


halt/reboot
-----------

A reboot or halt a device/node could be triggered by the main window.
