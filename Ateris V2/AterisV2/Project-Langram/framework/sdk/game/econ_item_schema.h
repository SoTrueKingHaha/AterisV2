#pragma once

#include "econ_item_constants.h"
#include "Steam/SteamTypes.h"


class CEconItemSchema;
class CEconItem;
class CEconSharedObjectCache;
class CSOItemRecipe;

union attribute_data_union_t
{
	float asFloat;
	uint32_t asUint32;
	uint8_t* asBlobPointer;
};

struct static_attrib_t
{
	static_attrib_t()
	{
		iDefIndex = 0;
		m_value.asBlobPointer = NULL;
#ifdef GC_DLL
		bForceGCToGenerate = false;
		m_pKVCustomData = NULL;
#endif // GC_DLL
	}

	~static_attrib_t()
	{
#ifdef GC_DLL
		if (m_pKVCustomData)
			m_pKVCustomData->deleteThis();
		m_pKVCustomData = NULL;
#endif
	}

	static_attrib_t(const static_attrib_t& rhs)
	{
		iDefIndex = rhs.iDefIndex;
		m_value = rhs.m_value;
#ifdef GC_DLL
		m_pKVCustomData = rhs.m_pKVCustomData ? rhs.m_pKVCustomData->MakeCopy() : NULL;
		bForceGCToGenerate = rhs.bForceGCToGenerate;
#endif
	}

	attrib_definition_index_t	iDefIndex;
	attribute_data_union_t m_value;
#ifdef GC_DLL
	bool	bForceGCToGenerate;
	KeyValues* m_pKVCustomData;
#endif // GC_DLL

	// Parses a single subsection from a multi-line attribute block that looks like:
	//
	//		"attributes"
	//		{
	//			"cannot trade"
	//			{
	//				"attribute_class"	"cannot_trade"
	//				"value"				"1"
	//			}
	//			"kill eater"
	//			{
	//				"attribute_class"	"kill_eater"
	//				"force_gc_to_generate" "1"
	//				"use_custom_logic"	"gifts_given_out"
	//			}
	//		}
	//
	// The "force_gc_to_generate" and "use_custom_logic" fields will only be parsed on the GC. Will return
	// true/false based on whether the whole attribute and value parsed successfully.
	bool BInitFromKV_MultiLine(const char* pszContext, KeyValues* pKVAttribute, CUtlVector<CUtlString>* pVecErrors);

	// Parses a single subsection from a single-line attribute block that looks like:
	//
	//		CharacterAttributes 
	//		{
	//			"increase buff duration"	9.0
	//			"damage bonus"	2.0 
	//		}
	//
	// It's impossible to specify GC-generated attributes in this format. Will return true/false based on
	// whether the whole attribute and value parsed successfully.
	bool BInitFromKV_SingleLine(const char* pszContext, KeyValues* pKVAttribute, CUtlVector<CUtlString>* pVecErrors, bool bEnableTerribleBackwardsCompatibilitySchemaParsingCode = true);

	// Data access helpers.
	const class CEconItemAttributeDefinition* GetAttributeDefinition() const;
	const class ISchemaAttributeType* GetAttributeType() const;
};

typedef	uint16_t	equipped_class_t;
typedef uint16_t	equipped_slot_t;
typedef uint8_t	equipped_preset_t;

#define INVALID_EQUIPPED_SLOT	((equipped_slot_t)-1)
#define INVALID_STYLE_INDEX		((style_index_t)-1)
#define INVALID_PRESET_INDEX	((equipped_preset_t)-1)

enum EEquipType_t
{
	EQUIP_TYPE_CLASS = 0,
	EQUIP_TYPE_ACCOUNT,

	EQUIP_TYPE_INVALID,
};

#define MUST_CHECK_RETURN 

class IEconItemPropertyGenerator
{
public:
	virtual ~IEconItemPropertyGenerator() {}

	MUST_CHECK_RETURN virtual bool BGenerateProperties(CEconItem* pItem) const = 0;
};

class CEconItemSeriesDefinition
{
public:
	CEconItemSeriesDefinition(void);
	CEconItemSeriesDefinition(const CEconItemSeriesDefinition& that);
	CEconItemSeriesDefinition& operator=(const CEconItemSeriesDefinition& rhs);

	~CEconItemSeriesDefinition(void) {}

	bool		BInitFromKV(KeyValues* pKVItem, CUtlVector<CUtlString>* pVecErrors = NULL);

	int32_t		GetDBValue(void) const { return m_nValue; }
	const char* GetName(void) const { return !m_strName.IsEmpty() ? m_strName.String() : "unknown"; }
	const char* GetLocKey(void) const { return !m_strLockKey.IsEmpty() ? m_strLockKey.String() : "unknown"; }
	const char* GetUiFile(void) const { return !m_strUiFile.IsEmpty() ? m_strUiFile.String() : "unknown"; }

private:

	// The value that the game/DB will know this series by
	int32_t		m_nValue;

	CUtlString	m_strName;			// Key Name
	CUtlString	m_strLockKey;		// Localization key
	CUtlString	m_strUiFile;		// Ui File (.res file)
};

class CEconItemRarityDefinition
{
public:
	CEconItemRarityDefinition(void);

	~CEconItemRarityDefinition(void) {}

	bool		BInitFromKV(KeyValues* pKVItem, KeyValues* pKVRarityWeights, CEconItemSchema& pschema, CUtlVector<CUtlString>* pVecErrors = NULL);

	int32_t		GetDBValue(void) const { return m_nValue; }
	const char* GetName(void) const { return !m_strName.IsEmpty() ? m_strName.String() : "unknown"; }
	const char* GetLocKey(void) const { return m_strLocKey.String(); }
	const char* GetWepLocKey(void) const { return m_strWepLocKey.String(); }
	const char* GetDropSound(void) const { return m_strDropSound.String(); }
	attrib_colors_t		GetAttribColor(void) const { return m_iAttribColor; }
	const char* GetNextRarity(void) const { return m_strNextRarity.String(); }
	int32_t		GetLootlistWeight(void) const { return m_nLootlistWeight; }

private:

	// The value that the game/DB will know this rarity by
	int32_t		m_nValue;

	attrib_colors_t		m_iAttribColor;

	// The English name of the rarity
	CUtlString	m_strName;

	// The localization key for this rarity.
	CUtlString  m_strLocKey;
	// The localization key for this rarity, for weapons.
	CUtlString  m_strWepLocKey;

	// The loot list name associated with this rarity.
	CUtlString  m_strDropSound;

	CUtlString  m_strNextRarity;

	int32_t		m_nLootlistWeight;
};

//-----------------------------------------------------------------------------
// CEconItemQualityDefinition
// Template Definition of a randomly created item
//-----------------------------------------------------------------------------
class CEconItemQualityDefinition
{
public:
	CEconItemQualityDefinition(void);
	CEconItemQualityDefinition(const CEconItemQualityDefinition& that);
	CEconItemQualityDefinition& operator=(const CEconItemQualityDefinition& rhs);

	~CEconItemQualityDefinition(void) {}

	bool		BInitFromKV(KeyValues* pKVItem, CUtlVector<CUtlString>* pVecErrors = NULL);


	int32_t		GetDBValue(void) const { return m_nValue; }
	const char* GetName(void) const { return !m_strName.IsEmpty() ? m_strName.Get() : "unknown"; }
	bool		CanSupportSet(void) const { return m_bCanSupportSet; }
	const char* GetHexColor(void) const { return !m_strHexColor.IsEmpty() ? m_strHexColor.Get() : "B2B2B2"; }

#ifdef DBGFLAG_VALIDATE
	void Validate(CValidator& validator, const char* pchName)
	{
		VALIDATE_SCOPE();
		ValidateObj(m_strName);
	}
#endif // DBGFLAG_VALIDATE

private:

	// The value that the game/DB will know this quality by
	int32_t			m_nValue;

	// The English name of the quality
	CUtlConstString	m_strName;

	// if this is true the support tool is allowed to set this quality level on any item
	bool			m_bCanSupportSet;

	// A hex string representing the color this quality should display as. Used primarily for display on the Web.
	CUtlConstString	m_strHexColor;
};

class CEconColorDefinition
{
public:
	bool		BInitFromKV(KeyValues* pKVColor, CUtlVector<CUtlString>* pVecErrors = NULL);

	const char* GetName(void) const { return m_strName.Get(); }
	const char* GetColorName(void) const { return m_strColorName.Get(); }		// meant for passing into VGUI styles, etc.
	const char* GetHexColor(void) const { return m_strHexColor.Get(); }

private:
	// The English name of this color. Only used for lookup.
	CUtlConstString m_strName;

	// The VGUI name of the color in our schema. This will be used to set values
	// for VGUI controls.
	CUtlConstString m_strColorName;

	// The hex string value of this color. This will be used for Web display.
	CUtlConstString m_strHexColor;
};

class CEconItemSetDefinition
{
public:
	CEconItemSetDefinition(void);
	CEconItemSetDefinition(const CEconItemSetDefinition& that);
	CEconItemSetDefinition& operator=(const CEconItemSetDefinition& rhs);

	~CEconItemSetDefinition(void) {}

	bool	BInitFromKV(KeyValues* pKVItemSet, CUtlVector<CUtlString>* pVecErrors = NULL);

	void	IterateAttributes(class IEconItemAttributeIterator* pIterator) const;

public:

	const char* m_pszName;
	const char* m_pszLocalizedName;
	CUtlVector<item_definition_index_t>		m_iItemDefs;
	int										m_iBundleItemDef;	// Item def of the store bundle for this set, if any
	bool									m_bIsHiddenSet;		// If true, this set and any bonuses will only be visible if the whole set is equipped.

	struct itemset_attrib_t
	{
		attrib_definition_index_t		m_iAttribDefIndex;
		float							m_flValue;
	};
	CUtlVector<itemset_attrib_t>	m_iAttributes;
};

//-----------------------------------------------------------------------------
class CEconItemCollectionDefinition
{
public:
	CEconItemCollectionDefinition(void);
	~CEconItemCollectionDefinition(void) {}

	bool	BInitFromKV(KeyValues* pKVItemCollection, CUtlVector<CUtlString>* pVecErrors = NULL);

	uint8_t	GetMinRarity() const { return m_iRarityMin; }
	uint8_t	GetMaxRarity() const { return m_iRarityMax; }

public:

	const char* m_pszName;
	const char* m_pszLocalizedName;
	const char* m_pszLocalizedDesc;
	CUtlVector<item_definition_index_t>		m_iItemDefs;

private:
	bool	m_bIsReferenceCollection;

	uint8_t		m_iRarityMin;
	uint8_t		m_iRarityMax;
};

class CEconItemPaintKitDefinition
{
public:
	CEconItemPaintKitDefinition(void);
	~CEconItemPaintKitDefinition(void);

private:
	const char* m_pszName;
	const char* m_pszLocalizedName;

	CUtlVector< KeyValues* >			m_vecPaintKitWearKVP;
};

//-----------------------------------------------------------------------------
class CEconOperationDefinition
{
public:
	CEconOperationDefinition(void);
	~CEconOperationDefinition(void);

	bool	BInitFromKV(KeyValues* pKVOperation, CUtlVector<CUtlString>* pVecErrors = NULL);

	const char* GetName() const { return m_pszName; }
	operation_definition_index_t GetOperationID() const { return m_unOperationID; }
	item_definition_index_t GetRequiredItemDefIndex() const { return m_unRequiredItemDefIndex; }
	item_definition_index_t GetGatewayItemDefIndex() const { return m_unGatewayItemDefIndex; }

	KeyValues* GetKVP() { return m_pKVItem; }

	const char* GetQuestLogOverrideResFile() const { return m_pszQuestLogResFile; }
	const char* GetQuestListOverrideResFile() const { return m_pszQuestListResFile; }

	RTime32	GetStartDate() const { return m_OperationStartDate; }
	RTime32 GetStopGivingToPlayerDate() const { return m_StopGivingToPlayerDate; }
	RTime32 GetStopAddingToQueueDate() const { return m_StopAddingToQueueDate; }

	const char* GetOperationLootlist() const { return m_pszOperationLootList; }
	bool	IsCampaign() const { return m_bIsCampaign; }
	uint32	GetMaxDropCount() const { return m_unMaxDropCount; }

#ifdef GC_DLL
	enum EContractRewardLootlist_t
	{
		REWARD_CASE,
		REWARD_WEAPON,

		NUM_REWARDS
	};
	const char* GetContractRewardLootlist(EContractRewardLootlist_t eType) const { return m_pszContractRewardLootlist[eType]; }

	RTime32	GetMinQueueFreq() const;
	RTime32	GetMaxQueueFreq() const;
	RTime32	GetMinDropFreq() const;
	RTime32	GetMaxDropFreq() const;

	uint8	GetNumSeededContracts() const { return m_unSeed; }
	uint16	GetNumMaxHeldDrops() const { return m_unMaxHeldDrops; }

	int		GetNumMaxQueueCount() const { return m_nMaxQueueCount; }

	uint8	GetMaxDropPerThink() const { return m_unMaxDropPerThink; }

#endif // GC_DLL

private:
	const char* m_pszName;
	operation_definition_index_t	m_unOperationID;

	// things operation periodically drops
	const char* m_pszOperationLootList;
	bool				m_bIsCampaign;
	uint32				m_unMaxDropCount;

	const char* m_pszQuestLogResFile;
	const char* m_pszQuestListResFile;

	item_definition_index_t m_unRequiredItemDefIndex;
	item_definition_index_t m_unGatewayItemDefIndex; // Defindex of the item users need to acquire in order to get the required item.  Could be the required item itself.

	RTime32				m_OperationStartDate;		// when the operation starts and gives out rewards
	RTime32				m_StopGivingToPlayerDate;	// when the operation stops giving quests to player
	RTime32				m_StopAddingToQueueDate;	// when the operation stops adding more quests to the bucket

#ifdef GC_DLL
	const char* m_pszContractRewardLootlist[NUM_REWARDS];

	// in seconds
	RTime32				m_rtQueueFreqMin;
	RTime32				m_rtQueueFreqMax;
	RTime32				m_rtDropFreqMin;
	RTime32				m_rtDropFreqMax;

	uint8				m_unSeed;
	uint16				m_unMaxHeldDrops;
	int					m_nMaxQueueCount;

	uint8				m_unMaxDropPerThink;

#endif // GC_DLL

	KeyValues* m_pKVItem;
};

//-----------------------------------------------------------------------------
// CEconLootListDefinition
// Definition of a loot list
//-----------------------------------------------------------------------------
class IEconLootList
{
public:
	virtual ~IEconLootList() {}

	MUST_CHECK_RETURN virtual bool BPublicListContents() const = 0;
	MUST_CHECK_RETURN virtual const char* GetLootListHeaderLocalizationKey() const = 0;
	MUST_CHECK_RETURN virtual const char* GetLootListFooterLocalizationKey() const = 0;
	MUST_CHECK_RETURN virtual const char* GetLootListCollectionReference() const = 0;

	class IEconLootListIterator
	{
	public:
		virtual ~IEconLootListIterator() {}
		virtual void OnIterate(item_definition_index_t unItemDefIndex) = 0;
	};

	virtual void EnumerateUserFacingPotentialDrops(IEconLootListIterator* pIt) const = 0;

#ifdef GC_DLL
	MUST_CHECK_RETURN virtual bool BGenerateSingleRollRandomItems(const CEconGameAccount* pGameAccount, bool bFreeAccount, CUtlVector<CEconItem*>* out_pvecItems, const CUtlVector< item_definition_index_t >* pVecAvoidItemDefs = NULL) const = 0;
#endif // GC_DLL
};

//-----------------------------------------------------------------------------
// Purpose: Attribute definition details
//-----------------------------------------------------------------------------
enum
{
	ATTDESCFORM_VALUE_IS_PERCENTAGE,			// Printed as:	((m_flValue*100)-100.0)
	ATTDESCFORM_VALUE_IS_INVERTED_PERCENTAGE,	// Printed as:	((m_flValue*100)-100.0) if it's > 1.0, or ((1.0-m_flModifier)*100) if it's < 1.0
	ATTDESCFORM_VALUE_IS_ADDITIVE,				// Printed as:	m_flValue
	ATTDESCFORM_VALUE_IS_ADDITIVE_PERCENTAGE,	// Printed as:	(m_flValue*100)
	ATTDESCFORM_VALUE_IS_OR,					// Printed as:  m_flValue, but results are ORd together instead of added
	ATTDESCFORM_VALUE_IS_DATE,					// Printed as a date
	ATTDESCFORM_VALUE_IS_ACCOUNT_ID,			// Printed as steam user name
	ATTDESCFORM_VALUE_IS_PARTICLE_INDEX,		// Printed as a particle description
	ATTDESCFORM_VALUE_IS_KILLSTREAKEFFECT_INDEX,// Printed as killstreak effect description
	ATTDESCFORM_VALUE_IS_KILLSTREAK_IDLEEFFECT_INDEX,  // Printed as idle effect description
	ATTDESCFORM_VALUE_IS_ITEM_DEF,				// Printed as item name
	ATTDESCFORM_VALUE_IS_FROM_LOOKUP_TABLE,		// Printed as a string from a lookup table, specified by the attribute definition name
};

// Coloring for attribute lines
enum attrib_effect_types_t
{
	ATTRIB_EFFECT_UNUSUAL = 0,
	ATTRIB_EFFECT_STRANGE,
	ATTRIB_EFFECT_NEUTRAL,
	ATTRIB_EFFECT_POSITIVE,
	ATTRIB_EFFECT_NEGATIVE,

	NUM_EFFECT_TYPES,
};

enum EAssetClassAttrExportRule_t
{
	k_EAssetClassAttrExportRule_Default = 0,
	k_EAssetClassAttrExportRule_Bucketed = (1 << 0),	// attribute exports bucketed value to Steam Community
	k_EAssetClassAttrExportRule_Skip = (1 << 1),	// attribute value is not exported to Steam Community
	k_EAssetClassAttrExportRule_GCOnly = (1 << 2),	// attribute only lives on GC and not exported to any external request
};

class CEconItemAttributeDefinition
{
public:
	CEconItemAttributeDefinition(void);
	CEconItemAttributeDefinition(const CEconItemAttributeDefinition& that);
	CEconItemAttributeDefinition& operator=(const CEconItemAttributeDefinition& rhs);

	~CEconItemAttributeDefinition(void);

	bool	BInitFromKV(KeyValues* pKVAttribute, CUtlVector<CUtlString>* pVecErrors = NULL);

	attrib_definition_index_t GetDefinitionIndex(void) const { return m_nDefIndex; }
	// Attribute name referenced in the db.
	const char* GetDefinitionName(void) const { return m_pszDefinitionName; }

	KeyValues* GetRawDefinition(void) const { return m_pKVAttribute; }

	// Data accessing
	bool		IsHidden(void) const { return m_bHidden; }
	bool		BForceWebSchemaOutput(void) const { return m_bWebSchemaOutputForced; }
	bool		BIsSetBonusAttribute(void) const { return m_bIsSetBonus; }
	bool		CanAffectMarketName(void) const { return m_bCanAffectMarketName; }
	bool		CanAffectRecipeComponentName(void) const { return m_bCanAffectRecipeComponentName; }
	bool		IsStoredAsInteger(void) const { return m_bStoredAsInteger; }
	bool		IsStoredAsFloat(void) const { return !m_bStoredAsInteger; }
	int			GetUserGenerationType(void) const { return m_iUserGenerationType; }
	bool		IsInstanceData() const { return m_bInstanceData; }
	EAssetClassAttrExportRule_t GetAssetClassAttrExportRule() const { return m_eAssetClassAttrExportRule; }
	uint32		GetAssetClassBucket() const { return m_unAssetClassBucket; }
	int			GetDescriptionFormat(void) const { return m_iDescriptionFormat; }
	const char* GetDescriptionString(void) const { return m_pszDescriptionString; }
	const char* GetArmoryDescString(void) const { return m_pszArmoryDesc; }
	const char* GetAttributeClass(void) const { return m_pszAttributeClass; }
	econ_tag_handle_t GetItemDefinitionTag(void) const { return m_ItemDefinitionTag; }
	attrib_effect_types_t GetEffectType(void) const { return m_iEffectType; }

	const class ISchemaAttributeType* GetAttributeType(void) const { return m_pAttrType; }

#ifndef GC_DLL
	void		ClearStringCache(void) const { m_iszAttributeClass = NULL_STRING; }
	string_t	GetCachedClass(void) const
	{
		return m_iszAttributeClass;
	}
#endif

#ifdef DBGFLAG_VALIDATE
	void Validate(CValidator& validator, const char* pchName)
	{
		VALIDATE_SCOPE();
		ValidatePtr(m_pKVAttribute);
	}
#endif // DBGFLAG_VALIDATE

private:
	// The raw keyvalues for this attribute definition.
	KeyValues* m_pKVAttribute;

	// Required valued from m_pKVAttribute:

	// The number used to refer to this definition in the DB
	attrib_definition_index_t	m_nDefIndex;

	// A pointer to the schema-global type data for this attribute. This maps attribute types to functionality
	// for loading/storing, both to memory and the DB.
	const class ISchemaAttributeType* m_pAttrType;

	// ---------------------------------------------
	// Display related data
	// ---------------------------------------------
	// If true, this attribute isn't shown in the item description
	bool		m_bHidden;

	// If true, this attribute's description is always output in web api calls regardless of the hidden flag.
	bool		m_bWebSchemaOutputForced;

	// Whether or not the value is stored as an integer in the DB.
	bool		m_bStoredAsInteger;

	// If this is true the attribute is counted as "instance" data for purposes of asset class in the Steam Economy. Non-instance
	// properties are considered things that can differentiate items at a fundamental level (ie., definition index, quality); instance
	// properties are more things like additional customizations -- score for strange items, paint color, etc.
	bool		m_bInstanceData;
	EAssetClassAttrExportRule_t	m_eAssetClassAttrExportRule;			// if this is true the attribute will not be exported for asset class
	uint32		m_unAssetClassBucket;									// if this is set then attribute value is bucketed when exported for asset class

	// Set item bonus attributes use a different attribute parser and make assumptions about memory layout. We
	// don't really use these for any new content currently and it isn't worth touching all the old code.
	//
	// At runtime, this flag is used to determine whether or not to rebuild dynamic attributes attached to
	// players on respawn.
	bool		m_bIsSetBonus;

	// Whether or not this attribute is supposed to only come from user actions. These attributes are used for
	// player item upgrades, etc. and cannot be set on items directly in the schema.
	int			m_iUserGenerationType;

	// Overall positive/negative effect. Used to color the attribute.
	attrib_effect_types_t m_iEffectType;

	// Contains the description format & string for this attribute
	int			m_iDescriptionFormat;
	const char* m_pszDescriptionString;

	// Contains information on how to describe items with this attribute in the Armory
	const char* m_pszArmoryDesc;

	// Used to allow unique items to specify attributes by name.
	const char* m_pszDefinitionName;

	// The class name of this attribute. Used in creation, and to hook the attribute into the actual code that uses it.
	const char* m_pszAttributeClass;

	// Allowed to affect the market bucketization name.  We dont want things like the strange level to affect the name,
	// but we do want things like crate series number and strangifier targets to get their own buckets.
	bool		m_bCanAffectMarketName;

	// Allowed to list itself in the name of an item in the recipe component description.
	bool		m_bCanAffectRecipeComponentName;

	// Do item definitions with this attribute specified automatically get an additional tag applied?
	econ_tag_handle_t	m_ItemDefinitionTag;

#ifndef GC_DLL
	mutable string_t	m_iszAttributeClass;	// Same as the above, but used for fast lookup when applying attributes.
#endif
};

//-----------------------------------------------------------------------------
// Visual data storage in item definitions
//-----------------------------------------------------------------------------
#define TEAM_VISUAL_SECTIONS			5
#define MAX_VISUALS_CUSTOM_SOUNDS		10

struct attachedparticlesystem_t
{
	attachedparticlesystem_t() :
		pszSystemName(NULL)
		, bFollowRootBone(NULL)
		, iCustomType(0)
		, nSystemID(0)
		, fRefireTime(0)			// only works for taunt effects, currently
		, bDrawInViewModel(false)
		, bUseSuffixName(false)
		, bHasViewModelSpecificEffect(false)
	{
		memset(pszControlPoints, 0, sizeof(pszControlPoints));
	}

	const char* pszSystemName;
	bool		bFollowRootBone;
	int			iCustomType;
	int			nSystemID;
	float		fRefireTime;				// only works for taunt effects, currently
	bool		bDrawInViewModel;
	bool		bUseSuffixName;
	bool		bHasViewModelSpecificEffect;

	const char* pszControlPoints[7];
};

enum
{
	kAttachedModelDisplayFlag_WorldModel = 0x01,
	kAttachedModelDisplayFlag_ViewModel = 0x02,

	kAttachedModelDisplayFlag_MaskAll = kAttachedModelDisplayFlag_WorldModel | kAttachedModelDisplayFlag_ViewModel,
};

struct attachedmodel_t
{
	const char* m_pszModelName;
	int m_iModelDisplayFlags;
};

enum wearableanimplayback_t
{
	WAP_ON_SPAWN,				// Play this animation immediately on spawning the wearable
	WAP_START_BUILDING,			// Game code will start this anim whenever a player wearing this item deploys their builder weapon.
	WAP_STOP_BUILDING,			// Game code will start this anim whenever a player wearing this item holsters their builder weapon.
	WAP_START_TAUNTING,			// Game code will start this anim whenever a player wearing this item taunts
	WAP_STOP_TAUNTING,				// Game code will start this anim whenever a player wearing this item stops taunting

	NUM_WAP_TYPES,
};

struct animation_on_wearable_t
{
	int						iActivity;
	const char* pszActivity;
	const char* pszReplacement;
	int						iReplacement; // Replacement activity to play. Might be set to one of kActivityLookup_Unknown/kActivityLookup_Missing.
	const char* pszSequence;
	const char* pszRequiredItem;
	const char* pszScene;
};

struct activity_on_wearable_t
{
	wearableanimplayback_t	iPlayback;
	int						iActivity;
	const char* pszActivity;
};

struct codecontrolledbodygroupdata_t
{
	const char* pFuncName;
	void* pFunc;
};


enum item_capabilities_t
{
	ITEM_CAP_NONE = 0,
	ITEM_CAP_PAINTABLE = 1 << 0,
	ITEM_CAP_NAMEABLE = 1 << 1,
	ITEM_CAP_DECODABLE = 1 << 2,
	ITEM_CAP_CAN_BE_CRAFTED_IF_PURCHASED = 1 << 3,		// was ITEM_CAP_CAN_MOD_SOCKET
	ITEM_CAP_CAN_CUSTOMIZE_TEXTURE = 1 << 4,
	ITEM_CAP_USABLE = 1 << 5,
	ITEM_CAP_USABLE_GC = 1 << 6,
	ITEM_CAP_CAN_GIFT_WRAP = 1 << 7,
	ITEM_CAP_USABLE_OUT_OF_GAME = 1 << 8,
	ITEM_CAP_CAN_COLLECT = 1 << 9,
	ITEM_CAP_CAN_CRAFT_COUNT = 1 << 10,
	ITEM_CAP_CAN_CRAFT_MARK = 1 << 11,
	ITEM_CAP_PAINTABLE_TEAM_COLORS = 1 << 12,
	ITEM_CAP_CAN_BE_RESTORED = 1 << 13,		// can users remove properties (paint, nametag, etc.) from this item via the in-game UI?
	ITEM_CAP_CAN_USE_STRANGE_PARTS = 1 << 14,
	ITEM_CAP_CAN_CARD_UPGRADE = 1 << 15,
	ITEM_CAP_CAN_STRANGIFY = 1 << 16,
	ITEM_CAP_CAN_KILLSTREAKIFY = 1 << 17,
	ITEM_CAP_CAN_CONSUME = 1 << 18,
	ITEM_CAP_CAN_SPELLBOOK_PAGE = 1 << 19,		// IT'S A VERB OKAY
	ITEM_CAP_HAS_SLOTS = 1 << 20,
	ITEM_CAP_DUCK_UPGRADABLE = 1 << 21,
	ITEM_CAP_CAN_UNUSUALIFY = 1 << 22,
	NUM_ITEM_CAPS = 23,
};

enum { ITEM_CAP_DEFAULT = ITEM_CAP_CAN_CRAFT_MARK | ITEM_CAP_CAN_BE_RESTORED | ITEM_CAP_CAN_USE_STRANGE_PARTS | ITEM_CAP_CAN_CARD_UPGRADE | ITEM_CAP_CAN_STRANGIFY | ITEM_CAP_CAN_KILLSTREAKIFY | ITEM_CAP_CAN_CONSUME | ITEM_CAP_CAN_GIFT_WRAP };	// what are the default capabilities on an item?
enum { ITEM_CAP_TOOL_DEFAULT = ITEM_CAP_NONE };

struct CEconItemDefinition
{
	DWORD dword0;
	KeyValues* m_pKVItem;
	WORD m_nDefIndex;
	DWORD dwordC;
	BYTE byte10;
	BYTE m_unMinItemLevel;
	BYTE m_unMaxItemLevel;
	BYTE gap13[3];
	WORD m_nDefaultDropQuantity;
	BYTE gap18[4];
	int int1C;
	DWORD dword20;
	BYTE gap24[4];
	DWORD dword28;
	DWORD dword2C;
	BYTE m_nPopularitySeed;
	const char* m_pszItemBaseName;
	bool m_bProperName;
	const char* m_pszItemTypeName;
	const char* m_pszItemDesc;
	DWORD m_rtExpiration;
	const char* m_pszInventoryModel;
	const char* m_pszInventoryImage;
	BYTE m_pszInventoryOverlayImages[20];
	int m_iInventoryImagePosition_0;
	int m_iInventoryImagePosition_1;
	int m_iInventoryImageSize_0;
	int m_iInventoryImageSize_1;
	int m_iInspectPanelDistance;
	const char* m_pszBaseDisplayModel;
	int m_iDefaultSkin;
	bool m_bLoadOnDemand;
	__declspec(align(2)) bool m_bHideBodyGroupsDeployedOnly;
	const char* m_pszWorldDisplayModel;
	const char* m_pszWorldExtraWearableModel;
	const char* m_pszWorldExtraWearableViewModel;
	const char* m_pszVisionFilteredDisplayModel;
	const char* m_pszCollectionReference;
	bool m_bAttachToHands;
	bool m_bAttachToHandsVMOnly;
	bool m_bFlipViewModel;
	bool m_bActAsWearable;
	bool m_bActAsWeapon;
	bool m_bIsTool;
	BYTE gap9E[30];
	const char* m_pszBrassModelOverride;
	void* m_pTool;
	DWORD dwordC4;
	int m_iCapabilities;
	BYTE gapCC[8];
	const char* m_pszItemClassname;
	const char* m_pszItemLogClassname;
	const char* m_pszItemIconClassname;
	const char* m_pszDefinitionName;
	const char* m_pszDatabaseAuditTable;
	bool m_bHidden;
	bool m_bShouldShowInArmory;
	bool m_bBaseItem;
	bool m_bImported;
	bool m_bIsPackBundle;
	BYTE gapED[11];
	const char* m_pszArmoryDesc;
	const char* m_pszXifierRemapClass;
	const char* m_pszBaseFunctionalItemName;
	const char* m_pszParticleSuffix;
	BYTE gap108[8];
	const char* m_pszArmoryRemap;
	const char* m_pszStoreRemap;
	const char* m_pszClassToken;
	const char* m_pszSlotToken;
	int m_iDropType;
	const char* m_pszHolidayRestriction;
	DWORD m_nVisionFilterFlags;
	DWORD m_iSubType;
	BYTE gap130[4];
	DWORD m_unEquipRegionMask;
	DWORD m_unEquipRegionConflictMask;
	WORD word13C;
	BYTE gap13E[22];
	bool m_bValidForShuffle;
	bool m_bValidForSelfMade;
	int int158;
	DWORD dword15C;
	BYTE gap160[4];
	char* pchar164;
	DWORD dword168;
};