#pragma warning ( disable : 4018 )
#include "WindowMenu.h"

#include "../../utils/math/math.h"

void CMenuWindow::render()
{
	int backupLeft, backupTop, backupRight, backupBottom; bool backupClippingDisabled;
	i::surface->GetClippingRect(backupLeft, backupTop, backupRight, backupBottom, backupClippingDisabled);

	i::surface->DisableClipping(false);
	i::surface->SetClippingRect(window.x, window.y, window.x + window.w, window.y + window.h);

	//background tabs
	draw.rect(window.x, window.y, window.w, window.h, vars.colors.menu.menu_background);

	//background groups
	draw.rect(window.x + 101, window.y + 19, window.w - 101, window.h, vars.colors.menu.menu_background2);

	//colored outline
	draw.outlined_rect(window.x, window.y, window.w, window.h, vars.colors.menu.menu_accent);

	//window line
	//draw.line( m_Window.x, m_Window.y + 19, m_Window.x + m_Window.w, m_Window.y + 19, vars.colors.testmenu.menu_accent );
	i::surface->DrawSetColor( vars.colors.menu.menu_accent.r, vars.colors.menu.menu_accent.g, vars.colors.menu.menu_accent.b, vars.colors.menu.menu_accent.a );
	i::surface->DrawFilledRectFade( window.x, window.y + 19, window.x + ( window.w / 2 ) + 52, window.y + 20, 255, 0, true );
	i::surface->DrawFilledRectFade( window.x + ( window.w / 2 ) + 52, window.y + 19, window.x + window.w, window.y + 20, 0, 255, true );

	//tab line
	draw.line( window.x + 101, window.y + 20, window.x + 101, window.y + window.h, vars.colors.menu.menu_accent );
	//draw.outlined_rect(m_Window.x, m_Window.y + 19, m_Window.w, 20, vars.colors.testmenu.Menu_Accent);
	draw.outlined_string(MENU, window.x + 5, window.y + 10, { 255, 255, 255, 255 }, ALIGN_CENTERVERTICAL, name);
	draw.outlined_string(MENU, window.x + window.w - 5, window.y + 4, { 255, 255, 255, 255 }, ALIGN_REVERSE, "| DEV Build |" );
	for (size_t i = 0; i < tabs.size(); i++)
	{
		const auto& tab = tabs.at(i);
		tab->render(i, current_tab);
	}

	i::surface->DisableClipping(backupClippingDisabled);
	i::surface->SetClippingRect(backupLeft, backupTop, backupRight, backupBottom);
}

void CMenuWindow::update()
{
	if (!input.in_focus() )
	{
		return;
	}

	bool held = input.is_held(VK_LBUTTON);

	if (input.in_area(get_x(), get_y(), get_w(), 20) && held)
	{
		dragging = true;

		if (!moving)
		{
			drag_delta.x = input.mouse_x - get_x();
			drag_delta.y = input.mouse_y - get_y();
			moving = true;
		}
	}

	if (dragging)
	{
		this->window.x = input.mouse_x - drag_delta.x;
		this->window.y = input.mouse_y - drag_delta.y;
	}

	if (!held)
	{
		dragging = false;
		moving = false;
	}

	int previousHeight = 0;
	for (size_t i = 0; i < tabs.size(); i++)
	{
		const auto& tab = tabs.at(i);
		tab->set_x(window.x + 1);
		tab->set_y(window.y + previousHeight + 20 );
		tab->update(i, current_tab, window);
		previousHeight += 30;
	}


}

void CMenuTab::render(int i, int t)
{
	if (i == t)
	{
		draw.rect(tab.x, tab.y, 100, 30, vars.colors.menu.menu_selected);
	}
	else if (input.in_area(tab.x, tab.y, 100, 30) && !window_menu.colorpicker_opened )
	{
		draw.rect(tab.x, tab.y, 100, 30, vars.colors.menu.menu_hovered);
	}
	draw.outlined_string(MENU, tab.x + (( 100 ) / 2), tab.y + 15, { 255, 255, 255, 255},ALIGN_CENTER, this->get_name());

	if (i != t)
	{
		return;
	}

	for (const auto& group : groups)
	{
		group->render();
	}

	if (window_menu.opened_combobox)
	{
		window_menu.opened_combobox->RenderCombo();
	}
	if (window_menu.opened_multi_combobox)
	{
		window_menu.opened_multi_combobox->RenderCombo();
	}
	if (window_menu.opened_colorpicker)
	{
		window_menu.opened_colorpicker->RenderPicker();
	}
}

void CMenuTab::update(int i, int& t, const Rect_t& windowpos)
{
	if (input.in_area(tab.x, tab.y, 100, 30) && input.is_pressed(VK_LBUTTON))
	{
		t = i;
		window_menu.opened_combobox = nullptr;
		window_menu.opened_multi_combobox = nullptr;
		window_menu.opened_colorpicker = nullptr;
	}

	if (t == i)
	{
		int evenx = windowpos.x + 107;
		int oddx = windowpos.x + 329;
		int eveny = windowpos.y + 25;
		int oddy = windowpos.y + 25;
		const int width = 217;
		for (size_t i = 0; i < groups.size(); i++)
		{
			const auto& group = groups.at(i);
			current_group = i;

			if (i % 2 == 0 || i == 0) // Even
			{
				group->set_position(evenx, eveny);
				group->set_id( i );
				int totalitems = 0;
				int totalheight = 0;
				int size = i == 0 ? ( windowpos.y + ( windowpos.h / 2 ) ) : ( windowpos.y + windowpos.h );
				for ( size_t i = 0; i < group->items.size( ); i++ )
				{
					auto type = group->items.at( i )->type;
					int yoffset =
						( type == Combobox || type == MultiCombobox || type == Keybind || type == ColorPicker || type == Textbox )
						? 32
						: ( ( type == SliderInt || type == SliderFloat ) 
							? 22 
							: ( type == Button || type == Checkbox || type == Space )
								? 20
								: ( type == Space )
									? 15
									: 0 );
					totalitems += yoffset;
				}
				totalheight = eveny + totalitems + 8 > ( i == 0 ? ( windowpos.y + ( windowpos.h / 2 ) ) : ( windowpos.y + windowpos.h ) ) ? totalitems + 8 : 25 + totalitems + 8;
				if ( i == 0 )
				{
					if ( eveny + totalheight > windowpos.y + ( windowpos.h / 2 ) )
					{
						totalheight -= ( eveny + totalitems + 8 ) - ( windowpos.y + ( windowpos.h / 2 ) ) + 5;
					}
				}
				if ( i == 2 )
				{
					if ( eveny + totalheight > windowpos.y + windowpos.h )
					{
						totalheight -= ( eveny + totalitems + 8 ) - ( windowpos.y + windowpos.h ) + 5;
					}
				}
				group->set_size(width, totalheight );
				eveny += totalheight + 5;

				int lowestpos = 0;
				for ( size_t i = 0; i < group->items.size( ); i++ )
				{
					lowestpos = group->items.at( i )->position.y + group->items.at( i )->position.h;
				}

				if ( input.in_area( group->position.x, group->position.y, group->position.w, group->position.h ) )
				{
					input.hovered = true;
					if ( input.scroll == 120 && group->offset < 0 )
					{
						group->offset += 20;
						input.scroll = 0;
					}
					else if ( input.scroll == -120 )
					{
						if ( i == 0 || i == 2 )
						{
							if ( lowestpos > size - 48 )
							{
								group->offset -= 20;
								input.scroll = 0;
							}
							else
							{
								input.scroll = 0;
							}
						}
					}
					else
					{
						input.scroll = 0;
					}
				}
			}
			else
			{
				group->set_position(oddx, oddy);
				group->set_id( i );
				int totalitems = 0;
				int totalheight = 0;
				int size = i == 1 ? ( windowpos.y + ( windowpos.h / 2 ) ) : ( windowpos.y + windowpos.h );
				for ( size_t i = 0; i < group->items.size( ); i++ )
				{
					auto type = group->items.at( i )->type;
					int yoffset =
						( type == Combobox || type == MultiCombobox || type == Keybind || type == ColorPicker || type == Textbox )
						? 32
						: ( ( type == SliderInt || type == SliderFloat )
							? 22
							: ( type == Button || type == Checkbox )
								? 20
								: ( type == Space )
									? 15
									: 0 );
					totalitems += yoffset;
				}
				totalheight = oddy + totalitems + 8 > ( i == 1 ? ( windowpos.y + ( windowpos.h / 2 ) ) : ( windowpos.y + windowpos.h ) ) ? totalitems + 8 : 25 + totalitems + 8;
				if ( i == 1 )
				{
					if ( oddy + totalitems + 8 > windowpos.y + ( windowpos.h / 2 ) )
					{
						totalheight -= ( oddy + totalitems + 8 ) - ( windowpos.y + ( windowpos.h / 2 ) ) + 5;
					}
				}
				if ( i == 3 )
				{
					if ( oddy + totalitems + 8 > windowpos.y + windowpos.h )
					{
						totalheight -= ( oddy + totalitems + 8 ) - ( windowpos.y + windowpos.h ) + 5;
					}
				}
				group->set_size(width, totalheight );
				oddy += totalheight + 5;

				int lowestpos = 0;
				for ( size_t i = 0; i < group->items.size( ); i++ )
				{
					lowestpos = group->items.at( i )->position.y + group->items.at( i )->position.h;
				}

				if ( input.in_area( group->position.x, group->position.y, group->position.w, group->position.h ) )
				{
					input.hovered = true;
					if ( input.scroll == 120 && group->offset < 0 )
					{
						group->offset += 20;
						input.scroll = 0;
					}
					else if ( input.scroll == -120 )
					{
						if ( i == 1 || i == 3 )
						{
							if ( lowestpos > size - 48 )
							{
								group->offset -= 20;
								input.scroll = 0;
							}
							else
							{
								input.scroll = 0;
							}
						}
					}
					else
					{
						input.scroll = 0;
					}
				}
			}

			group->update();
		}

		if (window_menu.opened_combobox)
		{
			window_menu.opened_combobox->UpdateCombo();
		}
		if (window_menu.opened_multi_combobox)
		{
			window_menu.opened_multi_combobox->UpdateCombo();
		}
	}
}

struct Pos_t
{
	int x;
	int y;
};

void CMenuGroupbox::render()
{
	draw.rect(position.x, position.y, position.w, position.h, vars.colors.menu.menu_background);
	draw.line( position.x, position.y + 18, position.x + position.w, position.y + 18, vars.colors.menu.menu_accent );
	//draw.outlined_rect(m_Position.x, m_Position.y, m_Position.w, 20, vars.colors.testmenu.Menu_Accent);
	draw.outlined_rect(position.x, position.y, position.w, position.h, vars.colors.menu.menu_accent);
	draw.outlined_string(MENU, position.x + 5, position.y + 3, { 255, 255, 255, 255}, ALIGN_DEFAULT, this->get_name());
	//draw.outlined_string(MENU, position.x + position.w, position.y + 3, { 255, 255, 255, 255}, ALIGN_REVERSE, "%i", position.h );

	for (const auto& item : items)
	{
		int backupLeft, backupTop, backupRight, backupBottom; bool backupClippingDisabled;
		i::surface->GetClippingRect( backupLeft, backupTop, backupRight, backupBottom, backupClippingDisabled );

		i::surface->DisableClipping( false );
		i::surface->SetClippingRect( position.x, position.y + 19, position.x + position.w, position.y + position.h - 1 );
		item->render();
		i::surface->DisableClipping( backupClippingDisabled );
		i::surface->SetClippingRect( backupLeft, backupTop, backupRight, backupBottom );
	}

	/*if ( m_Position.h > 41 )
	{
		const int leftx = m_Position.x + 7;
		const int topy = m_Position.y + 26;
		const int rightx = (m_Position.x + m_Position.w) - 8;
		const int bottomy = (m_Position.y + m_Position.h) - 8;

		draw.rect(leftx, topy, 8, 1, vars.colors.testmenu.Menu_Clear);
		draw.rect(leftx, topy, 1, 8, vars.colors.testmenu.Menu_Clear);

		draw.rect(leftx, bottomy, 8, 1, vars.colors.testmenu.Menu_Clear);
		draw.rect(leftx, bottomy - 7, 1, 8, vars.colors.testmenu.Menu_Clear);

		draw.rect(rightx - 7, topy, 8, 1, vars.colors.testmenu.Menu_Clear);
		draw.rect(rightx, topy, 1, 8, vars.colors.testmenu.Menu_Clear);

		draw.rect(rightx - 7, bottomy, 8, 1, vars.colors.testmenu.Menu_Clear);
		draw.rect(rightx, bottomy - 7, 1, 8, vars.colors.testmenu.Menu_Clear);
	}*/

}

void CMenuGroupbox::update()
{
	int x = position.x + 7;
	int y = ( position.y + 25 ) + offset;
	for (const auto& item : items)
	{
		item->update( x, y );

		if ( item->type == SliderInt || item->type == SliderFloat )
		{
			y += 22;
		}
		else if ( item->type == Combobox || item->type == MultiCombobox || item->type == Keybind || item->type == ColorPicker || item->type == Textbox )
		{
			y += 32;
		}
		else if ( item->type == Button || item->type == Checkbox )
		{
			y += 20;
		}
		else if ( item->type == Space )
		{
			y += 15;
		}
		else
		{
			y += 0;
		}
	}
}

void CItemSpace::render( )
{
	draw.outlined_string(MENU, position.x + 100, position.y - 3, { 255, 255, 255, 255 }, ALIGN_CENTERHORIZONTAL, this->get_name());
	if ( line )
	{
		draw.line( position.x - 6, position.y + 13, position.x + 210, position.y + 13, vars.colors.menu.menu_accent );
	}
}

bool CItemSpace::update(int x, int y)
{
	this->position = { x + override_x, y + override_y, 0, 0 };

	return false;
}

void CItemCheckbox::render()
{
	draw.outlined_string(MENU, position.x + 21, position.y + 9, { 255, 255, 255, 255 }, ALIGN_CENTERVERTICAL, this->get_name());

	int topx = position.x;
	int topy = position.y + 3;

	if (input.in_area(topx, topy, 14, 14))
	{
		draw.rect(topx, topy, 14, 14, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 14, 14, vars.colors.menu.menu_background);
	}
	draw.outlined_rect(topx, topy, 14, 14, vars.colors.menu.menu_accent );

	if (*this->var)
	{
		draw.rect( topx + 1, topy + 1, 12, 12, vars.colors.menu.menu_selected );

		/*draw.line( topx + 3, topy + 2, topx + 12, topy + 11, vars.colors.menu.menu_accent );
		draw.line( topx + 3, topy + 3, topx + 11, topy + 11, vars.colors.menu.menu_accent );
		draw.line( topx + 2, topy + 3, topx + 11, topy + 12, vars.colors.menu.menu_accent );

		draw.line( topx + 2, topy + 10, topx + 11, topy + 1, vars.colors.menu.menu_accent );
		draw.line( topx + 3, topy + 10, topx + 11, topy + 2, vars.colors.menu.menu_accent );
		draw.line( topx + 3, topy + 11, topx + 12, topy + 2, vars.colors.menu.menu_accent );*/
	}
}

bool CItemCheckbox::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (input.in_area(position.x, position.y + 3, 14, 14) && position.y < group->position.y + group->position.h )
	{
		if (input.is_pressed(VK_LBUTTON))
		{
			*this->var = !(*this->var);
			callback = true;
		}
	}

	return callback;
}

void CItemKeybind::render()
{
	draw.outlined_string(MENU, position.x + 20, position.y, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	if (input.in_area(topx, topy, 163, 16))
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_groupbox);
	}
	draw.outlined_rect(topx, topy, 163, 16, vars.colors.menu.menu_accent);

	if (!this->active)
	{
		const auto label = CItemKeybind::key_code_to_string(*var);
		draw.outlined_string(MENU, topx + ( 163 / 2), topy + 7, { 255, 255, 255, 255},ALIGN_CENTER, label.c_str());
	}
	else
	{
		draw.outlined_string(MENU, topx + ( 163 / 2), topy + 7, { 255, 255, 255, 255},ALIGN_CENTER, "Press key");
	}
}

bool CItemKeybind::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (input.in_area(topx, position.y + 15, 163, 16) )
	{
		if (input.is_pressed(VK_LBUTTON) && position.y < group->position.y + group->position.h )
		{
			this->active = true;
			callback = true;
			time_pressed = i::engine_client->Time();
		}
	}

	auto time = i::engine_client->Time();

	if (this->active && time > time_pressed + 0.1f)
	{
		input.prevent_inputs = false;
		if (input.is_pressed(VK_ESCAPE))
		{
			*var = 0x0;
			this->active = false;
			time_pressed = 0.0f;
			return callback;
		}

		if ( input.is_pressed( VK_LBUTTON ) && !input.in_area( topx, position.y + 15, 163, 16 ) )
		{
			this->active = false;
			time_pressed = 0.0f;
			return callback;
		}

		for (int i = 0; i < 256; i++)
		{
			if (blocked_keys.size())
			{
				if (std::find(blocked_keys.begin(), blocked_keys.end(), i) != blocked_keys.end())
				{
					continue;
				}
			}

			if (input.is_pressed(i) && input.in_area( topx, position.y + 15, 163, 16 ) )
			{
				callback = true;
				*var = i;
				active = false;
				time_pressed = 0.0f;
				break;
			}
		}
	}

	if (this->active)
	{
		input.prevent_inputs = true;
	}

	return callback;
}

void CItemButton::render()
{
	int topx = position.x + 20;
	int topy = position.y + 3;

	if (this->time_pressed != 0.0f)
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_selected);
	}
	else if (input.in_area(topx, topy, 163, 16))
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_groupbox);
	}
	draw.outlined_rect(topx, topy, 163, 16, vars.colors.menu.menu_accent );

	draw.outlined_string( MENU, topx + ( 163 / 2 ), topy + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, this->get_name( ) );
}

bool CItemButton::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (input.in_area(topx, position.y + 3, 163, 16) && position.y < group->position.y + group->position.h )
	{
		auto time = i::engine_client->Time();
		bool pressed = input.is_pressed(VK_LBUTTON);
		if (pressed)
		{
			if (this->time_pressed == 0.0f)
			{
				callback_func();
				callback = true;
				time_pressed = time;
			}
		}
		else if (time > this->time_pressed + 0.1f)
		{
			time_pressed = 0.0f;
		}
	}
	else
	{
		time_pressed = 0.0f;
	}

	return callback;
}


void CItemSliderInt::render()
{
	draw.outlined_string(MENU, position.x + 21, position.y + 1, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	bool accent = false;

	if (input.in_area(topx, topy, 163, 6))
	{
		draw.rect(topx, topy, 163, 6, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 6, vars.colors.menu.menu_background);
	}

	auto fraction = math.remap_val_clamped(*var, var_min, var_max, 0.0f, 1.0f);

	int sliderx = topx + (fraction * 163) + 1;
	if (sliderx > topx + 162)
	{
		sliderx = topx + 162;
	}

	int w = sliderx - topx;
	//draw.rect( topx, topy + 1, 3, 12, { 0,0,0,255 });
	draw.outlined_rect( topx, topy, 163, 6, vars.colors.menu.menu_widget );
	draw.rect( topx + 1, topy + 1, w - 1, 4, vars.colors.menu.menu_selected );
	draw.outlined_rect( topx + 1, topy, w, 6, vars.colors.menu.menu_accent );


	draw.outlined_string(MENU, topx + 164, position.y + 1, { 255, 255, 255, 255 }, ALIGN_REVERSE, this->format, *this->var);
}

bool CItemSliderInt::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;
	int topy = position.y + 15;

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (input.in_area(topx, topy, 163, 6) && position.y < group->position.y + group->position.h )
	{
		if (input.is_held(VK_LBUTTON))
		{
			float fraction = (float)(input.mouse_x - topx) / 163;
			fraction = std::clamp(fraction, 0.0f, 1.0f);

			int newval = var_min + (int)(fraction * (var_max - var_min));
			if (newval != *var)
			{
				*var = newval;
				callback = true;
			}
		}
	}
	auto fraction = math.remap_val_clamped( *var, var_min, var_max, 0.0f, 1.0f );

	int sliderx = topx + ( fraction * 163 ) + 1;
	if ( sliderx > topx + 162 )
	{
		sliderx = topx + 162;
	}

	int w = sliderx - topx;

	if (input.in_area( topx, topy, w, 6 ) && position.y < group->position.y + group->position.h )
	{
		if (input.is_pressed(VK_RBUTTON))
		{
			callback = true;
			*var = std::clamp(*var - step, var_min, var_max);
		}
	}
	else if ( input.in_area( topx + w, topy, 163 - w, 6 ) )
	{
		if (input.is_pressed(VK_RBUTTON))
		{
			callback = true;
			*var = std::clamp(*var + step, var_min, var_max);
		}
	}

	return callback;
}

void CItemSliderFloat::render()
{
	draw.outlined_string(MENU, position.x + 21, position.y + 1, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	bool accent = false;

	if (input.in_area(topx, topy, 163, 6))
	{
		draw.rect(topx, topy, 163, 6, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 6, vars.colors.menu.menu_background );
	}

	auto fraction = math.remap_val_clamped(*var, var_min, var_max, 0.0f, 1.0f);

	int sliderx = topx + (fraction * 163) + 1;
	if (sliderx > topx + 162)
	{
		sliderx = topx + 162;
	}

	int w = sliderx - topx;
	//draw.rect(sliderx, topy + 1, 3, 12, { 0,0,0,255 });
	draw.outlined_rect( topx, topy, 163, 6, vars.colors.menu.menu_widget );
	draw.rect( topx + 1, topy + 1, w - 1, 4, vars.colors.menu.menu_selected );
	draw.outlined_rect( topx + 1, topy, w, 6, vars.colors.menu.menu_accent );

	draw.outlined_string(MENU, topx + 164, position.y + 1, { 255, 255, 255, 255}, ALIGN_REVERSE, this->format, *this->var);
}

bool CItemSliderFloat::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;
	int topy = position.y + 15;

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (input.in_area(topx, topy, 163, 6) && position.y < group->position.y + group->position.h )
	{
		if (input.is_held(VK_LBUTTON))
		{
			float fraction = (float)(input.mouse_x - topx) / 163;
			fraction = std::clamp(fraction, 0.0f, 1.0f);

			float newval = var_min + (fraction * (var_max - var_min));
			if (newval != *var)
			{
				*var = newval;
				callback = true;
			}
		}
	}
	auto fraction = math.remap_val_clamped( *var, var_min, var_max, 0.0f, 1.0f );

	int sliderx = topx + ( fraction * 163 ) + 1;
	if ( sliderx > topx + 162 )
	{
		sliderx = topx + 162;
	}

	int w = sliderx - topx;
	if ( input.in_area( topx, topy, w, 6 ) && position.y < group->position.y + group->position.h )
	{
		if (input.is_pressed(VK_RBUTTON))
		{
			callback = true;
			*var = std::clamp(*var - step, var_min, var_max);
		}
	}
	else if ( input.in_area( topx + w, topy, 163 - w, 6 ) )
	{
		if (input.is_pressed(VK_RBUTTON))
		{
			callback = true;
			*var = std::clamp(*var + step, var_min, var_max);
		}
	}

	return callback;
}

void CItemTextbox::render()
{
	draw.outlined_string(MENU, position.x + 20, position.y + 1, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	if (input.in_area(topx, topy, 163, 16))
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_groupbox);
	}

	draw.outlined_rect(topx, topy, 163, 16, vars.colors.menu.menu_accent);

	const auto str = this->input_string->empty() && typing ? "..." : this->input_string->c_str();
	/*
	int backupLeft, backupTop, backupRight, backupBottom; bool backupClippingDisabled;
	i::surface->GetClippingRect(backupLeft, backupTop, backupRight, backupBottom, backupClippingDisabled);

	i::surface->DisableClipping(false);
	i::surface->SetClippingRect(topx + 1, topy, topx + 161, topy + 16);
	*/
	draw.outlined_string(MENU, topx + (163 / 2), topy + 7, { 255, 255, 255, 255 },ALIGN_CENTER, str);
	/*
	i::surface->DisableClipping(backupClippingDisabled);
	i::surface->SetClippingRect(backupLeft, backupTop, backupRight, backupBottom);*/
}

bool CItemTextbox::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;
	int topy = position.y + 15;

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	bool inarea = input.in_area(topx, topy, 163, 16);
	bool clicked = input.is_pressed(VK_LBUTTON);
	if (!typing)
	{
		if (inarea && position.y < group->position.y + group->position.h )
		{
			typing = clicked;
			input.key_pressed = 0x0;
		}
		else if (clicked)
		{
			typing = false;
		}
	}

	if (clicked && !inarea && typing)
	{
		typing = false;
	}

	if (typing)
	{
		if (input.key_pressed)
		{
			if (blocked_chars.size())
			{
				if (std::find(blocked_chars.begin(), blocked_chars.end(), input.key_pressed) != blocked_chars.end())
				{
				}
				else
				{
					if (input_string->length() < max_length)
					{
						if ((input.key_pressed >= '!' && input.key_pressed <= '~'))
						{
							input_string->push_back(input.key_pressed);
						}
					}
				}
			}

			input.key_pressed = 0x0;
		}
		if (input.is_pressed(VK_SPACE))
		{
			if (input_string->length() < max_length)
			{
				input_string->push_back(' ');
			}
		}

		bool isheld = input.is_held(VK_BACK);
		bool pressed = input.is_pressed(VK_BACK);
		auto time = i::engine_client->Time();
		auto delay = (pressed && !isheld) ? 0.3 : 0.1;
		if (time > remove_time + delay)
		{
			remove_time = time;
			if (pressed && !isheld)
			{

				if (!input_string->empty())
				{
					input_string->pop_back();
				}
			}
			else if (isheld)
			{
				if (!input_string->empty())
				{
					input_string->pop_back();
				}
			}
		}

		if (input.is_pressed(VK_RETURN) || input.is_pressed(VK_ESCAPE))
		{
			callback = true;
			typing = false;
			remove_time = 0.0f;
		}
	}
	else
	{
		remove_time = 0.0f;
	}

	return callback;
}

void CItemCombobox::render()
{
	draw.outlined_string(MENU, position.x + 20, position.y + 1, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	if (input.in_area(topx, topy, 163, 16))
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_groupbox);
	}
	draw.outlined_rect(topx, topy, 163, 16, vars.colors.menu.menu_accent);

	int cur_var = *var;

	auto str = ( cur_var < 0 || cur_var > combo_items.size()) ? "Unknown" : combo_items.at( cur_var );

	draw.outlined_string(MENU, topx + ( 163 / 2), topy + 7, { 255, 255, 255, 255 }, ALIGN_CENTER, str);
}

bool CItemCombobox::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;
	int topy = position.y + 15;

	bool pressed = input.is_pressed(VK_LBUTTON);

	if (open && window_menu.opened_combobox != this)
	{
		open = false;
	}

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (!open && input.in_area(topx, position.y + 15, 163, 16) && position.y < group->position.y + group->position.h )
	{
		auto time = i::engine_client->Time();

		if (pressed)
		{
			if (this->time_pressed == 0.0f)
			{
				open = true;
				callback = true;
				time_pressed = time;
				window_menu.opened_combobox = this;
			}
		}
		else if (time > this->time_pressed + 0.1f)
		{
			time_pressed = 0.0f;
		}
	}

	if (open)
	{
		input.prevent_inputs = true;
	}

	if (!open && window_menu.opened_combobox == this)
	{
		input.prevent_inputs = false;
		window_menu.opened_combobox = nullptr;
	}

	return callback;
}

void CItemCombobox::RenderCombo()
{
	if (!open)
	{
		return;
	}

	int topx = position.x + 20;
	int topy = position.y + 15;

	for (size_t i = 1; i < combo_items.size() + 1; i++)
	{
		const auto& it = combo_items.at(i - 1);

		int addedheight = (i * 15) + 1;

		if (*var == i - 1)
		{
			draw.rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_selected);
		}
		else if (input.in_area(topx, topy + addedheight, 163, 16))
		{
			draw.rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_hovered);
		}
		else
		{
			draw.rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_groupbox);
		}

		draw.outlined_rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_widget);

		draw.outlined_string(MENU, topx + 82, topy + addedheight + 7, { 255, 255, 255, 255},ALIGN_CENTER, it);
	}
}

void CItemCombobox::UpdateCombo()
{
	if (!open)
	{
		return;
	}

	int topx = position.x + 20;
	int topy = position.y + 15;

	input.prevent_inputs = false;
	if (open && !input.in_area(topx, position.y + 15, 163, 16 + (16 * combo_items.size())))
	{
		if (input.is_pressed(VK_LBUTTON))
		{
			open = false;
			time_pressed = 0.0f;
			return;
		}
	}
	input.prevent_inputs = true;

	for (size_t i = 1; i < combo_items.size() + 1; i++)
	{
		const auto& it = combo_items.at(i - 1);

		int addedheight = (i * 15) + 1;

		if (input.in_area(topx, topy + addedheight, 163, 16))
		{
			input.prevent_inputs = false;
			if (input.is_pressed(VK_LBUTTON))
			{
				*this->var = i - 1;
				time_pressed = i::engine_client->Time();
			}
			input.prevent_inputs = true;
		}
	}
}

#include <string>
#include <iostream>
#include "../../utils/utils.h"

void CItemMultiCombobox::render()
{
	draw.outlined_string(MENU, position.x + 20, position.y + 1, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	if (input.in_area(topx, topy, 163, 16))
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_hovered);
	}
	else
	{
		draw.rect(topx, topy, 163, 16, vars.colors.menu.menu_groupbox);
	}
	draw.outlined_rect(topx, topy, 163, 16, vars.colors.menu.menu_accent);

	std::string str;
	for (const auto& item : combo_items)
	{
		if (*item.second)
		{
			str = (str + item.first + ", ");
		}
	}
	if (str.empty())
	{
		str = "None";
	}
	else
	{
		str.pop_back();
		str.pop_back();
	}


	/*int backupLeft, backupTop, backupRight, backupBottom; bool backupClippingDisabled;
	i::surface->GetClippingRect(backupLeft, backupTop, backupRight, backupBottom, backupClippingDisabled);

	i::surface->DisableClipping(false);
	i::surface->SetClippingRect(topx + 1, topy, topx + 161, topy + 14);*/
	int w, h;
	draw.get_text_size( MENU, w, h, str.c_str( ) );
	bool align = w >= 163;
	if ( str.length( ) > 32 )
	{
		const int count = str.length( ) - ( 32 - 3 );
		str.erase( str.end( ) - ( int )count, str.end( ) );
		str.append( "..." );
	}
	draw.outlined_string(MENU, align ? topx + 5 : topx + (163 / 2), topy + 7, { 255, 255, 255, 255}, align ? ALIGN_CENTERVERTICAL : ALIGN_CENTER, str.c_str());

	/*i::surface->DisableClipping( backupClippingDisabled );
	i::surface->SetClippingRect(backupLeft, backupTop, backupRight, backupBottom);*/
}

bool CItemMultiCombobox::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;
	int topy = position.y + 15;

	bool pressed = input.is_pressed(VK_LBUTTON);

	if (open && window_menu.opened_multi_combobox != this)
	{
		open = false;
	}

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (!open && input.in_area(topx, position.y + 15, 163, 16) && position.y < group->position.y + group->position.h )
	{
		auto time = i::engine_client->Time();

		if (pressed)
		{
			if (this->time_pressed == 0.0f)
			{
				open = true;
				callback = true;
				time_pressed = time;
				window_menu.opened_multi_combobox = this;
			}
		}
		else if (time > this->time_pressed + 0.1f)
		{
			time_pressed = 0.0f;
		}
	}

	if (open)
	{
		input.prevent_inputs = true;
	}

	if (!open && window_menu.opened_multi_combobox == this)
	{
		input.prevent_inputs = false;
		window_menu.opened_multi_combobox = nullptr;
	}

	return callback;
}

void CItemMultiCombobox::RenderCombo()
{
	if (!open)
	{
		return;
	}

	int topx = position.x + 20;
	int topy = position.y + 15 + 15;

	for (size_t i = 0; i < combo_items.size(); i++)
	{
		const auto& it = combo_items.at(i);

		int addedheight = (i * 15) + 1;

		if (*it.second)
		{
			draw.rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_selected);
		}
		else if (input.in_area(topx, topy + addedheight, 129, 16 ))
		{
			draw.rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_hovered);
		}
		else
		{
			draw.rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_groupbox);
		}

		draw.outlined_rect(topx, topy + addedheight, 163, 16, vars.colors.menu.menu_widget);

		draw.outlined_string(MENU, topx + 5, topy + addedheight + 7, { 255, 255, 255, 255 }, ALIGN_CENTERVERTICAL, it.first);
	}
}

void CItemMultiCombobox::UpdateCombo()
{
	if (!open)
	{
		return;
	}

	int topx = position.x + 20;
	int topy = position.y + 15 + 16;

	input.prevent_inputs = false;
	if (open && !input.in_area(topx, position.y + 15, 163, 16 + (16 * combo_items.size())))
	{
		if (input.is_pressed(VK_LBUTTON))
		{
			open = false;
			time_pressed = 0.0f;
			return;
		}
	}
	input.prevent_inputs = true;

	for (size_t i = 0; i < combo_items.size(); i++)
	{
		const auto& it = combo_items.at(i);

		int addedheight = (i * 15) + 1;

		if (input.in_area(topx, topy + addedheight, 163, 16))
		{
			input.prevent_inputs = false;
			if (input.is_pressed(VK_LBUTTON))
			{
				*it.second = !*it.second;
				time_pressed = i::engine_client->Time();
			}
			input.prevent_inputs = true;
		}
	}
}

void CItemColorPicker::render()
{
	draw.outlined_string(MENU, position.x + 20, position.y + 1, { 255, 255, 255, 255 }, ALIGN_DEFAULT, this->get_name());

	int topx = position.x + 20;
	int topy = position.y + 15;

	if (input.in_area(topx, topy, 163, 16) && window_menu.opened_colorpicker != nullptr)
	{
		//draw.rect(topx, topy, 163, 16, vars.colors.testmenu.Menu_Hovered);
	}
	else
	{
		draw.rect(topx + 163 - 30, topy, 30, 16, *this->color);
		draw.outlined_rect( topx + 163 - 30, topy, 30, 16, vars.colors.menu.menu_accent );
	}
	draw.outlined_rect(topx, topy, 163 - 36, 16, vars.colors.menu.menu_accent);
	//RenderPicker();
}

bool CItemColorPicker::update(int x, int y)
{
	bool callback = false;

	this->position = { x, y, 0, 0 };

	int topx = position.x + 20;
	int topy = position.y + 15;

	bool pressed = input.is_pressed(VK_LBUTTON);

	if (open && window_menu.opened_colorpicker != this)
	{
		open = false;
		window_menu.colorpicker_opened = false;
	}

	const auto& group = window->tabs.at( window->current_tab )->groups.at( window->tabs.at( window->current_tab )->current_group );
	if (!open && input.in_area(topx, position.y + 15, 163 - 36, 16) && position.y < group->position.y + group->position.h )
	{
		auto time = i::engine_client->Time();

		if (pressed)
		{
			if (this->time_pressed == 0.0f)
			{
				open = true;
				window_menu.colorpicker_opened = true;
				callback = true;
				time_pressed = time;
				window_menu.opened_colorpicker = this;
			}
		}
		else if (time > this->time_pressed + 0.1f)
		{
			time_pressed = 0.0f;
		}
	}

	if (open)
	{
		input.prevent_inputs = true;
	}

	if (!open && window_menu.opened_colorpicker == this)
	{
		input.prevent_inputs = false;
		window_menu.opened_colorpicker = nullptr;
	}

	return callback;
}

void CItemColorPicker::RenderPicker()
{
	if (!open)
	{
		return;
	}

	input.prevent_inputs = false;

	int backupLeft, backupTop, backupRight, backupBottom; bool backupClippingDisabled;
	i::surface->GetClippingRect(backupLeft, backupTop, backupRight, backupBottom, backupClippingDisabled);

	i::surface->DisableClipping(false);
	i::surface->SetClippingRect(0, 0, draw.screen_width, draw.screen_height);

	draw.rect(0, 0, draw.screen_width, draw.screen_height, { 50,50,50,100 });
	draw.outlined_string(MENU, draw.screen_width / 2, draw.screen_height / 5, { 255, 255, 255, 255},ALIGN_CENTER, "Click anywhere to exit");

	int centerw = 291;
	int centerh = 185;
	int centerx = (draw.screen_width / 2) - (centerw / 2);
	int centery = (draw.screen_height / 2) - (centerh / 2);
	centerh += 15;

	if (open && !input.in_area(centerx, centery, centerw, centerh))
	{
		if (input.is_pressed(VK_LBUTTON) && i::engine_client->Time() > time_pressed + 0.3f)
		{
			open = false;
			window_menu.colorpicker_opened = false;
			time_pressed = 0.0f;
			input.prevent_inputs = false;
			return;
		}
	}

	CLR2HSV( *color, hsv );
	auto& square = *color;

	//draw.rect(0, 0, 300, 300, square);

	centerw = 291;
	centerh = 185;
	centerx = (draw.screen_width / 2) - (centerw / 2);
	centery = (draw.screen_height / 2) - (centerh / 2);
	centerh += 15;

	draw.rect(centerx, centery, centerw, centerh, vars.colors.menu.menu_groupbox);
	draw.outlined_rect(centerx, centery, centerw, centerh, vars.colors.menu.menu_accent);
	draw.outlined_rect(centerx, centery, centerw, 20, vars.colors.menu.menu_accent);
	draw.outlined_string(MENU, centerx + (centerw / 2), centery + 10, { 255, 255, 255, 255},ALIGN_CENTER, this->get_name());

	centerw = 250;
	centerh = 176;
	centerx = (draw.screen_width / 2) - (centerw / 2) - 15;
	centery = (draw.screen_height / 2) - (centerh / 2) + 20;

	const auto squareclr = HSVtoColor(hsv[0], 100, 100);

	square.r = squareclr.r;
	square.g = squareclr.g;
	square.b = squareclr.b;

	{
		// Main color picker part
		i::surface->DrawSetColor({ 255, 255, 255, 255 });
		i::surface->DrawFilledRectFade(centerx + 10, centery + 10, centerx + 120, centery + 130, 255, 0, true);
		i::surface->DrawSetColor({ square.r, square.g, square.b, 255 });
		i::surface->DrawFilledRectFade(centerx + 10, centery + 10, centerx + 130, centery + 130, 0, 255, true);
		i::surface->DrawFilledRectFade(centerx + 10, centery + 10, centerx + 130, centery + 130, 0, 128, true);
		i::surface->DrawSetColor({ 0, 0, 0, 255 });
		i::surface->DrawFilledRectFade(centerx + 10, centery + 10, centerx + 130, centery + 130, 0, 255, false);
		i::surface->DrawFilledRectFade(centerx + 10, centery + 10, centerx + 130, centery + 130, 0, 255, false);
		i::surface->DrawFilledRectFade(centerx + 10, centery + 10, centerx + 130, centery + 130, 0, 128, false);
		draw.outlined_rect(centerx + 9, centery + 9, 121, 121, vars.colors.menu.menu_accent);

		vector_2d picker;

		int width = 119;
		int height = 119;
		const float ratiox = width / 100.f;
		const float ratioy = height / 100.f;
		picker.x = hsv[1] * ratiox;
		picker.y = 120.f - (hsv[2] * ratioy);
		draw.outlined_rect(centerx + 10 + picker.x - 1, centery + 10 + picker.y - 1, 4, 4, { 0, 0, 0, 255 });

		if (input.in_area(centerx + 10, centery + 10, 120, 120))
		{
			if (input.is_held(VK_LBUTTON))
			{
				picker.x = std::clamp(input.mouse_x, (centerx + 10), (centerx + 120));
				picker.y = std::clamp(input.mouse_y, (centery + 10), (centery + 120));
				hsv[1] = ((((float)input.mouse_x - (float)(centerx + 10)) / ((float)(120))) * (100.f));
				hsv[2] = 100.f - ((((float)input.mouse_y - (float)(centery + 10)) / ((float)(120))) * (100.f));
			}
			else if (input.is_pressed_and_held(VK_LBUTTON))
			{
				picker.x = std::clamp(input.mouse_x, (centerx + 10), (centerx + 120));
				picker.y = std::clamp(input.mouse_y, (centery + 10), (centery + 120));
				hsv[1] = ((((float)input.mouse_x - (float)(centerx + 10)) / ((float)(120))) * (100.f));
				hsv[2] = 100.f - ((((float)input.mouse_y - (float)(centery + 10)) / ((float)(120))) * (100.f));
			}
		}

	}

	auto gradient = [](int x, int y, int w, int h, Color top_clr, Color bottom_clr, bool horizontal)
	{
		i::surface->DrawSetColor(top_clr.r, top_clr.g, top_clr.b, top_clr.a);
		i::surface->DrawFilledRectFade(x, y, x + w, y + h, 255, 255, horizontal);
		i::surface->DrawSetColor(bottom_clr.r, bottom_clr.g, bottom_clr.b, bottom_clr.a);
		i::surface->DrawFilledRectFade(x, y, x + w, y + h, 0, 255, horizontal);
	};
	
	{
		// Hue bit
		static Color hueColors[7] =
		{
			{ 255, 0, 0, 255 },
			{ 255, 200, 0, 255 },
			{ 0, 255, 0, 255 },
			{ 0, 255, 255, 255 },
			{ 0, 0, 255, 255 },
			{ 255, 0, 255, 255 },
			{ 255, 0, 0, 255 }
		};

		for (auto i = 0; i < 6; i++)
		{
			gradient(centerx + 144, (centery + 10) + i * (20.f), 12, 20, hueColors[i], hueColors[i + 1], false);
		}
		draw.outlined_rect(centerx + 143, centery + 9, 14, 121, vars.colors.menu.menu_accent);

		int height = 119;
		const float ratio = height / 359.f;
		float hueslidery = hsv[0] * ratio;

		//_x + 9, _y + 9, 121

		draw.outlined_rect(centerx + 143, centery + 9 + hueslidery, 14, 3, { 0,0,0, 255 });
		draw.outlined_rect(centerx + 144, centery + 10 + hueslidery, 12, 1, { 255,255,255, 255 } );
		if (input.in_area(centerx + 144, centery + 10, 12, 120))
		{
			if (input.is_held(VK_LBUTTON))
			{
				hueslidery = std::clamp(input.mouse_y, (centery + 10), (centery + 120));
				hsv[0] = ((((float)input.mouse_y - (float)(centery + 10)) / ((float)(120))) * (359.f));
			}
			else if (input.is_pressed_and_held(VK_LBUTTON))
			{
				hueslidery = std::clamp(input.mouse_y, (centery + 10), (centery + 120));
				hsv[0] = ((((float)input.mouse_y - (float)(centery + 10)) / ((float)(120))) * (359.f));
			}
		}
	}
	{
		i::surface->DrawSetColor({ 255, 255, 255, 255 });
		i::surface->DrawFilledRectFade(centerx + 10, centery + 145, centerx + 129, centery + 157, 0, 255, true);
		i::surface->DrawSetColor({ 0, 0, 0, 255 });
		i::surface->DrawFilledRectFade(centerx + 10, centery + 145, centerx + 129, centery + 157, 255, 0, true);
		draw.outlined_rect(centerx + 9, centery + 144, 121, 14, vars.colors.menu.menu_accent);
		auto width = 119;
		auto size = 255.f;
		const float ratio = width / size;
		float alphasliderx = ((float)square.a) * ratio;
		draw.outlined_rect(centerx + 9 + alphasliderx, centery + 144, 3, 14, { 0, 0, 0, 255 });
		draw.outlined_rect(centerx + 10 + alphasliderx, centery + 145, 1, 12, { 255,255,255, 255 });
		if (input.in_area(centerx + 10, centery + 145, 120, 10))
		{
			if (input.is_held(VK_LBUTTON))
			{
				alphasliderx = std::clamp(input.mouse_x, (centerx + 10), (centerx + 120));
				square.a = ((((float)input.mouse_x - (float)(centerx + 10)) / ((float)(120))) * (255.f));
			}
			else if (input.is_pressed_and_held(VK_LBUTTON))
			{
				alphasliderx = std::clamp(input.mouse_x, (centerx + 10), (centerx + 120));
				square.a = ((((float)input.mouse_x - (float)(centerx + 10)) / ((float)(120))) * (255.f));
			}
		}
	}
	

	const auto squareclr2 = HSVtoColor(hsv[0], hsv[1], hsv[2]);

	square.r = squareclr2.r;
	square.g = squareclr2.g;
	square.b = squareclr2.b;

	input.prevent_inputs = true;
}
