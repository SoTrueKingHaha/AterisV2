#pragma once

#include "vars.h"
#include "../../utils/draw/draw.h"
#include "Input.h"
#include <vector>
#include <utility>
#include <functional>
#include <memory>

class CMenuWindow;
class CMenuTab;
class CMenuGroupbox;
class CItem;
class CItemCheckbox;
class CItemButton;
class CItemKeybind;
class CItemCombobox;
class CItemSliderInt;
class CItemSliderFloat;
class CItemColorPicker;
class CItemMultiCombobox;
class CItemTextbox;

inline CMenuWindow* window = nullptr;
enum EItemType
{
	Item,
	Space,
	Checkbox,
	Button,
	Keybind,
	Combobox,
	SliderInt,
	SliderFloat,
	ColorPicker,
	MultiCombobox,
	Textbox,
};

class CItem
{
public:
	CItem(const char* name, EItemType type = Item) : name(name), type(type)
	{

	}

	virtual ~CItem() = default;

	virtual void render() = 0;
	virtual bool update(int x, int y) = 0;

	const char* get_name() const { return name; }

	const char* name;

	Rect_t position;

	EItemType type;
};

class CItemSpace : public CItem
{
public:
	CItemSpace( const char* name, bool line, int x = 0, int y = 0 ) : CItem(name, Space), line( line ), override_x( x ), override_y( y )
	{

	}

	void render( ) override;
	bool update(int x, int y) override;
	bool line;
	int override_x;
	int override_y;
};

class CMenuGroupbox
{
public:
	CMenuGroupbox(const char* name, std::vector<CItem*> items) :
		name(name),
		items(std::move(items))
	{

	}

	virtual ~CMenuGroupbox() = default;

	void render();
	void update();

	const char* get_name() const { return name; }
	std::vector<CItem*>& get_items() { return items; }
	int offset = 0;

	void set_position(int x, int y) { position.x = x; position.y = y; }
	void set_size(int w, int h) { position.w = w; position.h = h; }
	void set_id( int a ) { id = a; }

public:
	const char* name;
	std::vector<CItem*> items;
	Rect_t position;
	int id;
};

class CMenuTab
{
public:
	CMenuTab(const char* name, std::vector<CMenuGroupbox*> groups) :
		name(name),
		groups(std::move(groups))
	{

	}

	virtual ~CMenuTab() = default;

	void render(int i, int t);
	void update(int i, int &t, const Rect_t& windowpos);

	const char* get_name() { return name; }
	std::vector<CMenuGroupbox*>& get_groups() { return groups; }
	int get_x() { return tab.x; }
	int get_y() { return tab.y; }
	int get_w() { return tab.w; }
	int get_h() { return tab.h; }
	void set_x(int x) { tab.x = x; }
	void set_y(int y) { tab.y = y; }

public:
	const char* name;
	text_size name_size{ MENU, this->name };
	std::vector<CMenuGroupbox*> groups;
	int current_group = 0;
	Rect_t tab;
};

class CMenuWindow
{
public:
	CMenuWindow(const char* name, std::vector<CMenuTab*> tabs) :
		name(name),
		tabs(std::move(tabs))
	{

	}

	virtual ~CMenuWindow() = default;

	void render();
	void update();

	const char* get_name() { return name; }
	std::vector<CMenuTab*>& get_tabs() { return tabs; }
	int get_x() { return window.x; }
	int get_y() { return window.y; }
	int get_w() { return window.w; }
	int get_h() { return window.h; }

public:
	const char* name;
	std::vector<CMenuTab*> tabs;
	int current_tab = 0;
	Rect_t window = { 600, 300, 551, 501 };
	POINT drag_delta;
	bool dragging = false;
	bool moving = false;
};

class CItemCheckbox : public CItem
{
public:
	CItemCheckbox(const char* name, bool& var) :
		CItem(name, Checkbox), var(&var)
	{

	}

	void render() override;
	bool update(int x, int y) override;

	bool* var;
};

class CItemButton : public CItem
{
public:
	CItemButton(const char* name, std::function<void()> callback) :
		CItem(name, Button), callback_func(callback)
	{

	}

	void render() override;
	bool update(int x, int y) override;

	std::function<void()> callback_func;
	float time_pressed = 0.0f;
};

class CItemKeybind : public CItem
{
public:
	CItemKeybind(const char* name, int& key, std::vector<int> disallowedkeys = {}) :
		CItem(name, Keybind), var(&key), blocked_keys(std::move(disallowedkeys))
	{

	}

	void render() override;
	bool update(int x, int y) override;

	int* var;
	std::vector<int> blocked_keys;
	bool active;
	float time_pressed = 0.0f;

	inline static std::string key_code_to_string(int virtualKey)
	{
		switch (virtualKey)
		{
			case VK_LBUTTON: return "Mouse 1";
			case VK_RBUTTON: return "Mouse 2";
			case VK_MBUTTON: return "Mouse 3";
			case VK_XBUTTON1: return "Mouse 4";
			case VK_XBUTTON2: return "Mouse 5";
			case VK_SPACE: return "Space";
			case 0x0: return "None";
		}

		CHAR output[32] = { "\0" };

		if (const int result = GetKeyNameTextA(MapVirtualKeyA(virtualKey, MAPVK_VK_TO_VSC) << 16, output, 16))
		{
			return output;
		}

		return "Unknown";
	}
};

class CItemSliderInt : public CItem
{
public:
	CItemSliderInt(const char* name, int& var, int min, int max, int step = 1, const char* format = "%d") :
		CItem(name, SliderInt), var(&var), step(step), format(format), var_min(min), var_max(max)
	{

	}

	void render() override;
	bool update(int x, int y) override;

	int* var;
	int step;
	int slider_x;
	const char* format;
	int var_min;
	int var_max;
};

class CItemSliderFloat : public CItem
{
public:
	CItemSliderFloat(const char* name, float& var, float min, float max, float step = 0.1f, const char* format = "%.2f") :
		CItem(name, SliderFloat), var(&var), step(step), format(format), var_min(min), var_max(max)
	{

	}

	void render() override;
	bool update(int x, int y) override;

	float* var;
	float step;
	int slider_x;
	const char* format;
	float var_min;
	float var_max;
};

// I am here

class CItemTextbox : public CItem
{
public:
	CItemTextbox(const char* name, std::string* var, int maxlength = 20, std::vector<char> disallowedcharacters = {}) :
		CItem(name, Textbox), input_string(var), max_length(maxlength), blocked_chars(std::move(disallowedcharacters))
	{

	}

	void render() override;
	bool update(int x, int y) override;

	std::string* input_string;
	int max_length;
	std::vector<char> blocked_chars;
	bool typing = false;
	float remove_time = 0.0f;
};



class CItemCombobox : public CItem
{
public:
	CItemCombobox(const char* name, int& var, std::vector<const char*> items) :
		CItem(name, Combobox), var(&var), combo_items(std::move(items))
	{

	}

	void render() override;
	bool update(int x, int y) override;

	void RenderCombo();
	void UpdateCombo();

	std::vector<const char*> combo_items;
	int* var;
	bool open = false;
	float time_pressed = 0.0f;
};

class CItemMultiCombobox : public CItem
{
public:
	CItemMultiCombobox(const char* name, std::vector<std::pair<const char*, bool*>> items) :
		CItem(name, MultiCombobox), combo_items(std::move(items))
	{

	}

	void render() override;
	bool update(int x, int y) override;

	void RenderCombo();
	void UpdateCombo();

	std::vector<std::pair<const char*, bool*>> combo_items;
	bool open = false;
	float time_pressed = 0.0f;
};

class CItemColorPicker : public CItem
{
public:
	CItemColorPicker(const char* name, color& var) :
		CItem(name, ColorPicker), color(&var)
	{

	}

	void render() override;
	bool update(int x, int y) override;

	void RenderPicker();

	color* color;
	bool open = false;
	float time_pressed = 0.0f;
	float hsv[3];
	Rect_t window_pos;
};

class __window_menu
{
public:
	CItemCombobox *opened_combobox = nullptr;
	CItemMultiCombobox *opened_multi_combobox = nullptr;
	CItemColorPicker *opened_colorpicker = nullptr;
	bool colorpicker_opened = false;

	void run_menu( );
};
inline __window_menu window_menu;