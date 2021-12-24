Home Assistant
==============

安装docker
----------

.. code-block:: shell

    curl -fsSL https://get.docker.com | bash -s docker --mirror Aliyun


.. code-block:: shell

    $ pwd
    /home/pi
    $ mkdir homeassistant
    $ sudo docker run -d \
    --name homeassistant \
    --privileged \
    --restart=unless-stopped \
    -v /home/pi/homeassistant:/config \
    --network=host \
    ghcr.io/home-assistant/home-assistant:stable

了解更多，请参考 https://www.home-assistant.io/installation/raspberrypi

确认
----

.. code-block:: shell

    $ sudo docker ps -a
    CONTAINER ID   IMAGE                                          COMMAND   CREATED        STATUS        PORTS     NAMES
    f43aa176c789   ghcr.io/home-assistant/home-assistant:stable   "/init"   15 hours ago   Up 15 hours             homeassistant

配置
----

一旦 Home Assistant 容器成功运行，可以使用 :code:`http://<host>:8123` 访问 （替换为系统的主机名或 IP）

.. image:: ../../_static/user-guide/home-assistant/create-account.png

集成MQTT
--------

.. image:: ../../_static/user-guide/home-assistant/configuration.png

.. image:: ../../_static/user-guide/home-assistant/add-mqtt.png

.. image:: ../../_static/user-guide/home-assistant/config-mqtt.png

.. image:: ../../_static/user-guide/home-assistant/added.png
