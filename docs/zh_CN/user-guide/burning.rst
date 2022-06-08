烧录指南
========


概述
----

:code:`T-ZigBee` 板载了两个SOC，分别是 :code:`espressif` 的 :code:`ESP32-C3` 和
:code:`telink` 的 :code:`TLSR8258`。

其中，:code:`ESP32-C3` 集成了WiFI和BLE，:code:`TLSR8258` 集成ZigBee和BLE。
:code:`TLSR8258` 仅使用它的ZigBee功能。

对ESP32-C3和TLSR8258烧录固件，需要使用到 :code:`T-U2T`。通过拨码开关，选择对不同SOC
烧录固件。

ESP32-C3虽然支持使用USB下载固件，但是在一些开发环境下，无法直接使用USB查看调试信息，
导致不便。因此，我们使用ESP32-C3的UART0对固件进行下载和调试。

.. note::

    因为TLSR8258的电源受 :code:`ESP32-C3` 的GPIO控制，在烧录 :code:`TLSR8258` 前，
    请先烧录 `examples/factory_test <../../../../../examples/factory_test>`_
    这个程序，以打开 :code:`TLSR8258` 的电源。

ESP32-C3
---------

1. 连接 :code:`T-U2T` 连接到 :code:`T-ZigBee`

.. image:: ../../_static/user-guide/burning/connect.jpg

2. 设置拨码开关

.. image:: ../../_static/upload_mode_c3.png

3. 烧录

.. image:: ../../_static/user-guide/burning/arduino_setting.png

TLSR8258
---------

.. note::

    :code:`T-ZigBee` 出厂时，已经烧录了 `sampleGW_8258_20220302.bin <../../../../../firmware/sampleGW_8258_20220302.bin>`_，
    如果不对 :code:`TLSR8258` 做其他功能改动，不建议随意烧录 :code:`TLSR8258` 的固件。

1. 连接 :code:`T-U2T` 连接到 :code:`T-ZigBee`

.. image:: ../../_static/user-guide/burning/connect.jpg

2. 设置拨码开关

.. image:: ../../_static/upload_mode.png

3. 使用 :code:TlsrComSwireWriter` 烧录固件

请预先下载 `TlsrComSwireWriter <https://github.com/pvvx/TlsrComSwireWriter>`_。

.. image:: ../../_static/user-guide/burning/TlsrComSwireWriter.png

.. image:: ../../_static/user-guide/burning/TlsrComSwireWriter_setting.png

4. 烧录成功，板子上的红色指示灯会亮

.. image:: ../../_static/user-guide/burning/burning_successfully.jpg


订购信息
--------

========= ===================== ================
产品      订购渠道
========= ======================================
T-ZigBee  t-zigbee_AliExpress_  t-zigbee_TaoBao_
--------- --------------------- ----------------
T-U2T     t-u2t_AliExpress_     t-u2t_TaoBao_
========= ===================== ================

.. _t-zigbee_AliExpress: https://www.aliexpress.com/item/3256803996075052.html
.. _t-zigbee_TaoBao: https://item.taobao.com/item.htm?spm=a1z10.1-c-s.w4004-24322072003.14.59cd19e104w9za&id=673454311484
.. _t-u2t_AliExpress: https://www.aliexpress.com/item/3256802262618023.htm
.. _t-u2t_TaoBao: https://item.taobao.com/item.htm?spm=a1z10.5-c-s.w4002-24322072023.27.26b01c38ExJUUl&id=641950617122
