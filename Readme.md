Friend Unifying Platform version 1.0.0
==================================

We are happy to announce the release of FriendUP version 1.0.0.

FriendUP version 1.0.0 is the first complete open source release of the platform. It introduces a saturated vision for the next paradigm in computing. FriendUP comes with a powerful kernel-like web server that supports multiple protocols and data sources, next to resource- and user session management. This offers javascript developers a fresh backendless environment in which to accelerate their development of exciting new applications and games. 

![FriendUP Workspace](https://friendup.cloud/wp-content/uploads/2017/06/desktop-1.png "FriendUP Workspace")

Across devices
--------------

With its responsive desktop environment and client-side, javascript based technologies, FriendUP offers a new reality where you work and play online, independently from your local web enabled hardware. By supporting all screen formats, from mobile phones to laptops and VR, FriendUP is a single target through which you can reach the entire user market.
We have used considerable amount of time on making the FriendUP APIs simple and easy to understand. That is also why we are packaging the FriendUP Developers Manual with this release. In its third draft, it will soon cover the entire 1.0.x platform.

Distributed and powerful
------------------------

FriendUP aims to make the powerful emerging web technologies easy to work with. This is why we reach out to you, to help us enrich this platform with DOS drivers, modules, libraries and apps so that we can cover all of the interesting technologies out there that people use and love.
In the Future, FriendUP will be to the cloud users what Linux is for machines.

Getting started
===============

Just clone this repository, run the install.sh script and follow the on screen instructions. This script should run on most modern linux distributions. Post to the Developer Community if you run into any problems here.
```
git clone https://github.com/FriendSoftwareLabs/friendup
cd friendup/
./install.sh
```
We recommend setting up a dedicated user for your FriendUP installation. You will need the MySQL root password to allow the install script to create the database and user.

Dependencies
------------

The Friend installer relies on the following packages to work :

- sudo
- gcc

If you encounter an error during the dependencies installation process, please refer to the end of this file for a complete list of the necessary dependencies, and install them manually. Then restart the installer.

Running the serving kernel manually
-----------------------------------

Friend Core can be installed in any directory you wish, including root directories. The installer creates two global variables, FRIEND_HOME pointing to the cloned friendup directory, and FRIEND_PATH pointing to the directory where Friend Core has been build. After running the installer script, you will need to reboot your machine for these variables to be defined.

When you want to run Friend Core yourself, you enter into its build directory. Like this:
```
cd myfriend/build/
./FriendCore
```
If you want to run it without debug output in your console, you can use nohup:
```
nohup ./FriendCore >> /dev/null &
```
If you want to kill Friend Core and it's dedicated servers (see later), use the killfriend.sh script located in both the build directory and the friendup directory.
```
./killfriend.sh
```

You can run the install.sh script as many times as you want, and select different installation options. Your choices are saved and will be recovered the next time you run it. You can also have several versions of friendup on your machine with different setup options, as long as you do not try to run two Friend Cores at the same time.

Default login
-------------

Once the installation script is finished and your local FriendCore is up and running use these credentials to log in: *fadmin*/*securefassword*. The first thiing you may want to do, is add a new user : run the 'users' application that can be found in the 'System:Software/System' directory.

FriendNetwork
-------------

The installer will give you the option to install Friend Network. Friend Network needs Friend Core to run in TLS mode, so be sure to have TLS keys ready, or select 'Create self-signed keys' when the installer prompts you for it.

In order to function, Friend Network needs a node server running on the machine. Friend Network installer will automatically install the latest version of node, npm and n.

You will also need to provide links to a TURN server, a STUN server and the credentials to enter the TURN server.

Friend Network server will be automatically launched when you run Friend Core after a successful installation.

In order to kill Friend Core and all the associated servers, we suggest you use the 'killfriend.sh' script.

Friend Chat
-----------

The installer gives you the opportunity to install Friend Chat, our integrated text and video communication tool.

Friend Chat needs Friend Core to run with TLS protection : be sure to have keys ready before the installation, or just select 'Create self-encrypted keys' when the installer asks you for it.

The installer will also check for node.js, npm and n and install them if necessary.

As for Friend Network, you will need to provide links to a TURN server, a STUN server and the credentials to enter the TURN server.

Friend Chat needs two servers to function, the 'Presence' server, and the 'Friend Chat' server. Both servers will automatically be launched by Friend Core.

In order to kill Friend Core and all the associated servers, we suggest you use the 'killfriend.sh' script.

Documentation
-------------

You can find the developer documentation in the docs folder. An administration guide will be added soon.

Chat room
---------

You will find many of our developers and users on our IRC channel / chat room. Please choose a unique nick and join using the link below.

https://friendup.cloud/community/irc-channel/

If you have chosen to install Friend Chat, the application will automatically open the friendup IRC channel upon starting.

Licensing
=========

FriendUP is a large system consisting of several layers. It has a kernel core that is managing the Friend resources. This one is licensed under the MIT license. Then it has modules, DOS drivers and runtime linked libraries. These are licensed under the LGPLv3 license. Finally, we have the GUI for Friend - the Friend Workspace. This is licensed under the AGPLv3 license.

Developer Community
===================

We invite everybody to join our developer community at https://developers.friendup.cloud/.

Demo server
-----------

For those that just want to test our Workspace, you can try our demo after registering at https://friendup.cloud/try-the-friendup-demo/.

List of dependencies
====================

This is the list of dependencies Friend Core needs to function.

- libssh2-1-dev
- libssh-dev
- libssl-dev
- libaio-dev
- mysql-server
- php5-cli
- php5-gd
- php5-imap
- php5-mysql
- php5-curl
- libmysqlclient-dev
- build-essential
- libmatheval-dev
- libmagic-dev
- libgd-dev
- libwebsockets-dev
- rsync
- valgrind-dbg
- libxml2-dev
- php5-readline
- cmake
- ssh
- make
