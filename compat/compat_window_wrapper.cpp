/**************************************************************************/
/*  compat_window_wrapper.cpp                                                    */
/**************************************************************************/
/*                         This file is part of:                          */
/*                             GODOT ENGINE                               */
/*                        https://godotengine.org                         */
/**************************************************************************/
/* Copyright (c) 2014-present Godot Engine contributors (see AUTHORS.md). */
/* Copyright (c) 2007-2014 Juan Linietsky, Ariel Manzur.                  */
/*                                                                        */
/* Permission is hereby granted, free of charge, to any person obtaining  */
/* a copy of this software and associated documentation files (the        */
/* "Software"), to deal in the Software without restriction, including    */
/* without limitation the rights to use, copy, modify, merge, publish,    */
/* distribute, sublicense, and/or sell copies of the Software, and to     */
/* permit persons to whom the Software is furnished to do so, subject to  */
/* the following conditions:                                              */
/*                                                                        */
/* The above copyright notice and this permission notice shall be         */
/* included in all copies or substantial portions of the Software.        */
/*                                                                        */
/* THE SOFTWARE IS PROVIDED "AS IS", WITHOUT WARRANTY OF ANY KIND,        */
/* EXPRESS OR IMPLIED, INCLUDING BUT NOT LIMITED TO THE WARRANTIES OF     */
/* MERCHANTABILITY, FITNESS FOR A PARTICULAR PURPOSE AND NONINFRINGEMENT. */
/* IN NO EVENT SHALL THE AUTHORS OR COPYRIGHT HOLDERS BE LIABLE FOR ANY   */
/* CLAIM, DAMAGES OR OTHER LIABILITY, WHETHER IN AN ACTION OF CONTRACT,   */
/* TORT OR OTHERWISE, ARISING FROM, OUT OF OR IN CONNECTION WITH THE      */
/* SOFTWARE OR THE USE OR OTHER DEALINGS IN THE SOFTWARE.                 */
/**************************************************************************/

#include "compat_window_wrapper.h"

#ifdef LIMBOAI_GDEXTENSION

#include "../compat/editor_scale.h"
#include "../compat/editor_settings.h"
#include "../compat/limbo_compat.h"
#include "../compat/scene_tree.h"
#include "../compat/translation.h"
#include "../util/limbo_string_names.h"

#include <godot_cpp/classes/engine.hpp>
#include <godot_cpp/classes/input_event_key.hpp>
#include <godot_cpp/classes/input_event_mouse_button.hpp>
#include <godot_cpp/classes/input_event_shortcut.hpp>
#include <godot_cpp/classes/scene_tree.hpp>
#include <godot_cpp/classes/style_box.hpp>
#include <godot_cpp/classes/v_box_container.hpp>

using namespace godot;

// WindowWrapper

void CompatShortcutBin::_notification(int what) {
	switch (what) {
		case NOTIFICATION_READY:
			set_process_shortcut_input(true);
			break;
	}
}

void CompatShortcutBin::_bind_methods() {}

void CompatShortcutBin::_shortcut_input(const Ref<InputEvent> &p_event) {
	if (!get_window()->is_visible()) {
		return;
	}

	ERR_FAIL_NULL(get_parent());
	Viewport *vp = get_parent()->get_viewport();
	godot::Window *win = nullptr;
	while (vp) {
		win = Object::cast_to<godot::Window>(vp);
		if (win && win->is_visible()) {
			break;
		}
		if (!vp->get_parent()) {
			break;
		}

		vp = vp->get_parent()->get_viewport();
	}
	godot::Window *grandparent_window = win;

	ERR_FAIL_NULL(grandparent_window);

	if (Object::cast_to<InputEventKey>(p_event.ptr()) || Object::cast_to<InputEventShortcut>(p_event.ptr())) {
		// HACK: Propagate the window input to the editor main window to handle global shortcuts.
		// ! This hack crashes the editor if used in GDExtension. Disabled for now.
		// grandparent_window->push_input(p_event);

		if (grandparent_window->is_input_handled()) {
			get_viewport()->set_input_as_handled();
		}
	}
}

Rect2 CompatWindowWrapper::_get_default_window_rect() const {
	ERR_FAIL_COND_V(!is_inside_tree(), Rect2());

	// Assume that the control rect is the desidered one for the window.
	Transform2D xform = get_screen_transform();
	return Rect2(xform.get_origin(), xform.get_scale() * get_size());
}

Node *CompatWindowWrapper::_get_wrapped_control_parent() const {
	if (margins) {
		return margins;
	}
	return window;
}

void CompatWindowWrapper::_set_window_enabled_with_rect(bool p_visible, const Rect2 p_rect) {
	ERR_FAIL_NULL(wrapped_control);

	if (!is_window_available()) {
		return;
	}

	if (window->is_visible() == p_visible) {
		if (p_visible) {
			window->grab_focus();
		}
		return;
	}

	Node *parent = _get_wrapped_control_parent();

	if (wrapped_control->get_parent() != parent) {
		// Move the control to the window.
		wrapped_control->reparent(parent, false);

		_set_window_rect(p_rect);
		wrapped_control->set_anchors_and_offsets_preset(PRESET_FULL_RECT);

	} else if (!p_visible) {
		// Remove control from window.
		wrapped_control->reparent(this, false);
	}

	window->set_visible(p_visible);
	if (!p_visible) {
		emit_signal("window_close_requested");
	}
	emit_signal("window_visibility_changed", p_visible);
}

void CompatWindowWrapper::_set_window_rect(const Rect2 p_rect) {
	// Set the window rect even when the window is maximized to have a good default size
	// when the user remove the maximized mode.
	window->set_position(p_rect.position);
	window->set_size(p_rect.size);

	if (EDITOR_GET("interface/multi_window/maximize_window")) {
		window->set_mode(godot::Window::MODE_MAXIMIZED);
	}
}

void CompatWindowWrapper::_shortcut_input(const Ref<InputEvent> &p_event) {
	if (enable_shortcut.is_valid() && enable_shortcut->matches_event(p_event)) {
		set_window_enabled(true);
	}
}

void CompatWindowWrapper::set_wrapped_control(Control *p_control, const Ref<Shortcut> &p_enable_shortcut) {
	ERR_FAIL_NULL(p_control);
	ERR_FAIL_COND(wrapped_control);

	wrapped_control = p_control;
	enable_shortcut = p_enable_shortcut;
	add_child(p_control);
}

Control *CompatWindowWrapper::get_wrapped_control() const {
	return wrapped_control;
}

Control *CompatWindowWrapper::release_wrapped_control() {
	set_window_enabled(false);
	if (wrapped_control) {
		Control *old_wrapped = wrapped_control;
		wrapped_control->get_parent()->remove_child(wrapped_control);
		wrapped_control = nullptr;

		return old_wrapped;
	}
	return nullptr;
}

bool CompatWindowWrapper::is_window_available() const {
	return window != nullptr;
}

bool CompatWindowWrapper::get_window_enabled() const {
	return is_window_available() ? window->is_visible() : false;
}

void CompatWindowWrapper::set_window_enabled(bool p_enabled) {
	_set_window_enabled_with_rect(p_enabled, _get_default_window_rect());
}

Rect2i CompatWindowWrapper::get_window_rect() const {
	ERR_FAIL_COND_V(!get_window_enabled(), Rect2i());
	return Rect2i(window->get_position(), window->get_size());
}

int CompatWindowWrapper::get_window_screen() const {
	ERR_FAIL_COND_V(!get_window_enabled(), -1);
	return window->get_current_screen();
}

void CompatWindowWrapper::restore_window(const Rect2i &p_rect, int p_screen) {
	ERR_FAIL_COND(!is_window_available());
	ERR_FAIL_INDEX(p_screen, DisplayServer::get_singleton()->get_screen_count());

	_set_window_enabled_with_rect(true, p_rect);
	window->set_current_screen(p_screen);
}

void CompatWindowWrapper::restore_window_from_saved_position(const Rect2 p_window_rect, int p_screen, const Rect2 p_screen_rect) {
	ERR_FAIL_COND(!is_window_available());

	Rect2 window_rect = p_window_rect;
	int screen = p_screen;
	Rect2 restored_screen_rect = p_screen_rect;

	if (screen < 0 || screen >= DisplayServer::get_singleton()->get_screen_count()) {
		// Fallback to the main window screen if the saved screen is not available.
		screen = get_window()->get_window_id();
	}

	Rect2i real_screen_rect = DisplayServer::get_singleton()->screen_get_usable_rect(screen);

	if (restored_screen_rect == Rect2i()) {
		// Fallback to the target screen rect.
		restored_screen_rect = real_screen_rect;
	}

	if (window_rect == Rect2i()) {
		// Fallback to a standard rect.
		window_rect = Rect2i(restored_screen_rect.position + restored_screen_rect.size / 4, restored_screen_rect.size / 2);
	}

	// Adjust the window rect size in case the resolution changes.
	Vector2 screen_ratio = Vector2(real_screen_rect.size) / Vector2(restored_screen_rect.size);

	// The screen positioning may change, so remove the original screen position.
	window_rect.position -= restored_screen_rect.position;
	window_rect = Rect2i(window_rect.position * screen_ratio, window_rect.size * screen_ratio);
	window_rect.position += real_screen_rect.position;

	// All good, restore the window.
	window->set_current_screen(p_screen);
	if (window->is_visible()) {
		_set_window_rect(window_rect);
	} else {
		_set_window_enabled_with_rect(true, window_rect);
	}
}

void CompatWindowWrapper::enable_window_on_screen(int p_screen, bool p_auto_scale) {
	int current_screen = Object::cast_to<godot::Window>(get_viewport())->get_current_screen();
	int screen = p_screen < 0 ? current_screen : p_screen;

	bool auto_scale = p_auto_scale && !EDITOR_GET("interface/multi_window/maximize_window");

	if (auto_scale && current_screen != screen) {
		Rect2 control_rect = _get_default_window_rect();

		Rect2i source_screen_rect = DisplayServer::get_singleton()->screen_get_usable_rect(current_screen);
		Rect2i dest_screen_rect = DisplayServer::get_singleton()->screen_get_usable_rect(screen);

		// Adjust the window rect size in case the resolution changes.
		Vector2 screen_ratio = Vector2(source_screen_rect.size) / Vector2(dest_screen_rect.size);

		// The screen positioning may change, so remove the original screen position.
		control_rect.position -= source_screen_rect.position;
		control_rect = Rect2i(control_rect.position * screen_ratio, control_rect.size * screen_ratio);
		control_rect.position += dest_screen_rect.position;

		restore_window(control_rect, p_screen);
	} else {
		window->set_current_screen(p_screen);
		set_window_enabled(true);
	}
}

void CompatWindowWrapper::set_window_title(const String p_title) {
	if (!is_window_available()) {
		return;
	}
	window->set_title(p_title);
}

void CompatWindowWrapper::set_margins_enabled(bool p_enabled) {
	if (!is_window_available()) {
		return;
	}

	if (!p_enabled && margins) {
		margins->queue_free();
		margins = nullptr;
	} else if (p_enabled && !margins) {
		Size2 borders = Size2(4, 4) * EDSCALE;
		margins = memnew(MarginContainer);
		margins->add_theme_constant_override("margin_right", borders.width);
		margins->add_theme_constant_override("margin_top", borders.height);
		margins->add_theme_constant_override("margin_left", borders.width);
		margins->add_theme_constant_override("margin_bottom", borders.height);

		window->add_child(margins);
		margins->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	}
}

void CompatWindowWrapper::_bind_methods() {
	ADD_SIGNAL(MethodInfo("window_visibility_changed", PropertyInfo(Variant::BOOL, "visible")));
	ADD_SIGNAL(MethodInfo("window_close_requested"));
}

void CompatWindowWrapper::_notification(int p_what) {
	if (!is_window_available()) {
		return;
	}
	switch (p_what) {
		case NOTIFICATION_VISIBILITY_CHANGED: {
			if (get_window_enabled() && is_visible()) {
				// Grab the focus when WindowWrapper.set_visible(true) is called
				// and the window is showing.
				window->grab_focus();
			}
		} break;
		case NOTIFICATION_READY: {
			set_process_shortcut_input(true);
			window->connect("close_requested", callable_mp(this, &CompatWindowWrapper::set_window_enabled).bind(false));

		} break;
		case NOTIFICATION_THEME_CHANGED: {
			window_background->add_theme_stylebox_override("panel", get_theme_stylebox("PanelForeground", LW_NAME(EditorStyles)));
		} break;
	}
}

CompatWindowWrapper::CompatWindowWrapper() {
	if (SCENE_TREE()->get_root()->is_embedding_subwindows() || EDITOR_GET("interface/editor/single_window_mode") || !EDITOR_GET("interface/multi_window/enable")) {
		return;
	}

	window = memnew(godot::Window);
	window->set_wrap_controls(true);

	add_child(window);
	window->hide();

	// ! CompatShortcutBin crashes the editor if used in GDExtension. Disabled for now.
	// CompatShortcutBin *capturer = memnew(CompatShortcutBin);
	// window->add_child(capturer);

	window_background = memnew(Panel);
	window_background->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	window->add_child(window_background);

	// ProgressDialog::get_singleton()->add_host_window(window);
}

// LWScreenSelect

void CompatScreenSelect::_build_advanced_menu() {
	// Clear old screen list.
	while (screen_list->get_child_count(false) > 0) {
		Node *child = screen_list->get_child(0);
		screen_list->remove_child(child);
		child->queue_free();
	}

	// Populate screen list.
	const real_t height = real_t(get_theme_font_size("font_size")) * 1.5;

	int current_screen = get_window()->get_current_screen();
	for (int i = 0; i < DisplayServer::get_singleton()->get_screen_count(); i++) {
		Button *button = memnew(Button);

		Size2 screen_size = Size2(DisplayServer::get_singleton()->screen_get_size(i));
		Size2 button_size = Size2(height * (screen_size.x / screen_size.y), height);
		button->set_custom_minimum_size(button_size);
		screen_list->add_child(button);

		button->set_text(itos(i));
		button->set_text_alignment(HORIZONTAL_ALIGNMENT_CENTER);
		button->set_tooltip_text(vformat(TTR("Make this panel floating in the screen %d."), i));

		if (i == current_screen) {
			Color accent_color = get_theme_color("accent_color", LW_NAME(Editor));
			button->add_theme_color_override("font_color", accent_color);
		}

		button->connect("pressed", callable_mp(this, &CompatScreenSelect::_emit_screen_signal).bind(i));
		button->connect("pressed", callable_mp(static_cast<BaseButton *>(this), &CompatScreenSelect::set_pressed).bind(false));
		button->connect("pressed", callable_mp(static_cast<godot::Window *>(popup), &Popup::hide));
	}
}

void CompatScreenSelect::_emit_screen_signal(int p_screen_idx) {
	emit_signal("request_open_in_screen", p_screen_idx);
}

void CompatScreenSelect::_bind_methods() {
	ADD_SIGNAL(MethodInfo("request_open_in_screen", PropertyInfo(Variant::INT, "screen")));
}

void CompatScreenSelect::_notification(int p_what) {
	switch (p_what) {
		case NOTIFICATION_READY: {
			connect("gui_input", callable_mp(this, &CompatScreenSelect::_handle_mouse_shortcut));
			popup->connect("popup_hide", callable_mp(static_cast<BaseButton *>(this), &CompatScreenSelect::set_pressed).bind(false));
		} break;
		case NOTIFICATION_THEME_CHANGED: {
			set_button_icon(get_theme_icon("MakeFloating", LW_NAME(EditorIcons)));
			popup_background->add_theme_stylebox_override("panel", get_theme_stylebox("PanelForeground", LW_NAME(EditorStyles)));

			const real_t popup_height = real_t(get_theme_font_size("font_size")) * 2.0;
			popup->set_min_size(Size2(0, popup_height * 3));
		} break;
	}
}

void CompatScreenSelect::_handle_mouse_shortcut(const Ref<InputEvent> &p_event) {
	const Ref<InputEventMouseButton> mouse_button = p_event;
	if (mouse_button.is_valid()) {
		if (mouse_button->is_pressed() && mouse_button->get_button_index() == LW_MBTN(LEFT)) {
			_emit_screen_signal(get_window()->get_current_screen());
			accept_event();
		}
	}
}

void CompatScreenSelect::_show_popup() {
	// Adapted from /scene/gui/menu_button.cpp::show_popup
	if (!get_viewport()) {
		return;
	}

	Size2 size = get_size() * get_viewport()->get_canvas_transform().get_scale();

	popup->set_size(Size2(size.width, 0));
	Point2 gp = get_screen_position();
	gp.y += size.y;
	if (is_layout_rtl()) {
		gp.x += size.width - popup->get_size().width;
	}
	popup->set_position(gp);
	popup->popup();
}

void CompatScreenSelect::_pressed() {
	if (popup->is_visible()) {
		popup->hide();
		return;
	}

	_build_advanced_menu();
	_show_popup();
}

CompatScreenSelect::CompatScreenSelect() {
	set_tooltip_text(TTR("Make this panel floating.\nRight click to open the screen selector."));
	set_button_mask(LW_MBTN(RIGHT));
	set_flat(true);
	set_toggle_mode(true);
	set_focus_mode(FOCUS_NONE);
	set_action_mode(ACTION_MODE_BUTTON_PRESS);

	// Create the popup.
	const Size2 borders = Size2(4, 4) * EDSCALE;

	popup = memnew(Popup);
	add_child(popup);

	popup_background = memnew(Panel);
	popup_background->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
	popup->add_child(popup_background);

	MarginContainer *popup_root = memnew(MarginContainer);
	popup_root->add_theme_constant_override("margin_right", borders.width);
	popup_root->add_theme_constant_override("margin_top", borders.height);
	popup_root->add_theme_constant_override("margin_left", borders.width);
	popup_root->add_theme_constant_override("margin_bottom", borders.height);
	popup->add_child(popup_root);

	VBoxContainer *vb = memnew(VBoxContainer);
	vb->set_alignment(BoxContainer::ALIGNMENT_CENTER);
	popup_root->add_child(vb);

	Label *description = memnew(Label);
	description->set_text(TTR("Select Screen"));
	description->set_horizontal_alignment(HORIZONTAL_ALIGNMENT_CENTER);
	vb->add_child(description);

	screen_list = memnew(godot::HBoxContainer);
	screen_list->set_alignment(BoxContainer::ALIGNMENT_CENTER);
	vb->add_child(screen_list);

	popup_root->set_anchors_and_offsets_preset(PRESET_FULL_RECT);
}

#endif // ! LIMBOAI_GDEXTENSION
