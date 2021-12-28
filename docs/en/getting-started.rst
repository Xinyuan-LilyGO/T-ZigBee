Getting started
===============

Requirement
-----------

In order to use Zigbee2MQTT, we need the following hardware:

1. TTGO Zigbee2MQTT

2. A server that can run MQTT broker and Home Assistant (e.g. Raspberry Pi)

3. One or more Zigbee devices that will be paired with Zigbee2MQTT.

Set up and start Zigbee2MQTT
----------------------------

Connect to MQTT server
~~~~~~~~~~~~~~~~~~~~~~~

Modify :code:`examples/zigbee2mqtt/main/app_config.h`, for example:

.. literalinclude:: ../../examples/zigbee2mqtt/main/app_config.h
    :linenos:
    :language: c
    :lines: 16-19

Connect the device
------------------

Search for supported devices for your device and follow the instructions to pair.

Once you see something similar to the following in the log, your device is
paired and you can start to control it using the front end and MQTT messages.

::

    I (276254) Zigbee2MQTT: Successfully interviewed '0x00158d000774d938', device has successfully been paired

Home Assistant
---------------

.. note::

    Home Assistant integrates MQTT, please refer to :doc:`user-guide/home-assistant`

After Home Assistant integrates MQTT, the device information will be reported to Home Assistant

.. image:: ../_static/getting-started/check-device.png

Follow the steps below to add devices to the homepage

.. image:: ../_static/getting-started/edit-dashboard.png

.. image:: ../_static/getting-started/add-card.png

.. image:: ../_static/getting-started/select-device.png

.. image:: ../_static/getting-started/add -to-homepage.png

The addition is successful, you can see the changes in the measurement data in real time

.. image:: ../_static/getting-started/added.png
