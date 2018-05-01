===========================================
ShirabeEngine Documentation 
===========================================
---------------
Resource-System
---------------

.. resource_system.rst

..
   by shirabe/mbt
   on 2017/09/17

.. :author: boehm.marc.anton@gmail.com
.. :version: 0.1


.. raw:: pdf

    Spacer 0,30


.. class:: center

.. raw:: pdf

    Spacer 0,50


.. raw:: pdf

   PageBreak


.. contents::

.. section-numbering::


.. raw:: pdf

   PageBreak


Introduction
============

Notes in general

1. Any resource type is reuquired to derive from descriptor adapter holding the effective descriptor.

   It should expose a full, limited and/or possibly extended derivative adaption of the underlying descriptor.

2. For each platform agnostic implementation of a system resource type, there'll be a dedicated builder specialization,
   accepting a ResourceHandle for a specific device, as well as a reference to the resource manager itself 
   used to create the resources.


.. raw:: pdf

   PageBreak


.. header::

   .. image:: images/ADLogo.jpg
           :align: center

.. footer::

    .. class:: footright

        ###Page###/###Total###

    .. class:: footcenter

        ###Title### - 2014/09/05

..    .. class:: footleft