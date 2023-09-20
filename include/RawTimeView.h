/*! \file
*
* \verbatim
******************************************************************************
*                                                                            *
*  Copyright (c) 2020, b-plus GmbH. All rights reserved!                     *
*                                                                            *
*    All rights are exclusively reserved by b-plus GmbH,                     *
*    unless explicitly agreed otherwise.                                     *
*                                                                            *
*    Redistribution in source or any other form,                             *
*    with or without modification, is not permitted.                         *
*                                                                            *
*    You may use this code according to the license terms of b-plus.         *
*    Please contact b-plus at services@b-plus.com to get the actual          *
*    terms and conditions.                                                   *
*                                                                            *
******************************************************************************
\endverbatim
*
* \brief Definition of Raw Timestamp Visualization Object
* \author Harald Birkholz
* \copyright (C)2015-2022 b-plus technologies GmbH
* \date 25.04.2022
* \version 2.4.3
*
******************************************************************************/
#pragma once
#include "Core/Support/AvTimebaseImitation.h"


class CRawTimeView : public AvVis::CAvetoVisMeasObject
{
public:
	// Definition of constructor with initializer list
	CRawTimeView();

	// Default constructor
	~CRawTimeView() override = default;

	DECLARE_OBJECT_CLASS_NAME("Raw Time View")
	DECLARE_OBJECT_GROUP_ASSOC(AVETO::Core::g_szGroupGeneric)

	// Connector map
	BEGIN_AVETO_CONNECTOR_MAP()
		AVETO_CONNECTOR_INPUT("*", "Core packet input to be synchronized", ProcessData)
		AVETO_CONNECTOR_INPUT("MDS_TIMEBASE_SYNC_POINTS", "Synchronization points input", ProcessSyncData)
		// Optionally more input connectors
	END_AVETO_CONNECTOR_MAP()

	enum class ESyncPointSource : uint32_t
	{
		undefined = 0,
		connector,
		player,
		recorder
	};

	BEGIN_AVETO_ENUM_DEF_MAP(ESyncPointSource)
		AVETO_ENUM_DEF_ENTRY(ESyncPointSource::undefined, "Runtime timebase (AvCore::TranslateTime()).")
		AVETO_ENUM_DEF_ENTRY(ESyncPointSource::connector, "Connected stream (MDS_TIMEBASE_SYNC_POINTS).")
		AVETO_ENUM_DEF_ENTRY(ESyncPointSource::player, "Player stream (replay).")
		AVETO_ENUM_DEF_ENTRY(ESyncPointSource::recorder, "Recorder stream (live).")
	END_AVETO_ENUM_DEF_MAP()

	// Property map
	BEGIN_AVETO_PROPERTY_MAP()
		// Chain from base classes to gain access to their properties.
		AVETO_PROPERTY_CHAIN_BASE(AvVis::CAvetoVisMeasObject)
		AVETO_PROPERTY_ENTRY(
			m_uiMaxEntries, 
			"Maximum size of the table", 
			"The table is limited to this number of entries, updated on extension.")
		AVETO_PROPERTY_SET_READONLY_FLAG()
		AVETO_PROPERTY_ENUM_ENTRY(
			ESyncPointSource, 
			m_eSyncPointSource, 
			"Source of synchronization information", 
			"The source of synchronization information determine the translations.")
		AVETO_PROPERTY_RESET_READONLY_FLAG()
	END_AVETO_PROPERTY_MAP()
	   
	/**
	 * \brief Initialization function called when an instance of this class is created.
	 * \return Returns true if the initialization was successful, false if it failed.
	 */
	bool OnCreate() override;

	/**
	* \brief Property changed event. Overload this function to implement property event handling.
	* \param[in] szName The name of the property that changed.
	* \param[in] rvarOld Reference to the variant holding the value of the property.
	* \param[in] rvarNew Reference to the variant holding the value of the property.
	* \param[in] rPropInfo Reference to the property info structure.
	*/
	void OnPropChanged(
		const char* szName, 
		const AvCore::CAvetoVariant& rvarOld,
		const AvCore::CAvetoVariant& rvarNew, 
		AvCore::CPropertyInfo& rPropInfo) override;

	/**
	 * \brief Paints the UI of a instance of this class.
	 * \param[in] rPainter A painter object to draw the UI.
	 */
	void OnPaint(AvVis::CAvPainter& rPainter) override;

	/**
	 * \brief Processes received data packets.
	 * \param[in] rgsPackets An array of data packets.
	 * \param[in] uiPackets The amount of packets in the received array.
	 * \attention The number of packets in the array can be 0; if so rgsPackets is a nullptr!
	 */
	void ProcessData(const AVETO::Core::SDataPacket* rgsPackets, uint32_t uiPackets);

	/**
	 * \brief Processes received data packets.
	 * \param[in] rgsPackets An array of data packets.
	 * \param[in] uiPackets The amount of packets in the received array.
	 * \attention The number of packets in the array can be 0; if so rgsPackets is a nullptr!
	 */
	void ProcessSyncData(const AVETO::Core::SDataPacket* rgsPackets, uint32_t uiPackets);

private:
	/**
	 * \brief Convert a 64-bit-wide integer into a dot-separated string of decimal digits
	 * \param[in] iNumber The integer in question, e.g. 1024 to be converted into 1.024
	 */
	static std::string PrettyPrintScientificInteger(int64_t iNumber);

	/**
	 * \brief Convert a 64-bit-wide integer into a dot-separated string of decimal digits
	 * \param[in] iTimestamp The integer in question, e.g. 1024 to be converted into 1.024
	 */
	static std::string PrettyPrintTimestampUTC(int64_t iTimestamp);


	AvVis::CAvOldTable				m_Table;				//!< Table Widget 
	AvVis::CAvColumn				m_Column0;				//!< Column widget for ID.
	AvVis::CAvColumn				m_Column1;				//!< Column widget for Timestamp
	AvVis::CAvColumn				m_Column2;				//!< Column widget for cycle ID.
	AvVis::CAvColumn				m_Column3;				//!< Column widget for Data Length.
	AvVis::CAvSelect				m_Select0;				//!< Selectable Widget for ID.  //not used
	AvVis::CAvSelect				m_Select1;				//!< Selectable Widget for TimeStamp data.
	AvVis::CAvSelect				m_Select2;				//!< Selectable Widget for cycle ID data.
	AvVis::CAvSelect				m_Select3;				//!< Selectable Widget for packet DataLength.

	typedef std::map<uint32_t, std::tuple<int64_t, int64_t, int64_t, std::string>, std::less<>> TTableMap;
	TTableMap						m_mapTable;				//!< Table of timestamps with approximations
	uint32_t						m_uiMaxEntries;			//!< Maximum length of the table
	typedef std::shared_ptr<AVETO::Core::Support::CTimebaseImitation> TImitationPtr;
	TImitationPtr					m_ptrImitation;			//!< Shared pointer with the timebase imitation.
	ESyncPointSource				m_eSyncPointSource;		//!< Enumeration of sources for synchronization points.
};

// Defining this class to be an AVETO object to be found by the application.
DEFINE_AVETO_OBJECT(CRawTimeView)

