:github_url: hide

.. DO NOT EDIT THIS FILE!!!
.. Generated automatically from Godot engine sources.
.. Generator: https://github.com/godotengine/godot/tree/4.3/doc/tools/make_rst.py.
.. XML source: https://github.com/godotengine/godot/tree/4.3/modules/limboai/doc_classes/BTProbability.xml.

.. _class_BTProbability:

BTProbability
=============

**Inherits:** :ref:`BTDecorator<class_BTDecorator>` **<** :ref:`BTTask<class_BTTask>` **<** :ref:`BT<class_BT>`

BT decorator that executes its child task with a given probability.

.. rst-class:: classref-introduction-group

Description
-----------

BTProbability executes its child task with a given probability defined by :ref:`run_chance<class_BTProbability_property_run_chance>`.

Returns the result of the child task if it was executed; otherwise, it returns ``FAILURE``.

.. rst-class:: classref-reftable-group

Properties
----------

.. table::
   :widths: auto

   +-----------+------------------------------------------------------------+---------+
   | ``float`` | :ref:`run_chance<class_BTProbability_property_run_chance>` | ``0.5`` |
   +-----------+------------------------------------------------------------+---------+

.. rst-class:: classref-section-separator

----

.. rst-class:: classref-descriptions-group

Property Descriptions
---------------------

.. _class_BTProbability_property_run_chance:

.. rst-class:: classref-property

``float`` **run_chance** = ``0.5`` :ref:`🔗<class_BTProbability_property_run_chance>`

.. rst-class:: classref-property-setget

- |void| **set_run_chance**\ (\ value\: ``float``\ )
- ``float`` **get_run_chance**\ (\ )

Probability that defines how likely the child task will be executed.

.. |virtual| replace:: :abbr:`virtual (This method should typically be overridden by the user to have any effect.)`
.. |const| replace:: :abbr:`const (This method has no side effects. It doesn't modify any of the instance's member variables.)`
.. |vararg| replace:: :abbr:`vararg (This method accepts any number of arguments after the ones described here.)`
.. |constructor| replace:: :abbr:`constructor (This method is used to construct a type.)`
.. |static| replace:: :abbr:`static (This method doesn't need an instance to be called, so it can be called directly using the class name.)`
.. |operator| replace:: :abbr:`operator (This method describes a valid operator to use with this type as left-hand operand.)`
.. |bitfield| replace:: :abbr:`BitField (This value is an integer composed as a bitmask of the following flags.)`
.. |void| replace:: :abbr:`void (No return value.)`
