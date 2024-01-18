:github_url: hide

.. DO NOT EDIT THIS FILE!!!
.. Generated automatically from Godot engine sources.
.. Generator: https://github.com/godotengine/godot/tree/4.2/doc/tools/make_rst.py.
.. XML source: https://github.com/godotengine/godot/tree/4.2/modules/limboai/doc_classes/BTState.xml.

.. _class_BTState:

BTState
=======

**Inherits:** :ref:`LimboState<class_LimboState>`

A state node for :ref:`LimboHSM<class_LimboHSM>` that hosts a :ref:`BehaviorTree<class_BehaviorTree>`.

.. rst-class:: classref-introduction-group

Description
-----------

BTState is a :ref:`LimboState<class_LimboState>` node that manages a :ref:`BehaviorTree<class_BehaviorTree>` to provide behavior logic for the state. It instantiates and runs the :ref:`BehaviorTree<class_BehaviorTree>` resource, dispatching a state machine event upon ``SUCCESS`` or ``FAILURE``. Event names are customizable through :ref:`success_event<class_BTState_property_success_event>` and :ref:`failure_event<class_BTState_property_failure_event>`. For further details on state machine events, see :ref:`LimboState.dispatch<class_LimboState_method_dispatch>`.

.. rst-class:: classref-reftable-group

Properties
----------

.. table::
   :widths: auto

   +-----------------------------------------+------------------------------------------------------------+---------------+
   | :ref:`BehaviorTree<class_BehaviorTree>` | :ref:`behavior_tree<class_BTState_property_behavior_tree>` |               |
   +-----------------------------------------+------------------------------------------------------------+---------------+
   | String                                  | :ref:`failure_event<class_BTState_property_failure_event>` | ``"failure"`` |
   +-----------------------------------------+------------------------------------------------------------+---------------+
   | String                                  | :ref:`success_event<class_BTState_property_success_event>` | ``"success"`` |
   +-----------------------------------------+------------------------------------------------------------+---------------+

.. rst-class:: classref-section-separator

----

.. rst-class:: classref-descriptions-group

Property Descriptions
---------------------

.. _class_BTState_property_behavior_tree:

.. rst-class:: classref-property

:ref:`BehaviorTree<class_BehaviorTree>` **behavior_tree**

.. rst-class:: classref-property-setget

- void **set_behavior_tree** **(** :ref:`BehaviorTree<class_BehaviorTree>` value **)**
- :ref:`BehaviorTree<class_BehaviorTree>` **get_behavior_tree** **(** **)**

A :ref:`BehaviorTree<class_BehaviorTree>` resource that defines state behavior.

.. rst-class:: classref-item-separator

----

.. _class_BTState_property_failure_event:

.. rst-class:: classref-property

String **failure_event** = ``"failure"``

.. rst-class:: classref-property-setget

- void **set_failure_event** **(** String value **)**
- String **get_failure_event** **(** **)**

HSM event that will be dispatched when the behavior tree results in ``FAILURE``. See :ref:`LimboState.dispatch<class_LimboState_method_dispatch>`.

.. rst-class:: classref-item-separator

----

.. _class_BTState_property_success_event:

.. rst-class:: classref-property

String **success_event** = ``"success"``

.. rst-class:: classref-property-setget

- void **set_success_event** **(** String value **)**
- String **get_success_event** **(** **)**

HSM event that will be dispatched when the behavior tree results in ``SUCCESS``. See :ref:`LimboState.dispatch<class_LimboState_method_dispatch>`.

.. |virtual| replace:: :abbr:`virtual (This method should typically be overridden by the user to have any effect.)`
.. |const| replace:: :abbr:`const (This method has no side effects. It doesn't modify any of the instance's member variables.)`
.. |vararg| replace:: :abbr:`vararg (This method accepts any number of arguments after the ones described here.)`
.. |constructor| replace:: :abbr:`constructor (This method is used to construct a type.)`
.. |static| replace:: :abbr:`static (This method doesn't need an instance to be called, so it can be called directly using the class name.)`
.. |operator| replace:: :abbr:`operator (This method describes a valid operator to use with this type as left-hand operand.)`
.. |bitfield| replace:: :abbr:`BitField (This value is an integer composed as a bitmask of the following flags.)`