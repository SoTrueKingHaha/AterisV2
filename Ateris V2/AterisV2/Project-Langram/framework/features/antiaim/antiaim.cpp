#include "antiaim.h"
#include "../models/models.h"
#include "../aimbot/aimbot.h"

void __antiaim::fix_movement( CUserCmd* pCmd, const vector& vOldAngles, float fOldSideMove, float fOldForwardMove ) {
	//better movement fix roll and pitch above 90 and -90 l0l
	static auto cl_forwardspeed = convars.find_var( "cl_forwardspeed" );
	static auto cl_sidespeed = convars.find_var( "cl_sidespeed" );
	static auto cl_upspeed = convars.find_var( "cl_upspeed" );
	if ( !cl_upspeed || !cl_sidespeed || !cl_forwardspeed ) {
		return;
	}

	const float flMaxForwardSpeed = cl_forwardspeed->GetFloat( );
	const float flMaxSideSpeed = cl_sidespeed->GetFloat( );
	const float flMaxUpSpeed = cl_upspeed->GetFloat( );
	vector vecForward = { }, vecRight = { }, vecUp = { };
	math.angle_vectors( vOldAngles, &vecForward, &vecRight, &vecUp );
	vecForward.z = vecRight.z = vecUp.x = vecUp.y = 0.f;
	vecForward.normalize_in_place( );
	vecRight.normalize_in_place( );
	vecUp.normalize_in_place( );
	vector vecOldForward = { }, vecOldRight = { }, vecOldUp = { };
	math.angle_vectors( pCmd->viewangles, &vecOldForward, &vecOldRight, &vecOldUp );
	vecOldForward.z = vecOldRight.z = vecOldUp.x = vecOldUp.y = 0.f;	// these can all have 3 vectors can they not?
	vecOldForward.normalize_in_place( );
	vecOldRight.normalize_in_place( );
	vecOldUp.normalize_in_place( );
	const float flPitchForward = vecForward.x * pCmd->forwardmove;	//	chunky
	const float flYawForward = vecForward.y * pCmd->forwardmove;	//	chunky
	const float flPitchSide = vecRight.x * pCmd->sidemove;			//	chunky
	const float flYawSide = vecRight.y * pCmd->sidemove;			//	chunky
	const float flRollUp = vecUp.z * pCmd->sidemove;				//	chunky
	const float x = vecOldForward.x * flPitchSide + vecOldForward.y * flYawSide + vecOldForward.x * flPitchForward + vecOldForward.y * flYawForward + vecOldForward.z * flRollUp;
	const float y = vecOldRight.x * flPitchSide + vecOldRight.y * flYawSide + vecOldRight.x * flPitchForward + vecOldRight.y * flYawForward + vecOldRight.z * flRollUp;
	const float z = vecOldUp.x * flYawSide + vecOldUp.y * flPitchSide + vecOldUp.x * flYawForward + vecOldUp.y * flPitchForward + vecOldUp.z * flRollUp;
	pCmd->forwardmove = std::clamp( x, -flMaxForwardSpeed, flMaxForwardSpeed );
	pCmd->sidemove = std::clamp( y, -flMaxSideSpeed, flMaxSideSpeed );
	pCmd->upmove = std::clamp( z, -flMaxUpSpeed, flMaxUpSpeed );	//	not a good idea
}

float __antiaim::EdgeDistance( float edgeRayYaw ) {
	// Main ray tracing area
	CGameTrace trace;
	Ray_t ray;
	vector forward;
	const float sy = sinf( DEG2RAD( edgeRayYaw ) ); // yaw
	const float cy = cosf( DEG2RAD( edgeRayYaw ) );
	constexpr float sp = 0.f; // pitch: sinf(DEG2RAD(0))
	constexpr float cp = 1.f; // cosf(DEG2RAD(0))
	forward.x = cp * cy;
	forward.y = cp * sy;
	forward.z = -sp;
	forward = forward * 300.0f + GET_LOCAL_PLAYER( )->GetShootPosition( );
	ray.Init( GET_LOCAL_PLAYER( )->GetShootPosition( ), forward );
	// trace::g_pFilterNoPlayer to only focus on the enviroment
	CTraceFilterWorldAndPropsOnly Filter = {};
	i::engine_trace->TraceRay( ray, 0x4200400B, &Filter, &trace );

	const float edgeDistance = ( trace.m_vStartPos - trace.m_vEndPos ).length_2d( );
	return edgeDistance;
}

bool __antiaim::FindEdge( float edgeOrigYaw ) {
	// distance two vectors and report their combined distances
	float edgeLeftDist = EdgeDistance( edgeOrigYaw - 21 );
	edgeLeftDist = edgeLeftDist + EdgeDistance( edgeOrigYaw - 27 );
	float edgeRightDist = EdgeDistance( edgeOrigYaw + 21 );
	edgeRightDist = edgeRightDist + EdgeDistance( edgeOrigYaw + 27 );

	// If the distance is too far, then set the distance to max so the angle
	// isnt used
	if ( edgeLeftDist >= 260 ) { edgeLeftDist = 999999999.f; }
	if ( edgeRightDist >= 260 ) { edgeRightDist = 999999999.f; }

	// If none of the vectors found a wall, then dont edge
	if ( math.compare_float( edgeLeftDist, edgeRightDist ) ) { return false; }

	// Depending on the edge, choose a direction to face
	if ( edgeRightDist < edgeLeftDist ) {
		edgeToEdgeOn = 1;
		return true;
	}

	edgeToEdgeOn = 2;

	return true;
}

std::pair<float, float> __antiaim::get_rage_pitch( )
{
	const auto& pLocal = entity_cache.get_local( );
	std::pair<float, float> retnAngles = { 0.f, 0.f };
	switch ( vars.antiaim.pitch )
	{
		case 1: //automatic
		{
			switch ( pLocal->m_iClass( ) )
			{
				case CLASS_SCOUT:
				case CLASS_SOLDIER:
				case CLASS_PYRO:
				case CLASS_DEMOMAN:
				case CLASS_ENGINEER:
				case CLASS_MEDIC:
				{
					retnAngles.first = 91.0f;
					retnAngles.second = 91.0f;
					break;
				}
				case CLASS_HEAVY:
				case CLASS_SNIPER:
				case CLASS_SPY:
				{
					retnAngles.first = -91.0f;
					retnAngles.second = -91.0f;
					break;
				}
				default: break;
			}
			break;
		}

		case 2: //manual
		{
			switch ( vars.antiaim.real_pitch )
			{
				case 0: //up
				{
					if ( vars.antiaim.fake_pitch )
					{
						retnAngles.first = 271.0f;
					}
					else
					{
						retnAngles.first = -91.0f;
					}
					retnAngles.second = -91.0f;
					break;
				}

				case 1: //down
				{
					if ( vars.antiaim.fake_pitch )
					{
						retnAngles.first = -271.0f;
					}
					else
					{
						retnAngles.first = 91.0f;
					}
					retnAngles.second = 91.0f;
					break;
				}

				case 2: //jitter
				{
					if ( bPitchJitter )
					{
						retnAngles.first = -91.0f;
						retnAngles.second = -91.0f;
					}
					else
					{
						retnAngles.first = 271.0f;
						retnAngles.second = -91.0f;
					}
					/*if ( g_GlobalInfo.currentUserCmd->command_number % 5 == 0 )
					{
						retnAngles.first = 0.0f;
						retnAngles.second = 0.0f;
					}
					else if ( g_GlobalInfo.currentUserCmd->command_number % 4 == 0 )
					{
						retnAngles.first = -45.0f;
						retnAngles.second = -45.0f;
					}
					else if ( g_GlobalInfo.currentUserCmd->command_number % 3 == 0 )
					{
						retnAngles.first = 89.0f;
						retnAngles.second = 89.0f;
					}
					else if ( g_GlobalInfo.currentUserCmd->command_number % 2 == 0 )
					{
						retnAngles.first = -89.0f;
						retnAngles.second = -89.0f;
					}*/
					break;
				}
				default: break;
			}
			break;
		}
		default: break;
	}
	return retnAngles;
}

float __antiaim::get_real_rage_yaw( )
{
	float& lastAngleRef = bPacketFlip ? lastRealAngle : lastFakeAngle;
	float& lastAngleRefRotate = lastRealAngle;
	static float angle = 0.f;
	float retnAngle = 0.f;
	switch ( vars.antiaim.real_mode )
	{
		case 0: //static
		{
			retnAngle = vars.antiaim.real_yaw;
			if ( retnAngle < 1 && retnAngle > -1 )
				retnAngle = 0.1f;
			if ( bRealFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			break;
		}
		case 1: //edge
		{
			if ( edgeToEdgeOn == 1 ) { retnAngle = 90.f; }
			else if ( edgeToEdgeOn == 2 ) { retnAngle = -90.0f; }
			break;
		}
		case 2: //jitter
		{
			if ( bRealJitter )
			{
				if ( bRealFlip )
				{
					retnAngle += vars.antiaim.real_delta;
				}
				else
				{
					retnAngle -= vars.antiaim.real_delta;
				}
			}
			else
			{
				if ( bRealFlip )
				{
					retnAngle -= vars.antiaim.real_delta;
				}
				else
				{
					retnAngle += vars.antiaim.real_delta;
				}
			}
			retnAngle += vars.antiaim.real_yaw;
			if ( bRealFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			if ( retnAngle > 180.f ) { retnAngle -= 360.f; }
			if ( retnAngle < -180.f ) { retnAngle += 360.f; }
			break;
		}
		case 3: //spin
		{
			lastAngleRef += ( bPacketFlip ? +1 : -1 ) * vars.antiaim.real_spin;
			if ( lastAngleRef > 180.f ) { lastAngleRef -= 360.f; }
			if ( lastAngleRef < -180.f ) { lastAngleRef += 360.f; }
			retnAngle = lastAngleRef;
			break;
		}
		case 4: //rotate
		{
			static bool flip = false;

			if ( vars.antiaim.real_angle1 < vars.antiaim.real_angle2 )
			{
				if ( lastAngleRefRotate <= vars.antiaim.real_angle1 )
					flip = true;

				if ( lastAngleRefRotate >= vars.antiaim.real_angle2 )
					flip = false;
			}
			else
			{
				if ( lastAngleRefRotate <= vars.antiaim.real_angle2 )
					flip = true;

				if ( lastAngleRefRotate >= vars.antiaim.real_angle1 )
					flip = false;
			}

			if ( flip )
			{
				lastAngleRefRotate += 1 * vars.antiaim.real_rotate;
			}
			else
			{
				lastAngleRefRotate -= 1 * vars.antiaim.real_rotate;
			}

			if ( lastAngleRefRotate > 180.f ) { lastAngleRefRotate -= 360.f; }
			if ( lastAngleRefRotate < -180.f ) { lastAngleRefRotate += 360.f; }
			retnAngle = lastAngleRefRotate;
			if ( bRealFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			retnAngle += vars.antiaim.real_yaw;
			break;
		}
		default: break;
	}
	if ( retnAngle < 1 && retnAngle > -1 )
		retnAngle = 0.1f;

	return retnAngle;
}

float __antiaim::get_fake_rage_yaw( )
{
	float& lastAngleRef = bPacketFlip ? lastRealAngle : lastFakeAngle;
	float& lastAngleRefRotate = lastFakeAngle;
	float retnAngle = 0.f;
	switch ( vars.antiaim.fake_mode )
	{
		case 0: //static
		{
			retnAngle = vars.antiaim.fake_yaw;
			if ( retnAngle < 1 && retnAngle > -1 )
				retnAngle = 0.1f;
			if ( bFakeFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			break;
		}
		case 1: //edge
		{
			if ( edgeToEdgeOn == 1 ) { retnAngle = -90.f; }
			else if ( edgeToEdgeOn == 2 ) { retnAngle = 90.0f; }
			break;
		}
		case 2: //jitter
		{
			if ( bFakeJitter )
			{
				if ( bFakeFlip )
				{
					retnAngle += vars.antiaim.fake_delta;
				}
				else
				{
					retnAngle -= vars.antiaim.fake_delta;
				}
			}
			else
			{
				if ( bFakeFlip )
				{
					retnAngle -= vars.antiaim.fake_delta;
				}
				else
				{
					retnAngle += vars.antiaim.fake_delta;
				}
			}
			retnAngle += vars.antiaim.fake_yaw;
			if ( bFakeFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			if ( retnAngle > 180.f ) { retnAngle -= 360.f; }
			if ( retnAngle < -180.f ) { retnAngle += 360.f; }
			break;
		}
		case 3: //spin
		{
			lastAngleRef += ( bPacketFlip ? +1 : -1 ) * vars.antiaim.fake_spin;
			if ( lastAngleRef > 180.f ) { lastAngleRef -= 360.f; }
			if ( lastAngleRef < -180.f ) { lastAngleRef += 360.f; }
			retnAngle = lastAngleRef;
			break;
		}
		case 4: //rotate
		{
			/*static bool flip = false;
			if ( lastAngleRef <= vars.antiaim.FakeAngle1 )
				flip = true;
			else if ( lastAngleRef >= vars.antiaim.FakeAngle2 )
				flip = false;

			if ( flip )
			{
				lastAngleRef += 1.f * vars.antiaim.FakeRotate;
			}
			else
			{
				lastAngleRef -= 1.f * vars.antiaim.FakeRotate;
			}
			if ( lastAngleRef > 180.f ) { lastAngleRef -= 360.f; }
			if ( lastAngleRef < -180.f ) { lastAngleRef += 360.f; }
			retnAngle = lastAngleRef;
			retnAngle += vars.antiaim.FakeYaw;
			if ( bFakeFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			break;*/
			static bool flip = false;

			if ( vars.antiaim.fake_angle1 < vars.antiaim.fake_angle2 )
			{
				if ( lastAngleRefRotate <= vars.antiaim.fake_angle1 )
					flip = true;

				if ( lastAngleRefRotate >= vars.antiaim.fake_angle2 )
					flip = false;
			}
			else
			{
				if ( lastAngleRefRotate <= vars.antiaim.fake_angle2 )
					flip = true;

				if ( lastAngleRefRotate >= vars.antiaim.fake_angle1 )
					flip = false;
			}

			if ( flip )
			{
				lastAngleRefRotate += 1 * vars.antiaim.fake_rotate;
			}
			else
			{
				lastAngleRefRotate -= 1 * vars.antiaim.fake_rotate;
			}

			if ( lastAngleRefRotate > 180.f ) { lastAngleRefRotate -= 360.f; }
			if ( lastAngleRefRotate < -180.f ) { lastAngleRefRotate += 360.f; }
			retnAngle = lastAngleRefRotate;
			if ( bFakeFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			retnAngle += vars.antiaim.fake_yaw;
		}
		default: break;
	}
	if ( retnAngle < 1 && retnAngle > -1 )
		retnAngle = 0.1f;

	return retnAngle;
}

float __antiaim::get_legit_yaw( )
{
	float retnAngle = 0.f;
	switch ( vars.antiaim.legit_yaw_mode )
	{
		case 0: //static
		{
			retnAngle = vars.antiaim.legit_yaw;
			if ( retnAngle < 1 && retnAngle > -1 )
				retnAngle = 0.1f;
			if ( bRealFlip )
			{
				if ( retnAngle > 0.f ) { retnAngle -= 180.f; }
				else if ( retnAngle < 0.f ) { retnAngle += 180.f; }
			}
			break;
		}

		case 1: //edge
		{
			if ( edgeToEdgeOn == 1 ) { retnAngle = bPacketFlip ? 90.f : -90.f; }
			else if ( edgeToEdgeOn == 2 ) { retnAngle = bPacketFlip ? -90.0f : 90.f; }
			break;
		}
		default: break;
	}
	return retnAngle;
}

void __antiaim::run_real( C_TFPlayer* pLocal, C_TFWeaponBase* weapon, CUserCmd* pCmd, bool* pSendPacket )
{
	global_info.antiaim_active = false;
	global_info.real_viewangles = global_info.viewangles;
	//global_info.fake_viewangles = global_info.viewangles;
	global_info.antiaim = { false, false };

	if ( !vars.antiaim.antiaim_active || aimbot.IsAttacking( pCmd, weapon )/* || g_GlobalInfo.m_bForceSendPacket || g_GlobalInfo.m_bAvoidingBackstab*/ || global_info.shifting ) { return; }

	// AA toggle key
	if ( vars.antiaim.toggle_key && ( GetAsyncKeyState( vars.antiaim.toggle_key ) & 0x1 ) )
	{
		toggle = !toggle;
	}

	if ( toggle ) {
		if ( pLocal->deadflag( )
			 || pLocal->InCond( TF_COND_TAUNTING )
			 || pLocal->InCond( TF_COND_HALLOWEEN_GHOST_MODE )
			 || pLocal->InCond( TF_COND_HALLOWEEN_KART ) ) {
			return;
		}

		if ( pLocal->GetMoveType( ) == MOVETYPE_NOCLIP
			 || pLocal->GetMoveType( ) == MOVETYPE_LADDER
			 || pLocal->GetMoveType( ) == MOVETYPE_OBSERVER )
		{
			return;
		}

		if ( pLocal->InCond( TF_COND_SHIELD_CHARGE ) ) {
			return;
		}

		if ( ( pCmd->buttons & IN_ATTACK ) ) { return; }
		const vector vOldAngles = pCmd->viewangles;
		const float fOldSideMove = pCmd->sidemove;
		const float fOldForwardMove = pCmd->forwardmove;
		bool bYawSet = false;
		bool bPitchSet = vars.antiaim.pitch;
		if ( vars.antiaim.rage )
		{
			if ( GetAsyncKeyState( vars.antiaim.cycle_real_key ) & 0x1 )
			{
				bRealFlip = !bRealFlip;
			}

			bPitchJitter = !bPitchJitter;
			if ( vars.antiaim.pitch ) {
				const std::pair<float, float> angPair = get_rage_pitch( );
				pCmd->viewangles.x = angPair.first;
				//global_info.real_viewangles.x = angPair.second;
			}

			if ( vars.antiaim.real_mode == 1 || vars.antiaim.fake_mode == 1 )
			{
				FindEdge( pCmd->viewangles.y );
			}

			bYawSet = get_real_rage_yaw( );

			bRealJitter = !bRealJitter;
			float angOffset = get_real_rage_yaw( );
			pCmd->viewangles.y += angOffset;
			//global_info.real_viewangles.y = pCmd->viewangles.y;
			global_info.antiaim.first = true;
			//if ( bYawSet ) { *pSendPacket = bPacketFlip = !bPacketFlip; }
		}
		else
		{
			if ( GetAsyncKeyState( vars.antiaim.cycle_key ) & 0x1 )
			{
				bRealFlip = !bRealFlip;
			}

			if ( vars.antiaim.legit_yaw_mode == 1 )
			{
				FindEdge( pCmd->viewangles.y );
			}

			bYawSet = get_legit_yaw( );

			float angOffset = get_legit_yaw( );
			pCmd->viewangles.y += angOffset;
			//global_info.real_viewangles.y = pCmd->viewangles.y;
			global_info.antiaim.first = true;
		}
		global_info.antiaim_active = bYawSet || bPitchSet;

		fix_movement( pCmd, vOldAngles, fOldSideMove, fOldForwardMove );
	}
}

void __antiaim::run_fake( C_TFPlayer* pLocal, C_TFWeaponBase* weapon, CUserCmd* pCmd, bool* pSendPacket )
{
	global_info.antiaim_active = false;
	//global_info.real_viewangles = global_info.viewangles;
	global_info.fake_viewangles = global_info.viewangles;
	global_info.antiaim = { false, false };

	if ( !vars.antiaim.antiaim_active || aimbot.IsAttacking( pCmd, weapon )/*|| global_info.scout_jumping  || g_GlobalInfo.m_bForceSendPacket || g_GlobalInfo.m_bAvoidingBackstab*/ || global_info.shifting ) { return; }

	// AA toggle key
	if ( vars.antiaim.toggle_key && ( GetAsyncKeyState( vars.antiaim.toggle_key ) & 0x1 ) )
	{
		toggle = !toggle;
	}

	if ( toggle ) {
		if ( pLocal->deadflag( )
			 || pLocal->InCond( TF_COND_TAUNTING )
			 || pLocal->InCond( TF_COND_HALLOWEEN_GHOST_MODE )
			 || pLocal->InCond( TF_COND_HALLOWEEN_KART ) ) {
			return;
		}

		if ( pLocal->GetMoveType( ) == MOVETYPE_NOCLIP
			 || pLocal->GetMoveType( ) == MOVETYPE_LADDER
			 || pLocal->GetMoveType( ) == MOVETYPE_OBSERVER )
		{
			return;
		}

		if ( pLocal->InCond( TF_COND_SHIELD_CHARGE ) ) {
			return;
		}

		if ( ( pCmd->buttons & IN_ATTACK ) ) { return; }
		const vector vOldAngles = pCmd->viewangles;
		const float fOldSideMove = pCmd->sidemove;
		const float fOldForwardMove = pCmd->forwardmove;
		bool bYawSet = false;
		bool bPitchSet = vars.antiaim.pitch;
		if ( vars.antiaim.rage )
		{
			if ( GetAsyncKeyState( vars.antiaim.cycle_fake_key ) & 0x1 )
			{
				bFakeFlip = !bFakeFlip;
			}

			if ( vars.antiaim.pitch ) {
				const std::pair<float, float> angPair = get_rage_pitch( );
				pCmd->viewangles.x = angPair.first;
				//global_info.real_viewangles.x = angPair.second;
			}

			if ( vars.antiaim.fake_mode == 1 )
			{
				FindEdge( pCmd->viewangles.y );
			}

			bYawSet = get_fake_rage_yaw( );

			bFakeJitter = !bFakeJitter;
			float angOffset = get_fake_rage_yaw( );
			pCmd->viewangles.y += angOffset;
			//global_info.fake_viewangles.y = pCmd->viewangles.y;
			global_info.antiaim.second = true;
		}
		else
		{
			bYawSet = true;
			pCmd->viewangles.y += 0.1f;
			//global_info.fake_viewangles.y = pCmd->viewangles.y;
			global_info.antiaim.second = true;
		}
		//if ( bYawSet ) { *pSendPacket = bPacketFlip = !bPacketFlip; }
		global_info.antiaim_active = bYawSet || bPitchSet;

		fix_movement( pCmd, vOldAngles, fOldSideMove, fOldForwardMove );
	}
}

/*void CAntiAimBeta::Run( CUserCmd* pCmd, bool* pSendPacket ) {
	global_info.antiaim_active = false;
	global_info.real_viewangles = global_info.viewangles;
	global_info.fake_viewangles = global_info.viewangles;
	global_info.antiaim = { false, false };

	// AA toggle key
	if ( GetAsyncKeyState( vars.antiaim.ToggleKey ) & 0x8000 )
	{
		vars.antiaim.Active = !vars.antiaim.Active;
	}

	if ( !vars.antiaim.Active /* || g_GlobalInfo.m_bForceSendPacket || g_GlobalInfo.m_bAvoidingBackstab ) { return; }

	if ( const auto& pLocal = entity_cache.get_local( ) ) {
		if ( pLocal->deadflag( )
			 || pLocal->InCond( TF_COND_TAUNTING )
			 || pLocal->InCond( TF_COND_HALLOWEEN_GHOST_MODE )
			 || pLocal->InCond( TF_COND_HALLOWEEN_KART ) ) {
			return;
		}

		if ( pLocal->GetMoveType( ) == MOVETYPE_NOCLIP
			 || pLocal->GetMoveType( ) == MOVETYPE_LADDER
			 || pLocal->GetMoveType( ) == MOVETYPE_OBSERVER )
		{
			return;
		}

		if ( pLocal->InCond( TF_COND_SHIELD_CHARGE ) ) {
			return;
		}

		if ( ( pCmd->buttons & IN_ATTACK ) && global_info.can_attack ) { return; }
		const vector vOldAngles = pCmd->viewangles;
		const float fOldSideMove = pCmd->sidemove;
		const float fOldForwardMove = pCmd->forwardmove;
		static float realangle = 0.f;
		static float fakeangle = 0.f;

		//rage
		bool bYawSet = false;
		bool bPitchSet = vars.antiaim.Pitch;
		if ( vars.antiaim.Rage )
		{
			if ( GetAsyncKeyState( vars.antiaim.CycleRealKey ) & 0x8000 )
			{
				bRealFlip = !bRealFlip;
			}

			if ( GetAsyncKeyState( vars.antiaim.CycleFakeKey ) & 0x8000 )
			{
				bFakeFlip = !bFakeFlip;
			}

			if ( vars.antiaim.Pitch ) {
				const std::pair<float, float> angPair = GetRagePitch( );
				pCmd->viewangles.x = angPair.first;
				global_info.real_viewangles.x = angPair.second;
			}

			if ( vars.antiaim.RealMode == 1 || vars.antiaim.FakeMode == 1 )
			{
				FindEdge( pCmd->viewangles.y );
			}

			bYawSet = bPacketFlip ? GetRealRageYaw( ) : GetFakeRageYaw( );

			if ( bPacketFlip ) //real
			{
				float angOffset = GetRealRageYaw( );
				pCmd->viewangles.y += angOffset;
				global_info.real_viewangles = pCmd->viewangles;
				global_info.antiaim.first = true;
			}
			else //fake
			{
				float angOffset = GetFakeRageYaw( );
				pCmd->viewangles.y += angOffset;
				global_info.fake_viewangles.y = pCmd->viewangles.y;
				global_info.antiaim.second = true;
			}
		}
		//legit
		else
		{
			if ( GetAsyncKeyState( vars.antiaim.CycleKey ) & 0x8000 )
			{
				bRealFlip = !bRealFlip;
			}

			if ( vars.antiaim.LegitYawMode == 1 )
			{
				FindEdge( pCmd->viewangles.y );
			}

			bYawSet = bPacketFlip ? GetLegitYaw( ) : 0.1f;

			if ( *pSendPacket ) //real
			{
				pCmd->viewangles.y += 0.1f;
				global_info.fake_viewangles.y = pCmd->viewangles.y;
				global_info.antiaim.second = true;
			}
			else //fake
			{
				float angOffset = GetLegitYaw( );
				/*if ( Vars::Misc::CL_Move::Fakelag )
				{
					if ( g_GlobalInfo.m_bChoking )
					{
						pCmd->viewangles.y += angOffset;
						realangle = pCmd->viewangles.y;
					}
					else
						pCmd->viewangles.y = realangle;
				}
				else
				{
					pCmd->viewangles.y += angOffset;
				}
				pCmd->viewangles.y += angOffset;
				global_info.real_viewangles = pCmd->viewangles;
				global_info.antiaim.first = true;
			}
		}

		//if ( bYawSet ) { *pSendPacket = bPacketFlip = !bPacketFlip; }
		global_info.antiaim_active = bYawSet || bPitchSet;

		FixMovement( pCmd, vOldAngles, fOldSideMove, fOldForwardMove );
	}
}*/