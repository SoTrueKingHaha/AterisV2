#include "outlines.h"

bool __outlines::ShouldRun( )
{
	if ( i::engine_vgui->IsGameUIVisible( ) || !i::engine_client->IsConnected( ) || !i::engine_client->IsInGame( ) )
		return false;

	return true;
}

void __outlines::DrawModel( C_BaseEntity* pEntity, int nFlags, bool bIsDrawingModels )
{
	m_bRendering = true;

	if ( !bIsDrawingModels )
		m_bDrawingGlow = true;

	pEntity->DrawModel( nFlags );

	if ( bIsDrawingModels )
		m_DrawnEntities[ pEntity ] = true;

	if ( !bIsDrawingModels )
		m_bDrawingGlow = false;

	m_bRendering = false;
}

void __outlines::SetScale( int nScale, bool bReset = false )
{
	static IMaterialVar* pVar = nullptr;
	static IMaterialVar* pVarWf = nullptr;
	static bool bFound = false;
	static bool bFoundWf = false;

	if ( bReset ) { pVar = nullptr; bFound = false; return; }

	if ( !bFound && m_pMatBlurY )
	{
		pVar = m_pMatBlurY->FindVar( ( "$bloomamount" ), &bFound );
	}

	else if ( pVar )
	{
		pVar->SetIntValue( nScale );
	}

	if ( !bFoundWf && m_pMatBlurYwf )
	{
		pVarWf = m_pMatBlurYwf->FindVar( ( "$bloomamount" ), &bFoundWf );
	}

	else if ( pVarWf )
	{
		pVarWf->SetIntValue( nScale );
	}
}

void __outlines::init( )
{
	//CreateMaterials( );
	m_pMatGlowColor = i::material_system->FindMaterial( ( "dev/glow_color" ), "Other textures" );
	m_pMatGlowColor->IncrementReferenceCount( );

	m_pRtFullFrame = i::material_system->FindTexture( ( "_rt_FullFrameFB" ), "RenderTargets" );
	m_pRtFullFrame->IncrementReferenceCount( );

	m_pRenderBuffer1 = i::material_system->CreateNamedRenderTargetTextureEx(
		( "glow_buffer_1" ), m_pRtFullFrame->GetActualWidth( ), m_pRtFullFrame->GetActualHeight( ),
		RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, 0x1 );
	m_pRenderBuffer1->IncrementReferenceCount( );

	// This can cause a crash on inject for some reason

	m_pRenderBuffer2 = i::material_system->CreateNamedRenderTargetTextureEx(
		( "glow_buffer_2" ), m_pRtFullFrame->GetActualWidth( ), m_pRtFullFrame->GetActualHeight( ),
		RT_SIZE_LITERAL, IMAGE_FORMAT_RGB888, MATERIAL_RT_DEPTH_SHARED,
		TEXTUREFLAGS_CLAMPS | TEXTUREFLAGS_CLAMPT | TEXTUREFLAGS_EIGHTBITALPHA, 0x1 );
	m_pRenderBuffer2->IncrementReferenceCount( );

	/*static auto m_pMatBlurXkv = new KeyValues( "BlurFilterX" );
	static auto m_pMatBlurXwfkv = new KeyValues( "BlurFilterX" );
	static auto m_pMatBlurYkv = new KeyValues( "BlurFilterY" );
	static auto m_pMatBlurYwfkv = new KeyValues( "BlurFilterY" );
	static auto m_pMatHaloAddToScreenkv = new KeyValues( "UnlitGeneric" );

	m_pMatBlurXkv->SetString( "$basetexture", "glow_buffer_1" );
	m_pMatBlurXwfkv->SetString( "$basetexture", "glow_buffer_1" );
	m_pMatBlurXwfkv->SetBool( "$wireframe", true );

	m_pMatBlurYkv->SetString( "$basetexture", "glow_buffer_2" );
	m_pMatBlurYwfkv->SetString( "$basetexture", "glow_buffer_2" );
	m_pMatBlurYwfkv->SetBool( "$wireframe", true );

	m_pMatHaloAddToScreenkv->SetString( "$basetexture", "glow_buffer_1" );
	m_pMatHaloAddToScreenkv->SetString( "$additive", "1" );

	m_pMatBlurX = models.CreateNRef( "m_pMatBlurX", m_pMatBlurXkv, false );
	m_pMatBlurXwf = models.CreateNRef( "m_pMatBlurXwf", m_pMatBlurXwfkv, false );
	m_pMatBlurY = models.CreateNRef( "m_pMatBlurY", m_pMatBlurYkv, false );
	m_pMatBlurYwf = models.CreateNRef( "m_pMatBlurYwf", m_pMatBlurYwfkv, false );
	m_pMatHaloAddToScreen = models.CreateNRef( "m_pMatHaloAddToScreen", m_pMatHaloAddToScreenkv, false );*/

	{
		static auto m_pMatBlurXkv = new KeyValues( "BlurFilterX" );
		m_pMatBlurXkv->SetString( "$basetexture", "glow_buffer_1" );
		m_pMatBlurXkv->SetString( "$wireframe", "0" );
		m_pMatBlurX = models.CreateNRef( "m_pMatBlurX", m_pMatBlurXkv );
	}

	{
		static auto m_pMatBlurXwfkv = new KeyValues( "BlurFilterX" );
		m_pMatBlurXwfkv->SetString( "$basetexture", "glow_buffer_1" );
		m_pMatBlurXwfkv->SetString( "$wireframe", "1" );
		m_pMatBlurXwf = models.CreateNRef( "m_pMatBlurXwf", m_pMatBlurXwfkv );
	}

	{
		static auto m_pMatBlurYkv = new KeyValues( "BlurFilterY" );
		m_pMatBlurYkv->SetString( "$basetexture", "glow_buffer_2" );
		m_pMatBlurYkv->SetString( "$wireframe", "0" );
		m_pMatBlurY = models.CreateNRef( "m_pMatBlurY", m_pMatBlurYkv );
	}

	{
		static auto m_pMatBlurYwfkv = new KeyValues( "BlurFilterY" );
		m_pMatBlurYwfkv->SetString( "$basetexture", "glow_buffer_2" );
		m_pMatBlurYwfkv->SetString( "$wireframe", "1" );
		m_pMatBlurYwf = models.CreateNRef( "m_pMatBlurYwf", m_pMatBlurYwfkv );
	}

	{
		static auto m_pMatHaloAddToScreenkv = new KeyValues( "UnlitGeneric" );
		m_pMatHaloAddToScreenkv->SetString( "$basetexture", "glow_buffer_1" );
		m_pMatHaloAddToScreenkv->SetString( "$wireframe", "0" );
		m_pMatHaloAddToScreenkv->SetString( "$additive", "1" );
		m_pMatHaloAddToScreen = models.CreateNRef( "m_pMatHaloAddToScreen", m_pMatHaloAddToScreenkv );
	}
}

void __outlines::Render( )
{
	if ( !m_vecGlowEntities.empty( ) )
		m_vecGlowEntities.clear( );

	if ( !m_DrawnEntities.empty( ) )
		m_DrawnEntities.clear( );

	if ( const auto& pLocal = entity_cache.get_local( ) )
	{
		if ( !ShouldRun( ) || !pLocal )
			return;

		if ( auto pRenderContext = i::material_system->GetRenderContext( ) )
		{
			Outline( pLocal, pRenderContext );
		}
	}
}

void __outlines::Outline( C_TFPlayer* pLocal, IMatRenderContext* pRenderContext )
{
	if ( !vars.outlines.global.outlines_global )
	{
		return;
	}

	if ( !ShouldRun( ) )
		return;

	int w = draw.screen_width;
	int h = draw.screen_height;

	if ( w < 1 || h < 1 || w > 4096 || h > 2160 )
		return;

	SetScale( vars.outlines.global.outline_scale );

	ShaderStencilState_t StencilStateDisable = {};
	StencilStateDisable.m_bEnable = false;

	float flOriginalColor[ 3 ] = {};
	i::render_view->GetColorModulation( flOriginalColor );
	float flOriginalBlend = i::render_view->GetBlend( );

	if ( !models.m_bHasSetStencil )
	{
		ShaderStencilState_t StencilState = {};
		StencilState.m_bEnable = true;
		StencilState.m_nReferenceValue = 1;
		StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
		StencilState.m_PassOp = STENCILOPERATION_REPLACE;
		StencilState.m_FailOp = STENCILOPERATION_KEEP;
		StencilState.m_ZFailOp = STENCILOPERATION_REPLACE;
		StencilState.m_nTestMask = 0x0;
		StencilState.m_nWriteMask = 0xFF;
		StencilState.SetStencilState( pRenderContext );
	}

	i::render_view->SetBlend( 1.0f );
	i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

	if ( vars.outlines.world.outlines_world_enabled )
	{
		for ( const auto& pEntity : entity_cache.world[ WORLDALL ] )
		{
			if ( !pEntity || pEntity->IsDormant( ) )
				continue;

			if ( pEntity->GetClassID( ) == ETFClassID::CCaptureFlag )
			{
				const auto& pIntel = pEntity->As<C_CaptureFlag>( );

				if ( pIntel->m_nFlagStatus( ) == 1 && GET_ENT_H( pIntel->m_hPrevOwner( ) ) == pLocal )
					continue;
			}

			if ( !utils.is_on_screen( pLocal, pEntity ) )
				continue;
			
			if ( pEntity->IsProjectile( ) )
			{
				if ( pEntity->GetClassID( ) == ETFClassID::CTFGrenadePipebombProjectile )
				{
					const auto& thrower = GET_ENT_H( pEntity->As<C_TFGrenadePipebombProjectile>( )->m_hThrower( ) )->As<C_TFPlayer>( );
					if ( vars.outlines.world.outlines_world_enemy_only && thrower && thrower->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && thrower != pLocal && !thrower->PlayerOnFriendsList( ) )
					{
						continue;
					}

					if ( !vars.outlines.world.outlines_world_local && thrower && thrower == pLocal )
					{
						continue;
					}

					if ( !vars.outlines.world.outlines_world_friends && thrower && thrower->PlayerOnFriendsList( ) )
					{
						continue;
					}
				}
				else
				{
					if ( vars.outlines.world.outlines_world_enemy_only && pEntity->m_hOwnerEntity( ).Get( ) && pEntity->m_hOwnerEntity( ).Get( )->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && pEntity->m_hOwnerEntity( ).Get( ) != pLocal && !pEntity->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					{
						continue;
					}

					if ( !vars.outlines.world.outlines_world_local && pEntity->m_hOwnerEntity( ).Get( ) && pEntity->m_hOwnerEntity( ).Get( ) == pLocal )
					{
						continue;
					}

					if ( !vars.outlines.world.outlines_world_friends && pEntity->m_hOwnerEntity( ).Get( ) && pEntity->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					{
						continue;
					}
				}
			}

			color DrawColor = colors.outlines.get_world_color( pEntity, pLocal );

			m_vecGlowEntities.push_back( { pEntity, DrawColor, /*vars.outlines.world.world_outline_alpha,*/ false } );

			if ( !models.HasDrawn( pEntity ) )
				DrawModel( pEntity, STUDIO_RENDER, true );
		}

		/*for ( const auto &pEntity : g_EntityCache.GetGroup( EEntGroup::WORLD_NPC ) )
		{
			const auto &pNpc = pEntity->As<CBaseEntity>( );

			if ( !pNpc->IsAlive( ) || pNpc->IsDormant( ) )
				continue;

			if ( !utils.is_on_screen( pLocal, pNpc ) )
				continue;

			if ( vars.outlines.global.enemies && pNpc->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
				continue;

			color DrawColor = Colors::GetColorNPC( pNpc, pLocal );

			m_vecGlowEntities.push_back( { pNpc, DrawColor, Vars::Outline::World::OutlineAlpha, false } );

			if ( !models.HasDrawn( pNpc ) )
				DrawModel( pNpc, STUDIO_RENDER, true );
		}*/
	}

	if ( vars.outlines.buildings.outlines_buildings_enabled )
	{
		for ( const auto& pObject : entity_cache.buildings[ TEAMSALL ] )
		{
			if ( !pObject || pObject->IsDormant( ) )
				continue;

			if ( !utils.is_on_screen( pLocal, pObject ) )
				continue;

			if ( vars.outlines.buildings.outlines_buildings_enemy_only && pObject->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && pObject->m_hBuilder( ).Get( ) != pLocal && !pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
				continue;

			if ( !vars.outlines.buildings.outlines_buildings_local && pObject->m_hBuilder( ).Get( ) == pLocal )
			{
				continue;
			}

			if ( !vars.outlines.buildings.outlines_buildings_friends && pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
			{
				continue;
			}

			color DrawColor = colors.outlines.get_building_color( pObject, pLocal );

			if ( pObject->m_iHealth( ) != 0 )
				m_vecGlowEntities.push_back( { pObject, DrawColor, /*vars.outlines.buildings.buildings_outline_alpha,*/ false } );

			if ( !models.HasDrawn( pObject ) )
			{
				if ( pObject->m_iHealth( ) != 0 && !pObject->m_bPlacing( ) )
					DrawModel( pObject, STUDIO_RENDER, true );
			}
		}
	}

	if ( vars.outlines.players.outlines_players_enabled )
	{
		for ( const auto& pPlayer : entity_cache.players[ TEAMSALL ] )
		{
			if ( pPlayer->deadflag( ) || pPlayer->IsDormant( ) )
				continue;

			if ( !utils.is_on_screen( pLocal, pPlayer ) )
				continue;

			if ( pPlayer == pLocal )
			{
				continue;
			}

			if ( vars.outlines.players.outlines_players_enemy_only && pPlayer->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && !pPlayer->PlayerOnFriendsList( ) )
				continue;

			if ( !vars.outlines.players.outlines_players_friends && pPlayer->PlayerOnFriendsList( ) )
			{
				continue;
			}

			color DrawColor = colors.outlines.get_entity_color( pPlayer, pLocal );

			m_vecGlowEntities.push_back( { pPlayer, DrawColor, /*vars.outlines.players.players_outline_alpha,*/ false } );

			if ( !models.HasDrawn( pPlayer ) )
			{
				DrawModel( pPlayer, STUDIO_RENDER, true );
			}
			if ( vars.outlines.players.players_attachments_outline )
			{
				auto attach = GET_ENT_I( *( int* )( ( uintptr_t )pPlayer + 0x1B0 ) & 0xFFF )->As<C_BaseEntity>( );
				for ( int i = 0; i < 32; i++ )
				{
					if ( !attach )
						continue;

					if ( attach->GetClassID( ) == ETFClassID::CTFWearable )
					{
						m_vecGlowEntities.push_back( { attach, DrawColor, /*vars.outlines.players.players_outline_alpha,*/ false } );

						if ( !models.HasDrawn( attach ) )
							DrawModel( attach, STUDIO_RENDER, true );
					}

					attach = GET_ENT_I( *( int* )( ( uintptr_t )attach + 0x1B4 ) & 0xFFF )->As<C_BaseEntity>( );
				}

				if ( const auto& pWeapon = GET_ENT_H( pPlayer->m_hActiveWeapon( ) )->As<C_TFWeaponBase>( ) )
				{
					m_vecGlowEntities.push_back( { pWeapon, DrawColor, /*vars.outlines.players.players_outline_alpha,*/ false } );

					if ( !models.HasDrawn( pWeapon ) )
						DrawModel( pWeapon, STUDIO_RENDER, true );
				}
			}
		}

		if ( pLocal && !pLocal->deadflag( ) && !pLocal->IsDormant( ) && vars.outlines.players.outlines_player_local )
		{
			if ( i::input->CAM_IsThirdPerson( ) )
			{
				color DrawColor = colors.outlines.get_entity_color( pLocal, pLocal );

				m_vecGlowEntities.push_back( { pLocal, DrawColor, /*vars.outlines.players.players_outline_alpha,*/ false } );

				if ( !models.HasDrawn( pLocal ) )
					DrawModel( pLocal, STUDIO_RENDER, true );

				if ( vars.outlines.players.players_attachments_outline )
				{
					auto attach = GET_ENT_I( *( int* )( ( uintptr_t )pLocal + 0x1B0 ) & 0xFFF )->As<C_BaseEntity>( );
					for ( int i = 0; i < 32; i++ )
					{
						if ( !attach )
							continue;

						if ( attach->GetClassID( ) == ETFClassID::CTFWearable )
						{
							m_vecGlowEntities.push_back( { attach, DrawColor, /*vars.outlines.players.players_outline_alpha,*/ false } );

							if ( !models.HasDrawn( attach ) )
								DrawModel( attach, STUDIO_RENDER, true );
						}

						attach = GET_ENT_I( *( int* )( ( uintptr_t )attach + 0x1B4 ) & 0xFFF )->As<C_BaseEntity>( );
					}

					if ( const auto& pWeapon = entity_cache.get_weapon( ) )
					{
						m_vecGlowEntities.push_back( { pWeapon, DrawColor, /*vars.outlines.players.players_outline_alpha,*/ false } );

						if ( !models.HasDrawn( pWeapon ) )
							DrawModel( pWeapon, STUDIO_RENDER, true );
					}
				}
			}
		}
	}

	StencilStateDisable.SetStencilState( pRenderContext );

	if ( m_vecGlowEntities.empty( ) )
		return;

	i::model_render->ForcedMaterialOverride( m_pMatGlowColor );

	pRenderContext->PushRenderTargetAndViewport( );
	{
		pRenderContext->SetRenderTarget( m_pRenderBuffer1 );
		pRenderContext->Viewport( 0, 0, w, h );
		pRenderContext->ClearColor4ub( 0, 0, 0, 0 );
		pRenderContext->ClearBuffers( true, false, false );

		for ( const auto& GlowEntity : m_vecGlowEntities )
		{
			i::render_view->SetBlend( utils.clr2flt( GlowEntity.m_Color.a ) );
			i::render_view->SetColorModulation( utils.clr2flt( GlowEntity.m_Color.r ),
											   utils.clr2flt( GlowEntity.m_Color.g ),
											   utils.clr2flt( GlowEntity.m_Color.b ) );
			/*if ( GlowEntity.viewmodel )
			{
				bColoringViewmodel = true;
			}*/
			float oldinvis = { };
			if ( GlowEntity.m_pEntity->GetClassID( ) == ETFClassID::CTFPlayer )
			{
				if ( GlowEntity.m_pEntity->As<C_TFPlayer>( )->m_iClass( ) == CLASS_SPY )
				{
					oldinvis = GlowEntity.m_pEntity->As<C_TFPlayer>( )->m_flInvisibility( );
					GlowEntity.m_pEntity->As<C_TFPlayer>( )->SetInvisibility( 0.0f );
				}
			}
			DrawModel( GlowEntity.m_pEntity, STUDIO_RENDER, false );
			if ( GlowEntity.m_pEntity->GetClassID( ) == ETFClassID::CTFPlayer )
			{
				if ( GlowEntity.m_pEntity->As<C_TFPlayer>( )->m_iClass( ) == CLASS_SPY )
				{
					GlowEntity.m_pEntity->As<C_TFPlayer>( )->SetInvisibility( oldinvis );
				}
			}
			/*if ( GlowEntity.viewmodel )
			{
				bColoringViewmodel = false;
			}*/
		}

		StencilStateDisable.SetStencilState( pRenderContext );
	}
	pRenderContext->PopRenderTargetAndViewport( );

	if ( vars.outlines.global.blur_outline ) {
		pRenderContext->PushRenderTargetAndViewport( );
		{
			pRenderContext->Viewport( 0, 0, w, h );

			pRenderContext->SetRenderTarget( m_pRenderBuffer2 );
			pRenderContext->DrawScreenSpaceRectangle( m_pMatBlurX, 0,
													  0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
			//pRenderContext->DrawScreenSpaceRectangle(m_pMatBlurX, 0, 0, w, h, 0.0f, 0.0f, w, h, w, h);

			pRenderContext->SetRenderTarget( m_pRenderBuffer1 );
			pRenderContext->DrawScreenSpaceRectangle( m_pMatBlurY, 0,
													  0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
			//pRenderContext->DrawScreenSpaceRectangle(m_pMatBlurY, 0, 0, w, h, 0.0f, 0.0f, w, h, w, h);
		}
		pRenderContext->PopRenderTargetAndViewport( );
	}

	ShaderStencilState_t StencilState = {};
	StencilState.m_bEnable = true;
	StencilState.m_nWriteMask = 0x0;
	StencilState.m_nTestMask = 0xFF;
	StencilState.m_nReferenceValue = 0;
	StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_EQUAL;
	StencilState.m_PassOp = STENCILOPERATION_KEEP;
	StencilState.m_FailOp = STENCILOPERATION_KEEP;
	StencilState.m_ZFailOp = STENCILOPERATION_KEEP;
	StencilState.SetStencilState( pRenderContext );

	/*if ( bDrawViewmodel )
	{
		bColoringViewmodel = true;
	}*/

	if ( vars.outlines.global.wireframe_outline )
	{
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, -1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, -1, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 0, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 0, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 1, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 1, -1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, -1, 1, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
	}
	
	if ( vars.outlines.global.blur_outline ) {
		pRenderContext->DrawScreenSpaceRectangle( m_pMatHaloAddToScreen, 0, 0, w, h, 0.0f, 0.0f, w - 1, h - 1, w, h );
	}

	/*if ( bDrawViewmodel )
	{
		bColoringViewmodel = false;
	}*/

	StencilStateDisable.SetStencilState( pRenderContext );

	i::model_render->ForcedMaterialOverride( nullptr );
	i::render_view->SetColorModulation( flOriginalColor );
	i::render_view->SetBlend( flOriginalBlend );
}