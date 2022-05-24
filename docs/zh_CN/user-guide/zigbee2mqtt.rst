zigbee2mqtt
===========

配置
----

- 连接 :code:`LilyGo Zigbee2MQTT` 发出的热点，例如 :code:`LilyGo-5090`

.. image:: ../../_static/user-guide/zigbee2mqtt/wifi.jpg

- 在浏览器访问 :code:`192.168.4.1` 进入配置网页

.. image:: ../../_static/user-guide/zigbee2mqtt/web_index.jpg

- 配置需要连接的WiFi和mqtt服务器

.. image:: ../../_static/user-guide/zigbee2mqtt/web_settings.jpg

wifi和mqtt服务器连接成功后， :code:`LilyGo Zigbee2MQTT` 上的蓝色指示灯将会常亮。

按键行为
--------

.. csv-table::

    "单击", "禁止zigbee子设备加入网关"
    "双击", "允许zigbee子设备加入网关"

指示灯
-------

红色指示灯
~~~~~~~~~~

.. csv-table::

    "灭", "zigbee处于关闭状态"
    "亮", "zigbee处于运行状态"

绿色指示灯
~~~~~~~~~~

.. csv-table::

    "灭", "禁止zigbee子设备加入网关"
    "亮", "允许zigbee子设备加入网关"

蓝色指示灯
~~~~~~~~~~

.. csv-table::

    "灭",   "运行错误"
    "亮",   "正常运行"
    "快闪", "未连接上WiFi"
    "慢闪", "已连接上WiFi，未连接上mqtt服务器"
