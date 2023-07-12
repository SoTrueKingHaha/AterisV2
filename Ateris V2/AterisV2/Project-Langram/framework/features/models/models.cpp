#include "models.h"

bool __models::ShouldRun( )
{
	if ( i::engine_vgui->IsGameUIVisible( ) || !i::engine_client->IsConnected( ) || !i::engine_client->IsInGame( ) )
		return false;

	return true;
}

void __models::DrawModel( IClientEntity* pEntity, bool pOverlayMat, color color, float flAlpha )
{
	m_bRendering = true;
	pEntity->DrawModel( STUDIO_RENDER );
	if ( pOverlayMat )
	{
		if ( IMaterialVar* $phongtint = m_pMatOverlay->FindVar( "$phongtint", nullptr, false ) )
		{
			$phongtint->SetVecValue(
				utils.clr2flt( color.r ),
				utils.clr2flt( color.g ),
				utils.clr2flt( color.b ) );
		}
		if ( IMaterialVar* $envmaptint = m_pMatOverlay->FindVar( "$envmaptint", nullptr, false ) )
		{
			$envmaptint->SetVecValue(
				utils.clr2flt( color.r ),
				utils.clr2flt( color.g ),
				utils.clr2flt( color.b ) );
		}
		if ( IMaterialVar* $phongfresnelranges = m_pMatOverlay->FindVar( "$phongfresnelranges", nullptr, false ) )
		{
			$phongfresnelranges->SetVecValue( 0, 1.f, 2.f );
		}
		m_pMatOverlay->SetMaterialVarFlag( MATERIAL_VAR_WIREFRAME, false );

		i::render_view->SetBlend( flAlpha );
		i::model_render->ForcedMaterialOverride( m_pMatOverlay );

		pEntity->DrawModel( STUDIO_RENDER );
	}
	m_DrawnEntities[ pEntity ] = true;
	m_bRendering = false;
}

IMaterial* __models::CreateNRef( char const* szName, KeyValues* pKV, bool bSave )
{
	/*I::CVar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "Creating Reference: " );
	I::CVar->ConsoleColorPrintf( { 0, 255, 255, 255 }, szName );
	I::CVar->ConsoleColorPrintf( { 0, 255, 255, 255 }, "\n" );*/
	IMaterial* returnMaterial = i::material_system->CreateMaterial( szName, pKV );
	returnMaterial->IncrementReferenceCount( );

	int $flags{}, $flags_defined{}, $flags2{}, $flags_defined2{};
	if ( auto var = returnMaterial->FindVar( "$flags", nullptr ) )
		$flags = var->GetIntValueInternal( );

	if ( auto var = returnMaterial->FindVar( "$flags_defined", nullptr ) )
		$flags_defined = var->GetIntValueInternal( );

	if ( auto var = returnMaterial->FindVar( "$flags2", nullptr ) )
		$flags2 = var->GetIntValueInternal( );

	if ( auto var = returnMaterial->FindVar( "$flags_defined2", nullptr ) )
		$flags_defined2 = var->GetIntValueInternal( );

	backupInformation[ returnMaterial ] = { $flags, $flags_defined, $flags2, $flags_defined2 };
	matList.push_back( returnMaterial );
	++iInitCount;
	return returnMaterial;
}

void __models::init( )
{
	if ( bSetup ) { return; }

	{
		static auto m_pMatFlatkv = new KeyValues( "UnlitGeneric" );
		m_pMatFlatkv->SetString( "$basetexture", "vgui/white_additive" );
		m_pMatFlatkv->SetString( "$wireframe", "0" );
		m_pMatFlatkv->SetString( "$cloakPassEnabled", "1" );
		m_pMatFlat = CreateNRef( "m_pMatFlat", m_pMatFlatkv );
	}

	{
		static auto m_pMatShadedkv = new KeyValues( "VertexLitGeneric" );
		m_pMatShadedkv->SetString( "$basetexture", "vgui/white_additive" );
		m_pMatShadedkv->SetString( "$bumpmap", "vgui/white_additive" );
		//kv->SetString( "$halflambert", "1" );
		m_pMatShadedkv->SetString( "$selfillum", "1" );
		m_pMatShadedkv->SetString( "$selfillumfresnel", "1" );
		m_pMatShadedkv->SetString( "$selfillumfresnelminmaxexp", "[0 1 1]" );
		m_pMatShadedkv->SetString( "$wireframe", "0" );
		m_pMatShadedkv->SetString( "$cloakPassEnabled", "1" );
		m_pMatShaded = CreateNRef( "m_pMatShaded", m_pMatShadedkv );
	}

	{
		static auto m_pMatFresnelHDR0kv = new KeyValues( "VertexLitGeneric" );
		m_pMatFresnelHDR0kv->SetString( "$basetexture", "vgui/white_additive" );
		m_pMatFresnelHDR0kv->SetString( "$bumpmap", "models/player/shared/shared_normal" );
		//kv->SetString( "$halflambert", "1" );
		m_pMatFresnelHDR0kv->SetString( "$envmap", "skybox/sky_dustbowl_01" );
		m_pMatFresnelHDR0kv->SetString( "$envmapfresnel", "1" );
		m_pMatFresnelHDR0kv->SetString( "$phong", "1" );
		m_pMatFresnelHDR0kv->SetString( "$phongfresnelranges", "[0 1.5 2]" );
		m_pMatFresnelHDR0kv->SetString( "$selfillum", "1" );
		m_pMatFresnelHDR0kv->SetString( "$selfillumfresnel", "1" );
		m_pMatFresnelHDR0kv->SetString( "$selfillumfresnelminmaxexp", "[0.5 0.5 0]" );
		m_pMatFresnelHDR0kv->SetString( "$selfillumtint", "[0 0 0]" );
		m_pMatFresnelHDR0kv->SetString( "$envmaptint", "[0 1 0]" );
		m_pMatFresnelHDR0kv->SetString( "$wireframe", "0" );
		m_pMatFresnelHDR0kv->SetString( "$cloakPassEnabled", "1" );
		m_pMatFresnelHDR0 = CreateNRef( "m_pMatFresnelHDR0", m_pMatFresnelHDR0kv );
	}

	{
		static auto m_pMatFresnelHDR1kv = new KeyValues( "VertexLitGeneric" );
		m_pMatFresnelHDR1kv->SetString( "$basetexture", "vgui/white_additive" );
		m_pMatFresnelHDR1kv->SetString( "$bumpmap", "models/player/shared/shared_normal" );
		//kv->SetString( "$halflambert", "1" );
		m_pMatFresnelHDR1kv->SetString( "$envmap", "skybox/sky_dustbowl_01" );
		m_pMatFresnelHDR1kv->SetString( "$envmapfresnel", "1" );
		m_pMatFresnelHDR1kv->SetString( "$phong", "1" );
		m_pMatFresnelHDR1kv->SetString( "$phongfresnelranges", "[0 0.05 0.1]" );
		m_pMatFresnelHDR1kv->SetString( "$selfillum", "1" );
		m_pMatFresnelHDR1kv->SetString( "$selfillumfresnel", "1" );
		m_pMatFresnelHDR1kv->SetString( "$selfillumfresnelminmaxexp", "[0.5 0.5 0]" );
		m_pMatFresnelHDR1kv->SetString( "$selfillumtint", "[0 0 0]" );
		m_pMatFresnelHDR1kv->SetString( "$envmaptint", "[0 1 0]" );
		m_pMatFresnelHDR1kv->SetString( "$wireframe", "0" );
		m_pMatFresnelHDR1kv->SetString( "$cloakPassEnabled", "1" );
		m_pMatFresnelHDR1 = CreateNRef( "m_pMatFresnelHDR1", m_pMatFresnelHDR1kv );
	}

	{
		static auto m_pMatOverlaykv = new KeyValues( "VertexLitGeneric" );
		m_pMatOverlaykv->SetString( "$basetexture", "models/player/shared/ice_player" );
		m_pMatOverlaykv->SetString( "$bumpmap", "models/player/shared/shared_normal" );
		m_pMatOverlaykv->SetString( "$translucent", "1" );
		m_pMatOverlaykv->SetString( "$additive", "1" );
		m_pMatOverlaykv->SetString( "$phong", "1" );
		m_pMatOverlaykv->SetString( "$phongfresnelranges", "[0 0.5 1]" );
		m_pMatOverlaykv->SetString( "$phongtint", "[0 0 0]" );
		m_pMatOverlaykv->SetString( "$envmap", "skybox/sky_dustbowl_01" );
		m_pMatOverlaykv->SetString( "$envmapfresnel", "1" );
		m_pMatOverlaykv->SetString( "$envmaptint", "[1 1 1]" );
		m_pMatOverlaykv->SetString( "$selfillum", "1" );
		m_pMatOverlaykv->SetString( "$selfillumtint", "[0 0 0]" );
		m_pMatOverlaykv->SetString( "$wireframe", "0" );
		m_pMatOverlaykv->SetString( "$cloakPassEnabled", "1" );
		m_pMatOverlay = CreateNRef( "m_pMatOverlay", m_pMatOverlaykv );
	}

	bSetup = true;
}

void __models::Render( )
{
	if ( !m_DrawnEntities.empty( ) )
		m_DrawnEntities.clear( );

	m_bHasSetStencil = false;

	const auto& pLocal = entity_cache.get_local( );

	if ( !ShouldRun( ) || !pLocal )
		return;

	if ( const auto& pRenderContext = i::material_system->GetRenderContext( ) )
	{
		ShaderStencilState_t StencilState = {};
		StencilState.m_bEnable = true;
		StencilState.m_nReferenceValue = 1;
		StencilState.m_CompareFunc = STENCILCOMPARISONFUNCTION_ALWAYS;
		StencilState.m_PassOp = STENCILOPERATION_REPLACE;
		StencilState.m_FailOp = STENCILOPERATION_KEEP;
		StencilState.m_ZFailOp = STENCILOPERATION_REPLACE;
		StencilState.SetStencilState( pRenderContext );
		m_bHasSetStencil = true;

		Override( pLocal, pRenderContext );
	}

}

void __models::Override( C_TFPlayer* pLocal, IMatRenderContext* pRenderContext )
{
	if ( !vars.models.global.models_global )
		return;

	if ( !ShouldRun( ) )
		return;

	bool bMatWasForced = false;

	if ( vars.models.global.models_global )
	{
		switch ( vars.models.world.world_model )
		{
			case 0:
			{
				i::model_render->ForcedMaterialOverride( nullptr );
				m_pMatWorld = nullptr;
				bMatWasForced = true;
				break;
			}
			case 1:
			{
				i::model_render->ForcedMaterialOverride( m_pMatFlat );
				m_pMatWorld = m_pMatFlat;
				bMatWasForced = true;
				break;
			}
			case 2:
			{
				i::model_render->ForcedMaterialOverride( m_pMatShaded );
				m_pMatWorld = m_pMatShaded;
				bMatWasForced = true;
				break;
			}
			case 3:
			{
				if ( convars.find_var( "mat_hdr_level" )->GetInt( ) > 1 )
					m_pMatFresnel = m_pMatFresnelHDR1;
				else
					m_pMatFresnel = m_pMatFresnelHDR0;

				i::model_render->ForcedMaterialOverride( m_pMatFresnel );
				m_pMatWorld = m_pMatFresnel;
				bMatWasForced = true;
				break;
			}
			default: break;
		}
	}

	if ( !bMatWasForced )
		i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

	if ( vars.models.world.models_world_enabled )
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
					if ( vars.models.world.models_world_enemy_only && thrower && thrower->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && thrower != pLocal && !thrower->PlayerOnFriendsList( ) )
					{
						continue;
					}

					if ( !vars.models.world.models_world_local && thrower && thrower == pLocal )
					{
						continue;
					}

					if ( !vars.models.world.models_world_friends && thrower && thrower->PlayerOnFriendsList( ) )
					{
						continue;
					}
				}
				else
				{
					if ( vars.models.world.models_world_enemy_only && pEntity->m_hOwnerEntity( ).Get( ) && pEntity->m_hOwnerEntity( ).Get( )->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && pEntity->m_hOwnerEntity( ).Get( ) != pLocal && !pEntity->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					{
						continue;
					}

					if ( !vars.models.world.models_world_local && pEntity->m_hOwnerEntity( ).Get( ) && pEntity->m_hOwnerEntity( ).Get( ) == pLocal )
					{
						continue;
					}

					if ( !vars.models.world.models_world_friends && pEntity->m_hOwnerEntity( ).Get( ) && pEntity->m_hOwnerEntity( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
					{
						continue;
					}
				}
			}

			if ( vars.models.world.world_ignorez )
				pRenderContext->DepthRange( 0.0f, 0.2f );

			color WorldColor = colors.models.get_world_color( pEntity, pLocal );
			if ( bMatWasForced )
			{
				if ( vars.models.world.world_model != 3 )
					i::render_view->SetColorModulation( WorldColor );
				else
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				i::render_view->SetBlend( utils.clr2flt( WorldColor.a ) );

				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if ( vars.models.world.world_model == 3 )
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar( "$selfillumtint", &foundselfillumtint );
					if ( foundselfillumtint )
					{
						fresnelSelfillumtint->SetVecValue( utils.clr2flt( vars.colors.models.world.models_world_fresnel.r ),
														   utils.clr2flt( vars.colors.models.world.models_world_fresnel.g ),
														   utils.clr2flt( vars.colors.models.world.models_world_fresnel.b ) );
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar( "$envmaptint", &foundenvmaptint );
					if ( foundenvmaptint )
					{
						fresnelEnvmaptint->SetVecValue( utils.clr2flt( WorldColor.r ), utils.clr2flt( WorldColor.g ), utils.clr2flt( WorldColor.b ) );
					}
				}
			}

			if ( vars.models.world.world_model == 0 )
			{
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
				i::render_view->SetBlend( 1.0f );
			}

			DrawModel( pEntity, vars.models.world.world_overlay, WorldColor, vars.models.world.world_overlay_alpha );
			if ( vars.models.world.world_overlay )
			{
				if ( vars.models.world.world_model == 0 )
				{
					i::model_render->ForcedMaterialOverride( m_pMatWorld );
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
					i::render_view->SetBlend( 1.0f );
				}
				else
				{
					i::model_render->ForcedMaterialOverride( m_pMatWorld );
					i::render_view->SetColorModulation( utils.clr2flt( WorldColor.r ),
													   utils.clr2flt( WorldColor.g ),
													   utils.clr2flt( WorldColor.b ) );
					i::render_view->SetBlend( utils.clr2flt( WorldColor.a ) );
				}
			}

			pRenderContext->DepthRange( 0.0f, 1.0f );
		}

		/*for ( const auto &pEntity : g_EntityCache.GetGroup( EEntGroup::WORLD_NPC ) )
		{
			const auto &pNpc = pEntity->As<CBaseEntity>( );

			if ( !pNpc->IsAlive( ) || pNpc->IsDormant( ) )
				continue;

			if ( !utils.is_on_screen( pLocal, pNpc ) )
				continue;

			if ( vars.models.global.enemies && pNpc->m_iTeamNum( ) == pLocal->m_iTeamNum( ) )
				continue;

			if ( Vars::Models::World::ignorez )
				pRenderContext->DepthRange( 0.0f, 0.2f );

			color NPCColor = Colors::GetColorNPC( pNpc, pLocal );

			if ( bMatWasForced )
			{
				if ( vars.models.world.model != 3 )
					i::render_view->SetColorModulation( NPCColor );
				else
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				i::render_view->SetBlend( utils.clr2flt( NPCColor.a ) );

				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if ( vars.models.world.model == 3 )
				{
					IMaterialVar *fresnelSelfillumtint = m_pMatFresnel->FindVar( "$selfillumtint", &foundselfillumtint );
					if ( foundselfillumtint )
					{
						fresnelSelfillumtint->SetVecValue( utils.clr2flt( Vars::Colors::Fresnel.r ),
														   utils.clr2flt( Vars::Colors::Fresnel.g ),
														   utils.clr2flt( Vars::Colors::Fresnel.b ) );
					}
					IMaterialVar *fresnelEnvmaptint = m_pMatFresnel->FindVar( "$envmaptint", &foundenvmaptint );
					if ( foundenvmaptint )
					{
						fresnelEnvmaptint->SetVecValue( utils.clr2flt( NPCColor.r ), utils.clr2flt( NPCColor.g ), utils.clr2flt( NPCColor.b ) );
					}
				}
			}

			if ( vars.models.world.model == 0 )
			{
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
				i::render_view->SetBlend( 1.0f );
			}

			DrawModel( pNpc, Vars::Models::World::Overlay, NPCColor, Vars::Models::World::.overlay_alpha );
			if ( Vars::Models::World::Overlay )
			{
				if ( vars.models.world.model == 0 )
				{
					i::model_render->ForcedMaterialOverride( m_pMatWorld );
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
					i::render_view->SetBlend( 1.0f );
				}
				else
				{
					i::model_render->ForcedMaterialOverride( m_pMatWorld );
					i::render_view->SetColorModulation( utils.clr2flt( NPCColor.r ),
													   utils.clr2flt( NPCColor.g ),
													   utils.clr2flt( NPCColor.b ) );
					i::render_view->SetBlend( utils.clr2flt( NPCColor.a ) );
				}
			}
		}*/
	}

	if ( vars.models.global.models_global )
	{
		switch ( vars.models.buildings.buildings_model )
		{
			case 0:
			{
				i::model_render->ForcedMaterialOverride( nullptr );
				m_pMatBuilding = nullptr;
				bMatWasForced = true;
				break;
			}
			case 1:
			{
				i::model_render->ForcedMaterialOverride( m_pMatFlat );
				m_pMatBuilding = m_pMatFlat;
				bMatWasForced = true;
				break;
			}
			case 2:
			{
				i::model_render->ForcedMaterialOverride( m_pMatShaded );
				m_pMatBuilding = m_pMatShaded;
				bMatWasForced = true;
				break;
			}
			case 3:
			{
				if ( convars.find_var( "mat_hdr_level" )->GetInt( ) > 1 )
					m_pMatFresnel = m_pMatFresnelHDR1;
				else
					m_pMatFresnel = m_pMatFresnelHDR0;

				i::model_render->ForcedMaterialOverride( m_pMatFresnel );
				m_pMatBuilding = m_pMatFresnel;
				bMatWasForced = true;
				break;
			}
			default: break;
		}
	}

	if ( !bMatWasForced )
		i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

	if ( vars.models.buildings.models_buildings_enabled )
	{
		for ( const auto& pEntity : entity_cache.buildings[ TEAMSALL ] )
		{
			const auto& pObject = pEntity->As< C_BaseObject >( );

			if ( !pObject || pObject->IsDormant( ) )
				continue;

			if ( !utils.is_on_screen( pLocal, pObject ) )
				continue;

			if ( vars.models.buildings.models_buildings_enemy_only && pObject->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && pObject->m_hBuilder( ).Get( ) != pLocal && !pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
				continue;

			if ( !vars.models.buildings.models_buildings_local && pObject->m_hBuilder( ).Get( ) == pLocal )
			{
				continue;
			}

			if ( !vars.models.buildings.models_buildings_friends && pObject->m_hBuilder( ).Get( )->As<C_TFPlayer>( )->PlayerOnFriendsList( ) )
			{
				continue;
			}

			if ( vars.models.buildings.buildings_ignorez )
				pRenderContext->DepthRange( 0.0f, 0.2f );

			color BuildingColor = colors.models.get_building_color( pObject, pLocal );
			if ( bMatWasForced )
			{
				if ( vars.models.buildings.buildings_model != 3 )
					i::render_view->SetColorModulation( BuildingColor );
				else
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				i::render_view->SetBlend( utils.clr2flt( BuildingColor.a ) );

				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if ( vars.models.buildings.buildings_model == 3 )
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar( "$selfillumtint", &foundselfillumtint );
					if ( foundselfillumtint )
					{
						fresnelSelfillumtint->SetVecValue( utils.clr2flt( vars.colors.models.buildings.models_building_fresnel.r ),
														   utils.clr2flt( vars.colors.models.buildings.models_building_fresnel.g ),
														   utils.clr2flt( vars.colors.models.buildings.models_building_fresnel.b ) );
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar( "$envmaptint", &foundenvmaptint );
					if ( foundenvmaptint )
					{
						fresnelEnvmaptint->SetVecValue( utils.clr2flt( BuildingColor.r ), utils.clr2flt( BuildingColor.g ), utils.clr2flt( BuildingColor.b ) );
					}
				}
			}

			if ( vars.models.buildings.buildings_model == 0 )
			{
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
				i::render_view->SetBlend( 1.0f );
			}

			if ( pObject->m_iHealth( ) != 0 && !pObject->m_bPlacing( ) )
			{
				DrawModel( pObject, vars.models.buildings.buildings_overlay, BuildingColor, vars.models.buildings.buildings_overlay_alpha );
				if ( vars.models.buildings.buildings_overlay )
				{
					if ( vars.models.buildings.buildings_model == 0 )
					{
						i::model_render->ForcedMaterialOverride( m_pMatBuilding );
						i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
						i::render_view->SetBlend( 1.0f );
					}
					else
					{
						i::model_render->ForcedMaterialOverride( m_pMatBuilding );
						i::render_view->SetColorModulation( utils.clr2flt( BuildingColor.r ),
															utils.clr2flt( BuildingColor.g ),
															utils.clr2flt( BuildingColor.b ) );
						i::render_view->SetBlend( utils.clr2flt( BuildingColor.a ) );
					}
				}
			}

			pRenderContext->DepthRange( 0.0f, 1.0f );
		}
	}

	if ( vars.models.global.models_global )
	{
		switch ( vars.models.players.players_model )
		{
			case 0:
			{
				i::model_render->ForcedMaterialOverride( nullptr );
				m_pMatPlayer = nullptr;
				bMatWasForced = true;
				break;
			}
			case 1:
			{
				i::model_render->ForcedMaterialOverride( m_pMatFlat );
				m_pMatPlayer = m_pMatFlat;
				bMatWasForced = true;
				break;
			}
			case 2:
			{
				i::model_render->ForcedMaterialOverride( m_pMatShaded );
				m_pMatPlayer = m_pMatShaded;
				bMatWasForced = true;
				break;
			}
			case 3:
			{
				if ( convars.find_var( "mat_hdr_level" )->GetInt( ) > 1 )
					m_pMatFresnel = m_pMatFresnelHDR1;
				else
					m_pMatFresnel = m_pMatFresnelHDR0;

				i::model_render->ForcedMaterialOverride( m_pMatFresnel );
				m_pMatPlayer = m_pMatFresnel;
				bMatWasForced = true;
				break;
			}
			default: break;
		}
	}

	if ( !bMatWasForced )
		i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

	if ( vars.models.players.models_players_enabled )
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

			/*if ( pPlayer->m_nPlayerCond( ) & ( 1 << 4 ) )
				pPlayer->m_nPlayerCond( ) &= ~( 1 << 4 );*/

			if ( vars.models.players.models_players_enemy_only && pPlayer->m_iTeamNum( ) == pLocal->m_iTeamNum( ) && !pPlayer->PlayerOnFriendsList( ) )
				continue;

			if ( !vars.models.players.models_players_friends && pPlayer->PlayerOnFriendsList( ) )
			{
				continue;
			}

			if ( vars.models.players.players_ignorez )
				if ( pPlayer != pLocal )
					pRenderContext->DepthRange( 0.0f, 0.2f );

			color PlayerColor = colors.models.get_entity_color( pPlayer, pLocal );
			if ( bMatWasForced )
			{
				if ( vars.models.players.players_model != 3 )
					i::render_view->SetColorModulation( PlayerColor );
				else
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				i::render_view->SetBlend( utils.clr2flt( PlayerColor.a ) );

				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if ( vars.models.players.players_model == 3 )
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar( "$selfillumtint", &foundselfillumtint );
					if ( foundselfillumtint )
					{
						fresnelSelfillumtint->SetVecValue( utils.clr2flt( vars.colors.models.players.models_player_fresnel.r ),
														   utils.clr2flt( vars.colors.models.players.models_player_fresnel.g ),
														   utils.clr2flt( vars.colors.models.players.models_player_fresnel.b ) );
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar( "$envmaptint", &foundenvmaptint );
					if ( foundenvmaptint )
					{
						fresnelEnvmaptint->SetVecValue( utils.clr2flt( PlayerColor.r ), utils.clr2flt( PlayerColor.g ), utils.clr2flt( PlayerColor.b ) );
					}
				}
			}

			if ( vars.models.players.players_model == 0 )
			{
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
				i::render_view->SetBlend( 1.0f );
			}

			float oldinvis;
			if ( pPlayer->m_iClass( ) == CLASS_SPY )
			{
				oldinvis = pPlayer->m_flInvisibility( );
				pPlayer->SetInvisibility( 0.0f );
			}
			DrawModel( pPlayer, vars.models.players.players_overlay, PlayerColor, vars.models.players.players_overlay_alpha );
			if ( vars.models.players.players_overlay )
			{
				if ( vars.models.players.players_model == 0 )
				{
					i::model_render->ForcedMaterialOverride( m_pMatPlayer );
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
					i::render_view->SetBlend( 1.0f );
				}
				else
				{
					i::model_render->ForcedMaterialOverride( m_pMatPlayer );
					i::render_view->SetColorModulation( utils.clr2flt( PlayerColor.r ),
														utils.clr2flt( PlayerColor.g ),
														utils.clr2flt( PlayerColor.b ) );
					i::render_view->SetBlend( utils.clr2flt( PlayerColor.a ) );
				}
			}
			if ( pPlayer->m_iClass( ) == CLASS_SPY )
			{
				pPlayer->SetInvisibility( oldinvis );
			}

			auto pAttachment = GET_ENT_I( *( int* )( ( uintptr_t )pPlayer + 0x1B0 ) & 0xFFF )->As<C_BaseEntity>( );
			for ( int i = 0; i < 32; i++ )
			{
				if ( !pAttachment )
					continue;

				if ( pAttachment->GetClassID( ) == ETFClassID::CTFWearable )
				{
					if ( pAttachment->ShouldDraw( ) )
					{
						DrawModel( pAttachment, vars.models.players.players_overlay, PlayerColor, vars.models.players.players_overlay_alpha );
						if ( vars.models.players.players_overlay )
						{
							if ( vars.models.players.players_model == 0 )
							{
								i::model_render->ForcedMaterialOverride( m_pMatPlayer );
								i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
								i::render_view->SetBlend( 1.0f );
							}
							else
							{
								i::model_render->ForcedMaterialOverride( m_pMatPlayer );
								i::render_view->SetColorModulation( utils.clr2flt( PlayerColor.r ),
																	utils.clr2flt( PlayerColor.g ),
																	utils.clr2flt( PlayerColor.b ) );
								i::render_view->SetBlend( utils.clr2flt( PlayerColor.a ) );
							}
						}
					}
				}

				pAttachment = GET_ENT_I( *( int* )( ( uintptr_t )pAttachment + 0x1B4 ) & 0xFFF )->As<C_BaseEntity>( );
			}

			if ( const auto& pWeapon = GET_ENT_H( pPlayer->m_hActiveWeapon( ) )->As<C_TFWeaponBase>( ) )
			{
				if ( pWeapon->ShouldDraw( ) )
				{
					DrawModel( pWeapon, vars.models.players.players_overlay, PlayerColor, vars.models.players.players_overlay_alpha );
					if ( vars.models.players.players_overlay )
					{
						if ( vars.models.players.players_model == 0 )
						{
							i::model_render->ForcedMaterialOverride( m_pMatPlayer );
							i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
							i::render_view->SetBlend( 1.0f );
						}
						else
						{
							i::model_render->ForcedMaterialOverride( m_pMatPlayer );
							i::render_view->SetColorModulation( utils.clr2flt( PlayerColor.r ),
																utils.clr2flt( PlayerColor.g ),
																utils.clr2flt( PlayerColor.b ) );
							i::render_view->SetBlend( utils.clr2flt( PlayerColor.a ) );
						}
					}
				}
			}
		}

		if ( pLocal && !pLocal->deadflag( ) && !pLocal->IsDormant( ) && vars.models.players.models_player_local )
		{
			color LocalColor = colors.models.get_entity_color( pLocal, pLocal );

			if ( bMatWasForced )
			{
				if ( vars.models.players.players_model != 3 && vars.models.players.players_model != 0 )
					i::render_view->SetColorModulation( LocalColor );
				else
					i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

				i::render_view->SetBlend( utils.clr2flt( LocalColor.a ) );

				bool foundselfillumtint = false;
				bool foundenvmaptint = false;
				if ( vars.models.players.players_model == 3 )
				{
					IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar( "$selfillumtint", &foundselfillumtint );
					if ( foundselfillumtint )
					{
						fresnelSelfillumtint->SetVecValue( utils.clr2flt( vars.colors.models.players.models_player_fresnel.r ),
														   utils.clr2flt( vars.colors.models.players.models_player_fresnel.g ),
														   utils.clr2flt( vars.colors.models.players.models_player_fresnel.b ) );
					}
					IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar( "$envmaptint", &foundenvmaptint );
					if ( foundenvmaptint )
					{
						fresnelEnvmaptint->SetVecValue( utils.clr2flt( LocalColor.r ), utils.clr2flt( LocalColor.g ), utils.clr2flt( LocalColor.b ) );
					}
				}
			}

			if ( vars.models.players.players_model == 0 )
			{
				i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
				i::render_view->SetBlend( 1.0f );
			}

			if ( i::input->CAM_IsThirdPerson( ) )
			{
				DrawModel( pLocal, vars.models.players.players_overlay, LocalColor, vars.models.players.players_overlay_alpha );
				if ( vars.models.players.players_overlay )
				{
					if ( vars.models.players.players_model == 0 )
					{
						i::model_render->ForcedMaterialOverride( m_pMatPlayer );
						i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
						i::render_view->SetBlend( 1.0f );
					}
					else
					{
						i::model_render->ForcedMaterialOverride( m_pMatPlayer );
						i::render_view->SetColorModulation( utils.clr2flt( LocalColor.r ),
															utils.clr2flt( LocalColor.g ),
															utils.clr2flt( LocalColor.b ) );
						i::render_view->SetBlend( utils.clr2flt( LocalColor.a ) );
					}
				}
				auto pAttachment = GET_ENT_I( *( int* )( ( uintptr_t )pLocal + 0x1B0 ) & 0xFFF )->As<C_BaseEntity>( );

				if ( bMatWasForced )
				{
					if ( vars.models.players.players_model != 3 && vars.models.players.players_model != 0 )
						i::render_view->SetColorModulation( LocalColor );
					else
						i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );

					i::render_view->SetBlend( utils.clr2flt( LocalColor.a ) );

					bool foundselfillumtint = false;
					bool foundenvmaptint = false;
					if ( vars.models.players.players_model == 3 )
					{
						IMaterialVar* fresnelSelfillumtint = m_pMatFresnel->FindVar( "$selfillumtint", &foundselfillumtint );
						if ( foundselfillumtint )
						{
							fresnelSelfillumtint->SetVecValue( utils.clr2flt( vars.colors.models.players.models_player_fresnel.r ),
															   utils.clr2flt( vars.colors.models.players.models_player_fresnel.g ),
															   utils.clr2flt( vars.colors.models.players.models_player_fresnel.b ) );
						}
						IMaterialVar* fresnelEnvmaptint = m_pMatFresnel->FindVar( "$envmaptint", &foundenvmaptint );
						if ( foundenvmaptint )
						{
							fresnelEnvmaptint->SetVecValue( utils.clr2flt( LocalColor.r ), utils.clr2flt( LocalColor.g ), utils.clr2flt( LocalColor.b ) );
						}
					}
				}

				for ( int i = 0; i < 32; i++ )
				{
					if ( !pAttachment )
						continue;

					if ( pAttachment->GetClassID( ) == ETFClassID::CTFWearable )
					{
						if ( pAttachment->ShouldDraw( ) )
						{
							DrawModel( pAttachment, vars.models.players.players_overlay, LocalColor, vars.models.players.players_overlay_alpha );
							if ( vars.models.players.players_overlay )
							{
								if ( vars.models.players.players_model == 0 )
								{
									i::model_render->ForcedMaterialOverride( m_pMatPlayer );
									i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
									i::render_view->SetBlend( 1.0f );
								}
								else
								{
									i::model_render->ForcedMaterialOverride( m_pMatPlayer );
									i::render_view->SetColorModulation( utils.clr2flt( LocalColor.r ),
																		utils.clr2flt( LocalColor.g ),
																		utils.clr2flt( LocalColor.b ) );
									i::render_view->SetBlend( utils.clr2flt( LocalColor.a ) );
								}
							}
						}
					}

					pAttachment = GET_ENT_I( *( int* )( ( uintptr_t )pAttachment + 0x1B4 ) & 0xFFF )->As<C_BaseEntity>( );
				}

				if ( const auto& pWeapon = entity_cache.get_weapon( ) )
				{
					if ( pWeapon->ShouldDraw( ) )
					{
						DrawModel( pWeapon, vars.models.players.players_overlay, LocalColor, vars.models.players.players_overlay_alpha );
						if ( vars.models.players.players_overlay )
						{
							if ( vars.models.players.players_model == 0 )
							{
								i::model_render->ForcedMaterialOverride( m_pMatPlayer );
								i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
								i::render_view->SetBlend( 1.0f );
							}
							else
							{
								i::model_render->ForcedMaterialOverride( m_pMatPlayer );
								i::render_view->SetColorModulation( utils.clr2flt( LocalColor.r ),
																	utils.clr2flt( LocalColor.g ),
																	utils.clr2flt( LocalColor.b ) );
								i::render_view->SetBlend( utils.clr2flt( LocalColor.a ) );
							}
						}
					}
				}
			}
		}

		pRenderContext->DepthRange( 0.0f, 1.0f );
	}

	i::model_render->ForcedMaterialOverride( nullptr );
	i::render_view->SetColorModulation( 1.0f, 1.0f, 1.0f );
	pRenderContext->DepthRange( 0.0f, 1.0f );
}